from pathlib import Path
import os,subprocess,sys,json
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
for skin in range(5):
 for white in range(2):
  d=out/f'{skin}-{white}';d.mkdir(exist_ok=True)
  cmd=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K1 {skin}','spin K2 1','expect ui_font 2',f'spin K3 {white}','tap HOME']
  for scene in range(21):
   if scene:cmd+=['spin K1 1']
   cmd+=['wait 50',f'shot play-{scene}.png']
  for page,tabs in ((1,12),(2,5),(3,5),(4,4),(5,5),(6,3)):
   cmd+=['tap HOME',f'turn ALGO {page}']
   for tab in range(tabs):
    if tab:cmd+=['tap EDIT']
    cmd+=['wait 50',f'shot page{page}-{tab}.png']
  cmd+=['tap ARP','tap EDIT','spin K1 -19']
  for kit in range(19):
   if kit:cmd+=['spin K1 1']
   cmd+=['wait 50',f'shot kit-{kit}.png']
  cmd+=['tap HOME','turn ALGO 1','tap EDIT','spin K1 -36']
  for patch in range(36):
   if patch:cmd+=['spin K1 1']
   cmd+=['wait 50',f'shot patch-{patch}.png']
  cmd+=['expect large_text_overflow 0','expect large_font_used 1','expect drops 0']
  p=d/'panel.txt';p.write_text('\n'.join(cmd)+'\n')
  q=subprocess.run([host,str(p),str(d)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_SEED='1'))
  (d/'panel.log').write_text(q.stdout+q.stderr);assert q.returncode==0,(str(d),q.stdout+q.stderr)
print('PASS 1100 actual 240px Large screenshots: 21 intentions/19 kits/36 patches/all pages/subtabs x 5 skins x both backgrounds; measured text bounds and no command drops')
