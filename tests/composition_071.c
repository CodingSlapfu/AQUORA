/* Event scores only: no recording or audio file output. */
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static int16_t pcm[512];
int main(void){
 unsigned cases=0,bass=0,anchors=0,pickups=0,weak_rests=0,sections=0;
 unsigned mode_lead[4]={0},mode_piano[4]={0};
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<4;mode++)
 for(unsigned len=16;len<=64;len*=2)for(unsigned seed=1;seed<=2;seed++){
  sr_init(&e,1000+seed);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
  sr_generate_song(&e,seed*1777+scene,0);e.loop_length=len;sr_start(&e);
  int8_t home[16];memcpy(home,e.motif_home,16);
  unsigned first_lead=e.tracks[0].events,first_piano=e.tracks[4].events;
  for(unsigned n=0;n<8192;n++){
   unsigned next=form_index(&e,1),prior=form_index(&e,0);
   trigger(&e);
   unsigned now=form_index(&e,0);
   if(now!=prior){assert(now==next);sections++;}
   assert(!memcmp(home,e.motif_home,16));
   assert(e.running&&e.drum_density==8&&e.drum_presence==100);
   memset(e.scheduled,0,sizeof e.scheduled);
   e.frame_count+=e.tick_length;
  }
  assert(e.tracks[1].events&&e.tracks[7].events);
  mode_lead[mode]+=e.tracks[0].events-first_lead;
  mode_piano[mode]+=e.tracks[4].events-first_piano;
  cases++;
 }
 /* Actual shared kick/bass performance, including deliberately un-doubled
  * weaker kicks and anticipation; no requirement to duplicate every kick. */
 for(unsigned seed=1;seed<=64;seed++){
  sr_init(&e,seed);sr_generate_song(&e,seed,0);e.energy=.8f;e.loop_length=64;sr_start(&e);e.form_route[0]=12;
  for(unsigned s=0;s<64;s++){
   unsigned before=e.tracks[1].events;float kick=groove_kick(&e,s);
   bass_groove(&e,s);unsigned hit=e.tracks[1].events>before;
   if(kick==1.f){assert(hit);anchors++;}
   if(kick>0&&kick<.9f&&!hit)weak_rests++;
   if(!kick&&hit)pickups++;
   if(hit){bass++;assert(e.tracks[1].last_note>=33&&e.tracks[1].last_note<=60);}
  }
 }
 assert(weak_rests&&pickups&&anchors&&bass>anchors);
 for(unsigned mode=0;mode<4;mode++){
  assert(mode_lead[mode]>0&&mode_piano[mode]>0);
  printf("mode %u lead %u piano %u\n",mode,mode_lead[mode],mode_piano[mode]);
 }
 assert(mode_piano[SR_NEOCLASSICAL]>mode_piano[SR_CINEMATIC]*2u);
 /* Theme rhythm must survive the second bar of a 32-step phrase. */
 for(unsigned seed=1;seed<=24;seed++){
  sr_init(&e,seed);e.loop_length=32;e.freeze=1;e.variation=0;sr_start(&e);
  unsigned a=0,b=0;
  for(unsigned n=0;n<32;n++){
   unsigned prior=e.tracks[0].events;trigger(&e);
   if(n<16)a+=e.tracks[0].events-prior;else b+=e.tracks[0].events-prior;
   memset(e.scheduled,0,sizeof e.scheduled);
  }
  assert(a>0&&b>0);
 }
 /* Every planned rise has a payoff, reductions cannot chain together. */
 for(unsigned seed=1;seed<=100;seed++){
  sr_init(&e,seed);sr_generate_song(&e,seed,0);
  for(unsigned n=1;n<32;n++){
   unsigned a=e.form_route[n-1],b=e.form_route[n];
   if(a==20)assert(b==12||b==26);
   if(a==21)assert(b==28||b==2||b==12);
   assert(!(sr_sections[a].family==3&&sr_sections[b].family==3));
  }
 }
 /* Mixed output sanity, numerical peaks only. No file is written. */
 unsigned peak=0;
 for(unsigned mode=0;mode<4;mode++){
  sr_init(&e,42);sr_set_music_mode(&e,mode);sr_start(&e);
  for(unsigned n=0;n<2048;n++){
   sr_render(&e,pcm,256);
   for(unsigned j=0;j<512;j++){unsigned a=abs(pcm[j]);if(a>peak)peak=a;assert(a<32730);}
  }
 }
 printf("PASS %u intention/mode/loop/seed scores; %u transitions; bass %u anchors %u pickups %u weak-kick rests %u; PCM peak %u; theme/return/payoff contracts\n",cases,sections,bass,anchors,pickups,weak_rests,peak);
}
