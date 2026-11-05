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
static struct {int16_t xy[SR_TRACKS][33][2];uint8_t count[SR_TRACKS];unsigned busy_us;} vis_output __attribute__((aligned(64)));
static float vis_sine[128];
static unsigned worker_wait_us,worker_fallbacks;

static void visual_init(void)
{
    worker_wait_us=worker_fallbacks=0;
    for(unsigned i=0;i<128;i++)vis_sine[i]=fm_sinf(i*(6.28318530718f/128.f));
}

/* Project nested wavefronts onto a tilted plane. Fixed segments and a
 * lookup table keep camera motion independent of the audio deadline. */
/* Bounded artistic profiles, not a physical model of water or health effects.
 * Symmetry, branching, tilt, curvature, twist, halo differ for all intentions.
 * All trigonometry is a shared 128-entry table, never an audio FFT. */
static const float crystal_profile[SR_SCENES][6]={
 {6,3,.90f,.0018f, .18f,1.0f}, {4,7,.72f,.0022f,-.25f,.7f},
 {5,2,.85f,.0015f, .30f,1.2f}, {8,3,.93f,.0020f, .12f,.9f},
 {3,5,.78f,.0026f,-.35f,.6f}, {7,4,.88f,.0014f, .24f,1.1f},
 {6,5,.96f,.0017f,-.18f,1.3f}, {4,3,.80f,.0024f, .40f,.8f},
 {5,7,.91f,.0018f,-.28f,1.4f}, {8,5,.83f,.0021f, .35f,1.0f},
 {7,2,.98f,.0013f,-.12f,1.5f}, {3,7,.87f,.0023f, .28f,1.2f},
 {6,7,.94f,.0016f,-.40f,1.6f}
};
static float crystal_shape[6];
static float visual_sin(float phase)
{
    int a=(int)phase;float f=phase-a;
    if(f<0){a--;f+=1.f;}
    float x=vis_sine[(unsigned)a&127u];
    return x+(vis_sine[(unsigned)(a+1)&127u]-x)*f;
}
static void resonance_job(void)
{
    unsigned time=vis_input.phase;
    const float *target=crystal_profile[vis_input.scene%SR_SCENES];
    if(!crystal_shape[0])for(unsigned k=0;k<6;k++)crystal_shape[k]=target[k];
    else for(unsigned k=0;k<6;k++)crystal_shape[k]+=(target[k]-crystal_shape[k])*.15f;
    float bass=fm_clampf(vis_input.meter[SR_BASS]*42.f,0,8);
    float drums=fm_clampf(vis_input.meter[SR_DRUMS]*38.f,0,7);
    float choir=fm_clampf(vis_input.meter[SR_CHOIR]*25.f,0,4);
    float piano=fm_clampf((vis_input.meter[SR_PIANO]+vis_input.meter[SR_GUITAR])*26.f,0,7);
    float frequency=vis_input.frequency*.021f+(vis_input.tuning?0.f:.8f);
    float motif=vis_input.tonic*.18f;
    for(unsigned ring=0;ring<SR_TRACKS;ring++){
        float radius=vis_input.full?20.f+ring*14.f:14.f+ring*11.f;
        float level=fm_clampf(vis_input.meter[ring]*32.f,0,8);
        float hit=vis_input.onset[ring];
        for(unsigned j=0;j<=32;j++){
            float angle=j*4.f;
            float twist=ring*crystal_shape[4]+motif;
            float x=visual_sin(angle+32.f+time+twist);
            float z=visual_sin(angle+time+twist);
            float swell=visual_sin(angle*crystal_shape[0]-time*2.f+ring*9.f+frequency);
            float branch=visual_sin(angle*crystal_shape[1]+ring*frequency+time);
            float ripple=visual_sin(angle*2.f+time*3.f-ring*13.f);
            float r=radius+(bass+level+2.f)*swell+branch*(piano+hit*4.f);
            float projection=1.f/(1.f+z*r*crystal_shape[3]);
            float wave=(drums+hit*8.f)*ripple+(choir+vis_input.atmosphere*6.f)*swell;
            wave+=branch*choir*crystal_shape[5];
            vis_output.xy[ring][j][0]=120+(int)(x*r*projection);
            vis_output.xy[ring][j][1]=(vis_input.full?126:104)+
                (int)((z*r*(vis_input.full?crystal_shape[2]:.58f)+wave)*projection);
        }
        vis_output.count[ring]=33;
    }
}

static void visual_job(uint32_t unused)
{
    (void)unused;
    unsigned begin=plat_ticks24();
    if(vis_input.water)resonance_job();
    else for(unsigned t=0;t<SR_TRACKS;t++){
      vis_output.count[t]=33;
      unsigned phase=vis_input.phase+t*16+vis_input.event[t]*5;
      float pulse=vis_input.meter[t]*55.f;
      float radius=(vis_input.full?52.f+t*9.f:45.f+t*6.f)+(pulse>13.f?13.f:pulse);
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
      if(plat_cpu2_wait()!=0){worker_fallbacks++;visual_job(0);}
    }else visual_job(0);
    worker_wait_us=(plat_ticks24()-begin)/24;
    plat_cpu2_sync();
}
