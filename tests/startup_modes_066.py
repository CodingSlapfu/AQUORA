from pathlib import Path
import subprocess,os,array,wave,math,sys
host=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
levels=[]
for mode in range(3):
 d=out/str(mode);d.mkdir(exist_ok=True)
 c=['master 2048','tap HOME','turn ALGO 5']+['tap EDIT']*4+[f'spin K1 {mode-2}','wait 1600','reboot',f'expect startup_chime {mode}','expect intro 1','expect reset_visual_counters 1','wav cue.wav','wait 7800','expect geometry_jobs 0','expect worker_jobs 0','wait 300','wavstop','expect intro 0']
 p=d/'panel.txt';p.write_text('\n'.join(c)+'\n');r=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1',AQUORA_TEST_INTRO='1'));(d/'panel.log').write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
 with wave.open(str(d/'cue.wav')) as w:a=array.array('h',w.readframes(w.getnframes()))
 levels.append(math.sqrt(sum(x*x for x in a)/len(a)))
assert levels[0]==0 and .30<levels[1]/levels[2]<.40,levels
print('PASS cold startup default/mode persistence, OFF exact silence, QUIET ~35% PCM RMS, normal cue, 8s completion and zero visualizer jobs',levels)
