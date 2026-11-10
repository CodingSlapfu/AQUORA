from pathlib import Path
from PIL import Image
import os,subprocess,sys
h=sys.argv[1];root=Path(sys.argv[2]);root.mkdir(parents=True,exist_ok=True)
for loop in range(3):
 for fast in [0,1]:
  d=root/f'{loop}-{fast}';d.mkdir(exist_ok=True)
  cmd=['master 2048','tap HOME','wait 700','turn K1 20','turn ALGO 2',f'turn K4 {loop}',f'turn K3 {200 if fast else -200}','wait 1000']
  for n in range(8):cmd+=['wait 100',f'shot f{n}.png']
  cmd+=['expect drops 0','expect large_text_overflow 0']
  p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n');q=subprocess.run([h,str(p.resolve()),str(d.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'));(d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
  ims=[Image.open(d/f'f{n}.png').convert('RGB') for n in range(8)]
  assert len({im.crop((0,208,240,240)).tobytes() for im in ims})==1,(loop,fast,'footer')
  assert len({im.crop((35,48,233,208)).tobytes() for im in ims})>=3,(loop,fast,'motion')
print('PASS dense intensity100, 16/32/64 loops, minimum/maximum tempo, clipped moving graph and fixed footer in 48 exact240px captures')
