/* Owner acceptance probes for #64-70: actual clock, reachable sounds,
 * user precedence, continuous transport and audible section boundaries. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static int16_t pcm[512];
int main(void){
 unsigned combinations=0;unsigned long long steps=0;uint64_t all_patches=0;uint32_t all_kits=0;
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,730610+scene);sr_set_music_mode(&e,mode);sr_set_scene(&e,scene);
  uint64_t patches=0;uint32_t kits=0;unsigned min=200,max=0,tempos[121]={0};
  for(unsigned seed=1;seed<=4096;seed++){
   sr_generate_song(&e,seed*65537u+scene*13u+mode,0);
   unsigned bpm=(SR_RATE*15u+e.tick_length/2u)/e.tick_length;
   assert(bpm>=36&&bpm<=120);tempos[bpm]++;if(bpm<min)min=bpm;if(bpm>max)max=bpm;
   for(unsigned t=0;t<7;t++)patches|=1ull<<e.tracks[t].patch;
   kits|=1u<<e.drum_kit;assert(e.drum_presence==100);
  }
  /* Availability is global; musical contexts do not owe a coverage quota. */
  all_patches|=patches;all_kits|=kits;assert(max-min>=35);
  unsigned distinct=0;for(unsigned b=0;b<=120;b++)distinct+=!!tempos[b];assert(distinct>=35);
  e.motion_manual|=4;e.tick_length=7000;e.drum_presence=27;
  sr_generate_song(&e,555,0);assert(e.tick_length==7000&&e.drum_presence==27);
  sr_set_scene(&e,(scene+1)%21);assert(e.tick_length==7000&&e.drum_presence==27);
  e.sound_locks=1023;unsigned locked[8];for(unsigned t=0;t<7;t++)locked[t]=e.tracks[t].patch;locked[7]=e.drum_kit;
  uint32_t rng_before=e.rng;int8_t motif[16];memcpy(motif,e.melody,16);
  sr_generate_song(&e,777,1);assert(e.rng==rng_before&&!memcmp(motif,e.melody,16));
  for(unsigned t=0;t<7;t++)assert(locked[t]==e.tracks[t].patch);assert(locked[7]==e.drum_kit);
  for(unsigned length=16;length<=64;length*=2){
   e.freeze=0;e.variation=.7f;e.sound_locks=0;e.loop_length=length;sr_start(&e);
   uint32_t sections=0;unsigned tempo=e.tick_length;
   /* 2+ musical hours at 94 BPM even with the shortest phrase. Advance
    * actual sample clock for scheduled events; bypass per-sample DSP only. */
   for(unsigned n=0;n<49152;n++){
    sections|=1u<<form_index(&e,0);trigger(&e);
    assert(e.running&&e.tick_length==tempo&&e.role_target[SR_DRUMS]>=.37f);
    e.frame_count+=e.tick_length;memset(e.scheduled,0,sizeof e.scheduled);
   }
   assert(__builtin_popcount(sections)==32);steps+=49152;
   e.freeze=1;unsigned form=e.form_loop;for(unsigned n=0;n<128;n++)trigger(&e);assert(e.form_loop==form);
   sr_stop(&e);assert(!e.running);e.stop_frames=0;
  }
  printf("coverage scene%u mode%u weighted palette; BPM%u-%u unique%u\n",scene,mode,min,max,distinct);combinations++;
 }
 assert(all_patches==((1ull<<36)-1)&&all_kits==((1u<<19)-1));
 /* PCM at every section boundary, all kits and modes. Output limiter
  * ceiling, sustained pulse and post-boundary downbeat are measured. */
 double least=1e30;unsigned peak=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned kit=0;kit<19;kit++)for(unsigned slot=0;slot<32;slot++){
  sr_init(&e,555+kit);sr_set_music_mode(&e,mode);e.drum_kit=kit;e.drum_density=4;e.drum_hit=50;e.solo=7;e.loop_length=16;
  sr_start(&e);e.form_loop=slot*(2u+sr_scenes[e.scene].energy%3u);plan_roles(&e);e.sequence_step=16;e.loop_pos=14;
  e.tick_length=5513;e.tick_phase=e.tick_length;double pre=0,post=0;
  for(unsigned b=0;b<96;b++){
   sr_render(&e,pcm,256);
   for(unsigned i=0;i<512;i++){unsigned a=abs(pcm[i]);if(a>peak)peak=a;assert(a<32730);if(b<43)pre+=(double)a*a;else post+=(double)a*a;}
  }
  assert(pre>100000&&post>100000);if(post<least)least=post;
 }
 printf("PASS %u complete coverage grids; %llu timeline steps; 2432 actual PCM boundary windows, peak%u minimum post energy%.0f\n",combinations,steps,peak,least);
}
