#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
int main(void){
 unsigned total=0;
 puts("scene,mode,distinct_patches,distinct_kits,maximum_patch_percent,kit_repeats");
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,724);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
  unsigned patches[48]={0},kits[19]={0},counts[7][48]={{0}},repeats=0,prior=99;
  for(unsigned seed=1;seed<=2048;seed++){
   sr_generate_song(&e,seed*197u+scene*41u,0);
   assert(e.auto_gain&&e.auto_level&&e.drum_density==8&&e.drum_presence==100&&e.tracks[7].volume==1.f);
   for(unsigned t=0;t<7;t++){patches[e.tracks[t].patch]++;counts[t][e.tracks[t].patch]++;}
   kits[e.drum_kit]++;repeats+=e.drum_kit==prior;prior=e.drum_kit;total++;
  }
  unsigned max=0;for(unsigned p=0;p<48;p++){assert(patches[p]);for(unsigned t=0;t<7;t++)if(counts[t][p]>max)max=counts[t][p];}
  for(unsigned k=0;k<19;k++)assert(kits[k]);assert(!repeats);assert(max<2048*85/100);
  printf("%u,%u,48,19,%u,0\n",scene,mode,max*100/2048);
  unsigned kit=e.drum_kit;uint8_t locked[7];for(unsigned t=0;t<7;t++)locked[t]=e.tracks[t].patch;
  e.sound_locks=1023;float delay=e.delay_amount;int8_t motif[16];memcpy(motif,e.melody,16);uint32_t rng0=e.rng;
  sr_generate_song(&e,3117,1);assert(e.rng==rng0&&!memcmp(motif,e.melody,16)&&delay==e.delay_amount&&kit==e.drum_kit);
  for(unsigned t=0;t<7;t++)assert(locked[t]==e.tracks[t].patch);
 }
 printf("PASS %u generated songs, full48/19 coverage for each21x4, no consecutive kit repeats, locks/reroll/defaults\n",total);
}
