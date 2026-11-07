#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#define FM1B_CPU2 0
#define SR_TRACKS 8
#define fm_sinf sinf
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float a,float b){return fminf(fmaxf(x,a),b);}
static unsigned plat_ticks24(void){return 0;}
#include "visual-kernel.inc"
#include "original-reference.inc"
int main(void){
 visual_init();memset(&vis_input,0,sizeof vis_input);vis_input.water=vis_input.full=1;vis_input.frequency=528;vis_input.pitch=110;vis_input.meter[0]=.08;
 water_m=2+((110.f/450+528.f/1600)*.3f);water_n=3+((110.f/450+528.f/1600)*.6f);
 resonance_job();float before=0,after=0;for(unsigned i=0;i<512;i++)before+=fabsf(sand_field(sand_x[i],sand_y[i]));
 for(unsigned f=0;f<1200;f++){vis_input.phase++;resonance_job();}
 for(unsigned i=0;i<512;i++)after+=fabsf(sand_field(sand_x[i],sand_y[i]));
 printf("nodal residual: %f -> %f\n",before/512,after/512);assert(after<before*.8f);
 float low=water_m;vis_input.pitch=1760;for(unsigned f=0;f<300;f++){vis_input.phase++;resonance_job();}assert(water_m>low+.5f);
 memset(vis_input.meter,0,sizeof vis_input.meter);for(unsigned f=0;f<200;f++){vis_input.phase++;resonance_job();}assert(nodal_level<.00001f);for(unsigned y=0;y<30;y++)for(unsigned x=0;x<30;x++)assert(!vis_output.water_light[y][x]);
 for(unsigned full=0;full<2;full++)for(unsigned phase=0;phase<1024;phase+=7){
  vis_input.water=0;vis_input.full=full;vis_input.phase=phase;
  for(unsigned t=0;t<8;t++){vis_input.event[t]=phase/(t+1);vis_input.meter[t]=(t+1)*.01f;}
  visual_job(0);int16_t actual[8][33][2];memcpy(actual,vis_output.xy,sizeof actual);
  reference_original(0);assert(!memcmp(actual,vis_output.xy,sizeof actual));
 }
 puts("PASS original 33-vertex geometry exactly matches pinned 128662b across 294 normal/fullscreen snapshots");
 puts("PASS sand converges to nodes, higher pitch increases mode order, silence quiets water");
}
