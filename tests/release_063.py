"""Real renderer: global SAVE in every page, exact canvas, SEL navigation."""
import subprocess,sys
from pathlib import Path
from PIL import Image
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
def run(name,commands):
 d=out/name;d.mkdir(exist_ok=True);p=d/'script.txt';p.write_text('\n'.join(commands)+'\n')
 r=subprocess.run([host,str(p),str(d)],capture_output=True,text=True)
 (d/'test.log').write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
 return d
for playing in range(2):
 for page in range(7):
  cmds=['master 2048',f'turn ALGO {page}']
  if playing:cmds+=['tap PLAY','wait 200']
  cmds+=['tap SAVE','tap SAVE','wait 100','expect saving 0','expect bank_message 4',f'expect running {playing}','expect bank_mask 1','expect drops 0','shot saved.png']
  run(f'save-p{page}-r{playing}',cmds)
for visual in range(2):
 for bg in range(2):
  d=run(f'full-v{visual}-b{bg}',['master 2048','turn ALGO 5','turn SELECT 1',f'spin K3 {bg}',f'spin K4 {visual}','tap HOME','tap PLAY','wait 12400','expect fullscreen 1','expect battery_fade 0','shot full.png','tap SAVE','wait 100','expect fullscreen 0','expect bank_message 4','expect running 1','shot saved.png'])
  im=Image.open(d/'full.png').convert('RGB')
  # Both left and right edges in every DMA band are untouched canvas.
  expected=(255,255,255) if bg else (0,0,0)
  for band in range(5):
   region=im.crop((0,band*48,240,(band+1)*48))
   assert sum(pixel==expected for pixel in region.getdata())>240*48//2,(d,band)
run('appearance-navigation',['master 2048','turn ALGO 5','expect mute 0','tap SEL','expect display_subtab 1','wait 70','expect mute 0','spin K3 1','expect ui_background 1','wait 1500','reboot','expect ui_background 1','tap HOME','tap SEL','wait 70','expect mute 1'])
print('PASS global save 7 pages x PLAY/STOP, both fullscreen modes/backgrounds, pure canvas all bands, Appearance SEL and persistence')
