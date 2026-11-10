/* Actual post-gain selected-role PCM, with normal arrangement gating (no SOLO). */
#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
static sr_engine e;static int16_t pcm[512];
int main(int argc,char **argv){
 assert(argc==2);unsigned supported=0;
 for(unsigned mode=0;mode<3;mode++)for(unsigned role=0;role<8;role++){
  sr_init(&e,0x1234+mode*317);sr_set_music_mode(&e,mode);sr_set_scene(&e,mode*3);
  sr_set_loop_length(&e,32);e.variation=1;e.scope_track=role;sr_start(&e);
  char path[512];snprintf(path,sizeof path,"%s/mode%u-role%u.pcm",argv[1],mode,role);FILE *f=fopen(path,"wb");assert(f);
  double power=0;unsigned peak=0,changing=0,previous=99,heard=0;
  for(unsigned block=0;block<(SR_RATE*180u/256u);block++){
   unsigned pos=e.scope_write,phrase=e.phrase;sr_render(&e,pcm,256);
   for(unsigned j=0;j<64;j++){int sample=e.scope[(pos+j)&127];unsigned a=abs(sample);power+=(double)sample*sample;if(a>peak)peak=a;if(a>20)heard++;if(block<SR_RATE*16u/256u){int16_t s=sample;assert(fwrite(&s,2,1,f)==1);}}
   unsigned mask=0;for(unsigned t=0;t<8;t++)if(e.role_target[t]>.5f)mask|=1u<<t;
   if(previous!=99&&mask!=previous)changing++;previous=mask;
   if(e.phrase!=phrase)printf("STEM %u %u phrase%u target%.2f gain%.4f meter%.6f events%u\n",mode,role,e.phrase,e.role_target[role],e.role_gain[role],e.tracks[role].meter,e.tracks[role].events);
  }
  fclose(f);if(heard>64)supported|=1u<<role;assert(changing>0);assert(isfinite(power));
  printf("SUMMARY mode%u role%u peak%u nonquiet%u power%.0f changes%u\n",mode,role,peak,heard,power,changing);
  sr_stop(&e);for(unsigned n=0;n<100;n++)sr_render(&e,pcm,256);for(unsigned i=0;i<512;i++)assert(!pcm[i]);
 }
 printf("PASS 24 normal-gated real PCM stems, 180-second musical evolution, role-set changes, STOP silence; audible-capability mask %u (no per-song role quota)\n",supported);
}
