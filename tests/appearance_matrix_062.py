"""Native 240x240 renderer: seven pages, every theme/font/background combination."""
from pathlib import Path
from PIL import Image,ImageDraw
import os,subprocess,sys
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
count=0;thumbs=[]
for theme in range(5):
 for font in range(3):
  for background in range(2):
   dest=out/f't{theme}-f{font}-b{background}';dest.mkdir(exist_ok=True)
   commands=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K1 {theme}',f'spin K2 {font-1}',f'spin K3 {background}',f'expect ui_theme {theme}',f'expect ui_font {font}',f'expect ui_background {background}','wait 70','shot setup.png','tap HOME']
   for page,name in enumerate(('play','sound','evolve','mix','drums')):
    if page:commands+=['turn ALGO 1']
    commands+=['wait 60',f'expect page {page}',f'shot {name}.png']
    if page==3:commands+=['tap FX','wait 70','shot mix-strip.png','tap FX']
   commands+=['turn ALGO 2','expect page 6','wait 70','shot air.png','tap EDIT','wait 70','shot air-fx.png','expect drops 0']
   script=dest/'matrix.txt';script.write_text('\n'.join(commands)+'\n')
   run=subprocess.run([host,str(script),str(dest)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'))
   (dest/'matrix.log').write_text(run.stdout+run.stderr)
   assert run.returncode==0,run.stdout+run.stderr
   for shot in dest.glob('*.png'):
    im=Image.open(shot).convert('RGB');assert im.size==(240,240)
    assert len(im.getcolors(65536) or [])>3,shot
    count+=1
   thumbs.append((f'T{theme} F{font} '+('White' if background else 'Black'),Image.open(dest/'setup.png').convert('RGB')))
# All 30 actual rendered appearance states are different.
assert len({im.tobytes() for _,im in thumbs})==30
sheet=Image.new('RGB',(6*240,5*260),'#24303a');d=ImageDraw.Draw(sheet)
for i,(label,im) in enumerate(thumbs):
 x=i%6*240;y=i//6*260;sheet.paste(im,(x,y+20));d.text((x+6,y+4),label,fill='white')
sheet.save(out/'appearance-sample-sheet.png')
# Large mixer and key pages, full resolution with labels outside firmware image.
labels=['play','sound','evolve','mix','mix-strip','drums','air','air-fx']
sheet=Image.new('RGB',(4*240,2*260),'#24303a');d=ImageDraw.Draw(sheet)
for i,label in enumerate(labels):
 im=Image.open(out/'t1-f2-b0'/f'{label}.png');x=i%4*240;y=i//4*260
 sheet.paste(im,(x,y+20));d.text((x+6,y+4),label,fill='white')
sheet.save(out/'large-page-sheet.png')
print(f'PASS {count} actual 240x240 screenshots: five skins x three optical sizes x black/white, seven pages plus mixer strip/AIR mapping')
