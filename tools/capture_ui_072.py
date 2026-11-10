from pathlib import Path
from PIL import Image
import subprocess,os,json,hashlib,sys
host=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
pages={'play':['tap HOME'],'sound':['turn ALGO 1'],'evolve':['turn ALGO 2'],'arrange':['turn ALGO 2']+['tap EDIT']*6,'music-mode':['turn ALGO 2']+['tap EDIT']*4+['spin K1 3'],'learn':['turn ALGO 2']+['tap EDIT']*5,'mixer':['turn ALGO 3'],'mix-strip':['turn ALGO 3','tap FX'],'drums':['turn ALGO 4'],'air':['turn ALGO 6','tap EDIT'],'appearance':['turn ALGO 5','turn SELECT 1'],'original':['tap HOME'],'cymatics':['turn ALGO 5','turn SELECT 1','spin K4 1','tap HOME'],'chladni':['turn ALGO 5','turn SELECT 1','spin K4 2','tap HOME']}
rows=[]
for name,nav in pages.items():
 d=out/name;d.mkdir(exist_ok=True);cmd=['master 2048','tap HOME','wait 80','wait 600']+nav+['wait 120','expect running 1','expect drops 0','shot still.png']
 if name in ['evolve','mixer','original','cymatics','chladni']:
  for i in range(36):cmd+=['wait 100',f'shot f{i:02}.png']
 p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n');q=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'));(d/'panel.log').write_text(q.stdout+q.stderr);assert q.returncode==0,(name,q.stdout,q.stderr)
 Image.open(d/'still.png').save(out/f'{name}.png')
 if name in ['evolve','mixer','original','cymatics','chladni']:
  frames=[Image.open(d/f'f{i:02}.png').convert('RGB') for i in range(36)];unique=len({x.tobytes() for x in frames});assert unique>5,(name,unique)
  frames[0].save(out/f'{name}.apng',format='PNG',save_all=True,append_images=frames[1:],duration=100,loop=0,optimize=True)
 rows.append({'name':name,'renderer':'actual 0.7.2 firmware host RGB565 LCD','image_sha256':hashlib.sha256((out/f'{name}.png').read_bytes()).hexdigest(),'transport':'running','recorded_audio':False})
d=out/'intro';d.mkdir(exist_ok=True);p=d/'script.txt';p.write_text('wait 3500\nshot still.png\nexpect intro 1\n');q=subprocess.run([host,str(p),str(d)],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_INTRO='1'));assert q.returncode==0
Image.open(d/'still.png').save(out/'intro.png')
(out/'provenance.json').write_text(json.dumps({'version':'0.7.2','identity':'FM-1_73000702','source':os.environ.get('GITHUB_SHA','local'),'screens':rows},indent=2)+'\n')
print('PASS 15 actual240px screens, five36-frame APNGs, no audio recordings')
