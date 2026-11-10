from pathlib import Path
from PIL import Image,ImageChops
import subprocess,os,sys
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
images=[]
for font in range(3):
 d=out/str(font);d.mkdir(exist_ok=True)
 c=['master 2048','tap HOME','turn ALGO 5','turn SELECT 1',f'spin K2 {font-1}','wait 1600','reboot',f'expect ui_font {font}','wait 4500','expect intro 1','shot intro.png']
 p=d/'test.txt';p.write_text('\n'.join(c)+'\n')
 q=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1',AQUORA_TEST_INTRO='1'));(d/'run.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
 images.append(Image.open(d/'intro.png').crop((48,0,232,34)))
assert all(ImageChops.difference(images[0],i).getbbox() is None for i in images[1:])
print('PASS exact AQUORA Sans full-size startup wordmark pixels preserved for Small, Medium and Large preferences')
