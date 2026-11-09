"""Actual boot/save, bank and UI paths against unchanged host platform; restores files."""
from pathlib import Path
import subprocess,sys,shutil
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
p=root/'vendor/fm1-base/host/plat_host.c';original=p.read_text();binary=root/'device/build/host/sound-healing_host';backup=out/'host.saved';shutil.copy2(binary,backup)
s=original.replace('int main(int argc, char **argv)','int unused_host_main(int argc, char **argv)',1)
s+=r'''
#include <assert.h>
int main(void){
 boot();engine.running=requested_play=0;engine.stop_frames=0;intro_active=0;
 send(25,-5);send(62,-27);int32_t pcm[512];app_render(pcm,256,2048);
 assert(engine.motion_manual&4u);assert(engine.drum_presence==73);
 unsigned ticks=engine.tick_length;assert(save_preferences()==0);boot();
 assert(engine.tick_length==ticks&&(engine.motion_manual&4u)&&engine.drum_presence==73);
 engine.running=requested_play=0;engine.stop_frames=0;sr_generate_song(&engine,999,0);assert(engine.tick_length==ticks&&engine.drum_presence==73);
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++)for(unsigned len=16;len<=64;len*=2){
  sr_set_scene(&engine,scene);sr_set_music_mode(&engine,mode);sr_set_loop_length(&engine,len);engine.motion_manual|=4;engine.tick_length=ticks;engine.drum_presence=73;
  for(unsigned slot=0;slot<4;slot++){
   bank_slot=slot;bank_action=1;bank_confirm=1;arrangement_execute();assert(bank_message==BANK_SAVED);
   engine.tick_length=9000;engine.motion_manual=0;engine.drum_presence=100;bank_action=0;arrangement_execute();assert(bank_message==BANK_LOADED);
   assert(engine.tick_length==ticks&&(engine.motion_manual&4u)&&engine.drum_presence==73&&engine.scene==scene&&engine.music_mode==mode&&engine.loop_length==len);
  }
 }
 aq_settings old={0};old.version=0x41513039u;old.perfect_key=1;old.lighting=3;old.visual=2;old.font_size=1;old.background=1;old.air_depth=80;old.air_sensitivity=70;old.motion_manual=0;old.startup_sound=1;
 assert(plat_store_save(OBJ_SETTINGS,&old,offsetof(aq_settings,tempo_ticks))==0);boot();
 assert(lighting==3&&visual_mode==2&&ui_background==1&&engine.air_depth==80&&startup_chime==1&&!(engine.motion_manual&4u)&&engine.drum_presence==100);
 puts("PASS schema39 preferences migration; actual control/save/reboot manual BPM and presence;1008 bank roundtrips across21 intentions/four modes/three lengths/four banks");return 0;
}
'''
try:
 p.write_text(s);subprocess.run([sys.executable,'vendor/fm1-base/tools/build.py','device','--host','--release','0.6.10'],cwd=root,check=True,stdout=subprocess.DEVNULL)
 q=subprocess.run([str(binary)],cwd=root,text=True,capture_output=True);(out/'result.log').write_text(q.stdout+q.stderr);assert not q.returncode,q.stdout+q.stderr;print(q.stdout)
finally:p.write_text(original);shutil.copy2(backup,binary);backup.unlink()
