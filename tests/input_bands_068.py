"""Inject physical controls at LCD strip completion in the real host unity path.
Timing is a deterministic 5ms transfer fixture, NOT an FM-1 measurement.
Run in the reconstructed full source tree; restores platform source/binary.
"""
from pathlib import Path
import subprocess,sys,shutil
root=Path(__file__).resolve().parents[1]
p=root/'vendor/fm1-base/host/plat_host.c'
original=p.read_text();binary=root/'device/build/host/sound-healing_host'
backup=binary.with_suffix('.saved');shutil.copy2(binary,backup)
s=original.replace('static void lcd_blit(', 'static void band_inject(void);\nstatic void lcd_blit(',1)
s=s.replace('    blits++;','    blits++;\n    band_inject();',1)
s=s.replace('int main(int argc, char **argv)','int unused_host_main(int argc, char **argv)',1)
s+='''
#include <assert.h>
static unsigned inject, transfers, capture_ms, target_band, input_kind;
static int32_t fixture_block[512];
static void band_inject(void){
 if(!inject)return;
 transfers++;now_ms+=5;
 app_render(fixture_block,256,2048); /* audio preempts every strip */
 if(transfers==target_band){
  if(input_kind<7)enc_acc[input_kind]=1;
  else held_btn=1u<<B_HOME;
  capture_ms=now_ms;
 }
}
int main(void){
 boot();intro_active=0;intro_inputs_seen=1;
 for(unsigned run=0;run<2;run++)for(unsigned pg=0;pg<7;pg++)for(unsigned kind=0;kind<8;kind++)for(unsigned band=1;band<=4;band++){
  engine.running=requested_play=run;page=pg;debug=0;fullscreen=0;
  group=0;sound_picker=0;oldbuttons=held_btn=0;oldkeys=held_keys=0;
  dirty=1;unsigned previous=input_revision;
  transfers=0;target_band=band;input_kind=kind;inject=1;draw(1);inject=0;
  assert(transfers==target_band);assert(input_revision==previous+1);assert(dirty);
  assert(now_ms==capture_ms); /* dispatch at first safe boundary */
  unsigned applied=command_applied;int32_t block[512];
  app_render(block,256,2048);
  assert(qread==qwrite); /* no stranded command when playing or stopped */
  held_btn=0;app_input_only();draw(0);assert(!dirty);
  printf("PASS run=%u page=%u kind=%u strip=%u capture_to_dispatch_fixture_ms=0 commands=%u\\n",run,pg,kind,band,command_applied-applied);
 }
 return 0;
}
'''
try:
 p.write_text(s)
 subprocess.run([sys.executable,'vendor/fm1-base/tools/build.py','device','--host','--release','0.6.8'],cwd=root,check=True,stdout=subprocess.DEVNULL)
 subprocess.run([str(binary)],cwd=root,check=True)
finally:
 p.write_text(original);shutil.move(backup,binary)
