#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
extern void old_sr_init(sr_engine*,uint32_t),old_sr_render(sr_engine*,int16_t*,size_t),old_sr_start(sr_engine*),old_sr_set_scene(sr_engine*,uint8_t),old_sr_set_music_mode(sr_engine*,uint8_t);
static sr_engine a,b;static int16_t x[512],y[512];
int main(void){double power=0,error=0;unsigned max=0;
 for(unsigned mode=0;mode<3;mode++)for(unsigned scene=0;scene<13;scene++){
  sr_init(&a,6501);old_sr_init(&b,6501);sr_set_music_mode(&a,mode);old_sr_set_music_mode(&b,mode);sr_set_scene(&a,scene);old_sr_set_scene(&b,scene);
  a.auto_gain=b.auto_gain=1;sr_start(&a);old_sr_start(&b);
  for(unsigned block=0;block<800;block++){
   sr_render(&a,x,256);old_sr_render(&b,y,256);
   for(unsigned i=0;i<512;i++){int diff=x[i]-y[i];unsigned d=diff<0?-diff:diff;if(d>max)max=d;power+=(double)y[i]*y[i];error+=(double)diff*diff;}
  }
  assert(a.rng==b.rng&&a.sequence_step==b.sequence_step);
 }
 double snr=10*log10(power/(error+1e-9));printf("39 same-seed/intention/mode PCM comparisons, SNR %.2fdB, maximum difference %u/32768; clocks/RNG unchanged\n",snr,max);assert(snr>60&&max<32);
}
