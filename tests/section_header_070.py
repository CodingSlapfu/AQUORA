from pathlib import Path
from PIL import Image
import subprocess,sys,os
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
p=out/'panel.txt';p.write_text('master 2048\ntap HOME\nwait 400\nexpect running 1\nshot start.png\nwait 90000\nshot evolved.png\nexpect running 1\nexpect drops 0\n')
r=subprocess.run([host,str(p),str(out)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_BOOT_SEED='1'))
(out/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
before=Image.open(out/'start.png').crop((0,0,214,46));after=Image.open(out/'evolved.png').crop((0,0,214,46))
assert before.tobytes()!=after.tobytes(),'HOME section header did not update during unattended playback'
print('PASS real HOME NOW/NEXT header changes during 90s unattended playback without control input; no command drops')
