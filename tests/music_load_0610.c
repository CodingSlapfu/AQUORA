#define _POSIX_C_SOURCE 200809L
#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
static sr_engine e;static int16_t pcm[512];static double times[600];
static double us(void){struct timespec t;clock_gettime(CLOCK_THREAD_CPUTIME_ID,&t);return t.tv_sec*1e6+t.tv_nsec*.001;}
static int compare(const void *a,const void *b){double x=*(const double*)a,y=*(const double*)b;return (x>y)-(x<y);}
int main(void){
 for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,641);sr_set_music_mode(&e,mode);sr_start(&e);
  unsigned peak=0,voices=0;
  for(unsigned block=0;block<600;block++){
   if(block%45==0){
    sr_set_scene(&e,(block/45)%21);e.binaural_enabled=1;e.bed_type=4;e.bed_level=.9f;
    e.drum_density=8;e.drum_hit=100;e.reverb_amount=.8f;e.delay_amount=.7f;
    for(unsigned i=0;i<16;i++)sr_note_on(&e,i%7,48+i%7,80,8.f);
   }
   unsigned active=0;for(unsigned i=0;i<SR_VOICES;i++)active+=e.voices[i].active; if(active>voices)voices=active;
   double begin=us();sr_render(&e,pcm,256);times[block]=us()-begin;
   for(unsigned i=0;i<512;i++){unsigned p=abs(pcm[i]);if(p>peak)peak=p;}
  }
  assert(voices==16&&peak>100&&peak<32767);qsort(times,600,sizeof(double),compare);
  printf("MODE %u: 16 voices + density8/HIT100 + stereo binaural + Forest + wet FX + 21-intention rotation; peak %u; host callback p95 %.1fus p99 %.1fus max %.1fus (not FM-1 timing)\n",mode,peak,times[569],times[593],times[599]);
  sr_stop(&e);for(unsigned b=0;b<250;b++)sr_render(&e,pcm,256);assert(!e.running);for(unsigned i=0;i<SR_VOICES;i++)assert(!e.voices[i].gate);
 }
}
