"""Capture the real app_render output; host evidence, not physical DAC/RF evidence."""
from pathlib import Path
import array,json,math,os,subprocess,sys,wave
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,AQUORA_TEST_INTRO='1',AQUORA_TEST_SEED='1')
def run(name,commands):
 d=out/name;d.mkdir(exist_ok=True);p=d/'scenario.txt';p.write_text('\n'.join(commands)+'\n')
 r=subprocess.run([host,str(p),str(d)],env=env,text=True,capture_output=True)
 (d/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,(name,r.stdout+r.stderr)
 return d
controls=['tap '+b for b in 'FX SEL ENV LFO EDIT GLO HOME SAVE ARP SEQ PLAY REC OCT- OCT+'.split()]
controls+=['tapkey '+str(k) for k in range(27)]
controls+=['turn '+e+' 1' for e in 'SELECT ALGO PRESET K1 K2 K3 K4'.split()]
controls+=['master 3500']
for i,action in enumerate(controls):
 run('skip-'+str(i),['master 2048','wait 2000','expect intro 1',action,'wait 120','expect intro 0','expect running 0'])
stats={}
for master in (0,2048,4096):
 commands=[f'master {master}','wav startup.wav']
 for delay,name in ((100,'initial'),(2100,'spiral'),(400,'title-a'),(800,'title-aquo'),(900,'title-complete'),(3300,'tail')):
  commands += [f'wait {delay}',f'shot {name}.png']
 commands+=['expect geometry_jobs 0','expect worker_jobs 0','wait 700','expect intro 0','wavstop']
 d=run('master-'+str(master),commands)
 with wave.open(str(d/'startup.wav')) as w:
  assert w.getframerate()==44100 and w.getnchannels()==2
  a=array.array('h',w.readframes(w.getnframes()))
 peak=max(map(abs,a));rms=math.sqrt(sum(x*x for x in a)/len(a))
 dc=sum(a)/len(a);stereo=sum(a[i]!=a[i+1] for i in range(0,len(a),2))
 if master:assert 300<peak<6000 and rms>50 and abs(dc)<2 and stereo>10000
 else:assert peak==0
 stats[str(master)]=dict(peak=peak,rms=rms,dc=dc,stereo_different_frames=stereo)
(out/'startup-analysis.json').write_text(json.dumps(stats,indent=2)+'\n')
print('PASS 49 physical skip inputs consumed; exact app_render stereo PCM, master mute/levels, 8-second completion; zero intro worker/geometry jobs')
