from pathlib import Path
import subprocess,os,sys,json
h=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True);rows=[]
for running in [0,1]:
 for visual in range(3):
  for worker in ['ok','off','late','bad']:
   d=out/f'{running}-{visual}-{worker}';d.mkdir(exist_ok=True)
   cmd=['master 2048','tap HOME','wait 700','turn ALGO 5','turn SELECT 1',f'spin K2 {visual}','tap HOME']
   if not running:cmd+=['tap PLAY']
   cmd+=['wait 1200',f'expect running {running}']
   for n in range(10):cmd+=['tap HOME','turn ALGO 1','tap EDIT','spin K1 1','spin K2 1','spin K2 -1','turn SELECT 1','tap ARP','spin K2 1','spin K2 -1','tap HOME']
   cmd+=['expect drops 0','expect control_metrics 1']
   p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n');q=subprocess.run([h,str(p.resolve()),str(d.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1',FM1_WORKER_TEST=worker,AQUORA_TEST_CALLBACK_US='5400'))
   (d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
   rows.append({'running':running,'visual':visual,'worker':worker,'metrics':[x for x in q.stdout.splitlines() if x.startswith('CONTROL')]})
(out/'results.json').write_text(json.dumps(rows,indent=2)+'\n');print('PASS 24 real input/render/audio worker-load cases; no dropped commands; host latency metrics recorded, not FM-1 measurements')
