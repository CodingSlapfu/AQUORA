#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "engine.c"

static sr_engine e;
int main(int argc,char **argv)
{
 if(argc!=2)return 2;
 const unsigned scenes[]={4,9,14}, seeds[]={725,744,763};
 puts("mode,scene,seed,dry,starts,large_leaps,short_starts,register_outside,active_blocks,longest_run,word_starts,clipped");
 for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
   for(unsigned ix=0;ix<3;ix++){
     for(unsigned dry=0;dry<2;dry++){
       sr_init(&e,seeds[ix]);
       e.music_mode=mode;
       sr_set_scene(&e,scenes[ix]);
       sr_generate_song(&e,seeds[ix],0);
       e.tick_length=SR_RATE*15u/72u;
       e.tonic_override=48;
       e.master=.72f;
       e.vocals_enabled=1;
       e.vocal_presence=4;
       e.vocal_fx_manual=7;
       e.vocal_delay=e.vocal_chorus=e.vocal_reverb=e.vocal_tune=0;
       e.isolate=dry?SR_ISOLATE_TRACK1+SR_CHOIR:SR_ISOLATE_MIX;
       sr_start(&e);
       FILE *file=NULL;
       if(mode==SR_CINEMATIC&&ix==0){
         char name[512];
         snprintf(name,sizeof name,"%s/cinema-%s.pcm",argv[1],dry?"dry":"full");
         file=fopen(name,"wb"); if(!file)return 1;
       }
       unsigned starts=0,leaps=0,shorts=0,outside=0,activity=0,run=0,best=0,words=0,clip=0;
       int previous=-1;
       uint32_t born[SR_VOICES];
       for(unsigned j=0;j<SR_VOICES;j++)born[j]=UINT32_MAX;
       for(unsigned block=0;block<14400;block++){
         int16_t pcm[512];
         sr_render(&e,pcm,256);
         if(file&&fwrite(pcm,sizeof pcm,1,file)!=1)return 1;
         if(e.vocal_direct_peak>.002f){activity++; if(++run>best)best=run;}
         else run=0;
         for(unsigned j=0;j<SR_VOICES;j++){
           const sr_voice *voice=&e.voices[j];
           if(!voice->active||!voice->sample_zone||voice->born==born[j])continue;
           born[j]=voice->born;
           starts++;
           if(previous>=0&&abs(voice->midi-previous)>5)leaps++;
           previous=voice->midi;
           shorts+=voice->remain<SR_RATE/4u;
           outside+=voice->midi<62u||voice->midi>77u;
           words+=voice->sample_zone==VOCAL_DAY_ZONE+1u||voice->sample_zone==VOCAL_YOU_ZONE+1u||
                  voice->sample_zone>=VOCAL_LOVE_ZONE+1u;
         }
         for(unsigned j=0;j<512;j++)clip+=pcm[j]==32767||pcm[j]==-32768;
       }
       if(file)fclose(file);
       printf("%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n", mode,scenes[ix],seeds[ix],dry,starts,leaps,shorts,outside,activity,best,words,clip);
     }
   }
 }
}
