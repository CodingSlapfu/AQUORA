"""Real control paths with active audio and visual load; host metrics are not hardware latency."""
from pathlib import Path
import subprocess,os,sys,json
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
summary=[]
font=int(os.environ.get("AQUORA_FONT_FIXTURE","1"))
for running in (0,1):
 for visual in range(3):
  for worker in ('ok','off','late','bad'):
   d=out/f'{running}-{visual}-{worker}';d.mkdir(exist_ok=True)
   cmd=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K4 {visual}','tap HOME']
   if font==2:cmd=cmd[:-1]+['spin K2 1','tap HOME']
   if running:cmd+=['tap PLAY']
   cmd+=['wait 11000']
   for n in range(10):cmd+=['turn ALGO 1','tap EDIT','spin K1 1','spin K2 1','spin K2 -1','turn SELECT 1','tap ARP','spin K2 1','spin K2 -1','tap HOME']
   cmd+=['expect drops 0','expect control_metrics 1']
   p=d/'test.txt';p.write_text('\n'.join(cmd)+'\n')
   q=subprocess.run([host,str(p),str(d)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_SEED='1',FM1_WORKER_TEST=worker,AQUORA_TEST_CALLBACK_US='5400'))
   (d/'run.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
   metrics=[x for x in q.stdout.splitlines() if x.startswith('CONTROL')];assert len(metrics)==2
   summary.append({'running':running,'visual':visual,'worker':worker,'metrics':metrics})
(out/'metrics.json').write_text(json.dumps(summary,indent=2))
print('PASS 24 STOP/PLAY visual-worker/load cases, rapid encoder/navigation paths, no queue drops; latency histograms recorded separately')
