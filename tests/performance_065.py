"""Actual LCD frames under headroom/fallback fixtures; host times are not FM-1 times."""
from pathlib import Path
from PIL import Image
import os,subprocess,sys,json,statistics
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
results=[]
for visual in range(2):
 for full in range(2):
  for worker in ('ok','off','bad','late'):
   for pressure in (0,5400,5900):
    name=f'v{visual}-full{full}-{worker}-p{pressure}';d=out/name;d.mkdir(exist_ok=True)
    commands=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K4 {visual}','tap HOME','tap PLAY',f'wait {12300 if full else 1000}','expect reset_visual_counters 1']
    for n in range(18):commands+=['wait 100','expect perf_sample 1',f'shot f{n:02}.png']
    commands+=['expect running 1']
    p=d/'test.txt';p.write_text('\n'.join(commands)+'\n')
    r=subprocess.run([host,str(p),str(d)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_SEED='1',FM1_WORKER_TEST=worker,AQUORA_TEST_CALLBACK_US=str(pressure)))
    (d/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
    frames=[Image.open(d/f'f{n:02}.png').tobytes() for n in range(18)]
    unique=len(set(frames));assert unique>=6,(name,unique)
    perf=[list(map(int,line.split()[1:])) for line in r.stdout.splitlines() if line.startswith('PERF ')]
    assert len(perf)==18 and all(row[3]<=256 for row in perf)
    rows={}
    for column,label in ((0,'geometry_host_us'),(1,'draw_host_us')):
     values=sorted(x[column] for x in perf);rows[label]={'median':statistics.median(values),'p95':values[16],'p99':values[17],'max':max(values)}
    results.append({'case':name,'unique_frames':unique,'geometry_jobs':perf[-1][-1],'fallbacks':perf[-1][-2],**rows})
(out/'performance.json').write_text(json.dumps(results,indent=2)+'\n')
print('PASS both visualizers move at normal/near/over-deadline pressure; embedded/fullscreen; healthy/off/bad/late worker; bounded256 contours;48 real renderer cases')
