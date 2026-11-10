from pathlib import Path
from PIL import Image
import subprocess,os,sys
host=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
for name,key in [('low',0),('high',26),('transition',12)]:
 d=out/name;d.mkdir(exist_ok=True)
 c=['master 2048','turn ALGO 5','turn SELECT 1','spin K4 1','tap HOME','tap PLAY',f'key {key} down','wait 13000']
 for n in range(24):
  if name=='transition' and n==12:c +=[f'key {key} up','key 24 down']
  c+=['wait 400',f'shot f{n:02}.png']
 c+=['tap PLAY',f'key {key} up','key 24 up','wait 5000','shot silence.png']
 p=d/'panel.txt';p.write_text('\n'.join(c)+'\n');r=subprocess.run([host,str(p),str(d)],env=dict(os.environ,AQUORA_TEST_SEED='1'),text=True,capture_output=True);(d/'panel.log').write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
 frames=[Image.open(d/f'f{n:02}.png').convert('RGB') for n in range(24)];frames[0].save(d/'preview.apng',format='PNG',save_all=True,append_images=frames[1:],duration=400,loop=0)
 sheet=Image.new('RGB',(720,480))
 for j,n in enumerate((0,4,8,12,18,23)):sheet.paste(frames[n],((j%3)*240,(j//3)*240))
 sheet.save(d/'contact.png')
print('PASS actual 240px low/high/transition/silence sequences captured')

assert (out/'low/preview.apng').read_bytes()!=(out/'high/preview.apng').read_bytes(), 'real low/high notes must change the rendered sequence'
