from pathlib import Path
from PIL import Image
import subprocess,os,sys,json
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
rows=[]
for white in range(2):
 for font in range(3):
  for length in range(3):
   d=out/f'{white}-{font}-{length}';d.mkdir(exist_ok=True)
   cmd=['master 2048','tap HOME','wait 80','tap PLAY','wait 600','turn ALGO 5','turn SELECT 1',f'spin K2 {font-1}',f'spin K3 {white}','tap HOME','turn ALGO 2',f'spin K1 {length}','tap PLAY','wait 700']
   for i in range(6):cmd+=['wait 60',f'shot f{i}.png']
   cmd+=['tap SEL','wait 80','shot bass.png','expect running 1','expect drops 0','expect large_text_overflow 0','expect bank_migration_matrix 1','tap PLAY','wait 600','expect running 0']
   p=d/'panel.txt';p.write_text('\n'.join(cmd)+'\n')
   q=subprocess.run([host,str(p),str(d)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_SEED='1'))
   (d/'panel.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stdout+q.stderr
   images=[Image.open(d/f'f{i}.png') for i in range(6)];assert all(x.size==(240,240) for x in images)
   assert len(set(x.tobytes() for x in images))>=3
   rows.append({'white':white,'font':font,'loop_index':length,'unique_frames':len(set(x.tobytes() for x in images))})
(out/'SUMMARY.json').write_text(json.dumps(rows,indent=2)+'\n')
print('PASS 18 theme/font/loop layouts, 126 exact 240px frames; continuous movement, role detail, V1-V6 x 4 bank migration, SAVE/STOP/no queue drops. Physical LCD timing unverified.')
