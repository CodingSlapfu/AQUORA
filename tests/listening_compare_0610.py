from pathlib import Path
import subprocess,sys,array,json,math,hashlib
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
profiles=[('warm-pulse',7,0,0),('interlocking-lights',19,0,1),('quiet-broken-orbit',12,0,2),('slow-drift',17,1,0),('chamber-answer',4,2,0),('experimental-orbit',19,3,2)]
rows=[]
for version,engine,current in [('069',Path(sys.argv[1]),1),('0610',root/'device/dsp/engine.c',1)]:
 c='''#include <stdio.h>\n#include <assert.h>\n#include <stdlib.h>\n#include ENGINE_FILE\nstatic sr_engine e;static int16_t p[512];\nint main(int argc,char**argv){unsigned scene=atoi(argv[1]),mode=atoi(argv[2]),rhythm=atoi(argv[3]);sr_init(&e,12345);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,777,0);sr_set_loop_length(&e,64);e.variation=.6f;e.scope_track=SR_BASS;\n#if CURRENT\ne.rhythm_style=rhythm;\n#endif\nsr_start(&e);FILE*f=fopen(argv[4],"wb"),*b=fopen(argv[5],"wb"),*t=fopen(argv[6],"w");assert(f&&b&&t);fprintf(t,"frame,step,phrase,role,events,gain,meter\\n");unsigned last=99999;for(unsigned block=0;block<(SR_RATE*180u+255u)/256u;block++){unsigned pos=e.scope_write;sr_render(&e,p,256);fwrite(p,2,512,f);for(unsigned j=0;j<64;j++){int16_t s=e.scope[(pos+j)&127u];fwrite(&s,2,1,b);}if(last!=e.sequence_step){last=e.sequence_step;for(unsigned role=0;role<8;role++)fprintf(t,"%u,%u,%u,%u,%u,%.5f,%.7f\\n",e.frame_count,e.sequence_step,e.phrase,role,e.tracks[role].events,e.role_gain[role],e.tracks[role].meter);}}fclose(f);fclose(b);fclose(t);}\n'''
 src=out/(version+'.c');src.write_text(c);exe=out/version
 subprocess.run(['cc','-O2','-I'+str(root/'device/dsp'),'-DENGINE_FILE='+json.dumps(str(engine.resolve())),f'-DCURRENT={current}',str(src),'-lm','-o',str(exe)],check=True)
 for name,scene,mode,rhythm in profiles:
  base=out/f'{name}-{version}';pcm=base.with_suffix('.pcm');stem=out/f'{name}-{version}-bass.pcm';timeline=base.with_suffix('.csv')
  subprocess.run([str(exe),str(scene),str(mode),str(rhythm),str(pcm),str(stem),str(timeline)],check=True)
  a=array.array('h');a.frombytes(pcm.read_bytes());peak=max(abs(v) for v in a);rms=math.sqrt(sum(v*v for v in a)/len(a));assert 100<peak<32730 and rms>10
  rows.append(dict(name=name,version=version,scene=scene,mode=mode,rhythm=rhythm if current else 0,seconds=len(a)/88200,peak=peak,rms=rms,pcm_sha256=hashlib.sha256(a.tobytes()).hexdigest()))
level=min(2000,min(28000*x['rms']/x['peak'] for x in rows))
for row in rows:
 base=out/f'{row["name"]}-{row["version"]}';gain=level/row['rms'];row['review_gain']=gain;row['matched_rms_target']=level
 subprocess.run(['ffmpeg','-v','error','-y','-f','s16le','-ar','44100','-ac','2','-i',str(base.with_suffix('.pcm')),'-af',f'volume={gain:.9f}','-c:a','libopus','-b:a','96k',str(base.with_suffix('.ogg'))],check=True)
(out/'ANALYSIS.json').write_text(json.dumps(rows,indent=2))
(out/'README.txt').write_text('Six original 3-minute compositions, matched seed12345/song777, same intent/mode/64-step setup. Reference069 and candidate0610 use the same explicit rhythmic character; generated BPM/sound choices differ as required. Review Oggs are RMS level-matched with common safe target; raw hashes/peaks and gain in ANALYSIS. Post-gain Bass stems at11025Hz and real event/gain/meter CSVs retained. No human aesthetic approval claimed.\n')
print('PASS twelve real 180-second comparison renders, no digital ceiling hits, RMS-matched listening files, Bass stems and role timelines')
