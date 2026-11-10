#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../device/dsp/engine.c"
static sr_engine e;static int16_t pcm[512];
int main(void){
 uint32_t seen=0;double worst=0;
 for(unsigned scene=0;scene<21;scene++){
  unsigned f=sr_scenes[scene].frequency;assert(f<21&&!(seen&(1u<<f)));seen|=1u<<f;
  sr_init(&e,42);sr_set_scene(&e,scene);assert(e.frequency==f&&e.tone_level>0);
  sr_start(&e);e.isolate=SR_ISOLATE_FIXED;e.reverb_amount=e.delay_amount=0;e.binaural_enabled=0;
  for(unsigned n=0;n<256;n++)sr_render(&e,pcm,256);
  double first=-1,last=-1;unsigned crossings=0;float prior=0;unsigned at=0,peak=0;
  for(unsigned block=0;block<256;block++){
   sr_render(&e,pcm,256);
   for(unsigned n=0;n<256;n++,at++){
    float sample=pcm[n*2];assert(pcm[n*2]==pcm[n*2+1]);
    if(abs(pcm[n*2])>(int)peak)peak=abs(pcm[n*2]);
    if(prior<0&&sample>=0){double time=at-sample/(sample-prior);if(first<0)first=time;last=time;crossings++;}
    prior=sample;
   }
  }
  assert(crossings>100&&peak>30);
  double hz=(crossings-1)*SR_RATE/(last-first),error=fabs(hz-sr_frequencies[f].hz);
  assert(error<.1);if(error>worst)worst=error;
  assert(e.reference_hz_now==sr_frequencies[f].hz);
  float before=e.reference_hz_now;sr_set_frequency(&e,(f+1)%21);assert(!e.frequency_auto);
  sr_set_scene(&e,(scene+1)%21);assert(e.frequency==(f+1)%21);
  sr_render(&e,pcm,1);assert(fabsf(e.reference_hz_now-before)<2.f);
  e.tone_level=0;for(unsigned n=0;n<128;n++)sr_render(&e,pcm,256);
  for(unsigned n=0;n<512;n++)assert(abs(pcm[n])<=1);
  sr_set_frequency_auto(&e,1);assert(e.frequency==sr_scenes[e.scene].frequency);
 }
 assert(seen==((1u<<21)-1));
 assert(sr_frequencies[sr_scenes[6].frequency].hz==852&&sr_frequencies[sr_scenes[7].frequency].hz==528);
 printf("PASS 21 unique AUTO mappings; emitted stereo reference worst error %.6f Hz; manual precedence, gain/Hz ramps, zero level and stable anchors\n",worst);
}
