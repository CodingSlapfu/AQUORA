#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
static sr_engine e;static int16_t pcm[512];
int main(int argc,char **argv){
 assert(argc==2);unsigned cases=0;
 const unsigned scenes[]={0,13,17,20};
 for(unsigned scene=0;scene<4;scene++)for(unsigned mode=0;mode<3;mode++)for(unsigned length=16;length<=64;length*=2){
  sr_init(&e,12345);sr_set_scene(&e,scenes[scene]);sr_set_music_mode(&e,mode);sr_generate_song(&e,777,0);sr_set_loop_length(&e,length);
  e.midi_enabled=1;e.variation=.6f;e.scope_track=SR_BASS;sr_start(&e);
  char path[512];snprintf(path,sizeof path,"%s/scene%u-mode%u-len%u.pcm",argv[1],scenes[scene],mode,length);FILE *audio=fopen(path,"wb");assert(audio);
  snprintf(path,sizeof path,"%s/scene%u-mode%u-len%u.csv",argv[1],scenes[scene],mode,length);FILE *log=fopen(path,"w");assert(log);fputs("frame,step,phrase,role,note,velocity,gain,meter\n",log);
  unsigned peak=0,heard=0,events=0;double power=0;
  for(unsigned block=0;block<SR_RATE*32u/256u;block++){
   sr_render(&e,pcm,256);fwrite(pcm,2,512,audio);
   for(unsigned i=0;i<512;i++){unsigned v=abs(pcm[i]);if(v>peak)peak=v;power+=(double)v*v;heard+=v>20;}
   while(e.midi_read!=e.midi_write){unsigned word=e.midi_events[e.midi_read];e.midi_read=(e.midi_read+1u)&63u;unsigned t=word>>16;
    fprintf(log,"%u,%u,%u,%u,%u,%u,%.5f,%.6f\n",e.frame_count,e.sequence_step,e.phrase,t,(word>>8)&255u,word&255u,e.role_gain[t],e.tracks[t].meter);events++;
   }
  }
  fclose(audio);fclose(log);assert(heard>SR_RATE&&peak<32767&&events>10&&!e.midi_event_lost&&isfinite(power));
  printf("AUDIO scene%u mode%u len%u peak%u rms%.1f events%u\n",scenes[scene],mode,length,peak,sqrt(power/(SR_RATE*32u*2u)),events);cases++;
 }
 printf("PASS %u same-seed 16/32/64 actual stereo mixes and complete note/gain timelines; no clipping or lost diagnostics\n",cases);
}
