#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
static sr_engine e;
static int16_t pcm[512];
static const int scales[5][7]={{0,2,4,7,9,12,14},{0,2,3,5,7,10,12},{0,3,5,7,10,12,15},{0,2,5,7,9,12,14},{0,2,4,5,7,9,11}};
static unsigned events(void){
 unsigned hash=2166136261u;
 sr_start(&e);
 for(unsigned block=0;block<480;block++){
  sr_render(&e,pcm,256);
  for(unsigned t=0;t<8;t++)hash=(hash^e.tracks[t].events)*16777619u;
  for(unsigned v=0;v<SR_VOICES;v++)if(e.voices[v].active){
   int pitch=((int)e.voices[v].midi-(int)sr_tonic(&e)+120)%12;unsigned fits=0;
   for(unsigned i=0;i<7;i++)fits|=pitch==scales[sr_scenes[e.scene].scale][i]%12;
   assert(fits);hash=(hash^e.voices[v].midi)*16777619u;
  }
  for(unsigned i=0;i<512;i++)assert(pcm[i]>-32768);
 }
 sr_stop(&e);for(unsigned i=0;i<32;i++)sr_render(&e,pcm,256);
 assert(!e.running);return hash;
}
int main(void){
 unsigned count=0;
 for(unsigned mode=0;mode<3;mode++)for(unsigned scene=0;scene<13;scene++)for(unsigned seed=1;seed<=4;seed++)for(unsigned length=16;length<=64;length*=2){
  sr_init(&e,seed*719);sr_set_music_mode(&e,mode);sr_set_loop_length(&e,length);
  unsigned oldpatch[8]={0},oldhash=0;int oldmotif[16]={0};
  for(unsigned visit=0;visit<3;visit++){
   sr_set_scene(&e,(scene+1)%13);sr_set_scene(&e,scene);
   assert(e.frequency==sr_scenes[scene].frequency&&e.music_mode==mode);
   for(unsigned t=0;t<8;t++){
    unsigned patch=t==7?e.drum_kit:e.tracks[t].patch;
    if(visit)assert(patch!=oldpatch[t]);
    oldpatch[t]=patch;
   }
   unsigned changed=0;for(unsigned i=0;i<16;i++){changed|=oldmotif[i]!=e.melody[i];oldmotif[i]=e.melody[i];}
   if(visit)assert(changed);
   assert(e.motion[0].glide_ms<=800&&e.motion[1].vibrato_depth<=50);
   if(mode==2)assert(!e.motion[0].glide_mode&&!e.motion[1].glide_mode);
   unsigned hash=events();if(visit)assert(hash!=oldhash);oldhash=hash;count++;
  }
 }
 for(unsigned mode=0;mode<3;mode++)for(unsigned scene=0;scene<13;scene++){
  sr_init(&e,144);e.sound_locks=1u<<SR_LEAD|SR_SOUND_NATURE|SR_SOUND_FX;
  e.bed_type=0;e.reverb_amount=.23f;e.delay_amount=.11f;e.tracks[0].patch=12;e.tracks[2].mute=1;e.solo=4;
  e.motion[0]=(sr_motion){333,2,17,61};sr_motion_auto(&e,0,0);
  sr_set_music_mode(&e,mode);sr_set_scene(&e,scene);
  assert(e.tracks[0].patch==12&&!e.bed_type&&e.reverb_amount==.23f&&e.delay_amount==.11f&&e.tracks[2].mute&&e.solo==4);
  assert(e.motion[0].glide_ms==333&&e.motion[0].vibrato_depth==17);
  sr_motion_auto(&e,0,1);assert(!(e.motion_manual&1)&&e.motion[0].glide_ms!=333);
  sr_start(&e);for(unsigned i=0;i<200;i++){if(i%13==0){sr_set_scene(&e,(e.scene+1)%13);sr_set_music_mode(&e,(e.music_mode+1)%3);}sr_render(&e,pcm,256);}
  sr_stop(&e);for(unsigned i=0;i<200;i++)sr_render(&e,pcm,256);
  assert(!e.running);for(unsigned i=0;i<SR_VOICES;i++)assert(!e.voices[i].gate);
 }
 printf("PASS %u realizations: 13 intentions x 3 modes x 4 seeds x 16/32/64 x 3 revisits; actual patches/motifs/events, scales, motion AUTO/MANUAL, locks, live transitions\n",count);
}
