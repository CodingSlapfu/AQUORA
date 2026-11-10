from pathlib import Path
from PIL import Image,ImageChops
import subprocess,os,sys,json
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
def run(name,cmd):
 d=out/name;d.mkdir(exist_ok=True);p=d/'panel.txt';p.write_text('\n'.join(cmd)+'\n');r=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'));(d/'panel.log').write_text(r.stdout+r.stderr);assert not r.returncode,(name,r.stdout+r.stderr);return d
run('sound',['master 2048','turn ALGO 1','expect track 0','tap EDIT','shot lead.png','spin K2 2','wait 30','expect lead_glide 140','turn SELECT 1','expect track 1','shot bass.png','spin K2 3','wait 30','expect bass_glide 120','turn SELECT 1','expect track 2','shot pads-explanation.png','turn SELECT -2','expect track 0','tap EDIT','turn SELECT 7','expect track 7','tap EDIT','turn SELECT -7','expect track 0','expect directory_ok 1','expect route_unique 1'])
for mode in range(3):
 for led in range(4):
  run(f'persist-{mode}-{led}',['master 2048','turn ALGO 5','spin K2 '+str(led),'turn SELECT 1',f'spin K4 {mode}','spin K3 1','tap PLAY','wait 1900','expect preferences_saved 1','reboot',f'expect visual {mode}',f'expect lighting {led}','expect ui_background 1','expect boot_drums 1','shot restored.png'])
run('migration',['expect settings_066_fixture 1','reboot','expect visual 1','expect lighting 3','expect ui_background 1','expect boot_drums 1'])
run('bank-global',['master 2048','tap SAVE','wait 100','turn ALGO 5','turn SELECT 1','spin K4 2','wait 1600','tap EDIT','tap SEL','wait 100','expect visual 2','expect bank_message 5','shot bank.png'])
# Actual framebuffer at fixed times; master must affect DAC only, not pixels/telemetry.
summary=[]
for mode in range(3):
 for theme in range(2):
  for master in (0,4095):
   cmd=[f'master {master}','turn ALGO 5','turn SELECT 1',f'spin K3 {theme}',f'spin K4 {mode}','tap HOME','tap PLAY','wait 13000']
   for n in range(16):cmd+=['wait 250','expect visual_snapshot 1',f'shot f{n:02}.png']
   cmd+=['tap PLAY','wait 5000','expect visual_snapshot 1','shot settled.png']
   d=run(f'visual-{mode}-{theme}-{master}',cmd)
   frames=[Image.open(d/f'f{n:02}.png').convert('RGB') for n in range(16)]
   frames[0].save(d/'preview.apng',format='PNG',save_all=True,append_images=frames[1:],duration=250,loop=0)
   assert any(ImageChops.difference(frames[0],f).getbbox() for f in frames[1:])
  a=out/f'visual-{mode}-{theme}-0';b=out/f'visual-{mode}-{theme}-4095'
  for n in range(16):assert not ImageChops.difference(Image.open(a/f'f{n:02}.png'),Image.open(b/f'f{n:02}.png')).getbbox(),(mode,theme,n)
  summary.append({'mode':mode,'white':theme,'master_frames_equal':16})
# Show true 240x240 dark/light sequence frames, no mockup.
sheet=Image.new('RGB',(720,480))
for mode in range(3):
 for theme in range(2):sheet.paste(Image.open(out/f'visual-{mode}-{theme}-0/f08.png'),(mode*240,theme*240))
sheet.save(out/'three-modes.png');(out/'summary.json').write_text(json.dumps(summary,indent=2))
print('PASS actual SOUND encoder paths, 12 live preference/reboot combinations, 066 migration, global bank precedence, six 240px animated modes/themes, 96 identical master-zero/full frames')
