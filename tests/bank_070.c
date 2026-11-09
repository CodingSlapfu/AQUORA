#include "../device/dsp/engine.c"
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
int main(void){
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<4;mode++){
  sr_init(&engine,700+scene);sr_set_scene(&engine,scene);sr_set_music_mode(&engine,mode);
  sr_generate_song(&engine,777+scene*mode,0);engine.form_loop=153;sr_plan_sections(&engine);
  engine.auto_gain=engine.auto_level=0;engine.tracks[0].patch=47;
  engine.energy=.04f;engine.learned.count=3;
  for(unsigned i=0;i<3;i++){engine.learned.note[i]=60+i*2;engine.learned.step[i]=i*4;engine.learned.length[i]=2;}
  uint8_t expected[33];memcpy(expected,engine.form_route,33);
  arrangement r;arrangement_snapshot(&r);assert(arrangement_valid(&r));
  sr_init(&engine,99);arrangement_apply(&r);
  assert(!memcmp(expected,engine.form_route,33));
  assert(engine.tracks[0].patch==47&&!engine.auto_gain&&!engine.auto_level&&engine.learned.count==3&&engine.form_loop==153);
 }
 puts("PASS 84 current bank snapshots restore exact committed routes, new patch47, explicit OFF auto mixing, learned phrase and form position after reinitialization");
}
