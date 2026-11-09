/* Event/control/PCM acceptance; intentionally never writes audio recordings. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../device/dsp/engine.c"
static sr_engine e,other;
static int16_t pcm[512];
int main(void){
 unsigned contexts=0,anchors=0,changes=0;uint64_t patches=0;
 for(unsigned scene=0;scene<SR_SCENES;scene++)for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,700+scene);sr_set_music_mode(&e,mode);sr_set_scene(&e,scene);
  assert(e.auto_gain&&e.auto_level&&e.drum_density==8&&e.drum_level==1.f&&e.drum_presence==100&&e.tracks[7].volume==1.f);
  uint64_t context_patches=0;uint32_t kits=0;
  for(unsigned seed=1;seed<=16384;seed++){
   sr_generate_song(&e,seed,0);
   for(unsigned t=0;t<7;t++){patches|=1ull<<e.tracks[t].patch;context_patches|=1ull<<e.tracks[t].patch;}
   kits|=1u<<e.drum_kit;
   other=e;other.form_key=0;sr_plan_sections(&other);assert(!memcmp(e.form_route,other.form_route,33));
   /* A saved home motif/seed/form position reconstructs the committed route
    * even when live expressive energy or learned-note capture changes. */
   other.energy=.01f;other.learned.count=3;other.form_key=0;sr_plan_sections(&other);
   assert(!memcmp(e.form_route,other.form_route,33));
   for(unsigned slot=1;slot<32;slot++)assert(e.form_route[slot]!=e.form_route[slot-1]);
   assert(e.form_route[31]==15&&e.form_route[32]==28);
   if(seed>=512&&context_patches==((1ull<<SR_PATCHES)-1)&&kits==((1u<<SR_DRUM_KITS)-1))break;
  }
  assert(context_patches==((1ull<<SR_PATCHES)-1)&&kits==((1u<<SR_DRUM_KITS)-1));
  e.auto_gain=e.auto_level=0;e.drum_density=3;e.drum_level=.41f;e.drum_presence=61;
  sr_set_scene(&e,scene);sr_generate_song(&e,777,1);
  assert(!e.auto_gain&&!e.auto_level&&e.drum_density==3&&e.drum_level==.41f&&e.drum_presence==61);
  e.drum_density=8;e.drum_level=1;e.energy=.8f;e.variation=.4f;e.loop_length=64;sr_start(&e);
  for(unsigned rhythm=0;rhythm<3;rhythm++){
   e.rhythm_style=rhythm;
   for(unsigned s=0;s<64;s++){
    unsigned before=e.tracks[1].events;float kick=groove_kick(&e,s);
    bass_groove(&e,s);
    if(kick){assert(e.tracks[1].events==before+1);anchors++;}
    if(e.tracks[1].events!=before){assert(e.tracks[1].last_note>=33&&e.tracks[1].last_note<=60);}
   }
  }
  /* Hours of section scheduling through actual trigger, not label cycling. */
  for(unsigned len=16;len<=64;len*=2){
   e.loop_length=len;unsigned prior=form_index(&e,0);
   for(unsigned n=0;n<8192;n++){
    unsigned next=form_index(&e,1),current=form_index(&e,0);
    trigger(&e);e.frame_count+=e.tick_length;memset(e.scheduled,0,sizeof e.scheduled);
    unsigned now=form_index(&e,0);
    if(now!=current){assert(now==next);changes++;}
    assert(e.running&&e.drum_density==8&&e.drum_level==1.f);
    prior=now;
   }
   e.freeze=1;unsigned loop=e.form_loop;for(unsigned n=0;n<128;n++)trigger(&e);assert(e.form_loop==loop);e.freeze=0;
  }
  sr_stop(&e);e.stop_frames=0;contexts++;
 }
 assert(patches==((1ull<<SR_PATCHES)-1));
 unsigned peak=0;
 for(unsigned patch=0;patch<SR_PATCHES;patch++){
  sr_init(&e,42);e.bed_level=0;e.sacred_level=0;e.drums_enabled=0;sr_start(&e);e.freeze=1;e.timeline_enabled=1;
  e.tracks[0].patch=patch;sr_note_on(&e,0,60,100,2.f);
  assert(e.timeline[0].note==60&&e.timeline[0].duration>0);
  for(unsigned n=0;n<160;n++){sr_render(&e,pcm,256);for(unsigned j=0;j<512;j++){unsigned a=abs(pcm[j]);assert(a<32730);if(a>peak)peak=a;}}
  sr_stop(&e);for(unsigned n=0;n<100;n++)sr_render(&e,pcm,256);
  for(unsigned v=0;v<SR_VOICES;v++)assert(!e.voices[v].gate);
 }
 printf("PASS %u contexts; %u actual-kick bass anchors; %u truthful committed section transitions; 48 append-only patches; PCM ceiling %u; defaults/manual precedence, freeze, STOP, timeline\n",contexts,anchors,changes,peak);
}
