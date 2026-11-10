#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"
static sr_engine e,a,b;
int main(void){
 unsigned cases=0,low_notes=0,high_notes=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++){
   sr_init(&e,75);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,777+scene,0);
   unsigned frequency=e.frequency;float master=e.master;
   e.energy=0;e.current_energy=0;sr_start(&e);a=e;
   for(unsigned n=0;n<128;n++){trigger(&e);memset(e.scheduled,0,sizeof e.scheduled);}low_notes+=e.note_ons;
   e=a;e.energy=1;
   for(unsigned n=0;n<128;n++){trigger(&e);memset(e.scheduled,0,sizeof e.scheduled);}
   high_notes+=e.note_ons;assert(e.current_energy>.99f&&e.master==master&&e.frequency==frequency);
   assert(e.chord_gap>=120&&e.chord_gap<=220);
   for(unsigned i=0;i<16;i++)e.melody[i]=i%4;remember_motif(&e);assert(e.motif_count);
   for(unsigned k=0;k<100;k++){e.melody[0]=k%7;remember_motif(&e);}assert(e.motif_count<=4);
   cases++;
 }
 assert(high_notes>low_notes);
 sr_init(&e,75);e.response_enabled=1;sr_start(&e);unsigned before=e.tracks[SR_HARMONY].events;
 for(unsigned i=0;i<3;i++){sr_learn_event(&e,i*2,1,100+i*300);sr_learn_event(&e,i*2,0,220+i*300);}
 assert(e.capture.count==3&&e.response_pending);
 for(unsigned i=0;i<80;i++)response_tick(&e);
 assert(e.tracks[SR_HARMONY].events==before+3&&!e.response_pending&&!e.capture.count);
 assert(!e.learned.count); /* response alone does not force persistent learning */
 e.response_enabled=0;sr_learn_event(&e,1,1,3000);assert(!e.capture.count);
 sr_init(&e,75);e.response_enabled=1;sr_start(&e);
 for(unsigned i=0;i<3;i++){sr_learn_event(&e,i,1,100+i*200);sr_learn_event(&e,i,0,200+i*200);}
 sr_learn_event(&e,4,1,800);for(unsigned i=0;i<32;i++)response_tick(&e);assert(!e.response_wait);
 /* A playing key cannot be stolen by an automatic note. */
 for(unsigned i=0;i<SR_VOICES;i++){e.voices[i].active=e.voices[i].key=e.voices[i].gate=1;e.voices[i].midi=60;}
 unsigned ons=e.note_ons;sr_note_on(&e,SR_PIANO,70,80,1);assert(e.note_ons==ons);
 puts("PASS 84 intensity/mode/intentions, bounded scored motif memory, timed physical-key response, held-key priority");
 printf("Intensity emitted notes: sparse %u / rich %u\n",low_notes,high_notes);
}
