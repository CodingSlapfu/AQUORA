from pathlib import Path
import os,subprocess,sys,json
from PIL import Image
h=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
rows=[]
for theme,background,font in [(t,b,f) for t in (range(5) if "--all-themes" in sys.argv else [0])
                              for b in (range(2) if "--all-themes" in sys.argv else [0])
                              for f in range(3)]:
 d=out/f't{theme}-b{background}-font{font}';d.mkdir(exist_ok=True)
 cmd=['master 2048','tap HOME','wait 700','turn ALGO 5','turn SELECT 1',f'spin K1 {theme}',f'spin K2 {background}',
      'spin K3 -2',f'spin K3 {font}',f'expect ui_theme {theme}',f'expect ui_background {background}',
      f'expect ui_font {font}','tap HOME']
 for scene in range(22):
  cmd+=['turn PRESET -22' if scene==0 else 'turn PRESET 1','wait 800',f'shot intention{scene}.png']
 for page,tabs in [(1,13),(2,8),(3,5),(4,5),(5,5),(6,4)]:
  cmd+=['tap HOME',f'turn ALGO {page}']
  for tab in range(tabs):
   if tab:cmd+=['tap EDIT']
   cmd+=['wait 100',f'shot p{page}t{tab}.png']
 cmd+=['tap HOME','spin K2 1','wait 100','shot performance.png','spin PRESET 1','wait 800','shot performance-next.png','tap HOME','turn ALGO 2','tap EDIT','wait 1000']
 for n in range(6):cmd+=['wait 100',f'shot graph{n}.png']
 cmd+=['expect drops 0','expect large_text_overflow 0',f'expect ui_background {background}']
 p=d/'script.txt';p.write_text('\n'.join(cmd)+'\n')
 q=subprocess.run([h,str(p.resolve()),str(d.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_SEED='1'))
 (d/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
 for p in d.glob('*.png'):assert Image.open(p).size==(240,240)
 # Graph animation cannot write into the control footer.
 fs=[Image.open(d/f'graph{n}.png').convert('RGB') for n in range(6)]
 assert len({x.crop((0,208,240,240)).tobytes() for x in fs})==1,(font,'footer changed')
 assert len({x.crop((35,48,233,208)).tobytes() for x in fs})>1,(font,'graph stalled')
 rows.append({'theme':theme,'background':background,'font':font,'screens':len(list(d.glob('*.png'))),'size':[240,240],'footer_invariant':True,'font_overflows':0})
(out/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
print(f'PASS {len(rows)} theme/background/font combinations, 22 intentions, EDIT cycles and 240x240 LCD')
