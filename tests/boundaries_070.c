#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../device/dsp/engine.c"
static sr_engine e;static int16_t pcm[512];
int main(void){unsigned windows=0,peak=0;double minpost=1e30;
 for(unsigned mode=0;mode<4;mode++)for(unsigned kit=0;kit<19;kit++)for(unsigned section=0;section<32;section++){
  sr_init(&e,777);sr_set_music_mode(&e,mode);sr_start(&e);e.solo=7;e.drum_kit=kit;e.tick_length=5513;e.loop_length=16;
  e.form_loop=2u+sr_scenes[e.scene].energy%3u-1u;e.loop_pos=15;e.sequence_step=63;
  unsigned next=sr_sections[section].family==3u?28u:sr_sections[section].family==2u?10u:12u;
  e.form_route[0]=section;e.form_route[1]=next;plan_roles(&e);e.tick_phase=e.tick_length;
  double pre=0,post=0;unsigned remain=e.tick_length*5u;
  while(remain){unsigned n=remain>256?256:remain;sr_render(&e,pcm,n);
   for(unsigned j=0;j<n*2u;j++){unsigned a=abs(pcm[j]);if(a>peak)peak=a;assert(a<32730);if(e.sequence_step<65)pre+=(double)a*a;else post+=(double)a*a;}remain-=n;}
  assert(e.running&&form_index(&e,0)==next&&post>100000&&e.drum_density==8&&e.drum_level==1.f);
  if(post<minpost)minpost=post;windows++;
 }
 printf("PASS %u real PCM section-boundary windows across 32 functions x 19 kits x 4 modes; no unexpected post-boundary drum silence; peak %u; minimum post energy %.0f. No audio files written.\n",windows,peak,minpost);
}
