/* Structural safeguards, explicitly not aesthetic acceptance. */
#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
int main(void){
 uint64_t patches=0;uint32_t kits=0;unsigned anchored=0;
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,12345);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
  for(unsigned seed=1;seed<=512;seed++){
   sr_generate_song(&e,seed,0);unsigned notes=0,rests=0;
   for(unsigned i=0;i<16;i++){notes+=e.melody[i]>=0;rests+=e.melody[i]<0;}
   assert(notes>=7&&rests>=7);assert(e.melody[0]==e.melody[8]&&e.melody[2]==e.melody[10]);
   assert(e.melody[12]==2&&e.melody[14]==0);
   for(unsigned t=0;t<7;t++){assert(e.tracks[t].patch<SR_PATCHES);patches|=1ull<<e.tracks[t].patch;}
   kits|=1u<<e.drum_kit;
   unsigned family=ensemble(&e,seed);
   static const unsigned lead[]={13,14,16,29};anchored+=e.tracks[0].patch==lead[family];
   for(unsigned phrase=1;phrase<=16;phrase++){
    e.phrase=phrase;develop_motif(&e);unsigned changed=0;
    for(unsigned i=0;i<16;i++)changed+=e.melody[i]!=e.motif_home[i];
    assert(changed<=1);assert(e.melody[0]==e.motif_home[0]&&e.melody[14]==0);
   }
   e.sound_locks=1023;unsigned locked=e.tracks[0].patch;int8_t saved[16];memcpy(saved,e.melody,16);
   sr_generate_song(&e,seed+1,1);assert(e.tracks[0].patch==locked&&!memcmp(saved,e.melody,16));e.sound_locks=0;
  }
  for(unsigned slot=0;slot<32;slot++){
   e.form_loop=slot*(2u+sr_scenes[scene].energy%3u);
   for(unsigned step=0;step<64;step+=4){int d=motif_degree(&e,step);unsigned c=composition_chord(&e,step);
    assert(d<0||d==(int)c||d==(int)((c+2)%7)||d==(int)((c+4)%7));}
  }
 }
 assert(patches==((1ull<<SR_PATCHES)-1)&&kits==((1u<<19)-1));
 assert(anchored>21u*4u*512u/2u);
 printf("PASS phrase breaths/echo/cadence, <=1-note development, 32-section strong-beat harmony, locks/reroll; globally accessible36/19; shared lead anchor %u/43008. Human approval PENDING.\n",anchored);
}
