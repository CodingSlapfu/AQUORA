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
    float atmosphere, pitch;
    unsigned tuning, tonic;
    float onset[SR_TRACKS];
    float meter[SR_TRACKS];
} vis_input __attribute__((aligned(64)));
static struct {int16_t xy[SR_TRACKS][33][2];uint8_t count[SR_TRACKS];unsigned busy_us;int16_t nodes[256][4];uint16_t node_count;int16_t sand[512][2];uint8_t water_light[30][30];} vis_output __attribute__((aligned(64)));
static float vis_sine[128];
static unsigned worker_wait_us,worker_fallbacks;

static void visual_init(void)
{
    worker_wait_us=worker_fallbacks=0;
    for(unsigned i=0;i<128;i++)vis_sine[i]=fm_sinf(i*(6.28318530718f/128.f));
}

/* Original bounded water/sand illusion: grains move down the gradient of
 * standing-wave energy, settle near nodes, and lift/scatter on musical hits.
 * Surface normals illuminate travelling ripples, not marching contours.
 * This is a visual model, not a scientific or medical simulation. */
static float nodal_level;
static float sand_x[512],sand_y[512],sand_vx[512],sand_vy[512];
static unsigned sand_ready,water_last_phase;
static float water_m=2.f,water_n=3.f;
static float visual_sin(float phase)
{
    int a=(int)phase;float f=phase-a;
    if(f<0){a--;f+=1.f;}
    float x=vis_sine[(unsigned)a&127u];
    return x+(vis_sine[(unsigned)(a+1)&127u]-x)*f;
}
static float sand_field(float x,float y)
{
    return visual_sin((x+1.f)*32.f*water_m)*visual_sin((y+1.f)*32.f*water_n)
      -visual_sin((x+1.f)*32.f*water_n)*visual_sin((y+1.f)*32.f*water_m);
}
static void resonance_job(void)
{
    vis_output.node_count=0;
    if(!sand_ready){
      unsigned seed=0x41515541u;
      for(unsigned i=0;i<512;i++){
        seed=seed*1664525u+1013904223u;sand_x[i]=(seed>>8)*(2.f/16777216.f)-1.f;
        seed=seed*1664525u+1013904223u;sand_y[i]=(seed>>8)*(2.f/16777216.f)-1.f;
      }
      sand_ready=1;
    }
    float activity=0,hit=0;
    for(unsigned t=0;t<SR_TRACKS;t++){
      activity+=vis_input.meter[t]*8.f;hit+=vis_input.onset[t]*.06f;
    }
    float target=fm_clampf(activity+hit,0,1);
    nodal_level+=(target-nodal_level)*(target>nodal_level?.4f:.12f);
    float dt=(vis_input.phase-water_last_phase)*.035f;
    water_last_phase=vis_input.phase;
    dt=fm_clampf(dt,.015f,.1f);
    float pitch=vis_input.pitch>0?vis_input.pitch:(float)vis_input.frequency;
    float complexity=fm_clampf(pitch*(1.f/450.f)+vis_input.frequency*(1.f/1600.f),0,3.f);
    float goal_m=2.f+(vis_input.scene%3u)+complexity*.3f;
    float goal_n=3.f+((vis_input.scene*7u)%4u)+complexity*.6f;
    water_m+=(goal_m-water_m)*dt*.9f;
    water_n+=(goal_n-water_n)*dt*.9f;
    float cy=vis_input.full?120.f:96.f,sy=vis_input.full?117.f:44.f;
    for(unsigned i=0;i<512;i++){
      float x=sand_x[i],y=sand_y[i],f=sand_field(x,y);
      float gx=(sand_field(x+.008f,y)-sand_field(x-.008f,y))*62.5f;
      float gy=(sand_field(x,y+.008f)-sand_field(x,y-.008f))*62.5f;
      float drive=nodal_level*.08f;
      float scatter=hit*.11f*visual_sin(vis_input.phase*3.f+i*17.f);
      sand_vx[i]=(sand_vx[i]-fm_clampf(f*gx,-12,12)*drive*dt+scatter*dt)*.77f;
      sand_vy[i]=(sand_vy[i]-fm_clampf(f*gy,-12,12)*drive*dt-scatter*dt)*.77f;
      sand_x[i]=fm_clampf(x+sand_vx[i]*dt,-.99f,.99f);
      sand_y[i]=fm_clampf(y+sand_vy[i]*dt,-.99f,.99f);
      vis_output.sand[i][0]=120+(int)(sand_x[i]*117.f);
      vis_output.sand[i][1]=(int)(cy+sand_y[i]*sy);
    }
    for(unsigned y=0;y<30;y++)for(unsigned x=0;x<30;x++){
      float px=(x-14.5f)/15.f,py=(y-14.5f)/15.f;
      float radius=fm_sqrtf(px*px+py*py);
      float ripple=visual_sin(radius*(75.f+complexity*20.f)-vis_input.phase*3.f);
      float standing=sand_field(px,py);
      float light=nodal_level*(.45f+ripple*.18f+standing*.12f);
      vis_output.water_light[y][x]=(uint8_t)fm_clampf(light*15.f,0,15);
      /* Bright short water reflections, never an outline of the nodes. */
      if(nodal_level>.004f&&x%2u==0&&y%2u==0&&vis_output.node_count<256){
        int16_t *p=vis_output.nodes[vis_output.node_count++];
        unsigned glint=(x*37u+y*73u)^((x+y)*17u);
        p[0]=x*8+1+(glint&3u);
        p[2]=p[0]+1+(vis_output.water_light[y][x]>7?4:1);
        p[1]=(int)(cy+py*sy+(int)((glint>>3)&7u)-3+nodal_level*ripple*3.f);
        if(p[1]<0)p[1]=0;if(p[1]>239)p[1]=239;
        p[3]=p[1];
      }
    }
}

/* Original vibrating-plate mode. A bounded modal grid combines rectangular
 * interference with radial/angular basis functions. Particles descend |f|^2,
 * accumulating at stationary nodes; attacks loosen grains and bass dilates
 * the plate. Shared particle storage is re-seeded only on a mode switch. */
static float plate_grid[25][25],plate_blend,plate_activity,plate_order=4;
static unsigned particle_mode;
static float plate_sample(float x,float y)
{
  float gx=fm_clampf((x+1.f)*12.f,0,23.999f),gy=fm_clampf((y+1.f)*12.f,0,23.999f);
  unsigned ix=(unsigned)gx,iy=(unsigned)gy;float fx=gx-ix,fy=gy-iy;
  float a=plate_grid[iy][ix],b=plate_grid[iy][ix+1];
  float c=plate_grid[iy+1][ix],d=plate_grid[iy+1][ix+1];
  return (a+(b-a)*fx)*(1.f-fy)+(c+(d-c)*fx)*fy;
}
static void chladni_job(void)
{
  float energy=0,hit=0;
  for(unsigned t=0;t<SR_TRACKS;t++){energy+=vis_input.meter[t]*8.f;hit+=vis_input.onset[t]*.08f;}
  float target=fm_clampf(energy+hit,0,1);
  plate_activity+=(target-plate_activity)*(target>plate_activity?.55f:.10f);
  float pitch=vis_input.pitch>0?vis_input.pitch:vis_input.frequency;
  float order=2.f+fm_clampf(pitch/280.f,0,6.f);
  plate_order+=(order-plate_order)*.045f;
  float blend=(vis_input.scene%5u)*.16f+.15f;
  plate_blend+=(blend-plate_blend)*.035f;
  unsigned petals=2u+((unsigned)plate_order+vis_input.scene)%7u;
  float bass=vis_input.meter[1]*4.f;
  for(unsigned j=0;j<25;j++)for(unsigned i=0;i<25;i++){
    float x=((int)i-12)/12.f,y=((int)j-12)/12.f;
    float radius=fm_sqrtf(x*x+y*y+.00001f);
    float c=x/radius,prev=1.f,now=c;
    for(unsigned n=2;n<=petals;n++){float next=2.f*c*now-prev;prev=now;now=next;}
    float radial=visual_sin(radius*(32.f+plate_order*11.f-bass*5.f));
    float rect=visual_sin((x+1.f)*32.f*plate_order)*visual_sin((y+1.f)*32.f*(plate_order+1.f))
      -visual_sin((y+1.f)*32.f*plate_order)*visual_sin((x+1.f)*32.f*(plate_order+1.f));
    plate_grid[j][i]=rect*(1.f-plate_blend)+(radial+now*.65f)*plate_blend;
  }
  vis_output.node_count=0;
  float cy=vis_input.full?120.f:96.f,sy=vis_input.full?112.f:42.f;
  for(unsigned i=0;i<512;i++){
    float x=sand_x[i],y=sand_y[i],f=plate_sample(x,y);
    float gx=(plate_sample(x+.02f,y)-plate_sample(x-.02f,y))*25.f;
    float gy=(plate_sample(x,y+.02f)-plate_sample(x,y-.02f))*25.f;
    float impulse=hit*.0018f*visual_sin(i*17.f+vis_input.phase);
    sand_vx[i]=(sand_vx[i]-fm_clampf(f*gx,-6,6)*plate_activity*.0015f+impulse)*.78f;
    sand_vy[i]=(sand_vy[i]-fm_clampf(f*gy,-6,6)*plate_activity*.0015f-impulse)*.78f;
    sand_x[i]=fm_clampf(x+sand_vx[i],-.98f,.98f);
    sand_y[i]=fm_clampf(y+sand_vy[i],-.98f,.98f);
    vis_output.sand[i][0]=120+(int)(sand_x[i]*112.f);
    vis_output.sand[i][1]=(int)(cy+sand_y[i]*sy);
  }
  /* Thin interpolated zero crossings: nodal segments, never canned shapes. */
  for(unsigned y=0;y<24;y++)for(unsigned x=0;x<24;x++){
    float a=plate_grid[y][x],b=plate_grid[y][x+1],c=plate_grid[y+1][x];
    if((a<0)!=(b<0)&&(a<0)!=(c<0)&&vis_output.node_count<256){
      int16_t *p=vis_output.nodes[vis_output.node_count++];
      float px=((int)x-12)/12.f,py=((int)y-12)/12.f;
      p[0]=120+(int)((px+a/(a-b)/12.f)*112.f);p[1]=(int)(cy+py*sy);
      p[2]=120+(int)(px*112.f);p[3]=(int)(cy+(py+a/(a-c)/12.f)*sy);
    }
  }
}

static void visual_job(uint32_t unused)
{
    (void)unused;
    unsigned begin=plat_ticks24();
    if(vis_input.water){
      if(particle_mode!=vis_input.water){sand_ready=0;particle_mode=vis_input.water;}
      if(vis_input.water==2){
        if(!sand_ready){unsigned seed=0x43484c41u;for(unsigned i=0;i<512;i++){
          seed=seed*1664525u+1013904223u;sand_x[i]=(seed>>8)*(2.f/16777216.f)-1.f;
          seed=seed*1664525u+1013904223u;sand_y[i]=(seed>>8)*(2.f/16777216.f)-1.f;
          sand_vx[i]=sand_vy[i]=0;
        }sand_ready=1;}
        chladni_job();
      }else resonance_job();
    }
    else for(unsigned t=0;t<SR_TRACKS;t++){
      vis_output.count[t]=33;
      unsigned phase=vis_input.phase+t*16+vis_input.event[t]*5;
      float pulse=vis_input.meter[t]*140.f+vis_input.onset[t]*9.f;
      float radius=(vis_input.full?52.f+t*9.f:45.f+t*6.f)+(pulse>22.f?22.f:pulse);
      for(unsigned j=0;j<=32;j++){
        unsigned a=j*4;
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
    vis_input.water=page==0?visual_mode:0;
    vis_input.full=fullscreen;
    vis_input.frequency=(unsigned)sr_frequencies[engine.frequency].hz;
    vis_input.scene=engine.scene;
    vis_input.tuning=engine.tuning_432;vis_input.tonic=sr_tonic(&engine);
    vis_input.atmosphere=engine.transport_env*engine.bed_level+engine.reverb_wet*.6f;
    /* Audible voice-weighted pitch avoids a muted/stolen first slot freezing
     * the water mode. Runs only on visible-frame telemetry, never in audio. */
    float pitch_sum=0,pitch_weight=0;
    for(unsigned v=0;v<SR_VOICES;v++){
      const sr_voice *voice=&engine.voices[v];unsigned t=voice->track;
      if(!voice->active||!voice->gate||t>=SR_DRUMS||engine.tracks[t].mute||
         (engine.solo<SR_TRACKS&&engine.solo!=t))continue;
      float weight=voice->env*voice->peak*engine.tracks[t].volume;
      if(!voice->key&&!voice->external)weight*=engine.role_gain[t];
      pitch_sum+=voice->freq*weight;pitch_weight+=weight;
    }
    vis_input.pitch=pitch_weight>.00001f?pitch_sum/pitch_weight:0;
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
