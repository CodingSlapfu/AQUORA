/* Actual shared kit kernel and full signal path, compatible with 0.6.1 source. */
#include "../device/dsp/engine.c"
#include <stdio.h>
#include <stdlib.h>
static sr_engine e;static int16_t pcm[512];
int main(int argc,char **argv){
 if(argc!=4)return 2;unsigned kit=atoi(argv[1]),part=atoi(argv[2]),scene=atoi(argv[3]);
 if(kit>=SR_DRUM_KITS||part>4||scene>=SR_SCENES)return 2;
 sr_init(&e,412);sr_set_scene(&e,scene);sr_start(&e);
 e.bed_type=0;e.bed_level=0;e.drum_kit=kit;e.drums_enabled=1;
 e.drum_density=8;e.drum_hit=95;e.drum_level=1;
 e.reverb_amount=part==4?.32f:0;e.delay_amount=part==4?.18f:0;
 if(part<3){
   e.kick_env=part==0;e.hand_env=part==1;e.shaker_env=part==2;
   for(unsigned n=0;n<SR_RATE/4;n++){
     int16_t v=sampleout(drum_sample(&e));
     if(fwrite(&v,sizeof v,1,stdout)!=1)return 1;
   }
 }else{
   if(part==3)e.isolate=SR_ISOLATE_TRACK1+SR_DRUMS;
   for(unsigned block=0;block<3*SR_RATE/256;block++){
     sr_render(&e,pcm,256);
     if(fwrite(pcm,sizeof pcm,1,stdout)!=1)return 1;
   }
 }
 return 0;
}
