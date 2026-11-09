#include <stdio.h>
#include <assert.h>
#include ENGINE_FILE
static sr_engine e;
int main(void){
 unsigned count=0,sum=0,max=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++){
  sr_init(&e,12345);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,777,0);sr_start(&e);
  for(unsigned n=0;n<32;n++){
   e.loop_pos=0;e.sequence_step=e.loop_length*(n+1);e.evolve_next=1;trigger(&e);
   unsigned delta=0;for(unsigned i=0;i<16;i++)delta+=e.melody[i]!=e.motif_home[i];sum+=delta;if(delta>max)max=delta;count++;
  }
 }
 printf("{\"evolutions\":%u,\"mean_changed_theme_steps\":%.4f,\"max_changed_theme_steps\":%u}\n",count,(double)sum/count,max);
}
