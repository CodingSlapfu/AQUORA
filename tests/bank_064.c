#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define AQ_BANK_BASE 0x97000u
#define AQ_BANK_STRIDE 0x2000u
#define OBJ_BANK1 0u
static sr_engine engine;
static unsigned requested_play,qread,qwrite,state_revision;
static unsigned char flash[4][512];static unsigned sizes[4];
static unsigned plat_lock(void){return 0;}static void plat_unlock(unsigned n){(void)n;}
static int plat_store_size(unsigned slot){return sizes[slot];}
static int plat_store_load(unsigned slot,void *p,unsigned cap){unsigned n=sizes[slot];if(n>cap)n=cap;memcpy(p,flash[slot],n);return n;}
static int plat_store_save(unsigned slot,const void *p,unsigned n){assert(n<=512);memcpy(flash[slot],p,n);sizes[slot]=n;return 0;}
#ifndef AQ_ARRANGEMENT
#define AQ_ARRANGEMENT "../device/arrangement.c"
#endif
#include AQ_ARRANGEMENT
int main(int argc,char **argv){
 assert(argc==2);FILE *f;sr_init(&engine,633);sr_set_scene(&engine,4);
#ifdef LEGACY_FIXTURE
 arrangement record;engine.motion[0]=(sr_motion){333,2,17,61};arrangement_snapshot(&record);
 f=fopen(argv[1],"wb");assert(f);assert(fwrite(&record,1,sizeof record,f)==sizeof record);fclose(f);
 puts("PASS authentic published v0.6.3 serializer fixture");
#else
 f=fopen(argv[1],"rb");assert(f);sizes[0]=fread(flash[0],1,512,f);fclose(f);
 arrangement_init();assert(bank_status[0]==1);arrangement_execute();assert(bank_message==BANK_LOADED);
 assert(engine.music_mode==SR_STANDARD&&engine.motion[0].glide_ms==333&&(engine.motion_manual&1));
 for(unsigned mode=0;mode<3;mode++)for(unsigned slot=0;slot<4;slot++){
  sr_set_music_mode(&engine,mode);sr_set_scene(&engine,8+slot);
  sr_motion_auto(&engine,0,0);engine.motion[0]=(sr_motion){333,2,17,61};
  arrangement expected,actual;arrangement_snapshot(&expected);bank_slot=slot;bank_action=1;bank_confirm=1;arrangement_execute();assert(bank_message==BANK_SAVED);
  sr_init(&engine,77);arrangement_init();bank_slot=slot;arrangement_execute();assert(bank_message==BANK_LOADED);
  arrangement_snapshot(&actual);assert(actual.crc4==expected.crc4);
  assert(engine.music_mode==mode&&engine.song_seed==expected.song_seed&&engine.motion_manual==expected.motion_manual);
 }
 puts("PASS v0.6.3 -> v0.6.4 migration, all three modes/four banks, exact seed/patch/motif/motion-override recall");
#endif
}
