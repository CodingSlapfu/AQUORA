#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
int main(void){
 sr_init(&e,1777);sr_set_scene(&e,1);sr_set_music_mode(&e,SR_EXPERIMENTAL);
 sr_generate_song(&e,71177,0);e.loop_length=64;sr_start(&e);
 uint64_t frames=0;unsigned moves=0,returns=0,cycles=0;
 uint32_t key=e.form_key;int8_t home[16];memcpy(home,e.motif_home,16);
 for(unsigned step=0;step<262144;step++){
  unsigned before=form_index(&e,0),next=form_index(&e,1),phrase=e.phrase;
  trigger(&e);
  if(form_index(&e,0)!=before){assert(form_index(&e,0)==next);moves++;}
  if(e.form_key!=key){cycles++;key=e.form_key;}
  if(e.phrase!=phrase&&e.phrase%4u==0u){assert(!memcmp(home,e.melody,16));returns++;}
  assert(e.running&&e.tick_length&&e.drum_presence==100&&e.drum_density==8);
  assert(!memcmp(home,e.motif_home,16));
  memset(e.scheduled,0,sizeof e.scheduled);memset(e.ornaments,0,sizeof e.ornaments);
  e.frame_count+=e.tick_length;frames+=e.tick_length;
 }
 assert(frames>(uint64_t)SR_RATE*3600*8&&cycles>10&&returns>10&&moves>100);
 printf("PASS %.2f simulated hours; %u committed transitions, %u complete arc renewals, %u literal theme returns; no transport stop, tempo jump or drum-control mutation. Event scheduling, not physical audio soak.\n",(double)frames/SR_RATE/3600,moves,cycles,returns);
}
