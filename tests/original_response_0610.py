from pathlib import Path
import subprocess,sys,json
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
c=r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define FM1B_CPU2 0
#define SR_TRACKS 8
#define SR_BASS 1
#define fm_sinf sinf
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float a,float b){return fminf(fmaxf(x,a),b);}
static unsigned plat_ticks24(void){return 0;}
#include "kernel.inc"
static int16_t base[8][33][2];
int main(void){visual_init();vis_input.full=1;double sum=0;
 for(unsigned p=0;p<128;p+=7){memset(vis_input.meter,0,sizeof vis_input.meter);vis_input.phase=p;visual_job(0);memcpy(base,vis_output.xy,sizeof base);
 for(unsigned t=0;t<8;t++)vis_input.meter[t]=.05f;visual_job(0);
 for(unsigned t=0;t<8;t++)for(unsigned n=0;n<33;n++)for(unsigned k=0;k<2;k++)sum+=abs(vis_output.xy[t][n][k]-base[t][n][k]);}
 printf("%.0f\n",sum);}
'''
r={}
for label,path in [('baseline',Path(sys.argv[1])),('candidate',root/'device/visual_worker.c')]:
 d=out/label;d.mkdir(exist_ok=True);s=path.read_text();(d/'kernel.inc').write_text(s[:s.index('static void prepare_geometry')]);(d/'probe.c').write_text(c)
 subprocess.run(['cc','-O2',str(d/'probe.c'),'-lm','-o',str(d/'probe')],check=True);r[label]=float(subprocess.check_output([str(d/'probe')],text=True))
r['ratio']=r['candidate']/r['baseline'];assert 1.04<r['ratio']<1.2,r
(out/'metrics.json').write_text(json.dumps(r,indent=2));print('PASS Original subtle source-to-vertex activity boost:',r)
