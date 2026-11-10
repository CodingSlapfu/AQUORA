#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"
static sr_engine e,saved;
int main(void){
 unsigned transitions=0;unsigned bpm_sum[4]={0};
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++)for(unsigned density=0;density<2;density++){
  sr_init(&e,75);sr_set_music_mode(&e,mode);sr_set_scene(&e,scene);sr_generate_song(&e,1717+scene,0);e.energy=density;e.current_energy=density;sr_start(&e);
  unsigned bpm=(SR_RATE*15u+e.tick_length/2u)/e.tick_length;bpm_sum[mode]+=bpm;
  if(mode==SR_STANDARD)assert(bpm>=36&&bpm<=72);if(mode==SR_CINEMATIC)assert(bpm>=30&&bpm<=64);
  /* Section boundaries carry both FX rings. No DSP tail reset is allowed. */
  for(unsigned n=0;n<2048;n++){
   unsigned before=form_index(&e,0);e.reverb[0][7]=1234;e.delay[31]=-987;
   trigger(&e);memset(e.scheduled,0,sizeof e.scheduled);
   assert(e.reverb[0][7]==1234&&e.delay[31]==-987);
   if(form_index(&e,0)!=before)transitions++;
   assert(e.motif_count<=4);
  }
 }
 assert(transitions>1000&&bpm_sum[0]<bpm_sum[2]&&bpm_sum[1]<bpm_sum[3]);
 printf("PASS %u cross-boundary transitions across 4 modes/21 intentions/sparse+dense; FX rings preserved, bounded memory, mellow tempo distributions\n",transitions);
}
