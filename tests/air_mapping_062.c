/* Actual DSP mapping: baseline preserved, bounded temporary FX offsets. */
#include "serenity.h"
#include <assert.h>
#include <stdio.h>
static sr_engine e;
static int16_t pcm[512];
static void setup(unsigned mode,unsigned depth){
 sr_init(&e,432);sr_start(&e);e.bed_type=0;e.bed_level=0;
 e.reverb_amount=e.delay_amount=0;e.air_fx=mode;e.air_depth=depth;
 e.air_target=1;e.air_track=SR_TRACKS;
 for(unsigned n=0;n<180;n++)sr_render(&e,pcm,256);
 assert(e.reverb_amount==0&&e.delay_amount==0);
}
int main(void){
 setup(0,100);assert(e.reverb_wet>.7f&&e.delay_wet==0);
 setup(1,100);assert(e.reverb_wet==0&&e.delay_wet>.6f);
 setup(2,100);assert(e.reverb_wet>.7f&&e.delay_wet>.6f);
 setup(2,0);assert(e.reverb_wet==0&&e.delay_wet==0);
 setup(2,50);assert(e.reverb_wet>.35f&&e.reverb_wet<.41f);
 assert(e.delay_wet>.3f&&e.delay_wet<.36f);
 e.air_target=0;for(unsigned n=0;n<300;n++)sr_render(&e,pcm,256);
 assert(e.reverb_wet<.0001f&&e.delay_wet<.0001f);
 puts("PASS AIR Reverb/Echo/combined, depth zero/half/full, neutral restoration");
}
