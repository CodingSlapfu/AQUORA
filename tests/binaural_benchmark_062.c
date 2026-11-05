/* Host-only matched profile OFF/ON/OFF benchmark; NOT FM-1 hardware timing. */
#define _POSIX_C_SOURCE 200809L
#include "serenity.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#define BLOCKS 1200
static sr_engine e;
static int16_t pcm[512];
static uint64_t nanoseconds(void){
  struct timespec t; clock_gettime(CLOCK_THREAD_CPUTIME_ID,&t);
  return (uint64_t)t.tv_sec*1000000000ull+t.tv_nsec;
}
static void setup(void){
  sr_init(&e,731u);
  sr_generate_song(&e,731u,0);
  e.drums_enabled=1;e.drum_density=4;
  e.bed_type=4;e.bed_level=.35f;
  e.reverb_amount=.4f;e.delay_amount=.2f;
  e.binaural_level=.18f;
  for(unsigned r=0;r<SR_DRUMS;r++)e.tracks[r].patch=24u+(r%6u);
  sr_start(&e);
}
static double measure(unsigned active){
  setup();
  e.binaural_enabled=active;
  for(unsigned b=0;b<250;b++)sr_render(&e,pcm,256);
  uint64_t elapsed=0;
  volatile uint64_t checksum=0;
  for(unsigned b=0;b<BLOCKS;b++){
     uint64_t before=nanoseconds();sr_render(&e,pcm,256);
     elapsed+=nanoseconds()-before;
     checksum+=(unsigned)(uint16_t)pcm[(b%256)*2];
  }
  if(checksum==0x123456789u)puts("unlikely");
  return (double)elapsed/(1000.0*BLOCKS);
}
int main(void){
  double off1=measure(0),on=measure(1),off2=measure(0);
  printf("host matched DSP OFF1 %.2f us / ON %.2f us / OFF2 %.2f us per 256 frames\n",off1,on,off2);
  printf("host binaural ON increment %.2f us (not device timing)\n",on-(off1+off2)*.5);
  return 0;
}
