"""Three-way real PCM comparison. Measurements do not imply listening approval."""
from pathlib import Path
import subprocess,json,array,math,hashlib
root=Path(__file__).resolve().parents[1];out=root/'validation/musical-quality';audio=out/'audio';audio.mkdir(parents=True,exist_ok=True)
work=Path('/tmp/aquora-quality-render');work.mkdir(exist_ok=True)
refs={'069':'f9c36e3','0610':'654d725','candidate':None}
profiles=[('intimate',7,0),('air',17,1),('chamber',4,2),('orbit',19,3)]
c=r'''#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "engine.c"
static sr_engine e;static int16_t pcm[512];
int main(int n,char**v){unsigned seed=atoi(v[3]),seconds=atoi(v[4]);sr_init(&e,12345);sr_set_scene(&e,atoi(v[1]));sr_set_music_mode(&e,atoi(v[2]));sr_generate_song(&e,seed,0);sr_set_loop_length(&e,64);e.tick_length=SR_RATE*15u/72u;e.motion_manual|=4u;e.variation=.6f;e.rhythm_style=0;FILE*f=fopen(v[5],"wb"),*t=fopen(v[6],"w");assert(f&&t);fprintf(t,"frame,form_loop,phrase,role,events,last_note,gain,patch\n");sr_start(&e);unsigned prev=999;
for(unsigned b=0;b<(SR_RATE*seconds+255u)/256u;b++){sr_render(&e,pcm,256);fwrite(pcm,2,512,f);if(prev!=e.sequence_step){prev=e.sequence_step;for(unsigned r=0;r<8;r++)fprintf(t,"%u,%u,%u,%u,%u,%u,%.4f,%u\n",e.frame_count,e.form_loop,e.phrase,r,e.tracks[r].events,e.tracks[r].last_note,e.role_gain[r],r==7?e.drum_kit:e.tracks[r].patch);}}fclose(f);fclose(t);}
'''
rows=[]
for version,ref in refs.items():
 d=work/version;d.mkdir(exist_ok=True)
 for name in ['engine.c','serenity.h','learn.h','song_form.h']:
  data=subprocess.check_output(['git','show',f'{ref}:device/dsp/{name}'],cwd=root) if ref else (root/'device/dsp'/name).read_bytes()
  (d/name).write_bytes(data)
 (d/'render.c').write_text(c);exe=d/'render';subprocess.run(['cc','-O2',str(d/'render.c'),'-lm','-o',str(exe)],check=True)
 cases=[(name,scene,mode,seed,120) for name,scene,mode in profiles for seed in [777,2027]]+[('longform',7,0,777,720)]
 for name,scene,mode,seed,seconds in cases:
  stem=f'{name}-s{seed}-{version}';pcm=d/(stem+'.pcm');timeline=d/(stem+'.csv')
  subprocess.run([str(exe),str(scene),str(mode),str(seed),str(seconds),str(pcm),str(timeline)],check=True)
  a=array.array('h');a.frombytes(pcm.read_bytes());peak=max(abs(v) for v in a);rms=math.sqrt(sum(v*v for v in a)/len(a));assert peak<32730 and rms>10
  rows.append(dict(name=name,scene=scene,mode=mode,seed=seed,version=version,seconds=seconds,bpm=72,peak=peak,rms=rms,pcm_sha256=hashlib.sha256(a.tobytes()).hexdigest(),file=stem+'.ogg'))
  subprocess.run(['ffmpeg','-v','error','-y','-f','s16le','-ar','44100','-ac','2','-i',str(pcm),'-af',f'volume={1800/rms:.9f}','-c:a','libopus','-b:a','64k',str(audio/(stem+'.ogg'))],check=True)
  # retain compressed actual role events and post-gain timelines
  import gzip
  with gzip.open(audio/(stem+'.csv.gz'),'wb') as f:f.write(timeline.read_bytes())
  pcm.unlink();print(stem,peak,round(rms),flush=True)
(out/'ANALYSIS.json').write_text(json.dumps(rows,indent=2))
print('Rendered27 matched files; human listening approval PENDING.',flush=True)
