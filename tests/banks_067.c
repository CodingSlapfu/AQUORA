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
 assert(argc==2);sr_init(&engine,633);FILE *f;
#ifdef LEGACY_FIXTURE
 sr_set_scene(&engine,12);visual_mode=1;engine.drum_presence=63;arrangement r;arrangement_snapshot(&r);
 f=fopen(argv[1],"wb");assert(f);fwrite(&r,1,sizeof r,f);fclose(f);puts("PASS authentic 066 bank serializer");
#else
 f=fopen(argv[1],"rb");assert(f);sizes[0]=fread(flash[0],1,512,f);fclose(f);
 arrangement_init();visual_mode=2;arrangement_execute();assert(bank_message==BANK_LOADED&&engine.scene==12&&engine.drum_presence==63&&visual_mode==2);
 for(unsigned scene=0;scene<SR_SCENES;scene++)for(unsigned mode=0;mode<3;mode++)for(unsigned len=16;len<=64;len*=2){
  unsigned slot=scene%4;sr_set_scene(&engine,scene);sr_set_music_mode(&engine,mode);sr_set_loop_length(&engine,len);
  visual_mode=scene%3;arrangement expected;arrangement_snapshot(&expected);bank_slot=slot;bank_action=1;bank_confirm=1;arrangement_execute();assert(bank_message==BANK_SAVED);
  sr_init(&engine,77);arrangement_init();visual_mode=2;bank_slot=slot;arrangement_execute();assert(bank_message==BANK_LOADED);
  assert(visual_mode==2&&engine.scene==scene&&engine.music_mode==mode&&engine.loop_length==len&&engine.song_seed==expected.song_seed);
  assert(engine.drum_presence==((expected.legacy_crc>>10)&127u)-1u);
  assert(!memcmp(engine.melody,expected.melody,16)&&!memcmp(engine.motif_home,expected.motif_home,16));
  for(unsigned t=0;t<8;t++)assert(engine.tracks[t].patch==expected.track[t].patch&&engine.tracks[t].pan==expected.track[t].pan&&engine.tracks[t].send==expected.track[t].send);
 }
 puts("PASS authentic 066 migration; 189 scene/style/length banks, song/motif/routing recall and global visualizer precedence");
#endif
}
