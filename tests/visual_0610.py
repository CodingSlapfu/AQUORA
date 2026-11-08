from pathlib import Path
from PIL import Image,ImageChops
import subprocess,os,sys,json
old,new,out=sys.argv[1],sys.argv[2],Path(sys.argv[3]);out.mkdir(exist_ok=True,parents=True)
for version,host in [('baseline',old),('candidate',new)]:
 for mode in range(3):
  for theme in range(2):
   for master in (0,2048,4095):
    d=out/version/f'{mode}-{theme}-{master}';d.mkdir(parents=True,exist_ok=True)
    cmd=[f'master {master}','turn ALGO 5','turn SELECT 1',f'spin K3 {theme}',f'spin K4 {mode}','tap HOME','tap PLAY','wait 13000']
    for n in range(24):cmd+=['wait 150','expect visual_snapshot 1',f'shot f{n:02}.png']
    cmd+=['tap PLAY','wait 8000','shot stop.png']
    p=d/'panel.txt';p.write_text('\n'.join(cmd)+'\n')
    r=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'))
    (d/'panel.log').write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
    frames=[Image.open(d/f'f{n:02}.png').convert('RGB') for n in range(24)]
    for f in frames:assert f.size==(240,240)
    frames[0].save(d/'motion.apng',save_all=True,append_images=frames[1:],duration=150,loop=0)
   for n in range(24):
    a=Image.open(out/version/f'{mode}-{theme}-0/f{n:02}.png');b=Image.open(out/version/f'{mode}-{theme}-4095/f{n:02}.png')
    assert not ImageChops.difference(a,b).getbbox(),(version,mode,theme,n,'volume0 causality')
sheet=Image.new('RGB',(1440,480))
for mode in range(3):
 for theme in range(2):
  for v,version in enumerate(('baseline','candidate')):
   sheet.paste(Image.open(out/version/f'{mode}-{theme}-2048/f12.png'),((mode*2+v)*240,theme*240))
sheet.save(out/'comparison.png')
(out/'summary.json').write_text(json.dumps({'frames':864,'themes':2,'master':[0,50,100],'baseline':'v0.6.9','physical_assessment':'owner-unverified'},indent=2))
print('PASS 864 real 240px LCD frames; all visualizers/themes; master0/50/100; baseline/candidate sequences and STOP')
