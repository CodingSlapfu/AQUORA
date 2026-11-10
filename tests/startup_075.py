from pathlib import Path
from PIL import Image
import os,subprocess,sys,json
h=sys.argv[1];out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
cmd=['wait 2500','shot entry.png','wait 1700','shot peak.png','wait 3400','shot exit.png','expect intro 1','tap HOME','expect intro 0','expect drops 0']
p=out/'script.txt';p.write_text('\n'.join(cmd)+'\n');q=subprocess.run([h,str(p.resolve()),str(out.resolve())],capture_output=True,text=True,env=dict(os.environ,AQUORA_TEST_INTRO='1'));(out/'panel.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr
rows=[]
for name in ['entry','peak','exit']:
 im=Image.open(out/(name+'.png')).convert('RGB');assert im.size==(240,240)
 assert all(im.getpixel(p)==(0,0,0) for p in [(0,0),(239,0),(0,239),(239,239)])
 pixels=[(x,y) for y in range(98,143) for x in range(20,220) if 105<=im.getpixel((x,y))[0]<=130 and im.getpixel((x,y))[1]>=245 and 210<=im.getpixel((x,y))[2]<=240]
 assert pixels,name
 box=[min(x for x,y in pixels),min(y for x,y in pixels),max(x for x,y in pixels)+1,max(y for x,y in pixels)+1]
 if name!='entry':assert abs((box[0]+box[2])/2-120)<3 and abs((box[1]+box[3])/2-120)<3,(name,box)
 rows.append({'stage':name,'wordmark_ink_bounds':box})
(out/'results.json').write_text(json.dumps(rows,indent=2)+'\n');print('PASS exact240px intro entry/hold/exit, centered optical wordmark, safe bounds, black corners and skip')
