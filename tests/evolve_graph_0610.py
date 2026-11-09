from pathlib import Path
import subprocess,sys,shutil,os
from PIL import Image
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
p=root/'vendor/fm1-base/host/plat_host.c';original=p.read_text();binary=root/'device/build/host/sound-healing_host';backup=out/'host.saved';shutil.copy2(binary,backup)
s=original.replace('int main(int argc, char **argv)','int unused_host_main(int argc, char **argv)',1)
s+=r'''
#include <assert.h>
int main(int argc,char**argv){boot();engine.running=requested_play=0;intro_active=0;page=PAGE_EVOLVE;song_tab=0;fullscreen=0;
 for(unsigned bg=0;bg<2;bg++)for(unsigned density=0;density<3;density++)for(unsigned len=16;len<=64;len*=2)for(unsigned phase=0;phase<2;phase++)for(unsigned cursor=31+phase;cursor<=192;cursor+=32){
  ui_background=bg;engine.loop_length=len;evolve_step=cursor;
  for(unsigned i=0;i<64;i++)evolve_history[i]=density==2||i%(density==1?2u:8u)==0u?255:0;
  dirty=1;draw(1);char path[4096];snprintf(path,sizeof path,"%s/g%u-%u-%u-%u.png",argv[1],bg,density,len,cursor);png_write(path);
 }
 puts("PASS216 true240px EVOLVE images: sparse/moderate/dense,16/32/64, twelve history-wrap positions/two themes");return 0;}
'''
try:
 p.write_text(s);subprocess.run([sys.executable,'vendor/fm1-base/tools/build.py','device','--host','--release','0.6.10'],cwd=root,check=True,stdout=subprocess.DEVNULL)
 q=subprocess.run([str(binary),str(out)],cwd=root,text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_BOOT_SEED='730610'));assert not q.returncode,q.stdout+q.stderr
 for path in out.glob('g*.png'):
  im=Image.open(path).convert('RGB');assert im.size==(240,240)
  # Dense fixtures must show gaps between6px event columns, not a solid strip.
  if '-2-' in path.name:
   assert any(im.getpixel((15,y))!=im.getpixel((14,y)) for y in range(50,80)),path
 (out/'result.log').write_text(q.stdout);print(q.stdout)
finally:p.write_text(original);shutil.copy2(backup,binary);backup.unlink()
