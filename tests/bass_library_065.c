#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
static sr_engine e;static int16_t pcm[512];
int main(void){uint32_t hash[6];assert(SR_TRACKS==8&&SR_VOICES==16&&SR_PATCHES==36);
 for(unsigned patch=30;patch<36;patch++){
  sr_init(&e,6503);e.sound_locks=1u<<SR_BASS;e.tracks[SR_BASS].patch=patch;e.solo=SR_BASS;
  e.bed_type=0;e.bed_level=0;e.reverb_amount=e.delay_amount=0;e.sacred_mode=0;
  sr_start(&e);sr_note_on(&e,SR_BASS,36,100,1.f);
  uint32_t h=2166136261u;unsigned peak=0;
  for(unsigned b=0;b<500;b++){sr_render(&e,pcm,256);for(unsigned i=0;i<512;i++){unsigned p=pcm[i]<0?-pcm[i]:pcm[i];if(p>peak)peak=p;h=(h^(uint16_t)pcm[i])*16777619u;}}
  assert(peak>100&&peak<30000);for(unsigned old=0;old<patch-30;old++)assert(hash[old]!=h);hash[patch-30]=h;
  sr_stop(&e);for(unsigned b=0;b<100;b++)sr_render(&e,pcm,256);for(unsigned i=0;i<512;i++)assert(!pcm[i]);
  printf("PASS appended FM bass %u %s: distinct PCM peak %u, stop silence, existing pool\n",patch,sr_patches[patch].name,peak);
 }
}
