#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e[2];static int16_t pcm[2][512];
int main(void){
 for(unsigned kit=0;kit<19;kit++){
  sr_init(e,732);e->drum_kit=kit;e->solo=7;e->drum_level=1;sr_start(e);e->external_sync=1;e->tick_phase=1;e->kick_env=1;
  double power=0;unsigned peak=0,front=0;
  for(unsigned b=0;b<44;b++){sr_render(e,pcm[0],256);for(unsigned i=0;i<512;i++){unsigned a=abs(pcm[0][i]);if(a>peak)peak=a;if(b<2&&a>front)front=a;power+=(double)a*a;}}
  assert(peak>128&&peak<32730&&front>64&&power>10000);
  printf("KIT %u peak %u attack %u rms %.2f\n",kit,peak,front,sqrt(power/(44*512)));
 }
 for(unsigned role_index=0;role_index<5;role_index++)for(unsigned fx=0;fx<5;fx++){
  unsigned roles[]={1,2,4,7,8},role=roles[role_index];double delta=0,power[2]={0};unsigned peak=0;
  for(unsigned v=0;v<2;v++){
   sr_init(&e[v],732);sr_set_scene(&e[v],7);e[v].solo=role;e[v].air_track=role;e[v].air_fx=fx;e[v].air_depth=100;e[v].air_target=(float)v;
   sr_start(&e[v]);e[v].freeze=1;for(unsigned t=0;t<8;t++)e[v].role_target[t]=1;
  }
  for(unsigned b=0;b<SR_RATE*8u/256u;b++){
   for(unsigned v=0;v<2;v++)sr_render(&e[v],pcm[v],256);
   if(b<SR_RATE/256u)continue;
   for(unsigned i=0;i<512;i++){
    double d=(double)pcm[1][i]-pcm[0][i];delta+=d*d;
    for(unsigned v=0;v<2;v++){double a=pcm[v][i];power[v]+=a*a;if(abs(pcm[v][i])>peak)peak=abs(pcm[v][i]);}
   }
  }
  assert(delta>100000&&power[0]>100000&&power[1]>100000&&peak<32730);
  printf("AIR role%u fx%u delta_ratio %.5f wet_dry_db %.3f peak %u\n",role,fx,sqrt(delta/power[0]),10*log10(power[1]/power[0]),peak);
 }
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<3;mode++)for(unsigned length=16;length<=64;length*=2){
  sr_init(e,732);sr_set_scene(e,scene);sr_set_music_mode(e,mode);e->loop_length=length;e->rhythm_style=(scene+mode+length/16u)%3;e->chord_mode=2;e->fill_character=2;
  sr_start(e);e->tick_length=256;e->tick_phase=256;
  for(unsigned b=0;b<length*4;b++){sr_render(e,pcm[0],256);for(unsigned i=0;i<512;i++)assert(abs(pcm[0][i])<32730);}
  e->air_request=3;sr_stop(e);assert(!e->air_request);for(unsigned i=0;i<12;i++)assert(!e->scheduled[i].active);
 }
 puts("PASS 19 isolated synthesized kicks; 25 real dry/wet target/effect PCM pairs; 189 intent/mode/length rhythm-strum-fill profiles and STOP cancellation; no digital ceiling hits");
}
