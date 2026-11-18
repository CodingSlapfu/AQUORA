#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static void setup(unsigned scene,unsigned mode,unsigned section)
{
 sr_init(&e,725);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
 sr_start(&e);sr_set_vocals(&e,1,4);e.form_loop=0;e.form_route[0]=section;
 e.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
}
static uint32_t audio(unsigned *heard)
{
 int16_t pcm[256];uint32_t hash=2166136261u;
 for(unsigned n=0;n<350;n++){
  sr_render(&e,pcm,128);
  *heard+=e.vocal_direct_peak>.0001f;
  for(unsigned i=0;i<256;i++)hash=(hash^(uint16_t)pcm[i])*16777619u;
 }
 return hash;
}
int main(void)
{
 float lo[VF_COUNT],hi[VF_COUNT];
 for(unsigned id=0;id<VF_COUNT;id++){lo[id]=101;hi[id]=-1;}
 unsigned heard=0,cases=0;uint32_t previous=0;
 const unsigned sections[]={0,2,12,8,14};
 for(unsigned scene=0;scene<SR_SCENES;scene++)for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
  setup(scene,mode,2);
  for(unsigned family=0;family<5;family++){
   e.form_route[0]=sections[family];e.current_energy=family%2?.85f:.25f;
   vocal_production(&e,10000);
   for(unsigned id=0;id<VF_COUNT;id++){
    float v=e.vocal_param_now[id];assert(isfinite(v)&&v>=0&&v<=100);
    if(v<lo[id])lo[id]=v;if(v>hi[id])hi[id]=v;
   }
  }
  e.form_route[0]=2;
  unsigned on=e.vocals_enabled,presence=e.vocal_presence,level=e.vocal_level,send=e.vocal_fx_send;
  uint32_t hash=audio(&heard);assert(hash!=previous);previous=hash;
  assert(e.vocals_enabled==on&&e.vocal_presence==presence&&e.vocal_level==level&&e.vocal_fx_send==send);
  cases++;
 }
 assert(heard>1000);
 for(unsigned id=0;id<VF_COUNT;id++){
  printf("AUTO %u target range %.2f..%.2f\n",id,lo[id],hi[id]);assert(hi[id]-lo[id]>2.f);
 }
 setup(3,SR_STANDARD,2);
 assert(vocal_delivery(&e,0)==1&&vocal_delivery(&e,2)==4);
 unsigned identity=vocal_character(&e);assert(vocal_delivery(&e,1)==identity);
 e.music_mode=SR_CINEMATIC;assert(vocal_delivery(&e,2)==0);
 e.music_mode=SR_IDM;assert(vocal_delivery(&e,2)==3);
 /* Every manual parameter and all three main FX locks beat automation. */
 e.vocal_param_lock=(1u<<VF_COUNT)-1u;e.vocal_fx_manual=15;
 e.vocal_chorus=7;e.vocal_reverb=11;e.vocal_delay=13;e.vocal_tune=0;
 for(unsigned id=0;id<VF_COUNT;id++)e.vocal_param[id]=(id*7u)%101u;
 for(unsigned scene=0;scene<SR_SCENES;scene++){
  e.scene=scene;e.music_mode=scene%SR_MUSIC_MODES;e.current_energy=scene%2;
  vocal_production(&e,10000);
  for(unsigned id=0;id<VF_COUNT;id++)assert(e.vocal_param_now[id]==e.vocal_param[id]);
  assert(e.vocal_chorus_now==7&&e.vocal_room_now==11&&e.vocal_echo_now==13);
 }
 sr_voice v={0};v.midi=sr_tonic(&e)+25;v.freq=sr_note_frequency(&e,v.midi);
 assert(vocal_pitch_target(&e,&v)==v.freq);
 e.vocal_fx_manual=0;assert(vocal_pitch_target(&e,&v)!=v.freq);
 /* Saved hook cannot replace a manual HUM choice, including its onset. */
 setup(0,SR_STANDARD,12);e.vocal_articulation=7;e.vocal_reprise=1;
 e.vocal_hook_vowel[0]=VOCAL_YOU_ZONE+1;e.vocal_hook_attack[0]=8;
 assert(recorded_vocal_phrase(&e,0,0));
 unsigned found=0;
 for(unsigned i=0;i<SR_VOICES;i++)if(e.voices[i].active&&e.voices[i].sample_zone){
  assert(e.voices[i].sample_zone==VOCAL_HUM_ZONE+1);assert(!e.voices[i].vocal_consonant);found++;
 }
 assert(found);
 sr_set_vocals(&e,0,4);unsigned started=e.vocal_started;
 audio(&heard);assert(!e.vocals_enabled&&e.vocal_started==started);
 sr_set_vocals(&e,1,1);e.vocal_presence=0;audio(&heard);assert(!e.vocal_presence&&e.vocal_started==started);
 printf("PASS %u intention/mode direct-vocal PCM cases; 550 section profiles, all 20 AUTO ranges, locks, tune zero, articulation recall and OFF/presence zero\n",cases);
}
