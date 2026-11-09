#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "../device/dsp/engine.c"
#include "../device/air_sensor.h"
static sr_engine e;static int16_t pcm[882];
static unsigned active(void){unsigned n=0;for(unsigned i=0;i<16;i++)n+=e.voices[i].active;return n;}
static void init(void){sr_init(&e,732);sr_start(&e);e.tick_phase=1;e.external_sync=1; /* Isolate scheduled notes from autonomous phrase onsets. */e.solo=SR_GUITAR;e.role_target[SR_GUITAR]=1;e.chord_inversion=1;}
int main(void){
 init();e.chord_mode=1;chord_phrase(&e,0);assert(active()==3);for(unsigned i=0;i<3;i++)assert(e.voices[i].born==0);
 for(unsigned gap=80;gap<=140;gap+=10){
  init();e.chord_mode=2;e.chord_gap=gap;chord_phrase(&e,0);assert(active()==1);
  for(unsigned i=0;i<2;i++){
   unsigned due=(i+1u)*gap*SR_RATE/1000u;
   while(e.frame_count<due){unsigned n=due-e.frame_count;if(n>441)n=441;sr_render(&e,pcm,n);}
   sr_render(&e,pcm,1);assert(active()==i+2);
   assert(e.voices[i+1].born==due);
  }
  sr_stop(&e);for(unsigned i=0;i<12;i++)assert(!e.scheduled[i].active);
 }
 init();for(unsigned i=0;i<13;i++)schedule(&e,4,60,60,1,10000);assert(e.scheduled_overflows==1);
 aq_air_gesture g={0};assert(!aq_air_gesture_update(&g,0,1,1,100,70));
 assert(!aq_air_gesture_update(&g,90,1,2,200,70));assert(!aq_air_gesture_update(&g,90,1,2,240,70));
 assert(aq_air_gesture_update(&g,0,1,1,320,70));assert(!aq_air_gesture_update(&g,90,1,20,360,70));
 assert(!aq_air_gesture_update(&g,0,0,0,2200,70));assert(g.stage==0);
 for(unsigned t=2300;t<4000;t+=40)assert(!aq_air_gesture_update(&g,45+(t%3),1,1,t,70));
 unsigned hashes[3]={0};
 for(unsigned style=0;style<3;style++){
  sr_init(&e,732);e.rhythm_style=style;e.drum_density=4;sr_start(&e);
  for(unsigned block=0;block<800;block++){
   sr_render(&e,pcm,256);
   for(unsigned i=0;i<512;i++){assert(pcm[i]!=-32768);hashes[style]=hashes[style]*33u+(uint16_t)pcm[i];}
  }
 }
 assert(hashes[0]!=hashes[1]&&hashes[1]!=hashes[2]&&hashes[0]!=hashes[2]);
 puts("PASS Together sample simultaneity; all seven 80–140ms strum gaps sample-exact; STOP queue cancellation; bounded overflow; single deliberate RF pulse, cooldown/loss/noise rejection; three distinct rendered rhythms");
}
