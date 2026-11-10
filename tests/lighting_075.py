from pathlib import Path
import os,subprocess,sys,json
h=sys.argv[1];d=Path(sys.argv[2]);d.mkdir(parents=True,exist_ok=True)
cmd=['master 0','tap HOME','wait 700','turn ALGO 5','spin K1 -3','spin K2 -3','spin K3 -1','tap HOME']
for t in range(7):cmd+=['tap SEL','turn SELECT 1']
labels=[]
def samples(label):
 for n in range(60):cmd.extend(['wait 100','expect led_masks 1']);labels.append(label)
samples('drums');cmd+=['tap SEL','wait 500'];samples('silence')
cmd+=['turn SELECT -6','tap SEL','wait 500'];samples('tonal')
cmd+=['turn SELECT 6','tap SEL','wait 500'];samples('mixed')
cmd+=['tap PLAY','wait 500'];samples('stop')
cmd+=['key 1 down','wait 50','expect led_bright_mask 2','key 1 up','wait 100','expect led_bright_mask 0','expect drops 0'];p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n')
q=subprocess.run([h,str(p.resolve()),str(d.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'));(d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
masks=[tuple(map(int,x.split()[1:])) for x in q.stdout.splitlines() if x.startswith('LEDS ')];assert len(masks)==len(labels)
for label in ['drums','silence','tonal','mixed','stop']:
 rows=[row for tag,row in zip(labels,masks) if tag==label]
 if label=='drums':assert any(a for a,b in rows) and all(not b for a,b in rows)
 elif label=='mixed':assert any(a and b for a,b in rows)
 elif label=='tonal':assert any(b for a,b in rows) and all(not a for a,b in rows)
 else:assert all(not a and not b for a,b in rows)
(d/'masks.json').write_text(json.dumps(list(zip(labels,masks)))+'\n')
print('PASS 300 actual LED output snapshots: drums black-only, tonal white-only, simultaneous groups, mute silence, STOP silence, held-key precedence; master volume0; physical scan/polarity unverified')
