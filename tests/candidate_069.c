#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static void init(void){sr_init(&e,730069);e.variation=.6f;}
int main(void){
 for(unsigned length=16;length<=64;length*=2)for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++){
  init();sr_set_music_mode(&e,mode);sr_set_scene(&e,scene);sr_set_loop_length(&e,length);sr_start(&e);
  e.loop_pos=1;e.sequence_step=1;
  sr_set_scene(&e,(scene+1)%21);sr_set_scene(&e,(scene+2)%21);
  for(unsigned n=0;n<4;n++)trigger(&e);
  assert(e.scene==(scene+2)%21&&!e.pending_scene);
  unsigned seen=0;
  for(unsigned n=0;n<4096;n++){unsigned id=form_index(&e,0);seen|=1u<<id;trigger(&e);assert(e.music_mode==mode);}
  assert(__builtin_popcount(seen)>=12);
  e.freeze=1;unsigned form=e.form_loop;for(unsigned n=0;n<128;n++)trigger(&e);assert(e.form_loop==form);
 }
 init();sr_start(&e);sr_set_music_mode(&e,1);assert(e.pending_mode==2);sr_set_music_mode(&e,0);assert(!e.pending_mode);
 sr_set_scene(&e,4);sr_stop(&e);assert(e.scene==4&&!e.pending_scene);
 init();e.learn_mode=1;e.learn_strength=3;
 sr_learn_event(&e,0,1,1000);sr_learn_event(&e,0,0,1200);
 sr_learn_event(&e,4,1,1400);sr_learn_event(&e,4,0,1600);
 sr_learn_event(&e,7,1,1800);sr_learn_event(&e,7,0,2200);
 assert(e.capture.count==3&&e.capture.length[2]>0);
 sr_learn_action(&e,1);assert(e.learned.count==3&&!e.capture.count);
 for(unsigned n=0;n<16;n++)assert(motif_degree(&e,n)>=-1&&motif_degree(&e,n)<=6);
 sr_start(&e);e.learn_mode=2;for(unsigned n=0;n<40;n++)sr_learn_event(&e,n%27,1,3000+n*50);
 assert(e.capture.count<=16);sr_learn_action(&e,1);assert(e.learn_pending);
 e.loop_pos=0;trigger(&e);assert(!e.learn_pending&&!e.capture.count);
 sr_learn_action(&e,2);assert(!e.learned.count);
 puts("PASS 252 intention/mode/length cases: next-beat last-choice scene, 16-section routes, Freeze; bounded capture/apply/clear and scale mapping");
}
