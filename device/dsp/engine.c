#include "serenity.h"
#ifdef SR_DEVICE
#include "fastmath.h"
#define sinf fm_sinf
#define expf fm_expf
#define fabsf(x) ((x)<0.f?-(x):(x))
#define fmaxf(a,b) ((a)>(b)?(a):(b))
#define floorf fm_floorf
#define isfinite(x) (fm_fabsf(x) <= 3.402823466e38f)
extern void *memset(void *, int, size_t);
extern void *memcpy(void *, const void *, size_t);
#else
#include <math.h>
#include <string.h>
#endif

#define PI 3.14159265358979323846f
#define LUT 1024
#define LUT_MASK (LUT-1)
static float sin_table[LUT+1];
static uint8_t lut_ready = 0;

const sr_patch sr_patches[SR_PATCHES] = {
  {"Earth Sub", SR_SINE, .028f, .72f, .33f, .39f},
  {"Warm Temple Bass", SR_TRI, .02f, .82f, .37f, .31f},
  {"Sacred Pulse", SR_HOLLOW, .08f, 1.35f, .20f, .23f},
  {"Deep Halo Drone", SR_SINE, 1.8f, 3.4f, .28f, .23f},
  {"Evolving Warmth", SR_CHOIR_WAVE, 1.0f, 3.0f, .42f, .21f},
  {"Celestial Air", SR_HOLLOW, 2.4f, 4.2f, .33f, .18f},
  {"Felt Piano", SR_PLUCK, .004f, 1.25f, .52f, .32f},
  {"Soft Grand", SR_PLUCK, .004f, 1.85f, .58f, .29f},
  {"Glass Piano", SR_PLUCK, .003f, 2.8f, .68f, .19f},
  {"Nylon Guitar", SR_PLUCK, .008f, .85f, .43f, .25f},
  {"Harp Strings", SR_PLUCK, .006f, 1.50f, .72f, .22f},
  {"Soft Dulcimer", SR_PLUCK, .007f, 1.85f, .62f, .20f},
  {"Breath Flute", SR_SINE, .18f, 1.0f, .32f, .25f},
  {"Wooden Flute", SR_HOLLOW, .12f, 1.1f, .26f, .24f},
  {"Bamboo Air", SR_SINE, .34f, 1.7f, .19f, .22f},
  {"English Horn", SR_REED, .22f, 1.4f, .53f, .21f},
  {"Warm Clarinet", SR_REED, .11f, 1.1f, .37f, .22f},
  {"Crystal Bowl", SR_HOLLOW, .004f, 4.6f, .84f, .20f},
  {"Silver Bell", SR_HOLLOW, .003f, 3.3f, .72f, .18f},
  {"Angelic Voices", SR_CHOIR_WAVE, .80f, 3.8f, .48f, .23f},
  {"Chamber Strings", SR_REED, .65f, 2.8f, .30f, .22f},
  {"Viola Mist", SR_REED, .45f, 2.4f, .22f, .23f},
  {"Ocean Glass", SR_HOLLOW, 1.2f, 4.5f, .33f, .20f},
  {"Linen Pad", SR_CHOIR_WAVE, 1.5f, 3.9f, .25f, .23f},
  {"Moon Glass Keys", SR_SINE, .006f, 2.2f, .55f, .23f},
  {"Halo Choir FM", SR_SINE, 1.1f, 3.7f, .40f, .22f},
  {"Silk Harmonics", SR_SINE, 1.8f, 4.0f, .33f, .20f},
  {"Rain Harp", SR_PLUCK, .009f, 1.7f, .48f, .23f},
  {"Deep Resonant Bass", SR_SINE, .028f, .86f, .29f, .33f},
  {"Velvet Reed FM", SR_SINE, .19f, 1.5f, .45f, .24f}
};
#define SR_FIRST_FM_PATCH 24u
static const struct {
  float ratio, depth, envelope, detune;
} fm_operators[] = {
  {3.01f,.17f,.80f,.000f}, {2.f,.15f,.22f,.003f},
  {1.4142f,.14f,.30f,.001f}, {2.75f,.22f,.85f,.000f},
  {1.f,.12f,.38f,.000f}, {3.f,.18f,.46f,.002f}
};
_Static_assert(sizeof fm_operators / sizeof *fm_operators == SR_PATCHES-SR_FIRST_FM_PATCH,
               "Every FM patch needs an operator profile");
const sr_scene sr_scenes[SR_SCENES] = {
  {"Stillness",43,0,1,1,45,0}, {"Flow State",58,1,2,2,45,3},
  {"Radiance",68,0,3,3,45,8}, {"Grounding",48,2,2,4,41,2},
  {"Dreaming",52,3,2,5,45,6}, {"Ceremony",61,4,3,1,48,5},
  {"Anti-Curse",51,2,2,4,43,7}, {"Positive Healing",63,4,2,2,48,4},
  {"Creativity",66,1,3,7,46,3}, {"Inspiration",55,4,2,6,50,6},
  {"Luck",71,0,3,3,43,11}, {"Abundance",49,3,1,5,41,12},
  {"Attract Positivity",64,4,3,2,48,8}
};
const sr_frequency sr_frequencies[SR_FREQUENCIES] = {
  {"174 Hz",174.f}, {"285 Hz",285.f}, {"396 Hz",396.f},
  {"417 Hz",417.f}, {"528 Hz",528.f}, {"639 Hz",639.f},
  {"741 Hz",741.f}, {"852 Hz",852.f}, {"963 Hz",963.f},
  {"111 Hz",111.f}, {"136.1 Hz",136.1f}, {"777 Hz",777.f}, {"888 Hz",888.f}
};
const sr_drum_kit sr_drum_kits[SR_DRUM_KITS] = {
  {"River Hands", .75f,.42f,.23f,49,175,.99948f,.13f},
  {"Willow Brushes", .80f,.58f,.35f,49,175,.99948f,.13f},
  {"Hollow Earth", 1.00f,.29f,.16f,49,175,.99948f,.13f},
  {"Moonlit Felt", .89f,.52f,.24f,57,133,.99930f,.08f},
  {"Temple Air", .73f,.68f,.38f,67,255,.99942f,.17f},
  {"River Stones", .81f,.65f,.20f,60,328,.99924f,.24f},
  {"Warm Pulse", .97f,.40f,.19f,55,148,.99958f,.09f},
  {"Star Dust", .63f,.53f,.48f,69,413,.99921f,.33f},
  {"Rain Shells", .70f,.52f,.50f,64,286,.99935f,.27f},
  {"Willow Brush", .66f,.51f,.61f,58,204,.99937f,.38f},
  {"Crystal Taps", .72f,.75f,.28f,71,472,.99928f,.21f},
  {"Soft Studio", .94f,.55f,.35f,58,190,.99949f,.23f},
  {"Lunar Tide", .99f,.39f,.25f,47,160,.99966f,.12f},
  {"Sacred Earth", .96f,.67f,.30f,52,124,.99960f,.11f},
  {"Feather Ritual", .76f,.58f,.52f,66,243,.99931f,.31f},
  {"FM Moon Kit", .93f,.42f,.18f,50,154,.99955f,.14f},
  {"Temple Resonance", .87f,.62f,.24f,56,213,.99943f,.13f},
  {"Warm Pocket", .97f,.51f,.30f,54,168,.99946f,.27f},
  {"Stardust FM", .69f,.52f,.43f,65,337,.99926f,.24f}
};
const float sr_binaural_presets[SR_BEAT_PRESETS] = {1.f, 2.f, 4.f, 6.f, 8.f, 10.f, 14.f, 20.f};
static const uint16_t rv_len[4]={1061,1277,1453,1789};
static const int8_t scale_notes[5][7] = {
  {0,2,4,7,9,12,14}, {0,2,3,5,7,10,12}, {0,3,5,7,10,12,15},
  {0,2,5,7,9,12,14}, {0,2,4,5,7,9,11}
};
static const uint8_t default_patches[SR_TRACKS]={12,1,3,15,6,9,19,0};
static inline __attribute__((always_inline)) float clampf(float x,float lo,float hi){return x<lo?lo:(x>hi?hi:x);}
static inline __attribute__((always_inline)) uint32_t rng(sr_engine *e){e->rng=e->rng*1664525u+1013904223u;return e->rng;}
static inline __attribute__((always_inline)) float rnd(sr_engine *e){return (float)(rng(e)>>8)*(1.0f/16777216.0f);}
static inline __attribute__((always_inline)) float sine(float phase){
  float p=phase*(float)LUT; int i=(int)p; float f=p-(float)i;
  return sin_table[i&LUT_MASK]*(1.0f-f)+sin_table[(i+1)&LUT_MASK]*f;
}
static inline __attribute__((always_inline)) float wave(sr_wave w,float phase,float brightness){
  float a=sine(phase);
  switch(w){
   case SR_SINE:return a + brightness*.12f*sine(phase*2.f);
   case SR_TRI:return .79f*a + brightness*.16f*sine(phase*3.f);
   case SR_HOLLOW:return .72f*a + brightness*.23f*sine(phase*2.f);
   case SR_REED:return .56f*a + brightness*.27f*sine(phase*3.f) + brightness*.11f*sine(phase*5.f);
   case SR_PLUCK:return .62f*a + brightness*.25f*sine(phase*2.f) + brightness*.11f*sine(phase*3.f);
   case SR_CHOIR_WAVE:return .68f*a + .22f*sine(phase*3.f) + brightness*.13f*sine(phase*2.f);
   default:return a;
  }
}
float sr_note_frequency(const sr_engine *e,int midi){
  return (e->tuning_432?432.f:440.f)*expf(((float)midi-69.f)*.05776226505f);
}
static inline __attribute__((always_inline)) float sacred_hz(const sr_engine *e){
  float hz=sr_frequencies[e->frequency].hz;
  while(hz>110.f) hz*=.5f;
  while(hz<38.f) hz*=2.f;
  return hz;
}
float sr_sacred_bass_frequency(const sr_engine *e){return sacred_hz(e);}
static inline __attribute__((always_inline)) float patch_tone(const sr_patch *patch,const sr_track *track)
{
  return clampf(patch->brightness+.075f*track->tone,0.f,1.f);
}
static inline float patch_time(float seconds,int edit)
{
  return seconds*(1.f+.125f*edit);
}
float sr_patch_preview(const sr_track *track,float phase)
{
  const sr_patch *patch=&sr_patches[track->patch];
  float tone=patch_tone(patch,track);
  if(track->patch>=SR_FIRST_FM_PATCH){
    unsigned op=track->patch-SR_FIRST_FM_PATCH;
    return sine(1.f+phase+fm_operators[op].depth*(.25f+.75f*tone)*
                sine(phase*(fm_operators[op].ratio+fm_operators[op].detune)));
  }
  return wave(patch->wave,phase,tone);
}
float sr_patch_envelope(const sr_track *track,float seconds,int releasing)
{
  const sr_patch *patch=&sr_patches[track->patch];
  float time=releasing?fmaxf(patch_time(patch->release,track->release),.04f):
    fmaxf(patch_time(patch->attack,track->attack),.001f);
  float decay=expf(-seconds/time);
  return releasing?decay:1.f-decay;
}
static void make_motif(sr_engine *e){
  for(int i=0;i<16;i++){
    if((i%4)==0) e->melody[i]=(int8_t)(rng(e)%5);
    else if((rng(e)&7u)==0) e->melody[i]=-1;
    else e->melody[i]=(int8_t)(rng(e)%7);
  }
}
uint8_t sr_tonic(const sr_engine *e)
{
  return e->tonic_override?e->tonic_override:sr_scenes[e->scene].tonic;
}
static int motif_degree(const sr_engine *e,unsigned step)
{
  int degree=e->melody[step&15u];
  unsigned bar=step/16u;
  if(!bar||degree<0)return degree;
  if((step&7u)==0)return (int)(bar+e->scene)%5;
  return (degree+(int)bar*2)%7;
}
void sr_set_loop_length(sr_engine *e,uint8_t length)
{
  if(length!=16&&length!=32&&length!=64)return;
  if(e->running)e->pending_length=length;
  else{e->loop_length=length;e->pending_length=0;}
}

static void retune_keys(sr_engine *e)
{
  for(unsigned i=0;i<SR_VOICES;i++){
    sr_voice *v=&e->voices[i];
    if(v->active&&v->gate&&v->key){
      v->midi=sr_key_note(e,v->key-1);
      v->freq=sr_note_frequency(e,v->midi);
      if(v->track>SR_BASS||!e->motion[v->track].glide_mode)v->pitch=v->freq;
    }
  }
}
/* Each palette favors a particular member of each role's compatible pool.
 * Seeded draws are separate from the render RNG and from one another. */
static uint32_t song_draw(uint32_t seed,uint32_t stream)
{
  uint32_t x=seed+0x9e3779b9u*(stream+1u);
  x=(x^(x>>16))*0x7feb352du;
  x=(x^(x>>15))*0x846ca68bu;
  return x^(x>>16);
}
void sr_select_nature(sr_engine *e,uint32_t seed)
{
  if(e->sound_locks&SR_SOUND_NATURE)return;
  static const uint8_t palette[SR_SCENES][4]={
    {1,4,8,7},{2,10,5,11},{3,9,6,10},{4,8,7,1},
    {5,8,11,2},{1,4,7,11},{4,8,11,1},{2,10,9,5},
    {7,9,3,10},{6,9,4,2},{3,6,9,10},{5,4,8,1},{2,9,6,10}
  };
  unsigned style=e->nature_style;
  unsigned draw=song_draw(seed,8)%8u;
  unsigned accent=e->energy>.55f||e->tracks[SR_PIANO].patch==24;
  unsigned pick=draw<4?(style+accent)%4u:(draw-4u);
  e->bed_type=palette[e->scene][pick];
}
void sr_generate_song(sr_engine *e,uint32_t seed,int sounds_only)
{
  if(e->running||e->stop_frames)return;
  static const uint8_t pool[SR_DRUMS][4]={
    {12,13,14,16},{0,1,2,1},{3,4,5,23},{15,16,20,21},
    {6,7,8,7},{9,10,11,17},{19,23,4,20}
  };
  static const struct {
    uint8_t flavor,kits[3],density,hit,space;
  } palette[SR_SCENES]={
    {2,{0,1,2},2,18,3}, {0,{0,3,7},3,35,2},
    {1,{7,8,10},4,42,3}, {0,{2,5,13},2,24,1},
    {2,{3,8,12},2,21,4}, {3,{4,10,13},3,35,4},
    {0,{13,2,14},2,20,2}, {1,{3,4,11},3,30,3},
    {3,{8,10,11},4,48,2}, {2,{4,8,12},3,32,4},
    {1,{6,7,12},4,45,2}, {0,{2,5,14},2,25,3},
    {3,{3,10,14},3,39,3}
  };
  unsigned scene=e->scene;
  if(!sounds_only){
    e->rng=song_draw(seed,12);
    e->drum_rng=song_draw(seed,13);
    e->nature_rng=song_draw(seed,90);
    e->phrase=0;
    make_motif(e);
    memcpy(e->motif_home,e->melody,sizeof e->melody);
    e->energy=.12f+.17f*sr_scenes[scene].energy+.025f*(song_draw(seed,14)%7u);
    e->variation=.18f+.04f*(song_draw(seed,15)%7u);
  }
  static const uint8_t fm_patch[SR_DRUMS]={29,28,26,29,24,27,25};
  for(unsigned t=0;t<SR_DRUMS;t++){
    if(e->sound_locks&(1u<<t))continue;
    unsigned draw=song_draw(seed,t)%7u;
    unsigned favorite=(palette[scene].flavor+t)%4u;
    unsigned pick=draw<4?favorite:(favorite+draw-3u)%4u;
    if(pool[t][pick]==e->tracks[t].patch&&(song_draw(seed,20u+t)&3u))pick=(pick+1u)%4u;
    e->tracks[t].patch=song_draw(seed,40u+t)%9u<2u?fm_patch[t]:pool[t][pick];
  }
  if(!(e->sound_locks&(1u<<SR_DRUMS))){
    unsigned k=song_draw(seed,7)%5u;
    unsigned pick=k<3?0:k-2u;
    if(palette[scene].kits[pick]==e->drum_kit&&(song_draw(seed,31)&3u))pick=(pick+1u)%3u;
    e->drum_kit=song_draw(seed,48)%5u==0?15u+(palette[scene].flavor+pick)%4u:palette[scene].kits[pick];
    if(!sounds_only){
      e->drum_density=palette[scene].density+song_draw(seed,16)%2u;
      e->drum_hit=palette[scene].hit+song_draw(seed,17)%11u;
    }
  }
  sr_select_nature(e,seed);
  if(!(e->sound_locks&SR_SOUND_FX)){
    e->reverb_amount=.12f+.045f*palette[scene].space+.01f*(song_draw(seed,9)%8u);
    e->delay_amount=.06f+.025f*palette[scene].space+.01f*(song_draw(seed,10)%6u);
  }
}
void sr_set_scene(sr_engine *e,uint8_t scene){
  if(scene>=SR_SCENES)return;
  if(scene!=e->scene)e->play_queued=0;
  if(e->running&&scene!=e->scene)for(int i=0;i<SR_VOICES;i++){
    sr_voice *v=&e->voices[i];
    if(!v->key){e->note_offs+=v->gate!=0;v->gate=0;v->remain=0;v->release_coef=.0012f;}
  }
  e->scene=scene; e->tick_length=(uint32_t)((float)SR_RATE*60.f/(float)sr_scenes[scene].bpm/4.f);
  sr_select_nature(e,e->rng);
  if(e->frequency_auto)e->frequency=sr_scenes[scene].frequency;
  static const sr_motion intention_motion[SR_SCENES][2]={
    {{120,0,0,45},{90,0,0,32}}, {{105,1,5,44},{125,1,0,31}},
    {{70,1,4,49},{75,0,0,33}}, {{135,1,3,39},{165,1,0,29}},
    {{185,1,8,36},{175,1,2,28}}, {{150,1,6,41},{135,1,1,30}},
    {{100,1,3,46},{135,1,0,32}}, {{120,1,5,43},{125,1,0,32}},
    {{75,1,7,48},{90,0,0,34}}, {{145,1,7,40},{130,1,1,30}},
    {{65,1,4,51},{70,0,0,35}}, {{170,1,6,38},{165,1,1,29}},
    {{95,1,6,46},{110,1,0,32}}
  };
  for(unsigned t=0;t<=SR_BASS;t++)
    if(!(e->sound_locks&(1u<<t)))e->motion[t]=intention_motion[scene][t];
  if(scene==6||scene==7){
    if(!(e->sound_locks&(1u<<SR_DRUMS)))e->drum_kit=scene==6?13:3;
    if(!(e->sound_locks&SR_SOUND_FX))e->reverb_amount=scene==6?.37f:.41f;
    if(!(e->sound_locks&SR_SOUND_FX))e->delay_amount=scene==6?.13f:.22f;
    e->energy=scene==6?.42f:.58f;
  }
  if(scene>=8){
    static const uint8_t kits[]={8,4,12,2,10};
    if(!(e->sound_locks&(1u<<SR_DRUMS)))e->drum_kit=kits[scene-8];
    e->energy=.25f*sr_scenes[scene].energy;
    if(!(e->sound_locks&SR_SOUND_FX))e->reverb_amount=.22f+(scene-8)*.05f;
    if(!(e->sound_locks&SR_SOUND_FX))e->delay_amount=.11f+(12-scene)*.035f;
  }
  make_motif(e);
  memcpy(e->motif_home,e->melody,sizeof e->melody);
  memset(e->ornaments,0,sizeof e->ornaments);
  retune_keys(e);
}
void sr_set_frequency_lock(sr_engine *e,uint8_t enabled){
  e->pending_lock=!!enabled;
  if(e->running&&e->pending_lock!=e->frequency_lock){
    sr_stop(e);
    e->restart=1;
  }else if(!e->stop_frames)e->frequency_lock=e->pending_lock;
}
void sr_set_frequency_auto(sr_engine *e,uint8_t enabled){
  e->frequency_auto=!!enabled;
  if(enabled)e->frequency=sr_scenes[e->scene].frequency;
}
void sr_set_frequency(sr_engine *e,uint8_t f){
  if(f<SR_FREQUENCIES){e->frequency=f;e->frequency_auto=0;}
}
void sr_set_perfect_key(sr_engine *e,uint8_t on){e->perfect_key=on?1:0;retune_keys(e);}
void sr_set_tuning_432(sr_engine *e,uint8_t on){e->tuning_432=on?1:0;retune_keys(e);}
void sr_set_drums(sr_engine *e,uint8_t enabled){e->drums_enabled=enabled?1:0;}
void sr_set_drum_level(sr_engine *e,float level){if(isfinite(level))e->drum_level=clampf(level,0.f,1.f);}
void sr_set_drum_density(sr_engine *e,uint8_t density){if(density<=SR_DENSITY_MAX)e->drum_density=density;}
void sr_set_drum_kit(sr_engine *e,uint8_t kit){if(kit<SR_DRUM_KITS)e->drum_kit=kit;}
void sr_set_binaural(sr_engine *e,uint8_t enabled){e->binaural_enabled=enabled?1:0;}
void sr_set_binaural_rate(sr_engine *e,float hz){if(isfinite(hz)&&hz>=.5f&&hz<=40.f)e->binaural_rate=hz;}
void sr_set_binaural_carrier(sr_engine *e,float hz){if(isfinite(hz)&&hz>=80.f&&hz<=1000.f)e->binaural_carrier=hz;}
void sr_set_track_patch(sr_engine *e,uint8_t t,uint8_t patch){if(t<SR_TRACKS&&patch<SR_PATCHES)e->tracks[t].patch=patch;}
void sr_init(sr_engine *e,uint32_t seed){
  memset(e,0,sizeof(*e)); e->rng=seed?seed:0x73ac05u;
  if(!lut_ready){for(int i=0;i<=LUT;i++)sin_table[i]=sinf((float)i*2.f*PI/(float)LUT);lut_ready=1;}
  e->master=.78f; e->reverb_amount=.32f; e->delay_amount=.18f; e->bed_level=.18f;
  e->drums_enabled=1;e->drum_level=.66f;e->drum_presence=50;e->groove_strength=100;e->air_track=SR_TRACKS;e->drum_density=2;e->drum_hit=25;e->drum_rng=e->rng^0x6a09e667u;e->drum_kit=0;
  e->binaural_enabled=0;e->binaural_carrier=220.f;e->binaural_rate=6.f;
  e->binaural_level=.035f;e->tone_level=0.f;e->energy=.5f;e->variation=.3f;e->carrier_now=220.f;e->beat_now=6.f;
  for(int t=0;t<SR_TRACKS;t++){
    e->gain_trim[t]=1.f;
    e->tracks[t].patch=default_patches[t];e->tracks[t].volume=t==SR_BASS?.88f:t==SR_DRUMS?1.f:.68f; e->tracks[t].send=1.f;
    e->tracks[t].pan=(float)(t%4-1)*.13f;
  }
  e->perfect_key=1; e->solo=SR_TRACKS; e->sacred_level=.08f; e->sacred_mode=1; e->sacred_harmonics=.32f;
  e->motion[0]=(sr_motion){120,0,0,45}; e->motion[1]=(sr_motion){90,0,0,32};
  e->loop_length=16;
  e->play_grid=3;
  e->play_strength=100;
  e->frequency_auto=1;
  e->nature_rng=song_draw(seed,90);
  e->nature_blend=1.f;
  sr_set_scene(e,0);
}
/* The old transport fades as a unit. A restart clears all energy before
 * generating a fresh downbeat, even when STOP and PLAY share a block. */
static void clear_transport(sr_engine *e)
{
    e->play_queued=0;
    e->play_released=0;
    memset(e->ornaments,0,sizeof e->ornaments);
    for(unsigned i=0;i<SR_VOICES;i++)e->note_offs+=e->voices[i].gate!=0;
    memset(e->voices,0,sizeof(e->voices));
    memset(e->rev_pos,0,sizeof(e->rev_pos));
    e->delay_pos=0;
    e->rev_history=e->delay_history=0;
    e->reverb_wet=e->delay_wet=0;
    e->fx_mask=0;
    e->lock_phase=0;
    e->nature_audition=e->bird_wait=0;e->nature_peak=e->master_peak=0;
    e->kick_env=e->hand_env=e->shaker_env=e->drum_meter=0;
    e->binaural_env=e->transport_env=e->sacred_gain=e->limiter=0;
    e->wind_lp=e->rain_lp=e->hand_noise_lp=e->shaker_lp=e->bird_env=0;
    e->pitch_last[0]=e->pitch_last[1]=e->sacred_hz_now=0;
    for(int t=0;t<SR_TRACKS;t++)e->tracks[t].meter=0;
}
void sr_start(sr_engine *e)
{
    if(e->stop_frames){e->restart=1;return;}
    clear_transport(e);
    e->running=1;
    e->sequence_step=0;e->loop_pos=0;
    e->tick_phase=e->tick_length;
}
void sr_stop(sr_engine *e)
{
    if(e->running)e->stop_frames=256;
    e->restart=0;
    e->running=0;
}
static sr_voice *note_on(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,float beats){
  if(!e->running||track>=SR_DRUMS||midi>127||!velocity||e->tracks[track].mute)return 0;
  int slot=-1;float quiet=1.e9f;
  for(int i=0;i<SR_VOICES;i++){
    if(!e->voices[i].active){slot=i;break;}
    float score=e->voices[i].env+(e->voices[i].track==SR_BASS?.35f:e->voices[i].key&&e->voices[i].gate?.25f:0);
    if(score<quiet){quiet=score;slot=i;}
  }
  sr_voice *v=&e->voices[slot];e->note_offs+=v->gate!=0;e->note_ons++;float tail=v->active?v->filter*v->env*sr_patches[e->tracks[v->track].patch].gain:0.f;memset(v,0,sizeof(*v));v->tail=tail;v->active=v->gate=1;v->track=track;
  const sr_patch *p=&sr_patches[e->tracks[track].patch];
  v->born=e->frame_count;v->freq=sr_note_frequency(e,midi);v->pitch=v->freq;v->midi=midi;
  if(track<=SR_BASS){
    sr_motion *m=&e->motion[track];
    int held=0;
    for(int j=0;j<SR_VOICES;j++)if(&e->voices[j]!=v && e->voices[j].active &&
        e->voices[j].gate && e->voices[j].track==track)held=1;
    if((m->glide_mode==2||(m->glide_mode==1&&held))&&e->pitch_last[track]>0)
        v->pitch=e->pitch_last[track];
    e->pitch_last[track]=v->freq;
  }
  v->increment=v->pitch*(1.f/SR_RATE);
  e->tracks[track].events++;
  v->peak=(float)velocity/127.f;
  v->attack_coef=1.f-expf(-1.f/(fmaxf(patch_time(p->attack,e->tracks[track].attack),.001f)*SR_RATE));
  v->release_coef=1.f-expf(-1.f/(fmaxf(patch_time(p->release,e->tracks[track].release),.04f)*SR_RATE));
  v->remain=(uint32_t)clampf(beats*4.f*(float)e->tick_length,1.f,9.e6f);
  return v;
}
void sr_note_on(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,float beats)
{
    (void)note_on(e,track,midi,velocity,beats);
}
uint8_t sr_key_note(const sr_engine *e,unsigned key)
{
    if(!e->perfect_key)return (uint8_t)(53+key);
    int root=sr_tonic(e)+12;
    int degree=0;
    for(int note=root;note<=127;note++){
      for(int i=0;i<7;i++)if((note-root)%12==scale_notes[sr_scenes[e->scene].scale][i]%12){
        if((unsigned)degree++==key)return (uint8_t)note;
        break;
      }
    }
    return 127;
}
static void play_key(sr_engine *e,uint8_t track,unsigned key,unsigned tap)
{
    if(!e->perfect_key)sr_set_perfect_key(e,1);
    if(track==SR_DRUMS){
      if(!e->drums_enabled)return;
      if(key%3==0)e->kick_env=1;
      else if(key%3==1)e->hand_env=1;
      else e->shaker_env=1;
      e->tracks[SR_DRUMS].events++;
      return;
    }
    for(unsigned i=0;i<SR_VOICES;i++)if(e->voices[i].gate&&e->voices[i].key==key+1)return;
    sr_voice *voice=note_on(e,track,sr_key_note(e,key),96,1.f);
    if(voice){
      voice->key=tap?0:(uint8_t)(key+1);
      voice->remain=tap?(e->tick_length/4u+1u):SR_RATE*8u;
    }
}
static const uint8_t intention_feel[SR_SCENES]={0,5,3,2,7,4,2,4,6,7,4,3,5};

static uint32_t play_delay(const sr_engine *e,unsigned track)
{
    uint32_t length=e->tick_length, phase=e->tick_phase;
    unsigned grid=e->play_grid;
    if(grid==3)grid=length>7400u?2u:1u;
    unsigned step=e->sequence_step-1u;
    for(unsigned ahead=0;ahead<=2;ahead++){
      if(grid==4&&((step+ahead)&1u))continue;
      unsigned swing=e->play_feel&&track!=SR_DRUMS&&track!=SR_BASS?
        length*intention_feel[e->scene]/64u:0;
      uint32_t target=ahead*length+(((step+ahead)&1u)?swing:0);
      if(target>=phase)return ((target-phase)*e->play_strength+50u)/100u;
      if(grid==2){
        target=ahead*length+length/2u+swing;
        if(target>=phase)return ((target-phase)*e->play_strength+50u)/100u;
      }
    }
    return 0;
}

void sr_key_down(sr_engine *e,uint8_t track,unsigned key)
{
    if(!e->running||key>=27||track>=SR_TRACKS)return;
    if(e->keyboard_mode){
      e->pending_key=(uint8_t)((53u+key)%12u+1u);
      e->pending_physical_key=(uint8_t)(key+1u);
      return;
    }
    if(e->play_grid&&e->play_strength){
      e->play_due[key]=e->frame_count+play_delay(e,track);
      e->play_track[key]=track;
      e->play_released&=~(1u<<key);
      e->play_queued|=1u<<key;
    }else play_key(e,track,key,0);
}
void sr_key_up(sr_engine *e,unsigned key)
{
    if(key>=27)return;
    e->play_released|=e->play_queued&(1u<<key);
    for(int j=0;j<SR_VOICES;j++)if(e->voices[j].key==key+1){
      e->note_offs+=e->voices[j].gate!=0;
      e->voices[j].gate=0;

    }
}
static void drum_trigger(sr_engine *e,uint32_t step,uint8_t energy){
  if(!e->drums_enabled||!e->drum_density||e->drum_level<=0.f)return;
  uint32_t beat=step&15u;
  unsigned bar=e->loop_length>16?step/16u:0;
  if(e->variation>0.f&&e->phrase)bar^=(e->phrase/2u)&3u;
  unsigned hits=0;
  if(beat==0 || (beat==8 && e->drum_density>=2)){e->kick_env=1.f;hits++;}
  if((beat==4||beat==12) && (e->drum_density>=2 || (rng(e)&3u)==0)){
    e->hand_env=bar&&(beat==4?bar&1u:bar&2u)?.78f:1.f;hits++;
  }
  if(e->drum_density>=3 && (beat&1u)){e->shaker_env=.72f;hits++;}
  else if(e->drum_density==2 && (beat&3u)==2){e->shaker_env=.65f;hits++;}
  else if(e->drum_density==1 && beat==8 && (rng(e)&1u)==0){e->shaker_env=.45f;hits++;}
  if(e->drum_density>=4&&(beat&3u)==2){e->shaker_env=.56f;hits++;}
  if(e->drum_density>=5&&beat==(bar&1u?10u:6u)){e->hand_env=.48f;hits++;}
  if(e->drum_density>=6&&beat==(bar&1u?6u:10u)){e->kick_env=.54f;hits++;}
  if(e->drum_density>=7&&(beat==2||beat==14)){e->hand_env=.42f;hits++;}
  if(e->drum_density>=8&&(beat==3||beat==15)){e->hand_env=bar&1u?.52f:.37f;hits++;}
  /* One optional strike at most on an otherwise empty, eligible subdivision. */
  unsigned eligible=e->scene==6 ? (beat==3||beat==11) :
                    energy>=2 ? (beat==3||beat==7||beat==11||beat==15) :
                    (beat==7||beat==15);
  /* Answer bars add off-beat pickups, with a two-hit closing fill at full HIT. */
  if(bar&&e->drum_density>=2){
    if(bar&1u)eligible|=beat==5||beat==13;
    if(bar&2u)eligible|=beat==1||beat==9;
  }
  if(!hits&&eligible){
    e->drum_rng=e->drum_rng*1664525u+1013904223u;
    unsigned percent=e->drum_hit>SR_HIT_MAX?SR_HIT_MAX:e->drum_hit;
    if((e->drum_rng>>16)%100u<percent){
      if(beat==11||(bar&&(beat==5||beat==13)))e->hand_env=.55f;
      else e->shaker_env=.52f;
      e->optional_hits++;
      hits++;
    }
  }
  e->tracks[SR_DRUMS].events+=hits;
}
static inline __attribute__((always_inline)) float drum_sample(sr_engine *e){
  const sr_drum_kit *kit=&sr_drum_kits[e->drum_kit];float sound=0.f;
  unsigned fm=e->drum_kit>=15;
  if(e->kick_env>.0003f){
    float hz=kit->root+45.f*e->kick_env;
    e->kick_phase+=hz/(float)SR_RATE;if(e->kick_phase>=1.f)e->kick_phase-=1.f;
    float tone=fm?sine(1.f+e->kick_phase+.15f*e->kick_env*sine(e->kick_phase*2.f)):
                  sine(e->kick_phase)*.82f+sine(e->kick_phase*2.f)*.18f;
    sound+=tone*e->kick_env*kit->kick*.45f;
    e->kick_env*=kit->decay;
  }else e->kick_env=0.f;
  if(e->hand_env>.0003f){
    float noise=rnd(e)*2.f-1.f;
    e->hand_noise_lp+=(noise-e->hand_noise_lp)*kit->brush;
    e->hand_phase+=kit->skin*(1.f/SR_RATE);if(e->hand_phase>=1.f)e->hand_phase-=1.f;
    float tap=fm?sine(1.f+e->hand_phase+.21f*e->hand_env*sine(e->hand_phase*3.f)):
                 sine(e->hand_phase);
    sound+=(tap*.64f+e->hand_noise_lp*.36f)*e->hand_env*kit->hand*.43f;
    e->hand_env*=.99925f;
  }else e->hand_env=0.f;
  if(e->shaker_env>.0003f){
    float noise=rnd(e)*2.f-1.f;
    e->shaker_lp+=(noise-e->shaker_lp)*.35f;
    sound+=(noise-e->shaker_lp)*e->shaker_env*kit->shaker*.28f;
    e->shaker_env*=.9967f;
  }else e->shaker_env=0.f;
  sound*=1.14f;
  e->drum_meter=fmaxf(e->drum_meter*.9993f,fabsf(sound));
  return sound*e->drum_level;
}
static inline __attribute__((always_inline)) float advance(float *phase,float hz){
  *phase+=hz/(float)SR_RATE;
  if(*phase>=1.f)*phase-=1.f;
  return sine(*phase);
}
static inline __attribute__((always_inline)) void render_binaural(sr_engine *e,float *left,float *right){
  // Each ear gets a separate sinusoid, mixed AFTER shared FX to preserve
  // the ear-specific frequency difference. Never collapse to mono.
  float target=(e->running && e->binaural_enabled && e->solo==SR_TRACKS)?1.f:0.f;
  e->binaural_env+=(target-e->binaural_env)*.0009f;
  if(e->binaural_env<.00003f && !target)return;
  e->carrier_now+=(e->binaural_carrier-e->carrier_now)*.0005f;
  e->beat_now+=(e->binaural_rate-e->beat_now)*.0005f;
  float hz_left=e->carrier_now-e->beat_now*.5f;
  float hz_right=e->carrier_now+e->beat_now*.5f;
  *left+=advance(&e->binaural_phase_l,hz_left)*e->binaural_level*e->binaural_env;
  *right+=advance(&e->binaural_phase_r,hz_right)*e->binaural_level*e->binaural_env;
}
static const uint8_t ornament_track[]={SR_PIANO,SR_GUITAR,SR_LEAD,SR_HARMONY};
static const float ornament_gate[]={1.45f,1.7f,2.3f,1.8f};
_Static_assert(sizeof ornament_track==sizeof(((sr_engine *)0)->ornaments)/
    sizeof(((sr_engine *)0)->ornaments[0]), "Ornament slots must match roles");

/* Pitched offbeats may lean behind the shared step clock.
 * Bass, drums and the downbeat stay on the clock. Units are 1/64 step. */
static void ornament(sr_engine *e,unsigned slot,uint8_t midi,uint8_t velocity,float beats,
                     unsigned step,unsigned evolving)
{
  unsigned feel_units=e->groove_mode==1?0:e->groove_mode==2?6:intention_feel[e->scene];
  if(slot>=2){
    /* Fast patches tolerate a wider offset. Slow attacks already soften the pulse. */
    const sr_patch *patch=&sr_patches[e->tracks[ornament_track[slot]].patch];
    if(slot==2||patch->attack>.35f)feel_units=(feel_units+1u)/2u;
    if(e->tick_length>12000u)feel_units=(feel_units+1u)/2u;
  }
  unsigned offset=evolving&&!e->frequency_lock&&(step&15u)?
    (e->tick_length*feel_units*e->groove_strength+3200u)/6400u:0;
  if(offset&&offset<e->tick_length){
    e->ornaments[slot].phase=(uint16_t)offset;
    e->ornaments[slot].midi=midi;
    e->ornaments[slot].velocity=velocity;
  }else sr_note_on(e,ornament_track[slot],midi,velocity,beats);
}
static void trigger(sr_engine *e){
  if(!e->loop_pos){
    if(e->pending_length){e->loop_length=e->pending_length;e->pending_length=0;e->play_queued=0;}
    if(e->pending_key){
      e->tonic_override=36+e->pending_key-1;
      e->pending_key=e->pending_physical_key=0;
      for(unsigned i=0;i<SR_VOICES;i++){
        sr_voice *v=&e->voices[i];
        e->note_offs+=v->gate!=0;
        v->gate=0;v->remain=0;v->release_coef=.002f;
      }
    }
  }
  uint32_t absolute=e->sequence_step++;
  uint32_t s=absolute;
  unsigned idx=e->loop_pos++;
  if(e->loop_pos>=e->loop_length)e->loop_pos=0;
  if(e->loop_length!=16)s=idx;
  const sr_scene *sc=&sr_scenes[e->scene];
  int tonic=sr_tonic(e);
  uint8_t energy=(uint8_t)(1.f+e->energy*2.99f);
  /* The absolute counter survives phrase wrap; idx is only the loop position. */
  unsigned pace=e->evolve_pace?1u<<e->evolve_pace:(e->loop_length==16?2u:1u);
  if(absolute&&idx==0&&!e->freeze&&(e->evolve_next||
      ((absolute/e->loop_length)%pace==0&&e->variation>0.f&&rnd(e)<e->variation))){
    e->evolve_next=0;
    e->phrase++;
    if(e->phrase%4u==0)memcpy(e->melody,e->motif_home,sizeof e->melody);
    else make_motif(e);
  }
  int k=motif_degree(e,idx);
  /* At zero variation or before the first evolution, retain the original
   * voicings. Role decisions are otherwise independent of the audio RNG. */
  unsigned evolving=e->variation>0.f&&e->phrase!=0;
  unsigned section=(e->phrase/4u)%4u;
  unsigned bar=idx/16u;
  uint32_t phrase_seed=e->phrase*0x9e3779b9u+e->scene*31u+bar;
  unsigned contour=section==3?0u:section+1u;
  if(evolving){
    if(section==2&&energy>1)energy--;
    if(section==1&&energy<3)energy++;
  }
  drum_trigger(e,s,energy);
  if(evolving&&(s&15u)==0&&e->drums_enabled&&e->drum_density)
    e->kick_env*=.83f+.04f*(song_draw(phrase_seed,SR_DRUMS)&3u);
  if((s&7u)==0){
    int bass_note=tonic;
    if(e->scene>=6){
      static const uint8_t degrees[][4]={
        {0,2,4,1},{0,4,5,3},{0,5,3,1},{0,2,5,4},{0,4,1,5},{0,3,0,4},{0,4,5,2}
      };
      bass_note+=scale_notes[sc->scale][degrees[e->scene-6][(s/8u)&3u]];
    }
    unsigned bass_draw=evolving?song_draw(phrase_seed,SR_BASS):0;
    if(evolving&&contour&&(s&8u))
      bass_note=tonic+scale_notes[sc->scale][1u+bass_draw%4u];
    sr_note_on(e,SR_BASS,(uint8_t)bass_note,112-(bass_draw&7u),2.3f);
    if((s&15u)==0){
      unsigned pad=evolving&&contour?song_draw(phrase_seed,SR_DRONE)%3u:0;
      sr_note_on(e,SR_DRONE,(uint8_t)(tonic+12+scale_notes[sc->scale][pad]),
                 e->scene==6?86:76,8.f);
    }
  }
  if((s&3u)==0){
    unsigned draw=evolving?song_draw(phrase_seed,SR_PIANO+(s&15u)*8u):rng(e);
    if(draw%4u<energy){
      unsigned degree=(evolving?draw>>8:rng(e))%5u;
      int note=tonic+24+scale_notes[sc->scale][degree];
      ornament(e,0,(uint8_t)note,e->scene==7?91:e->scene==6?61:80,1.45f,s,evolving);
    }
  }
  if(k>=0 && (s%2u)==0){
    unsigned lead=evolving?song_draw(phrase_seed,SR_LEAD+(s&15u)*8u):rng(e);
    if(lead%5u<=energy){
      int note=tonic+24+scale_notes[sc->scale][k];
      ornament(e,2,(uint8_t)note,65+(evolving?(lead>>8)&7u:0),2.3f,s,evolving);
    }
    unsigned harmony=evolving?song_draw(phrase_seed,SR_HARMONY+(s&15u)*8u):rng(e);
    if(harmony%7u==0){
      unsigned degree=evolving&&contour?((unsigned)k+2u)%7u:(unsigned)k;
      int note=tonic+36+scale_notes[sc->scale][degree];
      ornament(e,3,(uint8_t)note,47,1.8f,s,evolving);
    }
  }
  if((s&7u)==4){
    unsigned draw=evolving?song_draw(phrase_seed,SR_GUITAR+(s&15u)*8u):rng(e);
    if(draw%4u<energy){
      unsigned degree=evolving?(draw>>8)%5u:(s/4u)%5u;
      int note=tonic+12+scale_notes[sc->scale][degree];
      ornament(e,1,(uint8_t)note,71,1.7f,s,evolving);
    }
  }
  if((s&15u)==0){
    unsigned degree=evolving&&contour?song_draw(phrase_seed,SR_CHOIR)%5u:(s/16u)%5u;
    int note=tonic+12+scale_notes[sc->scale][degree];
    sr_note_on(e,SR_CHOIR,(uint8_t)note,e->scene==6?80:65,7.f);
    sr_note_on(e,SR_HARMONY,(uint8_t)(note+12),52,3.8f);
    if(e->scene>=6){
      int third=scale_notes[sc->scale][e->scene==6?1:2];
      sr_note_on(e,SR_HARMONY,(uint8_t)(tonic+24+third),44,3.4f);
    }
  }
}
static float nature_noise(sr_engine *e)
{
  e->nature_rng=e->nature_rng*1664525u+1013904223u;
  return (float)(e->nature_rng>>8)*(2.f/16777216.f)-1.f;
}

static float birds(sr_engine *e,float noise)
{
  static const struct {
    uint16_t pitch, length;
    int16_t sweep;
    uint8_t notes;
  } calls[]={{1840,2800,950,3},{3200,1600,-1050,2},{2250,5100,-640,1},{2800,2000,620,4}};
  if(!e->bird_left){
    if(e->bird_wait){e->bird_wait--;return 0;}
    if(!e->bird_note){
      e->bird_phrase=(e->nature_rng>>20)&3u;
      e->bird_pan=noise*.52f;
    }
    unsigned call=e->bird_phrase;
    e->bird_left=calls[call].length;
    e->bird_env=1.f/e->bird_left;
    e->bird_pitch=calls[call].pitch+e->bird_note*170+noise*140.f;
    if(++e->bird_note==calls[call].notes){
      e->bird_note=0;
      e->bird_wait=2u*SR_RATE+((e->nature_rng>>12)&0x3ffffu);
    }else e->bird_wait=2400u+((e->nature_rng>>19)&4095u);
  }
  unsigned call=e->bird_phrase;
  float progress=e->bird_left*e->bird_env;
  float env=progress*(1.f-progress)*4.f;
  e->bird_left--;
  float hz=e->bird_pitch+calls[call].sweep*progress*progress;
  float fundamental=advance(&e->bird_phase,hz);
  return (fundamental*.077f+sine(e->bird_phase*2.f)*.014f+noise*.009f)*env*env;
}

static float bed_color(unsigned type,float wind,float rain,float air,float swell,float bird,float *pan)
{
  switch(type){
   case 1:return wind*.95f+rain*.19f;
   case 2:return (rain*.85f+air*.028f)*(.6f+swell*.55f);
   case 3:return rain*.58f+air*.085f;
   case 4:return wind*(.38f+swell*.17f)+air*.033f+bird*.53f;
   case 5:return (wind*.85f+rain*.85f)*swell;
   case 6:return bird+air*.015f;
   case 7:return air*.065f*swell;
   case 8:return wind*.24f+rain*.17f+bird*.19f+air*.046f*swell*swell;
   case 9:return wind*.38f+air*.045f*swell+bird*.7f;
   case 10:*pan*=.5f;return (rain*.90f+air*.065f)*(.65f+swell*.45f);
   case 11:return wind*.38f+rain*.75f+air*(.070f+swell*.03f);
   default:return 0.f;
  }
}

static float bed(sr_engine *e,float *side)
{
  if(e->nature_blend>=1.f&&e->bed_type!=e->nature_active){
    e->nature_from=e->nature_active;
    e->nature_active=e->bed_type;
    e->nature_blend=0;
  }
  if(!e->nature_active&&e->nature_blend>=1.f)return 0;
  float n=nature_noise(e);
  e->wind_lp+=(n-e->wind_lp)*.0008f;
  e->rain_lp+=(n-e->rain_lp)*.06f;
  float wind=e->wind_lp*3.5f;
  float rain=(e->rain_lp-e->wind_lp)*.26f;
  float air=n-e->rain_lp;
  float swell=.55f+.45f*advance(&e->bed_phase,.12f);
  unsigned mask=(1u<<e->nature_active)|(e->nature_blend<1.f?(1u<<e->nature_from):0);
  float bird=(mask&((1u<<4)|(1u<<6)|(1u<<8)|(1u<<9)))?birds(e,n):0;
  float pan=e->bird_pan;
  float stereo=e->nature_active!=0;
  float value=bed_color(e->nature_active,wind,rain,air,swell,bird,&pan);
  if(e->nature_blend<1.f){
    float previous_pan=e->bird_pan;
    float previous_stereo=e->nature_from!=0;
    stereo=previous_stereo+(stereo-previous_stereo)*e->nature_blend;
    float previous=bed_color(e->nature_from,wind,rain,air,swell,bird,&previous_pan);
    value=previous+(value-previous)*e->nature_blend;
    e->nature_blend+=1.f/(.5f*SR_RATE);
  }
  *side=(bird*pan*.34f+air*(swell-.55f)*.018f)*stereo;
  return value;
}
static inline __attribute__((always_inline)) float fx(sr_engine *e,unsigned mask,float dry,float *wide){
  float wet=0.f;
  if(mask&1u)for(int i=0;i<4;i++){
    uint16_t p=e->rev_pos[i];float delayed=e->rev_history>=rv_len[i]?(float)e->reverb[i][p]*(1.f/32768.f):0;
    float next=clampf(dry*.34f+delayed*.71f,-.97f,.97f);
    e->reverb[i][p]=(int16_t)(next*32767.f);
    e->rev_pos[i]=(uint16_t)(p+1u==rv_len[i]?0:p+1u);wet+=delayed*.25f;
  }
  if((mask&1u)&&e->rev_history<1789)e->rev_history++;
  float d=0;
  if(mask&2u){
  uint16_t p=e->delay_pos;d=e->delay_history>=16384?(float)e->delay[p]*(1.f/32768.f):0;
  if(e->delay_history<16384)e->delay_history++;
  float next=clampf(dry*.54f+d*.28f,-.97f,.97f);
  e->delay[p]=(int16_t)(next*32767.f);e->delay_pos=(uint16_t)((p+1u)&16383u);
  }
  *wide=(wet*.56f-d*.12f)*e->reverb_wet;
  return wet*e->reverb_wet+d*e->delay_wet;
}
static inline __attribute__((always_inline)) int16_t sampleout(float v){
  v=clampf(v,-.999f,.999f);return (int16_t)(v*32767.f);
}
static inline __attribute__((always_inline)) void bus_meter(sr_engine *e,unsigned bus,float value)
{
  float peak=fabsf(value);
  if(peak>e->bus_meter[bus])e->bus_meter[bus]=peak;
}
#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void render_samples(sr_engine *e,float *dry,size_t frames){
  float left[SR_TRACKS], right[SR_TRACKS], send[SR_TRACKS];
  int locked=e->frequency_lock;
  float lock_hz=locked?sr_frequencies[e->frequency].hz:0;
  int audition=!locked&&e->nature_audition>0;
  unsigned source=audition?SR_ISOLATE_NATURE:e->isolate;
  e->nature_peak*=1.f-(float)frames*.0005f;
  for(unsigned b=0;b<SR_BUS_COUNT;b++)e->bus_meter[b]*=1.f-(float)frames*.0005f;
  int full=source==SR_ISOLATE_MIX;
  struct voice_config {
    sr_voice *voice;
    float brightness, cutoff, gain, step, left, right, send;
    float fm_ratio, fm_depth, fm_envelope;
    sr_wave wave;
    uint8_t track, chorus;
  } active[SR_VOICES];
  unsigned count=0;
  float bass_envelope=0,bass_pitch=0;
  uint32_t bass_age=0xffffffffu;
  float reciprocal=1.f/(float)frames;
  float vibrato[2];
  for(int t=0;t<2;t++){
    sr_motion *m=&e->motion[t];
    e->vibrato_phase[t]+=(float)m->vibrato_rate*(.1f/SR_RATE)*frames;
    if(e->vibrato_phase[t]>=1)e->vibrato_phase[t]-=1;
    vibrato[t]=m->vibrato_depth?sine(e->vibrato_phase[t])*m->vibrato_depth*.0005776f:0;
  }
  e->air_wet+=(e->air_target-e->air_wet)*((float)frames*.0004f);
  for(int t=0;t<SR_TRACKS;t++){
    sr_track *track=&e->tracks[t];
    float level=(track->mute||(e->solo<SR_TRACKS&&e->solo!=t))?0:track->volume;
    if(!locked){
      float peak=e->input_peak[t];
      float target=e->auto_gain&&peak>.015f?clampf(.12f/peak,.72f,1.28f):1.f;
      e->gain_trim[t]+=(target-e->gain_trim[t])*(float)frames*.000012f;
      if(e->auto_gain)level*=e->gain_trim[t];
      if(e->auto_level){
        static const float balance[SR_TRACKS]={1.f,1.10f,.89f,.93f,.98f,.96f,.90f,1.06f};
        level*=balance[t];
      }
    }
    e->input_peak[t]*=1.f-(float)frames*.0005f;
    if(t==SR_DRUMS)level*=1.f+((int)e->drum_presence-50)*.006f;
    float send_level=track->send;
    if(e->air_track==SR_TRACKS||e->air_track==t)
      send_level+=(1.f-send_level)*e->air_wet;
    send[t]=level*send_level;
    if(!full&&source!=SR_ISOLATE_TRACK1+(unsigned)t)level=0;
    left[t]=level*(.5f-.31f*track->pan);
    right[t]=level*(.5f+.31f*track->pan);
  }
  for(int j=0;j<SR_VOICES;j++){
    sr_voice *v=&e->voices[j];
    if(!v->active)continue;
    if(e->frame_count-v->born>SR_RATE*30u&&v->release_coef<.003f){
      e->note_offs+=v->gate!=0;
      v->gate=0;v->remain=0;v->release_coef=.003f;e->watchdogs++;
    }
    if(v->track==SR_BASS&&v->gate){
      bass_envelope=fmaxf(bass_envelope,v->env);
      uint32_t age=e->frame_count-v->born;
      if(age<=bass_age){bass_age=age;bass_pitch=v->pitch;}
    }
    struct voice_config *config=&active[count++];
    sr_track *track=&e->tracks[v->track];
    const sr_patch *patch=&sr_patches[track->patch];
    config->voice=v;
    config->track=v->track;
    config->wave=patch->wave;
    unsigned is_fm=track->patch>=SR_FIRST_FM_PATCH;
    config->chorus=!is_fm&&(patch->wave==SR_CHOIR_WAVE||track->patch>=20);
    float brightness=patch_tone(patch,track);
    config->brightness=brightness*(v->freq>3000.f?.15f:1.f);
    config->fm_ratio=0;
    if(is_fm){
      unsigned op=track->patch-SR_FIRST_FM_PATCH;
      config->fm_ratio=fm_operators[op].ratio+fm_operators[op].detune;
      config->fm_depth=fm_operators[op].depth*(.25f+.75f*config->brightness);
      config->fm_envelope=fm_operators[op].envelope;
    }
    config->cutoff=.028f+brightness*.24f;
    config->gain=patch->gain;
    config->left=left[v->track];
    config->right=right[v->track];
    config->send=send[v->track]*.5f;
    float modulation=0;
    if(v->track<=SR_BASS){
      sr_motion *m=&e->motion[v->track];
      float amount=m->glide_mode&&m->glide_ms?clampf((float)frames*(1000.f/SR_RATE)/m->glide_ms,0,1):1;
      v->pitch+=(v->freq-v->pitch)*amount;
      modulation=vibrato[v->track];
    }
    config->step=(v->pitch*(1+modulation)*(1.f/SR_RATE)-v->increment)*reciprocal;
  }
  float bass_hz=bass_pitch*.5f;
  if(e->sacred_mode==2)bass_hz=sacred_hz(e);
  if(bass_hz>0){
    while(bass_hz>110.f)bass_hz*=.5f;
    if(bass_hz<38.f)bass_hz*=2.f;
    e->sacred_hz_now=bass_hz;
  }else bass_hz=e->sacred_hz_now;
  /* A musical bass overtone is a voice companion, not a last-pitch latch.
   * Reference mode remains an explicitly selected continuous tone. */
  float target_bass=e->sacred_mode&&bass_hz>=38.f&&e->running?
      e->sacred_level*(e->sacred_mode==2?1.f:clampf(bass_envelope*2.f,0,1)):0;
  /* Track meters are display-only. Decay once per render chunk. */
  float meter_decay=1.f-(float)frames*.00097f;
  for(int t=0;t<SR_TRACKS;t++)e->tracks[t].meter*=meter_decay;
  for(size_t i=0;i<frames;i++){
    float l=0.f,r=0.f,mix=0.f;
    float locked_sample=locked?advance(&e->lock_phase,lock_hz):0;
    for(unsigned a=0;a<count;a++){
      const struct voice_config *config=&active[a];
      sr_voice *v=config->voice;
      if(!v->active)continue;
      if(v->remain>0&&--v->remain==0&&v->gate){
        v->gate=0;e->note_offs++;
        if(v->key){v->release_coef=.002f;e->watchdogs++;}
      }
      if(v->gate)v->env+=(v->peak-v->env)*v->attack_coef;
      else v->env-=v->env*v->release_coef;
      if(!v->gate && v->env<.0005f){v->active=0;continue;}
      if(locked){
        float z=locked_sample*v->env*.11f;
        l+=z*config->left;r+=z*config->right;
        float mag=fabsf(z)*(config->left+config->right);
        if(mag>e->tracks[config->track].meter)e->tracks[config->track].meter=mag;
        continue;
      }
      v->increment+=config->step;
      v->phase+=v->increment;
      if(v->phase>=1.f)v->phase-=1.f;
      float s;
      if(config->fm_ratio){
        v->phase2+=v->increment*config->fm_ratio;
        if(v->phase2>=1.f)v->phase2-=1.f;
        float index=config->fm_depth*(1.f-config->fm_envelope*(1.f-v->env));
        s=sine(v->phase+index*sine(v->phase2)+1.f);
      }else s=wave(config->wave,v->phase,config->brightness);
      if(config->chorus){
        v->phase2+=v->increment*1.0023f;if(v->phase2>=1.f)v->phase2-=1.f;
        s=.75f*s+.25f*sine(v->phase2);
      }
      if(config->wave==SR_PLUCK)v->peak*=.99996f;
      v->filter+=(s-v->filter)*config->cutoff;
      float z=v->filter*v->env*config->gain+v->tail;
      v->tail*=.98f;
      l+=z*config->left;
      r+=z*config->right;
      mix+=z*config->send;
      float raw=fabsf(z);
      if(e->auto_gain&&raw>e->input_peak[config->track])e->input_peak[config->track]=raw;
      float mag=raw*(config->left+config->right);
      sr_track *track=&e->tracks[config->track];
      if(mag>track->meter)track->meter=mag;
    }
    if(locked&&!e->lock_textures){
      e->tracks[SR_DRUMS].meter=0;
      dry[3*i]=l;dry[3*i+1]=r;dry[3*i+2]=0;
      continue;
    }
    float drums=drum_sample(e);
    if(e->auto_gain&&fabsf(drums)>e->input_peak[SR_DRUMS])e->input_peak[SR_DRUMS]=fabsf(drums);
    l+=drums*left[SR_DRUMS];r+=drums*right[SR_DRUMS];
    mix+=drums*send[SR_DRUMS]*.5f;
    e->tracks[SR_DRUMS].meter=e->drum_meter*e->drum_level*(left[SR_DRUMS]+right[SR_DRUMS]);
    e->transport_env+=((e->running?1.f:0.f)-e->transport_env)*.0008f;
    float bed_gain=audition?.55f:e->bed_level;
    float nature=0, nature_side=0;
    if((audition||e->solo==SR_TRACKS)&&bed_gain>0&&
       (e->bed_type||e->nature_active||e->nature_blend<1.f)){
      nature=bed(e,&nature_side);
      float gain=bed_gain*e->transport_env;
      nature*=gain;
      nature_side*=gain;
    }
    if(full||source==SR_ISOLATE_NATURE){
      l+=nature*.72f+nature_side;r+=nature*.72f-nature_side;
      float peak=fabsf(nature*.72f);
      if(peak>e->nature_peak)e->nature_peak=peak;
      if(!(i&7u))bus_meter(e,SR_BUS_NATURE,nature*.72f);
    }
    mix+=nature*.5f;
    if(locked){
      dry[3*i]=l;dry[3*i+1]=r;dry[3*i+2]=0;
      continue;
    }
    e->sacred_gain+=(target_bass-e->sacred_gain)*.001f;
    float sacred=0;
    if(e->sacred_gain>.00001f){
      float base=advance(&e->bass_phase,bass_hz);
      sacred=(base*(1-e->sacred_harmonics)+sine(e->bass_phase*2.f)*e->sacred_harmonics)*e->sacred_gain;
    }
    sacred*=full?left[SR_BASS]+right[SR_BASS]:
      (e->tracks[SR_BASS].mute||(e->solo<SR_TRACKS&&e->solo!=SR_BASS)?0:e->tracks[SR_BASS].volume);
    float reference=e->tone_level>0&&e->solo==SR_TRACKS?advance(&e->tone_phase,sr_frequencies[e->frequency].hz)*e->tone_level*e->transport_env:0;
    if(full||source==SR_ISOLATE_FIXED){l+=reference;r+=reference;if(!(i&7u))bus_meter(e,SR_BUS_REFERENCE,reference);}
    if(full||source==SR_ISOLATE_SACRED){l+=sacred*.6f;r+=sacred*.6f;if(!(i&7u))bus_meter(e,SR_BUS_SACRED,sacred*.6f);}
    dry[3*i]=l; dry[3*i+1]=r; dry[3*i+2]=mix;
  }
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void apply_fx(sr_engine *e,float *dry,size_t frames)
{
  unsigned mask=e->fx_mask;
  int audible=e->isolate==SR_ISOLATE_MIX||e->isolate==SR_ISOLATE_FX;
  float reverb=e->reverb_amount;
  if(e->air_track==SR_TRACKS)reverb+=(.8f-reverb)*e->air_wet*.35f;
  float delay=e->delay_amount;
  for(size_t i=0;i<frames;i++){
    e->reverb_wet+=(reverb-e->reverb_wet)*.002f;
    e->delay_wet+=(delay-e->delay_wet)*.002f;
    float width=0;
    float wet=fx(e,mask,dry[3*i+2],&width);
    if(audible){
      if(!(i&7u))bus_meter(e,SR_BUS_FX,fmaxf(fabsf(wet+width),fabsf(wet-width)));
      dry[3*i]+=wet+width;
      dry[3*i+1]+=wet-width;
    }
  }
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void output_samples(sr_engine *e,const float *dry,int16_t *out,size_t frames)
{
  float ceiling=e->auto_level&&!e->frequency_lock?.70f:.78f;
  float peak=e->master_peak*(1.f-(float)frames*.0005f);
  for(size_t i=0;i<frames;i++){
    float l=dry[3*i],r=dry[3*i+1];
    float binaural_l=0,binaural_r=0;
    if(!e->frequency_lock&&!e->nature_audition)render_binaural(e,&binaural_l,&binaural_r);
    if(e->isolate==SR_ISOLATE_MIX||e->isolate==SR_ISOLATE_BINAURAL){
      if(!(i&7u))bus_meter(e,SR_BUS_BINAURAL,fmaxf(fabsf(binaural_l),fabsf(binaural_r)));
      l+=binaural_l;r+=binaural_r;
    }
    // Soft-knee master saturation controls peaks without a dynamic allocation.
    float level=fmaxf(fabsf(l),fabsf(r));
    if(level>peak)peak=level;
    float target=level>ceiling?ceiling/level:1.f;
    if(target<e->limiter || e->limiter==0.f)e->limiter=target;
    else e->limiter+=(target-e->limiter)*.00018f;
    float gain=e->limiter*e->master;
    if(e->stop_frames){gain*=e->stop_frames*(1.f/256.f);e->stop_frames--;}
    out[2*i]=sampleout(l*gain);out[2*i+1]=sampleout(r*gain);
    e->frame_count++;
  }
  e->master_peak=peak;
}

/* Explicit indirect call spans XIP -> SRAM; control-rate events remain in flash. */
static void (*volatile render_block)(sr_engine*,float*,size_t)=render_samples;
static void (*volatile process_fx)(sr_engine*,float*,size_t)=apply_fx;
static void (*volatile finish_block)(sr_engine*,const float*,int16_t*,size_t)=output_samples;
void sr_render(sr_engine *e,int16_t *out,size_t frames){
 float dry[64*3];
 e->fx_ticks=0;
 while(frames){
  if(!e->running&&!e->stop_frames){
    memset(out,0,frames*2*sizeof(*out));
    return;
  }
  if(e->running&&e->tick_phase>=e->tick_length){e->tick_phase=0;memset(e->ornaments,0,sizeof e->ornaments);trigger(e);}
  size_t n=frames>64?64:frames;
  if(e->running&&(e->ornaments[0].phase||e->ornaments[1].phase||
                 e->ornaments[2].phase||e->ornaments[3].phase)){
    for(unsigned slot=0;slot<sizeof ornament_track;slot++){
      unsigned phase=e->ornaments[slot].phase;
      if(!phase)continue;
      if(phase==e->tick_phase){
        e->ornaments[slot].phase=0;
        sr_note_on(e,ornament_track[slot],e->ornaments[slot].midi,
                   e->ornaments[slot].velocity,ornament_gate[slot]);
      }else if(phase>e->tick_phase&&n>phase-e->tick_phase)n=phase-e->tick_phase;
    }
  }

  if(e->running&&e->play_queued){
    for(unsigned key=0;key<27;key++){
      uint32_t bit=1u<<key;
      if(!(e->play_queued&bit))continue;
      int32_t remaining=(int32_t)(e->play_due[key]-e->frame_count);
      if(remaining<=0){
        play_key(e,e->play_track[key],key,e->play_released&bit);
        e->play_queued&=~bit;
        e->play_released&=~bit;
      }else if(n>(uint32_t)remaining)n=(uint32_t)remaining;
    }
  }

  if(e->running&&n>e->tick_length-e->tick_phase)n=e->tick_length-e->tick_phase;
  if(e->stop_frames&&n>e->stop_frames)n=e->stop_frames;
  unsigned fading=e->stop_frames;
  unsigned audition=e->nature_audition;
  render_block(e,dry,n);
  unsigned reverb=e->reverb_amount>0||e->reverb_wet>.00001f||(e->air_track==SR_TRACKS&&e->air_wet>.00001f);
  unsigned delay=e->delay_amount>0||e->delay_wet>.00001f;
  unsigned fx_mask=(reverb?1u:0u)|((reverb||delay)?2u:0u);
  if(e->fx_mask&~fx_mask){
    if(!(fx_mask&1)){e->rev_history=0;memset(e->rev_pos,0,sizeof(e->rev_pos));e->reverb_wet=0;}
    if(!(fx_mask&2)){e->delay_history=0;e->delay_pos=0;e->delay_wet=0;}
  }
  e->fx_mask=(uint8_t)fx_mask;
  if(fx_mask&&!e->frequency_lock&&!audition){
    uint32_t begin=e->clock_ticks?e->clock_ticks():0;
    process_fx(e,dry,n);
    if(e->clock_ticks)e->fx_ticks+=e->clock_ticks()-begin;
  }
  finish_block(e,dry,out,n);
  if(audition)e->nature_audition=audition>n?audition-(uint32_t)n:0;
  if(fading&&!e->stop_frames){
    unsigned restart=e->restart;
    clear_transport(e);
    e->restart=0;
    e->frequency_lock=e->pending_lock;
    if(restart){e->running=1;e->sequence_step=0;e->loop_pos=0;e->tick_phase=e->tick_length;}
  }
  if(e->running&&!fading)e->tick_phase+=(uint32_t)n;
  out+=n*2;frames-=n;
 }
}
