from pathlib import Path
from PIL import Image
import os,subprocess,sys,json
h=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True);results=[]
for visual in range(3):
 for worker in ['ok','off','bad','late']:
  for pressure in [0,5900]:
   d=out/f'{visual}-{worker}-{pressure}';d.mkdir(exist_ok=True)
   cmd=['master 2048','tap HOME','wait 700','turn ALGO 5','turn SELECT 1',f'spin K2 {visual}','tap HOME','spin K2 1','wait 300','expect fullscreen 1','expect reset_visual_counters 1']
   for n in range(8):cmd+=['wait 100','expect perf_sample 1',f'shot f{n}.png']
   cmd+=['spin PRESET 1','wait 800','expect fullscreen 1','expect drops 0']
   p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n');q=subprocess.run([h,str(p.resolve()),str(d.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1',FM1_WORKER_TEST=worker,AQUORA_TEST_CALLBACK_US=str(pressure)))
   (d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
   ims=[Image.open(d/f'f{n}.png').convert('RGB') for n in range(8)]
   assert all(im.size==(240,240) for im in ims);unique=len({im.crop((0,48,240,192)).tobytes() for im in ims});assert unique>=3,(visual,worker,pressure,unique)
   results.append({'visualizer':visual,'worker':worker,'callback_pressure_fixture_us':pressure,'unique_visual_frames':unique,'metrics':[x for x in q.stdout.splitlines() if x.startswith('PERF ')]})
(out/'results.json').write_text(json.dumps(results,indent=2)+'\n');print('PASS 24 Performance visualizer/fallback/pressure cases, accurate controls and moving240px frames; pressure is a host fixture, not measured hardware XRUNs')
