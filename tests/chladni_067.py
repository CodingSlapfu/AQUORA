from pathlib import Path
import subprocess,sys
r=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(exist_ok=True,parents=True)
s=(r/'device/visual_worker.c').read_text();(out/'kernel.inc').write_text(s[:s.index('static void prepare_geometry')])
c='''#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define FM1B_CPU2 0
#define SR_TRACKS 8
#define fm_sinf sinf
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float a,float b){return fminf(fmaxf(x,a),b);}
static unsigned plat_ticks24(void){return 0;}
#include "kernel.inc"
int main(void){visual_init();vis_input.water=2;vis_input.full=1;vis_input.pitch=110;vis_input.frequency=528;vis_input.meter[0]=.08;
visual_job(0);for(unsigned i=0;i<200;i++){vis_input.phase++;visual_job(0);}
unsigned seed=0x43484c41u;for(unsigned i=0;i<512;i++){
seed=seed*1664525u+1013904223u;sand_x[i]=(seed>>8)*(2.f/16777216.f)-1.f;
seed=seed*1664525u+1013904223u;sand_y[i]=(seed>>8)*(2.f/16777216.f)-1.f;sand_vx[i]=sand_vy[i]=0;}
float before=0,after=0;for(unsigned i=0;i<512;i++)before+=fabsf(plate_sample(sand_x[i],sand_y[i]));
for(unsigned i=0;i<1200;i++){vis_input.phase++;visual_job(0);assert(vis_output.node_count<=512);}
for(unsigned i=0;i<512;i++)after+=fabsf(plate_sample(sand_x[i],sand_y[i]));printf("Nodal residual %.6f -> %.6f, %u segments\\n",before/512,after/512,vis_output.node_count);assert(after<before*.8f);assert(vis_output.node_count>32);
float low=plate_order;vis_input.pitch=1760;for(unsigned i=0;i<300;i++){vis_input.phase++;visual_job(0);}assert(plate_order>low+3);
memset(vis_input.meter,0,sizeof vis_input.meter);for(unsigned i=0;i<300;i++){vis_input.phase++;visual_job(0);}assert(plate_activity<.00001f);
float prior=sand_x[0];for(unsigned i=0;i<100;i++){vis_input.phase++;visual_job(0);}assert(fabsf(prior-sand_x[0])<.0001f);
for(unsigned i=0;i<512;i++){assert(vis_output.sand[i][0]>=0&&vis_output.sand[i][0]<240);assert(vis_output.sand[i][1]>=0&&vis_output.sand[i][1]<240);}
puts("PASS actual Chladni modal geometry, nodal convergence, pitch response, bounds and silent settling");}
'''
p=out/'chladni.c';p.write_text(c);subprocess.run(['gcc','-O2',str(p),'-lm','-o',str(out/'chladni')],check=True);subprocess.run([str(out/'chladni')],check=True)
