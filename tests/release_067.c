/* Full scenario coverage of actual DSP/control/serializer, with accelerated
 * control time for form tests. Separate PCM renders establish audible output. */
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static int16_t pcm[512];
static unsigned scenes,profiles,forms;
static void step(void){e.tick_phase=e.tick_length;sr_render(&e,pcm,64);}
int main(void){
 const char *names[]={"Miracles","Blessing","Negative Energy Cleanse","Cell Regeneration","Anxiety Release","Bliss","Raise Positive Vibrations","Neuro-Freq Detox"};
 for(unsigned i=0;i<8;i++)assert(!strcmp(sr_scenes[i+13].name,names[i]));
 for(unsigned scene=0;scene<SR_SCENES;scene++)for(unsigned mode=0;mode<3;mode++)for(unsigned len=16;len<=64;len*=2){
  sr_init(&e,0x12345);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_set_loop_length(&e,len);
  assert(e.scene==scene&&e.music_mode==mode&&e.loop_length==len);
  assert(e.frequency<SR_FREQUENCIES&&e.bed_type&&e.bed_type<SR_BEDS);
  assert(e.motion[0].glide_ms&&e.motion[1].glide_ms&&e.drum_kit<SR_DRUM_KITS);
  assert(e.drum_presence>=30&&e.drum_presence<100&&e.drum_level>0&&e.drum_level<1);
  assert(fabsf(e.tracks[SR_DRUMS].pan)<.2f&&e.tracks[SR_DRUMS].send>0);
  e.drum_presence=99;e.tracks[SR_DRUMS].pan=.9f;e.tracks[SR_DRUMS].send=.9f;
  sr_set_scene(&e,scene);assert(e.drum_presence!=99&&e.tracks[SR_DRUMS].pan!=.9f&&e.tracks[SR_DRUMS].send!=.9f);
  e.sound_locks|=1u<<SR_DRUMS;e.drum_presence=99;sr_set_scene(&e,scene);assert(e.drum_presence==99);e.sound_locks=0;
  sr_set_scene(&e,(scene+1)%SR_SCENES);sr_set_scene(&e,scene);assert(e.scene==scene&&e.drum_presence!=99);
  int melody[64];for(unsigned i=0;i<len;i++)melody[i]=motif_degree(&e,i);
  if(len>16){int different=0;for(unsigned i=0;i<16;i++)different+=melody[i]!=melody[16+i];assert(different>0);}
  printf("PROFILE %u %u %u kit%u den%u hit%u presence%u level%.3f pan%.3f send%.3f delay%.3f reverb%.3f\n",scene,mode,len,e.drum_kit,e.drum_density,e.drum_hit,e.drum_presence,e.drum_level,e.tracks[7].pan,e.tracks[7].send,e.delay_amount,e.reverb_amount);
  for(unsigned variation=0;variation<3;variation++){
   e.variation=variation*.5f;sr_start(&e);
   unsigned previous=99,changes=0;
   for(unsigned phrase=0;phrase<64;phrase++){
    e.phrase=phrase;plan_roles(&e);unsigned section=song_section(&e);
    if(section==1||section==2||section==4)assert(e.role_target[7]==1);
    if(previous!=section)changes++;previous=section;
    for(unsigned i=0;i<len;i++){step();for(unsigned n=0;n<128;n++)assert(pcm[n]>=-32768&&pcm[n]<=32767);}
   }
   assert(changes>3);forms++;
   float targets[8];memcpy(targets,e.role_target,sizeof targets);e.freeze=1;e.evolve_next=1;
   for(unsigned i=0;i<len*2;i++)step();assert(!memcmp(targets,e.role_target,sizeof targets));e.freeze=0;
   unsigned next=(scene+1)%SR_SCENES;sr_set_scene(&e,next);assert(e.pending_scene==next+1&&e.scene==scene);
   e.loop_pos=0;step();assert(e.scene==next&&!e.pending_scene);
   sr_stop(&e);for(unsigned i=0;i<8;i++)sr_render(&e,pcm,256);
   for(unsigned i=0;i<512;i++)assert(!pcm[i]);sr_set_scene(&e,scene);
  }
  profiles++;scenes|=1u<<scene;
 }
 assert(profiles==189&&scenes==0x1fffffu);
 /* Eight hours of section decisions, separately from real-time rendering. */
 sr_init(&e,123);e.variation=.8f;
 for(unsigned phrase=0;phrase<8u*60u*4u;phrase++){
  e.phrase=phrase;plan_roles(&e);unsigned section=song_section(&e);
  if(section==1||section==2||section==4)assert(e.role_target[7]==1);
 }
 printf("PASS %u complete profiles, %u variation/form scenarios; same-intention refresh, locks, A/B/A, queued selection, Freeze, STOP; deterministic 8h form decisions (not hardware soak)\n",profiles,forms);
}
