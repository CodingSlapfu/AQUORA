"""Identical actual-DSP PCM drives baseline/candidate kernels. Host timing only."""
from pathlib import Path
import subprocess,sys,json
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
harness=r'''
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include ENGINE
#define FM1B_CPU2 0
#define fm_sinf sinf
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float a,float b){return fminf(fmaxf(x,a),b);}
static unsigned plat_ticks24(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return (unsigned)(t.tv_sec*24000000ull+t.tv_nsec*24u/1000u);}
#include "kernel.inc"
static sr_engine e;static int16_t pcm[4410],oldxy[512][2];static float oldgrid[625];
static uint32_t last[8];
int main(int argc,char**argv){unsigned mode=atoi(argv[1]),scene=atoi(argv[2]),style=atoi(argv[3]);
 sr_init(&e,730610);sr_set_music_mode(&e,style);sr_set_scene(&e,scene);sr_generate_song(&e,777,0);e.bed_level=0;e.tone_level=0;e.sacred_level=0;sr_start(&e);
 visual_init();vis_input.water=mode;vis_input.full=1;vis_input.frequency=528;
 double grain=0,field=0,quiet=0,total=0;unsigned peak=0,hash=2166136261u;float previous=0;
 for(unsigned frame=0;frame<640;frame++){
  if(frame==320)sr_stop(&e);
  sr_render(&e,pcm,2205);for(unsigned i=0;i<4410;i++)hash=(hash^(uint16_t)pcm[i])*16777619u;
  memset(vis_input.onset,0,sizeof vis_input.onset);
  for(unsigned t=0;t<8;t++){vis_input.meter[t]=e.tracks[t].meter;vis_input.onset[t]=e.tracks[t].events!=last[t]?1.f:0;last[t]=e.tracks[t].events;}
  float weight=0,pitch=0;previous=0;unsigned loud=0;for(unsigned i=0;i<16;i++)if(e.voices[i].active&&e.voices[i].gate){float w=e.voices[i].env*e.voices[i].peak*e.tracks[e.voices[i].track].volume;pitch+=e.voices[i].freq*w;weight+=w;if(w>previous){loud=e.voices[i].midi;previous=w;}}
  vis_input.pitch=weight>.00001f?pitch/weight:0;vis_input.scene=scene;vis_input.tonic=sr_tonic(&e);
#ifdef CANDIDATE
  vis_input.note_class=loud%12u;
#endif
  vis_input.phase=frame;
  unsigned begin=plat_ticks24();visual_job(0);unsigned us=(plat_ticks24()-begin)/24u;total+=us;if(us>peak)peak=us;
  double displacement=0,change=0;
  for(unsigned i=0;i<512;i++){displacement+=abs(vis_output.sand[i][0]-oldxy[i][0])+abs(vis_output.sand[i][1]-oldxy[i][1]);oldxy[i][0]=vis_output.sand[i][0];oldxy[i][1]=vis_output.sand[i][1];}
  if(mode==2)for(unsigned i=0;i<625;i++){float f=plate_grid[i/25][i%25];change+=fabsf(f-oldgrid[i]);oldgrid[i]=f;}
  if(frame>20&&frame<320){grain+=displacement/512.;field+=change/625.;}
  if(frame>600)quiet+=displacement/512.;
 }
 printf("%u %.9f %.9f %.9f %.2f %u\n",hash,grain/299.,field/299.,quiet/39.,total/640.,peak);
}
'''
rows=[]
for name,p in [('baseline',Path(sys.argv[1])),('candidate',root/'device/visual_worker.c')]:
 d=out/name;d.mkdir(exist_ok=True);s=p.read_text();(d/'kernel.inc').write_text(s[:s.index('static void prepare_geometry')]);(d/'probe.c').write_text(harness)
 subprocess.run(['cc','-O2','-I'+str(root/'device/dsp'),'-DENGINE='+json.dumps(str(root/'device/dsp/engine.c')), *(['-DCANDIDATE'] if name=='candidate' else []),str(d/'probe.c'),'-lm','-o',str(d/'probe')],check=True)
 for mode in (1,2):
  for scene,style in ((0,0),(4,2),(7,0),(19,3)):
   vals=subprocess.check_output([str(d/'probe'),str(mode),str(scene),str(style)],text=True).split()
   rows.append(dict(version=name,mode=mode,scene=scene,style=style,pcm_hash=int(vals[0]),grain_motion=float(vals[1]),field_motion=float(vals[2]),settled_grain=float(vals[3]),host_mean_us=float(vals[4]),host_peak_us=int(vals[5])))
for b in rows[:8]:
 c=next(x for x in rows[8:] if (x['mode'],x['scene'],x['style'])==(b['mode'],b['scene'],b['style']))
 assert c['pcm_hash']==b['pcm_hash']
 c['grain_ratio']=c['grain_motion']/max(b['grain_motion'],1e-9);c['field_ratio']=c['field_motion']/max(b['field_motion'],1e-9)
 assert c['settled_grain']<.01,(c,'STOP failed to settle')
 assert c['grain_ratio']>=(2.0 if c['mode']==2 else 1.15),c
(out/'metrics.json').write_text(json.dumps(rows,indent=2));print(json.dumps(rows[8:],indent=2))
