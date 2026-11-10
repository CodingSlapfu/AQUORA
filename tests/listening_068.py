from pathlib import Path
import subprocess,os,sys,json,wave,array,math,hashlib
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
summary=[]
for name,scene,mode,rhythm in [('warm-pulse',7,0,0),('interlocking-lights',19,0,1),('quiet-broken-orbit',12,0,2),('slow-drift',17,1,0)]:
 d=out/name;d.mkdir(exist_ok=True)
 cmd=['master 2048',f'spin K1 {scene}','turn ALGO 2','spin K4 2']
 if mode:cmd+=['tap EDIT']*4+[f'spin K1 {mode}']
 if rhythm:cmd+=['tap ARP']+['tap EDIT']*3+[f'spin K1 {rhythm}']
 cmd+=['tap HOME','tap PLAY','wait 1000','wav mix.wav','wait 180000','wavstop','expect drops 0']
 p=d/'panel.txt';p.write_text('\n'.join(cmd)+'\n')
 q=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'));(d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode
 with wave.open(str(d/'mix.wav')) as w:
  assert w.getnchannels()==2 and w.getframerate()==44100
  a=array.array('h',w.readframes(w.getnframes()));duration=w.getnframes()/44100
 peak=max(abs(v) for v in a);rms=math.sqrt(sum(v*v for v in a)/len(a));assert duration>=179.9 and 20<peak<32730
 subprocess.run(['ffmpeg','-v','error','-y','-i',str(d/'mix.wav'),'-c:a','libopus','-b:a','96k',str(out/(name+'.ogg'))],check=True)
 summary.append(dict(name=name,scene=scene,mode=mode,rhythm=rhythm,duration=duration,peak=peak,rms=rms,pcm_sha256=hashlib.sha256(a.tobytes()).hexdigest()))
(out/'ANALYSIS.json').write_text(json.dumps(summary,indent=2))
print('PASS four original three-minute stereo compositions, no digital ceiling hits or queue drops; subjective listening owner-unverified')
