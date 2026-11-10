#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
int main(void){
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++)for(unsigned length=16;length<=64;length*=2){
  sr_init(&e,72);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,811,0);e.loop_length=length;sr_start(&e);
  unsigned original=sr_arrange_section(&e,1);sr_arrange_queue(&e,12,1);assert(sr_arrange_section(&e,1)==12);sr_arrange_auto(&e);assert(sr_arrange_section(&e,1)==original);
  sr_arrange_queue(&e,17,3);unsigned ticks=0;while(!e.arrange_active&&ticks++<300){trigger(&e);memset(e.scheduled,0,sizeof e.scheduled);}
  assert(e.arrange_active==3&&sr_arrange_section(&e,0)==17&&!e.arrange_next);
  unsigned n=0;while(e.arrange_active&&n++<length*4){trigger(&e);memset(e.scheduled,0,sizeof e.scheduled);}
  assert(n==3*length&&!e.arrange_active);
 }
 puts("PASS 252 intention/mode/loop ARRANGE queue/cancel/boundary/duration cases");
}
