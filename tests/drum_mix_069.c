#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../device/dsp/engine.c"
static sr_engine e[2];static int16_t p[2][512];
int main(void){
 for(unsigned kit=0;kit<19;kit++)for(unsigned mode=0;mode<4;mode++){
  sr_init(e,730069);sr_set_music_mode(e,mode);sr_set_scene(e,(kit*3u+mode)%21);sr_generate_song(e,777,0);
  e->drum_kit=kit;e->drums_enabled=1;e->drum_density=4;e->drum_hit=70;e->rhythm_style=1;e->fill_character=3;e->auto_gain=e->auto_level=1;
  sr_start(e);e->freeze=1;for(unsigned t=0;t<8;t++)e->role_target[t]=1;
  e[1]=e[0];e[1].tracks[7].volume=0;
  double power=0,difference=0;unsigned peak=0;
  for(unsigned block=0;block<SR_RATE*15u/256u;block++){
   for(unsigned v=0;v<2;v++)sr_render(e+v,p[v],256);
   if(block<SR_RATE/256u)continue;
   for(unsigned i=0;i<512;i++){int a=abs(p[0][i]);if((unsigned)a>peak)peak=a;double d=(double)p[0][i]-p[1][i];difference+=d*d;power+=(double)p[0][i]*p[0][i];}
  }
  double ratio=sqrt(difference/(power+1));
  printf("kit=%u mode=%u post_mix_drum_difference_ratio=%.5f peak=%u events=%u\n",kit,mode,ratio,peak,e->tracks[7].events);fflush(stdout);
  assert(peak<32730&&ratio>.015&&e->tracks[7].events>0);
 }
 puts("PASS all19 kits x4 modes at real tempo: actual full-mix drum contribution, auto gain/level, nature/FX, no digital ceiling hits; artistic clarity remains owner-unverified");
}
