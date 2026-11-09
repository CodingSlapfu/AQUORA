from pathlib import Path
from PIL import Image
import subprocess,sys,os
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
p=out/'panel.txt'
commands=['master 2048','tap HOME','wait 80','tap PLAY','wait 300','turn ALGO 2']+['spin K3 8']*32+['tap HOME','tap PLAY','wait 400','expect running 1','shot start.png','wait 8200','shot evolved.png','expect running 1','expect drops 0','turn SELECT 1','wait 80','expect track 1','shot role.png']
p.write_text('\n'.join(commands)+'\n')
r=subprocess.run([host,str(p),str(out)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_BOOT_SEED='1'))
(out/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
before=Image.open(out/'start.png').crop((0,0,214,46));after=Image.open(out/'evolved.png').crop((0,0,214,46))
assert before.tobytes()!=after.tobytes(),'HOME section header did not update during unattended playback'
assert Image.open(out/'evolved.png').crop((0,31,191,46)).tobytes()!=Image.open(out/'role.png').crop((0,31,191,46)).tobytes(),'Selected role label did not update'
print('PASS real HOME NOW/NEXT header changes during 8.2s unattended playback before idle fullscreen without control input; no command drops')
