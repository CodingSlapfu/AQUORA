"""Compare actual candidate kernel with pinned release-067 kernel.
Run from reconstructed tree: python3 tests/visual_response_068.py BASELINE_WORKER OUTPUT.
"""
from pathlib import Path
import subprocess,sys,hashlib
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
c=r'''
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define FM1B_CPU2 0
#define SR_TRACKS 8
#define fm_sinf sinf
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float a,float b){return fminf(fmaxf(x,a),b);}
static unsigned plat_ticks24(void){return 0;}
#include "kernel.inc"
int main(void){
 visual_init();
 for(unsigned full=0;full<2;full++)for(unsigned phase=0;phase<1024;phase+=7){
  memset(&vis_input,0,sizeof vis_input);vis_input.full=full;vis_input.phase=phase;
  for(unsigned t=0;t<8;t++){vis_input.event[t]=phase/(t+1);vis_input.meter[t]=(t+1)*.01f;vis_input.onset[t]=t*.03f;}
  visual_job(0);for(unsigned t=0;t<8;t++)for(unsigned j=0;j<33;j++)printf("O %d %d\n",vis_output.xy[t][j][0],vis_output.xy[t][j][1]);
 }
 for(unsigned n=0;n<4;n++){
  float levels[]={.001f,.01f,.04f,.1f};memset(&vis_input,0,sizeof vis_input);
  vis_input.water=1;vis_input.frequency=528;vis_input.meter[0]=levels[n];nodal_level=0;resonance_job();
  printf("W %u %.8f\n",n,nodal_level);
 }
}
'''
outputs=[]
for name,worker in [('baseline',Path(sys.argv[1])),('candidate',root/'device/visual_worker.c')]:
 d=out/name;d.mkdir(exist_ok=True)
 s=worker.read_text();(d/'kernel.inc').write_text(s[:s.index('static void prepare_geometry')]);(d/'test.c').write_text(c)
 subprocess.run(['cc','-O2',str(d/'test.c'),'-lm','-o',str(d/'test')],check=True)
 q=subprocess.check_output([str(d/'test')],text=True);(d/'output.txt').write_text(q);outputs.append(q.splitlines())
a,b=outputs
original_a=[x for x in a if x.startswith('O ')];original_b=[x for x in b if x.startswith('O ')]
assert original_a==original_b
wa=[float(x.split()[2]) for x in a if x.startswith('W ')];wb=[float(x.split()[2]) for x in b if x.startswith('W ')]
assert 2.4<wb[0]/wa[0]<2.5 and all(wb[i]<wb[i+1] for i in range(3))
print('PASS Original: 294 normal/fullscreen snapshots, all 77,616 vertices exactly equal to pinned 067')
print('PASS Cymatics low-level response ratio',wb[0]/wa[0],'; medium/high levels stay distinct:',wb)
