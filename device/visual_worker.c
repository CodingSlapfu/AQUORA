/* SPDX-License-Identifier: MPL-2.0
 * Optional core-1 geometry job. Audio never submits or waits for worker jobs.
 * Inputs are frozen through completion; fallback holds a failed worker before reuse. */
#if defined(PLAT_HOST) && FM1B_CPU2
static int host_worker_on,host_worker_reason;
int plat_cpu2_set(int on){const char *m=getenv("FM1_WORKER_TEST");host_worker_on=on && !(m&&!strcmp(m,"off"));return host_worker_on?0:-1;}
int plat_cpu2_on(void){return host_worker_on;}
int plat_cpu2_run(void(*fn)(uint32_t),uint32_t arg){if(!host_worker_on)return 0;const char*m=getenv("FM1_WORKER_TEST");if(m&&strcmp(m,"ok")){host_worker_reason=!strcmp(m,"bad")?1:!strcmp(m,"late")?2:4;return 1;}fn(arg);return 1;}
int plat_cpu2_wait(void){if(host_worker_reason){host_worker_on=0;return -1;}return 0;}
void plat_cpu2_sync(void){}
int plat_cpu2_busy(void){return 0;}
int plat_cpu2_gave_up(void){return host_worker_reason;}
uint32_t plat_cpu2_bad(void){return host_worker_reason==1;}
int plat_cpu2_barred(void){return host_worker_reason!=0;}
uint32_t plat_cpu2_stack(void){return 0;}
#elif !FM1B_CPU2
int plat_cpu2_set(int on) { return on ? -1 : 0; }
int plat_cpu2_on(void) { return 0; }
int plat_cpu2_run(void (*job)(uint32_t), uint32_t arg)
{
    (void)job;
    (void)arg;
    return 0;
}
int plat_cpu2_wait(void) { return 0; }
void plat_cpu2_sync(void) {}
int plat_cpu2_gave_up(void) { return 0; }
uint32_t plat_cpu2_bad(void) { return 0; }
uint32_t plat_cpu2_stack(void) { return 0; }
#endif
static struct {
    unsigned phase, event[SR_TRACKS], water, frequency, scene, full;
    float atmosphere;
    unsigned tuning, tonic;
    float onset[SR_TRACKS];
    float meter[SR_TRACKS];
} vis_input __attribute__((aligned(64)));
static struct {int16_t xy[SR_TRACKS][33][2];uint8_t count[SR_TRACKS];unsigned busy_us;int16_t nodes[256][4];uint16_t node_count;} vis_output __attribute__((aligned(64)));
static float vis_sine[128];
static unsigned worker_wait_us,worker_fallbacks;

static void visual_init(void)
{
    worker_wait_us=worker_fallbacks=0;
    for(unsigned i=0;i<128;i++)vis_sine[i]=fm_sinf(i*(6.28318530718f/128.f));
}

/* Resonance Plate: Chladni-like standing-wave interference sampled on a fixed
 * 17x17 circular plate. Marching zero-crossings yields nodal contours, not
 * eight role rings. A radial term evokes water resonance; this is artwork,
 * not a scientific simulation. Fixed memory/256-segment cap, display only. */
static float nodal_level;
static float nodal_field[17][17];
static float visual_sin(float phase)
{
    int a=(int)phase;float f=phase-a;
    if(f<0){a--;f+=1.f;}
    float x=vis_sine[(unsigned)a&127u];
    return x+(vis_sine[(unsigned)(a+1)&127u]-x)*f;
}
static void nodal_segment(float ax,float ay,float bx,float by)
{
    if(vis_output.node_count>=256)return;
    int16_t *p=vis_output.nodes[vis_output.node_count++];
    float sy=vis_input.full?110.f:44.f;
    float cy=vis_input.full?120.f:96.f;
    p[0]=120+(int)(ax*110.f);p[1]=(int)(cy+ay*sy);
    p[2]=120+(int)(bx*110.f);p[3]=(int)(cy+by*sy);
}
static void resonance_job(void)
{
    vis_output.node_count=0;
    float activity=0,hit=0;
    for(unsigned t=0;t<SR_TRACKS;t++){activity+=vis_input.meter[t]*8.f;hit+=vis_input.onset[t]*.06f;}
    float target=fm_clampf(activity+hit,0,1);
    nodal_level+=(target-nodal_level)*(target>nodal_level?.4f:.12f);
    if(nodal_level<.004f)return;
    static const uint8_t modes[13][2]={{2,3},{3,4},{4,5},{2,4},{3,5},{4,6},{5,3},{5,6},{6,4},{3,7},{7,5},{4,7},{6,7}};
    unsigned m=modes[vis_input.scene%13][0],n=modes[vis_input.scene%13][1];
    float hz=(vis_input.frequency%241u)*(1.f/241.f);
    float radial=.12f+fm_clampf(vis_input.meter[SR_BASS]*9.f,0,.35f);
    float mix=.78f+.12f*visual_sin(vis_input.phase*.12f)+hit*.4f;
    /* Separable standing modes: 68 LUT evaluations instead of 2500.
     * The circular radial term remains per grid point. */
    float mx[17],nx[17];
    for(unsigned x=0;x<17;x++){
      float px=(x-8.f)/8.f;
      mx[x]=visual_sin((px+1.f)*32.f*m);
      nx[x]=visual_sin((px+1.f)*32.f*n);
    }
    for(unsigned y=0;y<17;y++)for(unsigned x=0;x<17;x++){
      float px=(x-8.f)/8.f,py=(y-8.f)/8.f;
      float r2=px*px+py*py;
      nodal_field[y][x]=mx[x]*nx[y]-mix*nx[x]*mx[y]+radial*visual_sin(r2*(85.f+hz*40.f)+32.f);
    }
    for(unsigned y=0;y<16;y++)for(unsigned x=0;x<16;x++){
      float px=(x-8.f)/8.f,py=(y-8.f)/8.f;
      if((px+.0625f)*(px+.0625f)+(py+.0625f)*(py+.0625f)>.91f)continue;
      float v[4]={nodal_field[y][x],nodal_field[y][x+1],nodal_field[y+1][x+1],nodal_field[y+1][x]};
      const float dx[4]={0,1,1,0},dy[4]={0,0,1,1};
      float ex[4],ey[4];unsigned count=0;
      for(unsigned k=0;k<4;k++){
        unsigned next=(k+1)&3;
        if((v[k]<0)!=(v[next]<0)){
          float f=v[k]/(v[k]-v[next]);
          ex[count]=px+(dx[k]+f*(dx[next]-dx[k]))/8.f;
          ey[count]=py+(dy[k]+f*(dy[next]-dy[k]))/8.f;count++;
        }
      }
      for(unsigned k=1;k<count;k+=2)nodal_segment(ex[k-1],ey[k-1],ex[k],ey[k]);
    }
}

static void visual_job(uint32_t unused)
{
    (void)unused;
    unsigned begin=plat_ticks24();
    if(vis_input.water)resonance_job();
    else for(unsigned t=0;t<SR_TRACKS;t++){
      vis_output.count[t]=17;
      unsigned phase=vis_input.phase+t*16+vis_input.event[t]*5;
      float pulse=vis_input.meter[t]*55.f;
      float radius=(vis_input.full?52.f+t*9.f:45.f+t*6.f)+(pulse>13.f?13.f:pulse);
      for(unsigned j=0;j<=16;j++){
        unsigned a=j*8;
        float petal=radius*(.82f+.18f*vis_sine[(3*a+phase)&127]);
        vis_output.xy[t][j][0]=120+(int)(petal*vis_sine[(a+phase)&127]);
        vis_output.xy[t][j][1]= (vis_input.full?120:96)+(int)(petal*(vis_input.full?1.04f:.44f)*vis_sine[(2*a+phase)&127]);
      }
    }
    vis_output.busy_us=(plat_ticks24()-begin)/24;
}

static void prepare_geometry(void)
{
    AQ_VIS_COUNT(geometry_jobs);
    vis_input.phase=plat_ms()/(visual_mode?35u:49u);
    unsigned flags=plat_lock();
    vis_input.water=visual_mode&&page==0;
    vis_input.full=fullscreen;
    vis_input.frequency=(unsigned)sr_frequencies[engine.frequency].hz;
    vis_input.scene=engine.scene;
    vis_input.tuning=engine.tuning_432;vis_input.tonic=sr_tonic(&engine);
    vis_input.atmosphere=engine.transport_env*engine.bed_level+engine.reverb_wet*.6f;
    for(unsigned t=0;t<SR_TRACKS;t++){
      vis_input.meter[t]=engine.tracks[t].meter;
      vis_input.onset[t]=engine.tracks[t].events!=vis_input.event[t]&&engine.tracks[t].meter>.00001f?1.f:vis_input.onset[t]*.82f;
      vis_input.event[t]=engine.tracks[t].events;
    }
    plat_unlock(flags);
    plat_cpu2_sync();
    unsigned begin=plat_ticks24();
    if(plat_cpu2_run(visual_job,0)){
      AQ_VIS_COUNT(worker_jobs);AQ_VIS_COUNT(worker_waits);
      if(plat_cpu2_wait()!=0){worker_fallbacks++;visual_job(0);}
    }else visual_job(0);
    worker_wait_us=(plat_ticks24()-begin)/24;
    plat_cpu2_sync();
}
