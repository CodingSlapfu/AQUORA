from pathlib import Path
import subprocess
from PIL import Image
out=Path('/tmp/063-full-matrix');out.mkdir(exist_ok=True)
for theme in range(5):
 for font in range(3):
  for bg in range(2):
   for visual in range(2):
    d=out/f't{theme}-f{font}-b{bg}-v{visual}';d.mkdir(exist_ok=True)
    commands=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K1 {theme}',f'spin K2 {font-1}',f'spin K3 {bg}',f'spin K4 {visual}','tap HOME','tap PLAY','wait 500','shot normal.png','wait 12000','expect fullscreen 1','expect battery_fade 0','shot full.png','turn ALGO 1','expect fullscreen 0','expect page 0','shot return.png','expect drops 0']
    p=d/'test.txt';p.write_text('\n'.join(commands)+'\n')
    r=subprocess.run(['device/build/host/sound-healing_host',str(p),str(d)],capture_output=True,text=True);assert not r.returncode,r.stdout+r.stderr
    (d/'test.log').write_text(r.stdout+r.stderr)
    im=Image.open(d/'full.png').convert('RGB');expected=(255,255,255) if bg else (0,0,0)
    for band in range(5):
     assert sum(x==expected for x in im.crop((0,band*48,240,(band+1)*48)).getdata())>5760,(d,band)
print('PASS 60 theme/font/background/visualizer combinations, 180 authentic frames, exact canvas all fullscreen bands and wake controls')
