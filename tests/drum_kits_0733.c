#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine engine;

int main(int argc,char **argv)
{
 uint32_t fingerprints[SR_DRUM_KITS];
 for(unsigned kit=0;kit<SR_DRUM_KITS;kit++){
   sr_init(&engine,733);
   sr_start(&engine);
   engine.external_sync=1;
   engine.isolate=SR_ISOLATE_TRACK1+SR_DRUMS;
   engine.master=.8f;
   engine.drum_kit=kit;
   engine.drum_density=SR_DENSITY_MAX;
   char path[384];
   FILE *stem=0;
   if(argc>1){snprintf(path,sizeof path,"%s/kit-%02u.raw",argv[1],kit+1u);stem=fopen(path,"wb");assert(stem);}
   uint32_t hash=2166136261u;
   unsigned peak=0;
   for(unsigned lane=0;lane<SR_DRUM_PARTS;lane++){
     unsigned lane_peak=0;
     drum_strike(&engine,lane,100);
     for(unsigned block=0;block<24;block++){
       int16_t pcm[256];
       sr_render(&engine,pcm,128);
       if(stem)assert(fwrite(pcm,sizeof pcm,1,stem)==1);
       for(unsigned i=0;i<256;i++){
         hash=(hash^(uint16_t)pcm[i])*16777619u;
         unsigned mag=pcm[i]<0?-(int)pcm[i]:pcm[i];
         if(mag>lane_peak)lane_peak=mag;
       }
     }
     assert(lane_peak>50u&&lane_peak<32767u);
     if(lane_peak>peak)peak=lane_peak;
   }
   if(stem)fclose(stem);
   for(unsigned i=0;i<kit;i++)assert(fingerprints[i]!=hash);
   fingerprints[kit]=hash;
   printf("%u %-18s 16 lanes peak=%u signature=%08x\n",kit+1u,sr_drum_kits[kit].name,peak,hash);
 }
 unsigned styles=0,divergent=0;
 for(unsigned scene=0;scene<SR_SCENES;scene++){
   unsigned before=0;
   for(unsigned seed=0;seed<16;seed++){
     sr_init(&engine,seed+1u);
     engine.music_mode=sr_intention_style(scene,seed);
     styles|=1u<<engine.music_mode;
     sr_set_scene(&engine,scene);
     sr_generate_song(&engine,seed+1u,0);
     unsigned mask=0;
     for(unsigned step=0;step<16;step++)if(groove_kick(&engine,step)>0.f)mask|=1u<<step;
     if(seed&&mask!=before)divergent++;
     before=mask;
   }
 }
 fprintf(stderr,"styles=%u divergence=%u\n",styles,divergent);
 assert(styles==31u&&divergent>SR_SCENES*6u);
 printf("PASS all styles and %u neighboring-seed groove changes\n",divergent);
}
