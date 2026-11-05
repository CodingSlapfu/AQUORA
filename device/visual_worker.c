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
static void resonance_job(void)
{
    unsigned time=vis_input.phase;
    unsigned petals=3u+vis_input.frequency/300u;
    float bass=fm_clampf(vis_input.meter[SR_BASS]*42.f,0,8);
    float drums=fm_clampf(vis_input.meter[SR_DRUMS]*38.f,0,7);
    float choir=fm_clampf(vis_input.meter[SR_CHOIR]*25.f,0,4);
    float piano=fm_clampf(vis_input.meter[SR_PIANO]*40.f,0,7);
    for(unsigned ring=0;ring<SR_TRACKS;ring++){
        float radius=vis_input.full?20.f+ring*14.f:14.f+ring*11.f;
        float level=fm_clampf(vis_input.meter[ring]*32.f,0,8);
        unsigned hit=vis_input.event[ring]*9u;
        for(unsigned j=0;j<=32;j++){
            unsigned angle=j*4u;
            float x=vis_sine[(angle+32u+time)&127u];
            float z=vis_sine[(angle+time)&127u];
            float swell=vis_sine[(angle*petals-time*2u+ring*9u)&127u];
            float ripple=vis_sine[(angle*2u+time*3u+hit)&127u];
            float r=radius+(bass+level)*swell;
            float projection=1.f/(1.f+z*r*.0017f);
            float wave=(drums+piano)*ripple+(choir+vis_input.atmosphere*6.f)*swell;
            vis_output.xy[ring][j][0]=120+(int)(x*r*projection);
            vis_output.xy[ring][j][1]=(vis_input.full?120:104)+
                (int)((z*r*(vis_input.full?.93f:.58f)+wave)*projection);
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
    vis_input.atmosphere=engine.transport_env*engine.bed_level+engine.reverb_wet*.6f;
    for(unsigned t=0;t<SR_TRACKS;t++){
      vis_input.meter[t]=engine.tracks[t].meter;
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
