from pathlib import Path
import os,subprocess,sys,json
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True);rows=[]
for mode in range(4):
 for n in range(21):
  seed=(mode<<8)+n
  p=out/'panel.txt';p.write_text(f'master 0\nwait 100\nexpect running 1\nexpect boot_starts 1\nexpect scene {seed%21}\nexpect music_mode {mode}\nexpect drums 1\nexpect drops 0\n')
  q=subprocess.run([host,str(p),str(out)],env=dict(os.environ,AQUORA_TEST_BOOT_SEED=str(seed)),capture_output=True,text=True)
  assert q.returncode==0,q.stdout+q.stderr
  rows.append({'seed':seed,'scene':seed%21,'mode':mode})
(out/'distribution.json').write_text(json.dumps(rows));print('PASS deterministic boot injection: all21 intentions x all4 modes, autoplay once, drums enabled, zero command drops')
