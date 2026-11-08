from pathlib import Path
import sys,subprocess,json
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[2]);out.mkdir(exist_ok=True,parents=True)
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
int main(void){visual_init();vis_input.water=2;vis_input.full=1;vis_input.frequency=528;vis_input.pitch=220;
 for(unsigned n=0;n<300;n++){vis_input.phase=n;chladni_job();}
 for(unsigned frame=0;frame<240;frame++){
  for(unsigned t=0;t<8;t++){vis_input.meter[t]=frame<120?.002f*(t+1):0;vis_input.onset[t]=frame<120&&frame%16==t?1:0;}
  vis_input.phase=300+frame;chladni_job();
  printf("%u",frame);for(unsigned j=0;j<25;j++)for(unsigned i=0;i<25;i++)printf(",%.7f",plate_grid[j][i]);puts("");
 }
}
'''
rows={}
for name,p in [('baseline',Path(sys.argv[1])),('candidate',root/'device/visual_worker.c')]:
 d=out/name;d.mkdir(exist_ok=True);s=p.read_text();(d/'kernel.inc').write_text(s[:s.index('static void prepare_geometry')]);(d/'test.c').write_text(c)
 subprocess.run(['cc','-O2',str(d/'test.c'),'-lm','-o',str(d/'test')],check=True)
 lines=subprocess.check_output([str(d/'test')],text=True).splitlines();a=[[float(v) for v in line.split(',')[1:]] for line in lines]
 def motion(start,end):return sum(sum(abs(x-y) for x,y in zip(a[f],a[f-1]))/625 for f in range(start,end))/(end-start)
 rows[name]={'music_field_motion':motion(1,120),'settled_field_motion':motion(220,240)}
assert rows['candidate']['music_field_motion']>rows['baseline']['music_field_motion']*2.5
assert rows['candidate']['settled_field_motion']<.00001
(out/'metrics.json').write_text(json.dumps(rows,indent=2));print(rows)
print('PASS fixed pitch/seed: music causes strong nodal-field movement; silence settles. Field metric is not owner perceptual acceptance.')
