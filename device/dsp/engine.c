#include "serenity.h"
#ifdef SR_DEVICE
#include "fastmath.h"
#define sinf fm_sinf
#define expf fm_expf
#define fabsf(x) ((x)<0.f?-(x):(x))
#define fmaxf(a,b) ((a)>(b)?(a):(b))
#define floorf fm_floorf
#define isfinite(x) ((x)>=-3.402823466e38f&&(x)<=3.402823466e38f)
extern void *memset(void *, int, size_t);
extern void *memcpy(void *, const void *, size_t);
#else
#include <math.h>
#include <string.h>
#endif

static inline __attribute__((always_inline)) void midi_push(sr_engine *e,unsigned role,unsigned note,unsigned velocity)
{
 if(!e->midi_enabled)return;
 unsigned next=(e->midi_write+1u)&63u;
 if(next==e->midi_read){e->midi_event_lost++;return;}
 e->midi_events[e->midi_write]=(role<<16)|(note<<8)|velocity;
 e->midi_write=(uint8_t)next;
}
static inline __attribute__((always_inline)) void release_voice(sr_engine *e,sr_voice *v)
{
 if(v->gate){
  e->note_offs++;
  if(!v->external)midi_push(e,v->track,v->midi,0);
 }
 v->gate=0;
}
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
  {"Evolving Warmth", SR_REED, 1.0f, 3.0f, .42f, .21f},
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
  {"Angelic Strings", SR_REED, .80f, 3.8f, .48f, .23f},
  {"Chamber Strings", SR_REED, .65f, 2.8f, .30f, .22f},
  {"Viola Mist", SR_REED, .45f, 2.4f, .22f, .23f},
  {"Ocean Glass", SR_HOLLOW, 1.2f, 4.5f, .33f, .20f},
  {"Linen Pad", SR_REED, 1.5f, 3.9f, .25f, .23f},
  {"Moon Glass Keys", SR_SINE, .006f, 2.2f, .55f, .23f},
  {"Halo Choir FM", SR_SINE, 1.1f, 3.7f, .40f, .22f},
  {"Silk Harmonics", SR_SINE, 1.8f, 4.0f, .33f, .20f},
  {"Rain Harp", SR_PLUCK, .009f, 1.7f, .48f, .23f},
  {"Deep Resonant Bass", SR_SINE, .028f, .86f, .29f, .33f},
  {"Velvet Reed FM", SR_SINE, .19f, 1.5f, .45f, .24f},
  {"Pure Sub FM",SR_SINE,.018f,.70f,.20f,.34f},
  {"Rounded Ember FM",SR_SINE,.035f,1.1f,.32f,.32f},
  {"Wood Resonance FM",SR_SINE,.012f,.85f,.47f,.29f},
  {"Harmonic Root FM",SR_SINE,.045f,1.4f,.55f,.27f},
  {"Plucked Earth FM",SR_PLUCK,.004f,.46f,.58f,.30f},
  {"Cinema Depth FM",SR_SINE,.32f,2.6f,.36f,.28f},
  {"Nylon Resonance FM",SR_PLUCK,.006f,.78f,.34f,.24f},
  {"Steel String FM",SR_PLUCK,.003f,1.25f,.64f,.20f},
  {"Palm Mute FM",SR_PLUCK,.002f,.23f,.38f,.26f},
  {"Chorus Guitar FM",SR_SINE,.018f,1.35f,.46f,.21f},
  {"Dusk Harp FM",SR_PLUCK,.005f,1.65f,.31f,.24f},
  {"Sunlit Harp FM",SR_PLUCK,.003f,2.05f,.69f,.18f},
  {"Ripple Harp FM",SR_PLUCK,.008f,.91f,.51f,.21f},
  {"Air Flute FM",SR_SINE,.14f,.65f,.22f,.24f},
  {"Bamboo Legato FM",SR_SINE,.075f,.92f,.30f,.23f},
  {"Alto Reed FM",SR_SINE,.06f,.72f,.48f,.22f},
  {"Tenor Velvet FM",SR_SINE,.095f,.88f,.36f,.24f},
  {"Breathy Sax FM",SR_SINE,.13f,1.15f,.55f,.20f},
  {"Alto Saxophone",SR_SINE,.037f,.42f,.72f,.23f},
  {"Tenor Saxophone",SR_SINE,.058f,.53f,.52f,.26f},
  {"Chamber Clarinet",SR_SINE,.045f,.38f,.46f,.24f},
  {"Oboe",SR_SINE,.065f,.48f,.78f,.20f},
  {"Bassoon",SR_SINE,.071f,.55f,.49f,.26f},
  {"Concert Flute",SR_SINE,.075f,.46f,.27f,.27f},
  {"Alto Flute",SR_SINE,.125f,.64f,.19f,.27f},
  {"Piccolo",SR_SINE,.043f,.28f,.35f,.20f},
  {"Soprano Ah Choir",SR_SINE,.62f,2.9f,.46f,.23f},
  {"Male Ooh Choir",SR_SINE,.84f,2.5f,.27f,.26f},
  {"Mixed Vowel Choir",SR_SINE,.55f,3.2f,.40f,.22f},
  {"Angelic Air Choir",SR_SINE,1.1f,4.0f,.32f,.23f},
  {"Pizzicato Cello",SR_PLUCK,.012f,.24f,.39f,.27f},
  {"Rosewood Mallet",SR_PLUCK,.003f,.45f,.55f,.24f},
  {"Kalimba Tines",SR_PLUCK,.002f,.62f,.63f,.22f}
};
#define SR_LEGACY_PATCHES 24u
static const struct {
  float ratio, depth, envelope, detune;
} fm_operators[] = {
  /* Original bank IDs: bass/drone, pads, struck strings, winds and voices.
   * Depth approaches zero on sub bass; higher ratios brighten transients. */
  {1.f,.022f,.12f,0.f}, {2.f,.12f,.34f,0.f},
  {1.f,.18f,.35f,0.f}, {1.f,.045f,.19f,.0006f},
  {2.f,.15f,.24f,.0013f}, {1.5f,.11f,.52f,.002f},
  {2.f,.17f,.71f,0.f}, {3.004f,.24f,.83f,0.f},
  {3.79f,.29f,.86f,0.f}, {2.f,.22f,.81f,0.f},
  {3.f,.18f,.72f,0.f}, {4.01f,.26f,.83f,0.f},
  {1.f,.08f,.33f,0.f}, {2.f,.10f,.28f,0.f},
  {3.f,.064f,.20f,0.f}, {1.f,.31f,.32f,0.f},
  {2.f,.28f,.33f,0.f}, {2.73f,.26f,.22f,0.f},
  {3.91f,.36f,.57f,0.f}, {2.f,.17f,.28f,.0018f},
  {1.f,.25f,.29f,.0023f}, {2.f,.17f,.29f,.0011f},
  {1.4142f,.16f,.31f,.001f}, {2.f,.08f,.22f,.0025f},
  {3.01f,.17f,.80f,.000f}, {2.f,.15f,.22f,.003f},
  {1.4142f,.14f,.30f,.001f}, {2.75f,.22f,.85f,.000f},
  {1.f,.12f,.38f,.000f}, {3.f,.18f,.46f,.002f},
  {1.f,.035f,.90f,0.f},{1.001f,.12f,.50f,0.f},
  {2.f,.26f,.85f,.002f},{3.f,.29f,.35f,0.f},
  {2.5f,.40f,.97f,0.f},{.5f,.22f,.18f,.001f},
  {1.f,.32f,.96f,0.f},{3.f,.47f,.98f,.001f},
  {2.f,.25f,.995f,0.f},{1.002f,.19f,.60f,.003f},
  {2.f,.23f,.94f,0.f},{3.f,.42f,.97f,0.f},
  {2.5f,.31f,.985f,.001f},{1.f,.055f,.30f,.001f},
  {2.f,.09f,.40f,0.f},{1.f,.40f,.54f,.001f},
  {1.f,.28f,.42f,0.f},{3.f,.32f,.38f,.002f},
  {1.f,.30f,.25f,0.f},{1.f,.24f,.38f,0.f},
  {2.f,.21f,.18f,0.f},{1.f,.43f,.16f,0.f},{2.f,.29f,.35f,0.f},
  {1.f,.065f,.55f,0.f},{2.f,.045f,.65f,0.f},{1.f,.11f,.42f,0.f},
  {2.f,.20f,.50f,.003f},{1.f,.135f,.34f,.001f},
  {2.f,.25f,.46f,.002f},{3.f,.12f,.50f,.003f},
  {2.f,.31f,.96f,0.f},{3.99f,.27f,.99f,0.f},{5.95f,.33f,.99f,0.f}
};
_Static_assert(sizeof fm_operators / sizeof *fm_operators == SR_PATCHES,
               "Every FM patch needs an operator profile");
const sr_scene sr_scenes[] = {
  {"Stillness",43,0,1,1,45,0}, {"Flow State",58,1,2,2,45,3},
  {"Radiance",68,0,3,3,45,8}, {"Grounding",48,2,2,4,41,2},
  {"Dreaming",52,3,2,5,45,6}, {"Ceremony",61,4,3,1,48,5},
  {"Anti-Curse",51,2,2,4,43,7}, {"Positive Healing",63,4,2,2,48,4},
  {"Creativity",66,1,3,7,46,13}, {"Inspiration",55,4,2,6,50,14},
  {"Luck",71,0,3,3,43,11}, {"Abundance",49,3,1,5,41,12},
  {"Attract Positivity",64,4,3,2,48,15},
  {"Miracles",57,3,2,6,48,16}, {"Blessing",46,4,1,2,43,17},
  {"Negative Energy Cleanse",54,2,2,4,41,18}, {"Cell Regeneration",50,0,1,3,45,1},
  {"Anxiety Release",42,3,1,5,41,10}, {"Bliss",60,0,2,6,48,9},
  {"Raise Positive Vibrations",69,4,3,7,46,19}, {"Neuro-Freq Detox",47,1,1,4,43,20},
  {"Fortune",62,4,3,3,47,21}
};
_Static_assert(sizeof sr_scenes/sizeof *sr_scenes==SR_SCENES,"Complete scene metadata");
const sr_frequency sr_frequencies[SR_FREQUENCIES] = {
  {"174 Hz",174.f}, {"285 Hz",285.f}, {"396 Hz",396.f},
  {"417 Hz",417.f}, {"528 Hz",528.f}, {"639 Hz",639.f},
  {"741 Hz",741.f}, {"852 Hz",852.f}, {"963 Hz",963.f},
  {"111 Hz",111.f}, {"136.1 Hz",136.1f}, {"777 Hz",777.f}, {"888 Hz",888.f},
  {"444 Hz",444.f}, {"480 Hz",480.f}, {"432 Hz",432.f}, {"999 Hz",999.f},
  {"672 Hz",672.f}, {"384 Hz",384.f}, {"1024 Hz",1024.f}, {"256 Hz",256.f}, {"726 Hz",726.f}
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
/* Time constants for the body and the shorter pick/hammer brightness
 * transient. Preserve patch IDs and use the existing per-voice filter. */
static const struct {float body,attack_color;} pluck_decay[SR_PATCHES]={
 [6]={.43f,.045f}, [7]={.75f,.07f}, [8]={.98f,.08f},
 [9]={.30f,.026f}, [10]={.65f,.035f}, [11]={.48f,.055f},
 [27]={.55f,.04f}, [34]={.24f,.028f},
 [36]={.39f,.021f}, [37]={.51f,.037f}, [38]={.095f,.017f},
 [40]={.71f,.032f}, [41]={.88f,.045f}, [42]={.43f,.025f},
 [60]={.155f,.024f}, [61]={.29f,.016f}, [62]={.44f,.032f},
};

/* Control-rate wind articulation. Rates are Hz, depth is cents, and the
 * entrance delay is in seconds. A tongued onset begins on pitch. */
static const struct {float rate,depth,delay;} wind_motion[]={
 {5.5f,13.f,.22f},{5.0f,16.f,.27f},{4.6f,5.f,.31f},{5.4f,9.f,.25f},
 {4.8f,7.f,.29f},{5.2f,11.f,.24f},{4.9f,10.f,.29f},{5.8f,7.f,.18f}
};

const float sr_binaural_presets[SR_BEAT_PRESETS] = {1.f, 2.f, 4.f, 6.f, 8.f, 10.f, 14.f, 20.f};
const char *const sr_drum_parts[SR_DRUM_PARTS]={
  "Kick","Snare","Closed Hat","Kick 2","Clap","Open Hat","Pedal Hat","Rim",
  "Snare 2","Low Tom","High Tom","Crash","Ride","Shaker","Conga","Bell"
};
const uint8_t sr_drum_notes[SR_DRUM_PARTS]={36,38,42,35,39,46,44,37,40,45,50,49,51,70,64,53};

#include "drum_waves.inc"
#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".pool"),aligned(4)))
#endif
static int8_t drum_pcm[13][DRUM_WAVE_LENGTH];
#include "vocal_samples.inc"
#include "vocal_day.inc"
#include "vocal_you.inc"
enum {VOCAL_DAY_ZONE=VOCAL_ZONES,VOCAL_YOU_ZONE,VOCAL_EH_ZONE,VOCAL_EE_ZONE,
      VOCAL_HUM_ZONE,VOCAL_LOVE_ZONE,VOCAL_BABY_ZONE,VOCAL_ME_ZONE,
      VOCAL_DREAMS_ZONE,VOCAL_SHINE_ZONE,VOCAL_FREE_ZONE,VOCAL_FLY_ZONE,VOCAL_STAY_ZONE,
      VOCAL_AWAY_ZONE,VOCAL_HOME_ZONE,VOCAL_HOPE_ZONE,VOCAL_ALIVE_ZONE,
      VOCAL_TONIGHT_ZONE,VOCAL_FOREVER_ZONE,VOCAL_LAST_ZONE=VOCAL_FOREVER_ZONE};
int sr_vocal_sample_valid(unsigned id){return id<=VOCAL_LAST_ZONE+1u;}
#if defined(SR_DEVICE) && !defined(PLAT_HOST)
#define VOCAL_POOL __attribute__((section(".pool"),aligned(4)))
#else
#define VOCAL_POOL
#endif
#include "vocal_extensions.inc"
#include "vocal_dreams.inc"
#include "sung_words.inc"

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".pool"),aligned(4)))
#endif
static int8_t vocal_pcm[VOCAL_ZONES][VOCAL_LENGTH];
#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".pool"),aligned(4)))
#endif
static int8_t vocal_day_pcm[VOCAL_DAY_LENGTH];
#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".pool"),aligned(4)))
#endif
static int8_t vocal_you_pcm[VOCAL_YOU_LENGTH];
static const struct {
  const int8_t *pcm;
  unsigned length,loop,release,attack;
  uint8_t mouth[3];
  float hz;
} vocal_words[]={
  {vocal_day_pcm,VOCAL_DAY_LENGTH,VOCAL_DAY_LOOP,VOCAL_DAY_RELEASE,VOCAL_DAY_ATTACK,{4,5,6},vocal_day_hz},
  {vocal_you_pcm,VOCAL_YOU_LENGTH,VOCAL_YOU_LOOP,VOCAL_YOU_RELEASE,VOCAL_YOU_ATTACK,{6,3,3},300.f},
  {vocal_eh_pcm,4189u,1758u,2766u,670u,{1,1,1},525.0000000f},
  {vocal_ee_pcm,4190u,1759u,2767u,670u,{5,5,5},525.0000000f},
  {vocal_hum_pcm,4410u,965u,3797u,706u,{4,4,4},186.8644068f},
  {vocal_love_pcm,11686u,5182u,7438u,1870u,{1,1,1},234.5744681f},
  {vocal_baby_pcm,8820u,4934u,7238u,5909u,{4,5,5},229.6875000f},
  {vocal_me_pcm,10804u,5090u,6578u,1729u,{4,5,5},355.6451613f},
  {vocal_dreams_pcm,VOCAL_DREAMS_LENGTH,VOCAL_DREAMS_LOOP,VOCAL_DREAMS_RELEASE,
   VOCAL_DREAMS_ATTACK,{6,5,4},212.0192308f},
  AQUORA_SUNG_WORDS
};
static uint8_t vocal_pcm_ready;

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
  float a=sin_table[i&LUT_MASK];
  return a+(sin_table[(i+1)&LUT_MASK]-a)*f;
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
  unsigned op=track->patch;
  return sine(1.f+phase+fm_operators[op].depth*(.25f+.75f*tone)*
              sine(phase*(fm_operators[op].ratio+fm_operators[op].detune)));
}
float sr_patch_envelope(const sr_track *track,float seconds,int releasing)
{
  const sr_patch *patch=&sr_patches[track->patch];
  float time=releasing?fmaxf(patch_time(patch->release,track->release),.04f):
    fmaxf(patch_time(patch->attack,track->attack),.001f);
  float decay=expf(-seconds/time);
  return releasing?decay:1.f-decay;
}
/* One profile per intention. The fields describe the shape of the
 * melody, where it breathes, and the harmonic family it tends toward. */
static const uint8_t intention_score[SR_SCENES][3]={
  {0,0,0},{2,1,1},{1,2,2},{3,3,3},{4,4,4},{5,5,5},{1,6,0},
  {2,7,3},{4,0,1},{0,1,4},{5,2,2},{3,3,5},{1,4,4},{2,5,2},
  {0,6,5},{4,7,0},{5,0,3},{3,1,1},{0,2,5},{5,3,1},{2,4,4},{1,5,0}
};
static uint32_t song_draw(uint32_t seed,uint32_t stream);
static unsigned song_section(const sr_engine *e);
static void plan_roles(sr_engine *e);
static unsigned form_index(const sr_engine *e,unsigned ahead);
static unsigned section_harmony(unsigned section);
static unsigned score_bar(const sr_engine *e,unsigned step)
{
  unsigned current=e->loop_pos?e->loop_pos-1u:e->sequence_step?e->loop_length-1u:0u;
  return e->form_loop+step/16u-current/16u;
}
static void make_motif(sr_engine *e,int revisit)
{
  static const uint8_t contour[6][5]={
    {0,1,2,4,2},{4,3,2,1,2},{2,4,3,5,4},
    {0,2,1,3,2},{2,1,0,2,1},{0,4,5,4,2}
  };
  static const uint8_t grid[8][6]={
    {0,2,5,8,11,14},{0,3,6,9,11,14},{0,4,7,10,12,15},{0,3,5,8,12,14},
    {1,4,6,9,12,15},{0,2,4,7,11,14},{0,4,6,10,13,15},{1,3,7,8,12,15}
  };
  const uint8_t *profile=intention_score[e->scene];
  uint32_t variation=song_draw(e->song_seed,311);
  const uint8_t *shape=contour[profile[0]];
  const uint8_t *beat=grid[(profile[1]+2u*e->music_mode+(variation&3u))&7u];
  for(unsigned i=0;i<16;i++)e->melody[i]=-1;
  int offset=(int)((variation>>2)%3u)-1;
  for(unsigned i=0;i<5;i++){
    if(e->music_mode==SR_CINEMATIC&&i==1u&&((variation>>7)&1u))continue;
    int degree=shape[i]+offset;
    if(i==2u||i==3u)degree+=(int)((variation>>(i*3u+4u))%3u)-1;
    e->melody[beat[i]]=(int8_t)(degree<0?0:degree>6?6:degree);
  }
  e->melody[beat[5]]=(int8_t)((variation>>15)&1u?2:0);
  uint16_t fingerprint=0;
  for(unsigned i=0;i<16;i++)fingerprint=(uint16_t)(31u*fingerprint+(unsigned)(e->melody[i]+1));
  uint16_t *previous=&e->last_theme[e->music_mode][e->scene];
  if(revisit&&*previous==fingerprint){
    int note=e->melody[beat[3]];
    int replacement=note>=4?note-2:note+2;
    e->melody[beat[3]]=(int8_t)replacement;
    fingerprint=0;
    for(unsigned i=0;i<16;i++)fingerprint=(uint16_t)(31u*fingerprint+(unsigned)(e->melody[i]+1));
  }
  *previous=fingerprint;

}
static void develop_motif(sr_engine *e){
  memcpy(e->melody,e->motif_home,sizeof e->melody);
  if((e->phrase&3u)==0u)return;
  /* Develop the answer that was actually composed. Preserve its opening
   * pickup and final cadence even when the intention uses another rhythm. */
  unsigned answer[14],count=0;
  for(unsigned i=1;i<15;i++)if(e->melody[i]>=0)answer[count++]=i;
  if(count<3u)return;
  unsigned at=answer[1u+song_draw(e->song_seed,e->phrase/4u+614u)%(count-2u)];
  int direction=(e->phrase&2u)?-1:1;
  int degree=e->melody[at]+direction;
  e->melody[at]=(int8_t)(degree<0?0:degree>6?6:degree);
  if((e->phrase&3u)==3u&&at+1u<answer[count-1u]&&e->melody[at+1u]<0){
    e->melody[at+1u]=e->melody[at];
    e->melody[at]=-1;
  }
}
uint8_t sr_tonic(const sr_engine *e)
{
  return e->tonic_override?e->tonic_override:sr_scenes[e->scene].tonic;
}
/* Eight-bar harmonic sentences. Each variant arrives on a stable cadence;
 * the bridge changes color while the head retains the original theme. */
static unsigned phrase_chord(const sr_engine *e,unsigned bar)
{
  static const uint8_t progressions[6][8]={
    {0,3,5,4,0,1,4,0},{0,5,3,4,2,3,4,0},{0,2,3,0,5,1,4,0},
    {0,1,5,3,0,5,4,0},{0,3,1,4,5,3,4,0},{0,5,2,5,3,1,4,0}
  };
  unsigned absolute=score_bar(e,bar*16u);
  unsigned position=absolute&7u;
  unsigned family=song_section(e);
  unsigned identity=(intention_score[e->scene][2]+e->music_mode+
    song_draw(e->song_seed,327)%6u)%6u;
  if(e->freeze||e->variation<=0.f)return progressions[identity][position];
  unsigned section=form_index(e,0);
  unsigned bars_left=sr_section_remaining(e);
  if(absolute>e->form_loop&&absolute-e->form_loop>=bars_left){
    unsigned next=form_index(e,1);
    return section_harmony(next);
  }
  unsigned center=section_harmony(section);
  if(absolute==e->form_loop&&bars_left==1u){
    unsigned destination=section_harmony(form_index(e,1));
    if(family==4u)return 0;
    return (destination+4u)%7u;
  }
  if(family==2u||family==3u)identity=(identity+section)%6u;
  if(family==4u)center=0;
  /* The head of a new scene states its center. Long, mellow modes
   * occasionally sustain a chord across the two-bar melodic answer. */
  if(bars_left==2u*(2u+sr_scenes[e->scene].energy%3u)&&absolute==e->form_loop)return center;
  if(e->music_mode<=SR_CINEMATIC&&family==3u)position&=~1u;
  return (progressions[identity][position]+center)%7u;
}
#include "learn.h"
static int motif_degree_base(const sr_engine *e,unsigned step)
{
  unsigned bar=score_bar(e,step),pos=step&15u;
  unsigned section=song_section(e);
  int degree=e->melody[pos];
  if(degree<0)return degree;
  if(!e->freeze&&e->variation>0.f){
    if(section==3u&&pos>=4u&&pos<12u)return -1;
    if(section==2u&&pos>=4u&&pos<12u)degree+=2;
    if((bar&1u)&&pos>=8u&&pos<14u){
      /* A second-bar answer uses the same head and a different tail. */
      if(section==1u)degree+=(e->music_mode==SR_NEOCLASSICAL)?-1:1;
      if(section==2u&&e->music_mode>=SR_EXPERIMENTAL)degree--;
    }
    if(section==4u&&pos>=7u&&pos<13u)degree--;
  }
  if((bar&7u)==7u&&pos>=14u)return 0;
  return degree<0?0:degree>6?6:degree;
}
/* Strong beats settle on the current triad; intervening scale notes carry
 * tension. Choose the nearest degree instead of wrapping a transposition. */
static int fit_degree(int degree,unsigned chord){
  if(degree<0)return degree;
  int best=degree,dist=99;
  for(unsigned i=0;i<3;i++){
    int d=(int)((chord+2u*i)%7u),delta=d-degree;if(delta<0)delta=-delta;
    if(delta<dist){dist=delta;best=d;}
  }
  return best;
}
static unsigned composition_chord(const sr_engine *e,unsigned step);
static int motif_degree(const sr_engine *e,unsigned step){
  int d=motif_degree_base(e,step);
  unsigned chord=composition_chord(e,step);
  if((step&3u)==0u||(step&15u)==14u)d=fit_degree(d,chord);
  return learned_degree(e,step,d);
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
#include "song_form.h"
/* Control-rate profiles: mode is independent of Nature style/evolution. */
  static const sr_motion intention_motion[][2]={
    {{120,0,0,45},{90,0,0,32}}, {{105,1,5,44},{125,1,0,31}},
    {{70,1,4,49},{75,0,0,33}}, {{135,1,3,39},{165,1,0,29}},
    {{185,1,8,36},{175,1,2,28}}, {{150,1,6,41},{135,1,1,30}},
    {{100,1,3,46},{135,1,0,32}}, {{120,1,5,43},{125,1,0,32}},
    {{75,1,7,48},{90,0,0,34}}, {{145,1,7,40},{130,1,1,30}},
    {{65,1,4,51},{70,0,0,35}}, {{170,1,6,38},{165,1,1,29}},
    {{95,1,6,46},{110,1,0,32}},
    {{160,1,7,39},{140,1,1,29}}, {{190,1,3,35},{180,1,0,26}},
    {{110,1,4,42},{155,1,0,30}}, {{145,1,3,41},{150,1,0,28}},
    {{220,1,2,31},{210,1,0,25}}, {{130,1,6,43},{120,1,1,30}},
    {{80,1,5,49},{95,0,0,34}}, {{175,1,3,37},{185,1,0,27}},
    {{88,1,5,47},{112,1,1,34}}
  };
_Static_assert(sizeof intention_motion/sizeof *intention_motion==SR_SCENES,"Complete motion profiles");
static void apply_motion_profile(sr_engine *e)
{
  for(unsigned t=0;t<2;t++)if(!(e->motion_manual&(1u<<t))){
    sr_motion m=intention_motion[e->scene][t];
    if(e->music_mode==SR_CINEMATIC){
      m.glide_ms=(uint16_t)(m.glide_ms*6u/5u);
      m.glide_mode=e->scene==0?0:1;
      m.vibrato_depth=e->scene==0?0:(t?0:3+e->scene%3);
      m.vibrato_rate=t?28:34+e->scene%8;
    }else if(e->music_mode==SR_NEOCLASSICAL){
      m.glide_mode=0;m.glide_ms=t?45:55;
      m.vibrato_depth=t?0:(e->scene%3==0?0:2+e->scene%3);
      m.vibrato_rate=t?30:46;
    }
    /* AUTO is an expressive ceiling, not an instruction to slide every note.
     * Keep the saved MANUAL path unchanged, including explicit OFF. */
    m.glide_ms=(uint16_t)(m.glide_ms*(t?28u:35u)/100u);
    if(m.glide_ms<18u)m.glide_ms=18u;
    if(m.glide_ms>80u)m.glide_ms=80u;
    m.vibrato_depth=(uint8_t)((m.vibrato_depth+1u)/2u);
    e->motion[t]=m;
  }
}
unsigned sr_legacy_motion_manual(const sr_motion *motion,unsigned scene)
{
  unsigned mask=0;
  for(unsigned role=0;role<2;role++){
    unsigned matched=0;
    for(unsigned i=0;i<SR_SCENES;i++)if(scene>=SR_SCENES||scene==i){
      sr_motion m=intention_motion[i][role],v=motion[role];
      matched|=m.glide_ms==v.glide_ms&&m.glide_mode==v.glide_mode&&m.vibrato_rate==v.vibrato_rate&&m.vibrato_depth==v.vibrato_depth;
    }
    if(!matched)mask|=1u<<role;
  }
  return mask;
}
void sr_motion_auto(sr_engine *e,unsigned role,int automatic)
{
  if(role>SR_BASS)return;
  if(automatic){e->motion_manual&=~(1u<<role);apply_motion_profile(e);}
  else e->motion_manual|=1u<<role;
}
/* Shared ensembles: intimate wood, luminous air, chamber, velvet FM.
 * A song chooses one ensemble, then favors compatible substitutions. No
 * coverage quota, no forced rejection of a musically appropriate repeat. */
static unsigned ensemble(const sr_engine *e,uint32_t seed){
  /* wood / air / chamber / velvet: emotional setting before seed choice. */
  static const uint8_t intention[SR_SCENES][4]={
    {6,5,2,1},{6,2,3,4},{3,6,2,3},{7,1,4,2},{2,6,3,5},
    {3,4,6,2},{5,2,5,3},{7,3,4,2},{3,3,4,6},{3,5,5,3},
    {4,5,3,3},{6,3,5,2},{4,6,3,3},{3,7,3,3},{5,6,4,1},
    {6,2,5,3},{6,4,3,2},{7,5,2,1},{5,6,3,3},{3,6,3,5},{6,3,4,2},{4,5,7,2}
  };
  static const uint8_t mode[SR_MUSIC_MODES][4]={{4,2,2,3},{2,5,2,4},{3,1,7,1},{2,3,1,7},{2,1,2,8}};
  unsigned total=0,w[4];
  for(unsigned i=0;i<4;i++){w[i]=intention[e->scene][i]*mode[e->music_mode][i];total+=w[i];}
  unsigned draw=song_draw(seed,211)%total;
  for(unsigned i=0;i<4;i++){if(draw<w[i])return i;draw-=w[i];}
  return 0;
}
static unsigned fresh_patch(sr_engine *e,unsigned role,uint32_t seed)
{
  static const uint8_t home[]={1,1,1,2,2,2,4,4,4,5,5,5,0,0,0,0,0,5,5,6,3,3,2,2,4,6,2,5,1,0,1,1,1,1,1,1,5,5,5,5,5,5,5,0,0,0,0,0, 0,0,0,0,0,0,0,0, 6,6,6,6, 5,5,5};
  _Static_assert(sizeof home==SR_PATCHES,"Every patch needs an automatic role");
  static const uint8_t voices[4][7]={{50,1,23,21,6,36,56},{54,0,5,20,24,40,59},{53,31,23,20,7,60,58},{48,30,26,21,24,62,57}};
  unsigned family=ensemble(e,seed),previous=e->recent_patches[e->music_mode][e->scene][role];
  unsigned anchor=voices[family][role],weights[SR_PATCHES],total=0;
  /* Respect an explicitly locked foreground when balancing the ensemble. */
  unsigned lead=(e->sound_locks&1u)?e->tracks[SR_LEAD].patch:voices[family][SR_LEAD];
  for(unsigned p=0;p<SR_PATCHES;p++){
    unsigned valid=home[p]==role || (role==SR_HARMONY&&home[p]==SR_LEAD) ||
      (role==SR_CHOIR&&(home[p]==SR_DRONE||home[p]==SR_HARMONY));
    unsigned w=0;
    if(valid){
      float distance=fabsf(sr_patches[p].brightness-sr_patches[anchor].brightness);
      w=distance<.12f?12u:distance<.25f?4u:1u;
      if(p==anchor)w+=12u;
      /* Joint intention/mode color, while every valid patch retains weight. */
      w+=2u+song_draw(e->scene*43u+e->music_mode*101u,p+role*53u)%7u;
      if(role!=SR_LEAD&&sr_patches[lead].brightness>.45f&&sr_patches[p].brightness>.6f)w=1;
      if(role==SR_GUITAR&&sr_patches[p].release>3.f&&family!=1u)w=1;
      if(p+1u==previous)w=1u;
    }
    weights[p]=w;total+=w;
  }
  if(!total)return e->tracks[role].patch;
  unsigned draw=song_draw(seed,role)%total;
  for(unsigned p=0;p<SR_PATCHES;p++)if(draw<weights[p]){
    e->recent_patches[e->music_mode][e->scene][role]=p+1u;return p;
  }else draw-=weights[p];
  return anchor;
}
void sr_select_nature(sr_engine *e,uint32_t seed)
{
  if(e->sound_locks&SR_SOUND_NATURE)return;
  static const uint8_t palette[][4]={
    {1,4,8,7},{2,10,12,11},{3,9,6,10},{4,8,7,1},
    {5,8,11,13},{1,4,7,11},{4,8,11,1},{2,10,9,5},
    {7,9,3,10},{6,9,4,2},{3,6,9,10},{5,4,8,1},{2,9,6,10},
    {9,10,2,6},{1,7,4,9},{8,11,5,4},{2,10,7,1},
    {5,2,8,13},{6,9,2,10},{3,6,12,9},{4,8,7,5},{6,10,7,12}
  };
  _Static_assert(sizeof palette/sizeof *palette==SR_SCENES,"Complete nature profiles");
  unsigned style=(e->nature_style+e->music_mode)%4u;
  unsigned draw=song_draw(seed,8)%8u;
  unsigned accent=e->energy>.55f||e->tracks[SR_PIANO].patch==24||
    (e->tick_length&&e->tick_length<SR_RATE*15u/65u);
  unsigned pick=draw<4?(style+accent)%4u:(draw-4u);
  e->bed_type=palette[e->scene][pick];
}
static void apply_sound_profile(sr_engine *e,uint32_t seed,int sounds_only)
{
  static const struct {
    uint8_t flavor,kits[3],density,hit,space;
  } palette[]={
    {2,{0,1,2},2,18,3}, {0,{0,3,7},3,35,2},
    {1,{7,8,10},4,42,3}, {0,{2,5,13},2,24,1},
    {2,{3,8,12},2,21,4}, {3,{4,10,13},3,35,4},
    {0,{13,2,14},2,20,2}, {1,{3,4,11},3,30,3},
    {3,{8,10,11},4,48,2}, {2,{4,8,12},3,32,4},
    {1,{6,7,12},4,45,2}, {0,{2,5,14},2,25,3},
    {3,{3,10,14},3,39,3},
    {2,{7,10,18},3,29,4}, {0,{1,4,16},1,17,4},
    {3,{5,13,14},3,33,2}, {1,{0,3,15},2,22,3},
    {2,{1,3,12},1,13,4}, {1,{7,8,16},3,31,4},
    {3,{6,10,18},4,44,2}, {0,{2,9,17},2,19,2},
    {1,{4,11,16},3,34,3}
  };
  _Static_assert(sizeof palette/sizeof *palette==SR_SCENES,"Complete sound profiles");
  unsigned scene=e->scene;
  for(unsigned t=0;t<SR_DRUMS;t++)if(!(e->sound_locks&(1u<<t)))
    e->tracks[t].patch=fresh_patch(e,t,seed);
  if(!(e->sound_locks&(1u<<SR_DRUMS))){
    unsigned weights[SR_DRUM_KITS],total=0;
    unsigned previous=e->recent_patches[e->music_mode][scene][SR_DRUMS];
    for(unsigned k=0;k<SR_DRUM_KITS;k++){
      static const uint8_t mode_kits[SR_MUSIC_MODES][5]={{0,3,7,10,15},{1,5,12,16,18},{2,4,8,13,17},{6,9,11,14,18},{4,6,10,14,17}};
      unsigned w=3u;
      for(unsigned j=0;j<3;j++)if(k==palette[scene].kits[j])w+=j?5u:9u;
      for(unsigned j=0;j<5;j++)if(k==mode_kits[e->music_mode][j])w+=12u;
      w+=song_draw(scene*71u+e->music_mode*193u,k+41u)%5u;
      if(k+1u==previous||k==e->drum_kit)w=0u;
      weights[k]=w;total+=w;
    }
    unsigned draw=song_draw(seed,7)%total,kit=0;
    for(;kit<SR_DRUM_KITS-1u;kit++){if(draw<weights[kit])break;draw-=weights[kit];}
    e->drum_kit=kit;e->recent_patches[e->music_mode][scene][SR_DRUMS]=kit+1u;
    if(!sounds_only){
      /* Presence is user-owned; composition only controls rhythmic density. */
      /* User-owned level/density stay stable through automatic arranging. */
      e->tracks[SR_DRUMS].pan=((int)(scene%5u)-2)*.035f;
      if(!(e->track_send_manual&(1u<<SR_DRUMS)))e->tracks[SR_DRUMS].send=.08f+.035f*palette[scene].space+(e->music_mode==SR_CINEMATIC?.06f:0.f);
      e->groove_mode=0;e->groove_strength=55u+palette[scene].hit/2u+e->music_mode*5u;

    }
  }
  sr_select_nature(e,seed);
  if(!(e->sound_locks&SR_SOUND_NATURE)&&!e->nature_level_manual)
    e->bed_level=.045f+.018f*palette[scene].space
      +.005f*(song_draw(seed,91)%7u)+(e->nature_style==1?.018f:0.f)
      -(e->music_mode==SR_CINEMATIC?.018f:0.f);
  if(!e->tone_level_manual){
    unsigned reference=song_draw(seed,92)%8u;
    e->tone_level=reference<2u?0.f:.004f+.002f*reference;
  }
  if(!(e->sound_locks&SR_SOUND_FX)){
    e->reverb_amount=.145f+.047f*palette[scene].space+.01f*(song_draw(seed,9)%8u);
    e->delay_amount=.07f+.027f*palette[scene].space+.01f*(song_draw(seed,10)%6u);
    if(e->music_mode==SR_CINEMATIC){e->reverb_amount+=.10f;e->delay_amount*=.8f;}
    if(e->music_mode==SR_NEOCLASSICAL){e->reverb_amount*=.68f;e->delay_amount*=.45f;}
    if(e->music_mode==SR_EXPERIMENTAL){e->reverb_amount*=.92f;e->delay_amount*=1.3f;}
  }
}
/* Real sixteenth-note clock, shared by LEDs, FX, fills and quantization.
 * Bit 2 of the persisted manual mask locks an explicitly edited tempo. */
static void song_tempo(sr_engine *e,uint32_t seed)
{
  if(e->motion_manual&4u)return;
  static const uint8_t ranges[SR_MUSIC_MODES][2]={
    [SR_STANDARD]={62,89},
    [SR_CINEMATIC]={48,76},
    [SR_NEOCLASSICAL]={64,104},
    [SR_EXPERIMENTAL]={79,116},
    [SR_IDM]={108,159},
  };
  unsigned lo=ranges[e->music_mode][0], hi=ranges[e->music_mode][1];
  unsigned previous=(SR_RATE*15u+e->tick_length/2u)/(e->tick_length?e->tick_length:1u);
  seed^=(uint32_t)e->rhythm_style*0x9e3779b9u;
  unsigned range=hi-lo+1u;
  /* Two draws favor each mode's characteristic pacing while keeping
     occasional slower and faster compositions. */
  unsigned bpm=lo+(song_draw(seed,202)%range+song_draw(seed,203)%range)/2u;
  uint8_t *last=&e->last_auto_bpm[e->music_mode][e->scene];
  while(bpm==previous||bpm==*last)bpm=bpm==hi?lo:bpm+1u;
  *last=(uint8_t)bpm;
  unsigned old=e->tick_length;
  e->tick_length=(SR_RATE*15u+bpm/2u)/bpm;
  if(old&&old!=e->tick_length&&e->tick_phase<old)
    e->tick_phase=(uint32_t)e->tick_phase*e->tick_length/old;
}
void sr_set_auto_tempo(sr_engine *e,int automatic)
{
  if(automatic){
    e->motion_manual&=~4u;
    song_tempo(e,e->song_seed);
  }else e->motion_manual|=4u;
}
void sr_set_rhythm_style(sr_engine *e,unsigned style)
{
  if(style>2u||e->rhythm_style==style)return;
  e->rhythm_style=style;
  song_tempo(e,e->song_seed);
}

static void song_expression(sr_engine *e,unsigned seed,int fresh)
{
  unsigned character=intention_score[e->scene][2];
  if(!(e->expression_manual&SR_MANUAL_ENERGY))
    e->energy=.12f+.17f*sr_scenes[e->scene].energy+
        (fresh?.025f*(song_draw(seed,14)%7u):0.f);
  if(!(e->expression_manual&SR_MANUAL_VARIATION)){
    unsigned detail=(character+song_draw(seed,15)%7u)%7u;
    float base=e->music_mode<=SR_CINEMATIC?.16f:.23f;
    e->variation=base+.045f*detail;
  }
}
void sr_generate_song(sr_engine *e,uint32_t seed,int sounds_only)
{
  if(e->running||e->stop_frames)return;
  if(!sounds_only){
    e->learned.count=e->capture.count=e->learn_pending=0;e->form_loop=0;e->motif_count=e->motif_cursor=0;e->response_pending=0;
    e->arrange_next=e->arrange_length=e->arrange_active=e->arrange_elapsed=0;
    memset(e->vocal_hook,0,sizeof e->vocal_hook);
    memset(e->vocal_hook_vowel,0,sizeof e->vocal_hook_vowel);
    memset(e->vocal_hook_transition,0,sizeof e->vocal_hook_transition);
    memset(e->vocal_hook_attack,0,sizeof e->vocal_hook_attack);
    e->vocal_reprise=0;
    e->vocal_pitch_last=0.f;
    e->song_seed=song_draw(seed,100u+e->music_mode);
    song_tempo(e,e->song_seed);
    e->rng=song_draw(seed,12);
    e->drum_rng=song_draw(seed,13);
    e->nature_rng=song_draw(seed,90);
    e->phrase=0;
    make_motif(e,0);
    memcpy(e->motif_home,e->melody,sizeof e->melody);
    song_expression(e,seed,1);
  }
  apply_sound_profile(e,seed,sounds_only);
  apply_motion_profile(e);
  sr_plan_sections(e);
}

void sr_set_scene(sr_engine *e,uint8_t scene){
  if(scene>=SR_SCENES)return;
  e->pending_scene=0;
  e->play_queued=0;e->air_request=0;
  memset(e->scheduled,0,sizeof e->scheduled);
  memset(e->ornaments,0,sizeof e->ornaments);
  if(e->running)for(int i=0;i<SR_VOICES;i++){
    sr_voice *v=&e->voices[i];
    if(!v->key&&!v->external){release_voice(e,v);v->remain=0;v->release_coef=.0015f;}
  }
  e->scene=scene;
  e->vocal_entry_pending=e->vocals_enabled;
  e->vocal_call=0;
  memset(e->vocal_hook,0,sizeof e->vocal_hook);
    memset(e->vocal_hook_vowel,0,sizeof e->vocal_hook_vowel);
    memset(e->vocal_hook_transition,0,sizeof e->vocal_hook_transition);
    memset(e->vocal_hook_attack,0,sizeof e->vocal_hook_attack);
  e->vocal_reprise=0;
  e->vocal_pitch_last=0.f;
  e->phrase=e->form_loop=e->loop_pos=0;
  e->sequence_step=e->learn_origin=0;
  e->arrange_next=e->arrange_length=e->arrange_active=e->arrange_elapsed=0;
  e->learned.count=e->capture.count=e->learn_pending=0;
  e->motif_count=e->motif_cursor=e->response_pending=e->evolve_next=0;
  e->song_seed=song_draw(e->song_seed^((uint32_t)scene*0x9e3779b9u),++e->visit_nonce+100u+e->music_mode*31u);
  e->rng=song_draw(e->song_seed,12);e->drum_rng=song_draw(e->song_seed,13);
  e->nature_rng=song_draw(e->song_seed,90);
  if(e->frequency_auto)e->frequency=sr_scenes[scene].frequency;
  song_tempo(e,e->song_seed);
  song_expression(e,e->song_seed,0);
  apply_sound_profile(e,e->song_seed,0);
  apply_motion_profile(e);
  make_motif(e,1);
  memcpy(e->motif_home,e->melody,sizeof e->melody);
  sr_plan_sections(e);
  plan_roles(e);
  if(e->running&&!e->external_sync&&!e->profile_applying)e->tick_phase=e->tick_length;
  retune_keys(e);
}
void sr_set_music_mode(sr_engine *e,uint8_t mode)
{
  if(mode>=SR_MUSIC_MODES)return;
  if(mode==e->music_mode){e->pending_mode=0;return;}
  if(e->running&&!e->profile_applying){e->pending_mode=mode+1u;return;}
  e->pending_mode=0;e->music_mode=mode;
  /* Force one realization through the same safe scene-transition path. */
  sr_set_scene(e,e->scene);
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
void sr_set_vocals(sr_engine *e,unsigned enabled,unsigned presence)
{
  if(!enabled||!e->vocals_enabled){
    e->vocal_entry_pending=!!enabled;
    e->vocal_call=0;
  }
  e->vocals_enabled=!!enabled;
  e->vocal_presence=presence<1u?1u:presence>4u?4u:presence;
  if(!enabled)for(unsigned i=0;i<SR_VOICES;i++){
    sr_voice *voice=&e->voices[i];
    if(!voice->active||!voice->sample_zone)continue;
    release_voice(e,voice);
    voice->remain=0;
    voice->release_coef=.0012f;
  }
}
void sr_set_track_patch(sr_engine *e,uint8_t t,uint8_t patch){if(t<SR_TRACKS&&patch<SR_PATCHES)e->tracks[t].patch=patch;}
void sr_init(sr_engine *e,uint32_t seed){
    if(!vocal_pcm_ready){
      memcpy(vocal_pcm,vocal_flash,sizeof vocal_pcm);
      memcpy(vocal_day_pcm,vocal_day_flash,sizeof vocal_day_pcm);
      memcpy(vocal_you_pcm,vocal_you_flash,sizeof vocal_you_pcm);
      memcpy(vocal_eh_pcm,vocal_eh_flash,sizeof vocal_eh_pcm);
      memcpy(vocal_ee_pcm,vocal_ee_flash,sizeof vocal_ee_pcm);
      memcpy(vocal_hum_pcm,vocal_hum_flash,sizeof vocal_hum_pcm);
      memcpy(vocal_love_pcm,vocal_love_flash,sizeof vocal_love_pcm);
      memcpy(vocal_baby_pcm,vocal_baby_flash,sizeof vocal_baby_pcm);
      memcpy(vocal_me_pcm,vocal_me_flash,sizeof vocal_me_pcm);
      memcpy(vocal_dreams_pcm,vocal_dreams_flash,sizeof vocal_dreams_pcm);
      memcpy(drum_pcm,drum_flash,sizeof drum_pcm);
      vocal_pcm_ready=1;
    }

  memset(e,0,sizeof(*e));
    e->vocal_presence=4;
    e->vocal_level=100;
    e->vocal_level_smoothed=e->vocal_auto_gain=1.f;
    e->vocal_fx_send=100;
    e->vocal_send_now=1.f;
    e->vocal_reverb=38;
    memset(e->drum_levels,100,sizeof e->drum_levels);
    e->drum_solo=SR_DRUM_PARTS; e->rng=seed?seed:0x73ac05u;
  if(!lut_ready){
    for(int i=0;i<=LUT;i++)sin_table[i]=sinf((float)i*2.f*PI/(float)LUT);
    lut_ready=1;
  }
  e->auto_gain=e->auto_level=1;
  e->chord_gap=190; e->master=.78f;
  e->fx_decay=e->fx_feedback=e->fx_width=50;
  e->fx_duck_gain=1.f;
  e->fx_decay_now=.89f;e->fx_feedback_now=.46f;e->fx_width_now=1.f; e->reverb_amount=.32f; e->delay_amount=.18f; e->bed_level=.18f;
  e->learn_strength=2;e->drums_enabled=1;e->drum_level=1.f;e->drum_presence=100;e->groove_strength=100;e->air_track=SR_TRACKS;e->air_depth=100;e->drum_density=SR_DENSITY_MAX;e->drum_hit=25;e->drum_rng=e->rng^0x6a09e667u;e->drum_kit=0;
  for(unsigned part=0;part<3;part++)e->drum_part_gain[part]=1.f;
  for(unsigned lane=0;lane<4;lane++)e->slicer_gain[lane]=1.f;
  e->binaural_enabled=0;e->binaural_carrier=220.f;e->binaural_rate=6.f;
  e->binaural_level=.035f;e->tone_level=.012f;e->energy=.5f;e->variation=.3f;e->carrier_now=220.f;e->beat_now=6.f;
  for(int t=0;t<SR_TRACKS;t++){
    e->gain_trim[t]=e->role_gain[t]=e->role_target[t]=1.f;
    e->tracks[t].patch=default_patches[t];e->tracks[t].volume=t==SR_BASS?.88f:t==SR_DRUMS?1.f:.68f; e->tracks[t].send=1.f;
    e->tracks[t].pan=((int)(t%4)-1)*.13f;
  }
  e->perfect_key=1; e->solo=SR_TRACKS; e->sacred_level=.08f; e->sacred_mode=1; e->sacred_harmonics=.32f;
  e->motion[0]=(sr_motion){120,0,0,45}; e->motion[1]=(sr_motion){90,0,0,32};
  e->loop_length=16;
  e->play_grid=3;
  e->play_strength=100;
  e->frequency_auto=1;
  e->nature_rng=song_draw(seed,90);
  e->nature_activity=e->nature_distance=2;
  e->scope_track=SR_TRACKS;
  e->nature_blend=1.f;
  sr_set_scene(e,0);
}
/* The old transport fades as a unit. A restart clears all energy before
 * generating a fresh downbeat, even when STOP and PLAY share a block. */
static void clear_transport(sr_engine *e)
{
    e->drum_active=e->drum_subsample=0;
    e->drum_aux_from=e->drum_aux_to=0.f;
    e->external_audio=0;
    e->play_queued=0;
    e->play_released=0;
    memset(e->ornaments,0,sizeof e->ornaments);
    for(unsigned i=0;i<SR_VOICES;i++)release_voice(e,&e->voices[i]);
    memset(e->voices,0,sizeof(e->voices));
    memset(e->rev_pos,0,sizeof(e->rev_pos));
    e->delay_pos=0;
    e->rev_history=e->delay_history=0;
    e->vocal_fx_pos=e->vocal_fx_history=e->vocal_fx_tail=0;
    e->vocal_fx_left=e->vocal_fx_right=e->vocal_fx_input=0.f;
    e->vocal_fx_tone=e->vocal_phaser_in=e->vocal_phaser_out=0.f;
    e->reverb_wet=e->delay_wet=e->fx_delay_low=0;
    e->fx_filter_low=e->fx_crush_hold=e->vocal_face_peak=0.f;
    e->fx_repeat_left=0;
    e->fx_grain_written=e->fx_grain_phase=e->fx_shimmer_phase=0;
    e->fx_duck_env=0.f;e->fx_duck_gain=1.f;
    e->fx_mask=0;
    e->lock_phase=0;
    e->preview_frames=0;
    e->nature_audition=e->bird_wait=0;
    e->bird_left=e->bird_note=0;
    e->nature_active=e->nature_from=0;
    e->nature_mono=e->nature_side=0;
    e->nature_blend=1.f;e->nature_peak=e->master_peak=0;
    e->kick_env=e->hand_env=e->shaker_env=e->drum_meter=0;
    e->binaural_env=e->transport_env=e->sacred_gain=e->limiter=0;
    e->wind_lp=e->rain_lp=e->hand_noise_lp=e->shaker_lp=e->bird_env=0;
    e->pitch_last[0]=e->pitch_last[1]=e->sacred_hz_now=0;
    for(int t=0;t<SR_TRACKS;t++)e->tracks[t].meter=0;
}
/* Section instrumentation is a seeded musical choice, not an eight-role
 * quota. Bass anchors most passages; support colours depend on intention,
 * energy, music mode and current harmony. No audio RNG is consumed. */
/* A shared form holds orchestration for multiple phrases. Role choices use
 * section identity, never independent per-phrase drum coin tosses. */
static unsigned composition_chord(const sr_engine *e,unsigned step){
 return phrase_chord(e,step/16u);
}

const char *sr_section_name(const sr_engine *e,unsigned next){return sr_sections[form_index(e,!!next)].name;}
static unsigned song_section(const sr_engine *e){return sr_sections[form_index(e,0)].family;}
static unsigned section_harmony(unsigned section){return sr_sections[section].harmony;}
static void plan_roles(sr_engine *e)
{
    if(e->variation<=0.f){
      for(unsigned t=0;t<SR_TRACKS;t++)e->role_target[t]=1.f;
      return;
    }
    static const uint8_t sections[5]={
      0x77, /* opening: lead, bass, pads, piano, plucks, choir */
      0x7f, /* statement: add upper harmony */
      0x7f, /* bloom: the full pitched ensemble */
      0x77, /* hush: fewer melodic attacks, same tonal bed */
      0x7f  /* return: answer with upper harmony */
    };
    unsigned family=song_section(e);
    unsigned active=sections[family]|e->sound_locks|(1u<<SR_DRUMS);
    unsigned featured=sr_sections[form_index(e,0)].roles|e->sound_locks|(1u<<SR_DRUMS);
    /* A suspension exposes its answering voices. Bass and pad continue
     * through the handover while absent parts finish their natural tails. */
    if(family==3u)active=featured|(1u<<SR_BASS)|(1u<<SR_DRONE);
    for(unsigned t=0;t<SR_TRACKS;t++){
      unsigned bit=1u<<t;
      e->role_target[t]=!(active&bit)?0.f:(featured&bit)?1.f:.74f;
    }

}
void sr_start(sr_engine *e)
{
    if(e->stop_frames){e->restart=1;return;}
    clear_transport(e);
    e->running=1;
    e->vocal_entry_pending=e->vocals_enabled;
    e->vocal_call=0;
    e->sequence_step=0;e->loop_pos=0;e->learn_origin=0;
    e->external_clock_count=255;
    e->tick_phase=e->tick_length;
    sr_plan_sections(e);plan_roles(e);
}
void sr_stop(sr_engine *e)
{
    if(e->running)e->stop_frames=256;
    e->restart=0;
    e->running=0;e->response_pending=0;
    memset(e->scheduled,0,sizeof e->scheduled);e->air_request=0;
    if(e->rhythm_pending){sr_set_rhythm_style(e,e->rhythm_pending-1u);e->rhythm_pending=0;}
    if(e->pending_mode){e->music_mode=e->pending_mode-1u;e->pending_mode=0;sr_set_scene(e,e->scene);}
}
static unsigned timeline_slot(sr_engine *e){
 unsigned slot=e->timeline_write;
 e->timeline_write=(slot+1u)%96u;
 return slot;
}

/* Control patch loudness after AUTO LEVEL. Using the nominal gain as
 * the correction would let AUTO LEVEL counteract most of the change. */
static inline __attribute__((always_inline)) float calibrated_patch_gain(unsigned patch)
{
    static const float legacy_trim[24]={
        1.f,1.f,1.f,1.f,.76f,1.f,.73f,1.f,1.f,.74f,1.f,1.f,
        1.f,1.f,1.f,1.f,.65f,1.f,1.f,.78f,.66f,.64f,.81f,.82f
    };
    return sr_patches[patch].gain*(patch<24u?legacy_trim[patch]:1.f);
}

/* Two hot callbacks reduce future polyphony and release a quiet optional
 * voice. Slow recovery avoids oscillating around a processor deadline. */
void sr_deadline_observe(sr_engine *e,unsigned callback_us)
{
 if(callback_us>4900u){
   e->deadline_low=0;
   if(++e->deadline_high<2u)return;
   e->deadline_high=0;
   unsigned cap=e->voice_limit?e->voice_limit:SR_VOICES;
   unsigned floor=callback_us>=5800u?6u:9u;
   if(cap>floor)e->voice_limit=cap-1u;
   sr_voice *quiet=0;
   unsigned active=0;
   for(unsigned i=0;i<SR_VOICES;i++){
     sr_voice *v=&e->voices[i];
     active+=!!v->active;
     if(v->active&&v->gate&&!v->key&&!v->external&&!v->sample_zone&&v->track>SR_BASS&&
        (!quiet||v->env<quiet->env))quiet=v;
   }
   if(quiet&&e->voice_limit&&active>e->voice_limit){
     release_voice(e,quiet);
     quiet->remain=0;
     quiet->release_coef=.006f;
   }
 }else{
   e->deadline_high=0;
   if(!e->voice_limit||callback_us>=3200u){e->deadline_low=0;return;}
   if(++e->deadline_low>=80u){
     e->deadline_low=0;
     if(++e->voice_limit>=SR_VOICES)e->voice_limit=0;
   }
 }
}

static sr_voice *recorded_note(sr_engine *,int,unsigned,unsigned,float);
static sr_voice *recorded_live_note(sr_engine *,int,unsigned,unsigned,float);

static unsigned vocal_articulation(unsigned kind,unsigned position,unsigned *attack,unsigned *transition)
{
 static const uint8_t vowel[]={0,0,2,1,VOCAL_EE_ZONE,VOCAL_EH_ZONE,
     VOCAL_EH_ZONE,VOCAL_HUM_ZONE};
 *attack=*transition=0;
 if(kind<8u){
   if(kind==6u)*attack=8u;
   return vowel[kind];
 }
 position%=3u;
 if(kind==8u){
   static const uint8_t vowels[]={1,VOCAL_EH_ZONE,VOCAL_EE_ZONE},onsets[]={4,5,1};
   *attack=onsets[position];return vowels[position];
 }
 if(kind==9u||kind==10u){*attack=kind==9u?2u:3u;return 0;}
 if(kind==11u){static const uint8_t attacks[]={6,4,7};*attack=attacks[position];return position==2u?1u:0u;}
 if(kind==12u){static const uint8_t words[]={VOCAL_BABY_ZONE,VOCAL_LOVE_ZONE,VOCAL_YOU_ZONE};return words[position];}
 if(kind==13u)return position==1u?0u:1u;
 if(kind==14u){*transition=1u;return 1u;}
 if(kind==15u){*transition=VOCAL_EE_ZONE+1u;return 2u;}
 if(kind==16u){*transition=2u;return VOCAL_EE_ZONE;}
 if(kind==17u){*transition=2u;return VOCAL_EH_ZONE;}
 *transition=3u;return VOCAL_HUM_ZONE;
}

static sr_voice *allocate_note(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,
                               float beats,unsigned singer,sr_voice *replacement){
  if((!e->running&&!e->preview_frames&&!e->midi_suppress)||track>=SR_DRUMS||midi>127||!velocity||e->tracks[track].mute)return 0;
  unsigned active=0, held[SR_DRUMS]={0};
  sr_voice *free_voice=0;
  if(!replacement){
    for(unsigned i=0;i<SR_VOICES;i++){
      sr_voice *v=&e->voices[i];
      if(!v->active){
        if(!free_voice)free_voice=v;
        if(!e->voice_limit)break;
        continue;
      }
      active++;
      if(v->gate&&!v->sample_zone)held[v->track]++;
    }
    if(free_voice&&active<(e->voice_limit?e->voice_limit:SR_VOICES))replacement=free_voice;
  }
  if(!replacement){
    float best=1.e9f;
    for(unsigned i=0;i<SR_VOICES;i++){
      sr_voice *v=&e->voices[i];
      if(!v->active||(v->gate&&(v->key||v->external||v->sample_zone)))continue;
      if(v->gate&&v->track==SR_BASS&&track!=SR_BASS&&held[SR_BASS]<=1u)continue;
      unsigned age=e->frame_count-v->born;
      unsigned other_role=v->track!=track||singer||v->sample_zone;
      if(v->gate&&other_role&&!singer&&held[track]&&held[v->track]<=1u)continue;
      if(v->gate&&other_role&&!singer&&age<SR_RATE/64u&&held[v->track]<=1u)continue;
      float score=v->env;
      if(v->gate){
        score+=2.f;
        if(held[v->track]<=1u&&other_role)score+=1.f;
        if(age<SR_RATE/16u)score+=.75f;
        if(v->track==SR_LEAD)score+=.35f;
        if(v->track==SR_BASS)score+=1.75f;
      }
      if(score<best){best=score;replacement=v;}
    }
  }
  if(!replacement)return 0;
  sr_voice *v=replacement;
  if(v->active&&v->gate&&!v->sample_zone)e->role_evicted[v->track]++;
  release_voice(e,v);
  e->note_ons++;
  float tail=v->active?v->tail+v->filter*v->env*(v->sample_zone?.27f:calibrated_patch_gain(v->patch)):0.f;
  memset(v,0,sizeof(*v));
  v->tail=tail;v->active=v->gate=1;v->track=track;v->patch=e->tracks[track].patch;
  const sr_patch *p=&sr_patches[e->tracks[track].patch];
  e->tracks[track].last_note=midi;
  v->born=e->frame_count;v->freq=sr_note_frequency(e,midi);v->pitch=v->freq;v->midi=midi;
  if(track<=SR_BASS){
    sr_motion *m=&e->motion[track];
    int held=0;
    for(int j=0;j<SR_VOICES;j++)if(&e->voices[j]!=v && e->voices[j].active &&
        e->voices[j].gate && e->voices[j].track==track)held=1;
    int expression=1;
    if(!(e->motion_manual&(1u<<track))){
      /* Connected neighboring tones at a phrase answer/bloom; detached
       * starts, large leaps and grounding bass pulses stay straight. */
      int neighboring=0;
      for(unsigned j=0;j<SR_VOICES;j++){
        const sr_voice *prior=&e->voices[j];
        int interval=(int)prior->midi-(int)midi;
        if(prior!=v&&prior->active&&prior->gate&&prior->track==track&&
           interval&&interval>=-5&&interval<=5)neighboring=1;
      }
      unsigned position=e->sequence_step&15u;
      unsigned patch=e->tracks[track].patch;
      unsigned sax=track==SR_LEAD&&(patch==48u||patch==49u);
      expression=neighboring&&position!=0&&
          (sax?beats>=.75f&&(position>=8u||e->music_mode==SR_CINEMATIC):
               beats>=1.5f&&(position==6u||(song_section(e)==2u&&position==10u)));
      if(track==SR_BASS)expression=expression&&beats>=3.f&&sr_scenes[e->scene].energy==0;
    }
    if(expression&&(m->glide_mode==2||(m->glide_mode==1&&held))&&e->pitch_last[track]>0)
        v->pitch=e->pitch_last[track];
    e->pitch_last[track]=v->freq;
  }
  v->increment=v->pitch*(1.f/SR_RATE);
  e->tracks[track].events++;
  v->peak=(float)velocity/127.f;
  v->attack_coef=1.f-expf(-1.f/(fmaxf(patch_time(p->attack,e->tracks[track].attack),.001f)*SR_RATE));
  v->release_coef=1.f-expf(-1.f/(fmaxf(patch_time(p->release,e->tracks[track].release),.04f)*SR_RATE));
  v->remain=(uint32_t)clampf(beats*4.f*(float)e->tick_length,1.f,9.e6f);
  if(e->timeline_enabled){
    unsigned n=timeline_slot(e);
    e->timeline[n].frame=e->frame_count;e->timeline[n].duration=v->remain;
    e->timeline[n].role=track;e->timeline[n].note=midi;e->timeline[n].velocity=velocity;
  }
  if(!e->midi_suppress)midi_push(e,track,midi,velocity);
  return v;
}

static sr_voice *note_on(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,float beats,unsigned singer)
{
  return allocate_note(e,track,midi,velocity,beats,singer,0);
}

int sr_midi_event_pop(sr_engine *e,uint32_t *event)
{
 if(e->midi_read==e->midi_write)return 0;
 *event=e->midi_events[e->midi_read];e->midi_read=(e->midi_read+1u)&63u;return 1;
}
void sr_midi_note(sr_engine *e,unsigned role,unsigned channel,unsigned note,unsigned velocity)
{
 if(role>=SR_DRUMS||channel>15||note>127||velocity>127)return;
 if(!velocity){
  for(unsigned i=0;i<SR_VOICES;i++)if(e->voices[i].external&&e->voices[i].channel==channel&&e->voices[i].midi==note)
   release_voice(e,&e->voices[i]);
  return;
 }
 e->midi_suppress=1;
 sr_voice *v=role==SR_CHOIR&&e->vocals_enabled&&e->vocal_play?recorded_live_note(e,note,0,velocity,1.f):
     note_on(e,(uint8_t)role,(uint8_t)note,(uint8_t)velocity,1.f,0);
 e->midi_suppress=0;
 if(v){v->external=1;v->channel=(uint8_t)channel;v->remain=0xffffffffu;e->external_audio=1;}
}
void sr_midi_panic(sr_engine *e)
{
 for(unsigned i=0;i<SR_VOICES;i++)if(e->voices[i].external){
  release_voice(e,&e->voices[i]);e->voices[i].release_coef=.003f;
 }
}
void sr_midi_bend(sr_engine *e,unsigned channel,int value)
{
 float ratio=expf((float)value*(.11552453f/8192.f));
 for(unsigned i=0;i<SR_VOICES;i++)if(e->voices[i].external&&e->voices[i].channel==channel){
  sr_voice *v=&e->voices[i];v->freq=sr_note_frequency(e,v->midi)*ratio;
  v->pitch=v->freq;v->increment=v->freq*(1.f/SR_RATE);
 }
}

void sr_note_on(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,float beats)
{
    if(track>=SR_DRUMS)return;
    e->role_requested[track]++;
    if(e->running&&!e->preview_frames){
      if(e->solo>=SR_TRACKS&&e->role_target[track]==0.f)return;
      if(e->role_target[track]>0.f)
        velocity=(uint8_t)(velocity*(.55f+.45f*e->role_target[track])+.5f);
    }
    if(!note_on(e,track,midi,velocity,beats,0))e->role_refused[track]++;
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
      e->role_target[SR_DRUMS]=1.f;
      if(key%3==0)e->kick_env=1;
      else if(key%3==1)e->hand_env=1;
      else e->shaker_env=1;
      e->tracks[SR_DRUMS].events++;
      return;
    }
    for(unsigned i=0;i<SR_VOICES;i++)if(e->voices[i].gate&&e->voices[i].key==key+1)return;
    unsigned note=sr_key_note(e,key);
    unsigned zone=e->vocal_play?key%3u+(note>66u?3u:0u):0u;
    unsigned consonant=0,transition=0;
    if(e->vocal_articulation)zone=vocal_articulation(e->vocal_articulation,key,&consonant,&transition);
    sr_voice *voice=track==SR_CHOIR&&e->vocals_enabled&&e->vocal_play?recorded_live_note(e,note,zone,96,1.f):
        note_on(e,track,note,96,1.f,0);
    if(voice){
      voice->vocal_consonant=consonant;
      voice->vocal_vowel_to=transition;
      voice->vocal_morph_rate=1.f/(float)(SR_RATE*3u);
      voice->key=tap?0:(uint8_t)(key+1);
      voice->remain=tap?(e->tick_length/4u+1u):SR_RATE*8u;
    }
}
static const uint8_t intention_feel[]={0,5,3,2,7,4,2,4,6,7,4,3,5,6,3,4,3,2,5,6,3,6};
_Static_assert(sizeof intention_feel/sizeof *intention_feel==SR_SCENES,"Complete feel profiles");

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
    if(key>=27||track>=SR_TRACKS)return;
    int singer=e->vocals_enabled&&e->vocal_play;
    if(singer)track=SR_CHOIR;
    if(!e->running){e->preview_role=track;e->preview_frames=SR_RATE*10u;play_key(e,track,key,0);return;}
    if(e->keyboard_mode&&!singer){
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
      release_voice(e,&e->voices[j]);

    }
}
/* Compose underneath the owner's density cap. A breakdown thins prepared
 * subdivisions while retaining its kick/bass thread and displayed defaults. */
static unsigned groove_density(const sr_engine *e,unsigned step){
 unsigned cap=e->drum_density;if(!cap||e->variation<=0.f)return cap;
 unsigned current=form_index(e,0),next=form_index(e,1);
 unsigned level[3]={2,5,8};
 unsigned density=level[sr_sections[current].pulse];
 if(sr_section_remaining(e)==1u&&!e->freeze){
  unsigned target=level[sr_sections[next].pulse];
  unsigned position=step&15u;
  density=(density*(16u-position)+target*position)/16u;
 }
 return density<cap?density:cap;
}
static float groove_kick(const sr_engine *e,unsigned step){
 if(!e->drums_enabled||!e->drum_density||e->drum_level<=0.f)return 0;
 if(e->drum_manual_mask&1u)return (e->drum_pattern[0]>>(step&15u))&1u ? 1.f : 0.f;
 unsigned density=groove_density(e,step);
 unsigned beat=step&15u,bar=score_bar(e,step);

 if(!beat||(beat==8u&&density>=2u))return 1.f;
 if(e->music_mode==SR_IDM&&density>=2u&&(beat==4u||beat==12u))return .92f;
 unsigned groove=(intention_score[e->scene][1]+e->music_mode+song_draw(e->song_seed,370))&3u;
 static const uint8_t reply[4][2]={{6,10},{11,7},{14,6},{10,13}};
 if(density>=4u&&beat==reply[groove][bar&1u])return .54f;
 if((e->rhythm_style==1u||e->music_mode==SR_IDM)&&density>=2u&&(beat==3u||beat==11u))return .65f;
 if(e->rhythm_style==2u&&beat==10u)return .68f;
 if(!e->freeze&&e->variation>0.f){
  unsigned id=form_index(e,0);
  if(sr_sections[id].pulse==2u&&beat==((id&1u)?10u:6u))return .5f;
  if(id==24u&&(beat==7u||beat==15u))return .46f;
 }
 return 0;
}
static void bass_groove(sr_engine *e,unsigned step){
 unsigned beat=step&15u,chord=composition_chord(e,step);
 float kick=groove_kick(e,step);
 unsigned motif=song_draw(e->song_seed,SR_BASS+400u)%3u;
 unsigned pickup=(beat==7u||beat==15u||(e->music_mode==SR_IDM&&(beat==3u||beat==11u)))&&e->energy>.55f&&
                  (step/16u+motif)%2u==1u;
 /* Firm anchors, deliberately un-doubled weak kicks and a recognizable
  * answer/pickup. The bass is a phrase partner, not a kick MIDI duplicate. */
 if(kick>0.f&&kick<.9f&&((motif+step/16u)&1u)==0u&&!pickup)return;
 if(!kick&&!pickup&&(e->drums_enabled||(beat&7u)))return;
 unsigned degree=chord;
 if(beat&&kick<.9f)degree=(chord+(motif==1u?2u:4u))%7u;
 if(pickup)degree=(composition_chord(e,(step+1u)%e->loop_length)+6u)%7u;
 int note=sr_tonic(e)+scale_notes[sr_scenes[e->scene].scale][degree];
 int prior=e->tracks[SR_BASS].last_note;
 if(beat&&prior){while(note-prior>7)note-=12;while(prior-note>7)note+=12;}
 while(note<33)note+=12;while(note>60)note-=12;
 unsigned gap=1;while(gap<8u&&!groove_kick(e,(step+gap)%e->loop_length))gap++;
 float length=(float)gap*(e->music_mode==SR_CINEMATIC?.22f:e->music_mode==SR_EXPERIMENTAL?.12f:.16f);
 if(pickup)length=.16f;
 sr_note_on(e,SR_BASS,note,pickup?76:kick>.9f?108:91,length);
}
static void drum_record(sr_engine *e,unsigned lane,unsigned velocity)
{
 unsigned bit=1u<<lane;
 e->drum_steps[e->drum_playhead]|=bit|(velocity>=96u?bit<<16u:0u);
 if(e->timeline_enabled){
   unsigned n=timeline_slot(e);
   e->timeline[n]=(sr_event){e->frame_count,e->tick_length/2u,SR_DRUMS,sr_drum_notes[lane],velocity,0};
 }
}
static void drum_strike(sr_engine *e,unsigned lane,unsigned velocity)
{
 drum_record(e,lane,velocity);
 e->tracks[SR_DRUMS].events++;
 float peak=velocity*(1.f/127.f);
 if(lane==0)e->kick_env=peak;
 else if(lane==1)e->hand_env=peak;
 else if(lane==2)e->shaker_env=peak;
 else{
   unsigned voice=lane-3u;
   e->drum_active|=1u<<voice;
   e->drum_phase[voice]=0;
   e->drum_amp[voice]=peak;
 }
 if((lane==2||lane==6)&&(e->drum_active&(1u<<2u)))e->drum_amp[2]*=.18f;
}

/* Stable accents from one bar/section score. The secondary kick never
 * replaces groove_kick(), which is the bassline's anchor. */
/* A stable draw per score position keeps detents nested and repeatable. */
static int drum_hit_allowed(const sr_engine *e,unsigned step,unsigned lane)
{
 unsigned rank=song_draw(e->song_seed, 701u+step*17u+lane+1093u*e->form_loop)%100u;
 return rank<e->drum_hit;
}

static void drum_colors(sr_engine *e,unsigned step,unsigned density)
{
 unsigned beat=step&15u,bar=e->form_loop,section=song_section(e);
 unsigned mask=0;
 if(density>=2&&beat==12&&section==2u)mask|=1u<<4;
 if(density>=3&&(beat==6||beat==14))mask|=1u<<(section==2u?5:6);
 if(density>=3&&beat==10&&(bar&1u))mask|=1u<<7;
 if(density>=4&&beat==15&&section!=3u)mask|=1u<<8;
 if(density>=4&&beat==7&&section==2u)mask|=1u<<3;
 if(density>=4&&beat==2&&(bar&1u))mask|=1u<<13;
 if(density>=5&&beat==0&&section==2u&&(bar&3u)==0u)mask|=1u<<11;
 if(density>=5&&(beat==0||beat==8)&&section==2u&&(bar&1u))mask|=1u<<12;
 if(density>=5&&beat==11&&section!=3u&&(bar&1u))mask|=1u<<14;
 if(density>=3&&(beat==8||beat==12)&&section==4u)mask|=1u<<15;
 if(density>=5&&sr_section_remaining(e)==1u){
   if(beat==13)mask|=1u<<10;
   if(beat==15)mask|=1u<<9;
 }
 mask&=~e->drum_manual_mask;
 while(mask){
   unsigned part=__builtin_ctz(mask);
   mask&=mask-1u;
   if(drum_hit_allowed(e,step,part))
     drum_strike(e,part,part==11u?77u:part==3u?86u:60u);
 }
}
static void drum_trigger(sr_engine *e,uint32_t step,uint8_t energy){
  e->drum_playhead=step&63u;
  e->drum_steps[e->drum_playhead]=0;
  if(!e->drums_enabled||!e->drum_density||e->drum_level<=0.f)return;
  unsigned density=groove_density(e,step);
  uint32_t beat=step&15u;
  unsigned bar=e->loop_length>16?step/16u:0;

  float prior_hand=e->hand_env, prior_hat=e->shaker_env;
  unsigned parts=0;
  uint32_t stable_draw=song_draw(e->song_seed+e->scene+bar*13u,beat);
  float kick=groove_kick(e,step);if(kick){e->kick_env=kick;parts|=1u;}
  if((beat==4||beat==12) && (density>=2 || (stable_draw&3u)==0)){
    e->hand_env=bar&&(beat==4?bar&1u:bar&2u)?.78f:1.f;parts|=2u;
  }
  if(density>=3 && (beat&1u)){e->shaker_env=.72f;parts|=4u;}
  else if(density==2 && (beat&3u)==2){e->shaker_env=.65f;parts|=4u;}
  else if(density==1 && beat==8 && (stable_draw&1u)==0){e->shaker_env=.45f;parts|=4u;}
  if(density>=4&&(beat&3u)==2){e->shaker_env=.56f;parts|=4u;}
  if(density>=5&&beat==(bar&1u?10u:6u)){e->hand_env=.48f;parts|=2u;}
  if(density>=7&&(beat==2||beat==14)){e->hand_env=.42f;parts|=2u;}
  if(density>=8&&(beat==3||beat==15)){e->hand_env=bar&1u?.52f:.37f;parts|=2u;}
  /* A bloom grows within the musical phrase, without global gain ramps.
   * Longer phrases have more gradual development and a small final answer. */
  if(!e->freeze&&e->fill_character!=3&&song_section(e)==2u&&e->loop_length>16){
    unsigned bar_progress=step/16u;
    if(bar_progress&&(beat&3u)==2u){e->shaker_env=.25f+.12f*bar_progress;parts|=4u;}
    if(bar_progress==e->loop_length/16u-1u&&beat==14u){e->hand_env=.43f;parts|=2u;}
  }
  /* Optional rhythm character: the downbeat is always preserved. */
  if((e->rhythm_style==1||e->music_mode==SR_IDM)&&density>=2){
    if(beat==7||beat==14){e->hand_env=.48f;parts|=2u;}
    if((beat&1u)&&((bar+beat)%3u)){e->shaker_env=.38f+.08f*(bar&1u);parts|=4u;}
  }else if(e->rhythm_style==2){
    if(beat==6||beat==13){e->hand_env=.42f;parts|=2u;}
  }
  /* Authored section percussion: current kit, no generator gating at
   * boundaries. Sparse functions keep the downbeat/tonal anchor. */
  if(!e->freeze&&e->variation>0.f){
    unsigned id=form_index(e,0),pulse=sr_sections[id].pulse;
    unsigned character=id%8u;
    if(pulse&&bar%2u&&beat==(character&1u?14u:6u)){e->hand_env=.28f+.03f*(character%3u);parts|=2u;}
    if(id==20u&&(beat&3u)==3u){e->shaker_env=.26f+.09f*(step/16u);parts|=4u;}
    if(id==30u&&(beat%3u)==2u){e->shaker_env=.32f;parts|=4u;}
  }
  /* One optional strike at most on an otherwise empty, eligible subdivision. */
  unsigned eligible=e->scene==6 ? (beat==3||beat==11) :
                    energy>=2 ? (beat==3||beat==7||beat==11||beat==15) :
                    (beat==7||beat==15);
  /* Answer bars add off-beat pickups, with a two-hit closing fill at full HIT. */
  if(bar&&density>=2){
    if(bar&1u)eligible|=beat==5||beat==13;
    if(bar&2u)eligible|=beat==1||beat==9;
  }
  if(eligible){
    e->drum_rng=e->drum_rng*1664525u+1013904223u;
    unsigned percent=e->drum_hit>SR_HIT_MAX?SR_HIT_MAX:e->drum_hit;
    unsigned allowance=e->music_mode==SR_CINEMATIC?22u:e->music_mode==SR_NEOCLASSICAL?34u:55u;
    if((stable_draw>>16)%100u<percent*allowance/100u){
      if(beat==11||(bar&&(beat==5||beat==13))){e->hand_env=.55f;parts|=2u;}
      else {e->shaker_env=.52f;parts|=4u;}
      e->optional_hits++;
    }
  }
  if((parts&2u)&&beat!=4u&&beat!=12u&&!drum_hit_allowed(e,step,1)){
    parts&=~2u;e->hand_env=prior_hand;
  }
  if((parts&4u)&&!drum_hit_allowed(e,step,2)){
    parts&=~4u;e->shaker_env=prior_hat;
  }
  unsigned manual=e->drum_manual_mask&~1u;
  if(manual&2u)e->hand_env=prior_hand;
  if(manual&4u)e->shaker_env=prior_hat;
  parts&=~manual;
  e->tracks[SR_DRUMS].events+=__builtin_popcount(parts);
  if(parts&1u)drum_record(e,0,(unsigned)(e->kick_env*127.f+.5f));
  if(parts&2u)drum_record(e,1,(unsigned)(e->hand_env*127.f+.5f));
  if(parts&4u){
    drum_record(e,2,(unsigned)(e->shaker_env*127.f+.5f));
    if(e->drum_active&(1u<<2u))e->drum_amp[2]*=.18f;
  }
  drum_colors(e,step,density);
  while(manual){
    unsigned part=__builtin_ctz(manual);
    manual&=manual-1u;
    if(e->drum_pattern[part]&(1u<<beat))drum_strike(e,part,96);
  }
}
void sr_preview_sound(sr_engine *e,unsigned role)
{
  if(e->running||e->stop_frames||e->external_audio||e->frequency_lock||role>=SR_TRACKS)return;
  if(!e->preview_frames)clear_transport(e);
  e->nature_audition=0;
  e->preview_role=role;
  e->preview_frames=SR_RATE;
  for(unsigned i=0;i<SR_VOICES;i++){
    release_voice(e,&e->voices[i]);
    e->voices[i].release_coef=.002f;
  }
  if(role==SR_DRUMS){
    if(!e->tracks[role].mute){e->kick_env=.62f;e->hand_env=.51f;e->shaker_env=.4f;}
  }else sr_note_on(e,role,(uint8_t)(sr_tonic(e)+(role==SR_BASS?0:24)),72,1.3f);
}

void sr_audition_vocal(sr_engine *e)
{
  const sr_track *vocal=&e->tracks[SR_CHOIR];
  if(!e->vocals_enabled||vocal->mute||!e->vocal_level||
     e->frequency_lock||e->stop_frames||e->nature_audition||
     (e->solo<SR_TRACKS&&e->solo!=SR_CHOIR)||
     (e->isolate!=SR_ISOLATE_MIX&&e->isolate!=SR_ISOLATE_TRACK1+SR_CHOIR))return;
  if(!e->running){
    if(!e->preview_frames)clear_transport(e);
    e->nature_audition=0;
    e->preview_role=SR_CHOIR;
    e->preview_frames=SR_RATE*3u;
  }
  recorded_note(e,sr_tonic(e)+24,0,96,1.1f);
}

void sr_audition_drum(sr_engine *e,unsigned lane)
{
  if(lane>=SR_DRUM_PARTS||e->frequency_lock||e->tracks[SR_DRUMS].mute)return;
  if(!e->running){
    if(e->stop_frames||e->external_audio)return;
    if(!e->preview_frames)clear_transport(e);
    e->nature_audition=0;
    e->preview_role=SR_DRUMS;
    e->preview_frames=SR_RATE;
  }
  drum_strike(e,lane,104);
}

static inline __attribute__((always_inline)) float drum_lane_level(const sr_engine *e,unsigned lane)
{
 return (e->drum_mutes&(1u<<lane))||(e->drum_solo<SR_DRUM_PARTS&&e->drum_solo!=lane)
        ?0.f:e->drum_levels[lane]*.01f;
}

/* Update the lightweight one-shot bus at its native 11.025 kHz rate. Linear
 * interpolation is shared by the whole bus. All live reads are from SRAM. */
static inline __attribute__((always_inline)) float drum_aux(sr_engine *e)
{
 if(!e->drum_active&&e->drum_aux_to==0.f&&e->drum_aux_from==0.f)return 0.f;
 unsigned phase=e->drum_subsample;
 if(!phase){
   e->drum_aux_from=e->drum_aux_to;
   float mixed=0.f;
   unsigned live=e->drum_active;
   while(live){
     unsigned i=__builtin_ctz(live);
     live&=live-1u;
     unsigned index=e->drum_phase[i]>>8u;
     if(index>=DRUM_WAVE_LENGTH){e->drum_active&=~(1u<<i);continue;}
     mixed+=drum_pcm[i][index]*e->drum_amp[i]*drum_lane_level(e,i+3u);
     /* Each kit now has a distinct sampled-drum tuning. The old
      * modulo-seven rule repeated identical rates across 19 kits.
      * One table lookup per subsampled voice keeps CPU use bounded. */
     static const uint16_t kit_sample_rate[SR_DRUM_KITS]={
       226u,242u,263u,208u,251u,278u,231u,295u,247u,220u,
       273u,237u,201u,282u,256u,304u,214u,291u,268u
     };
     e->drum_phase[i]+=kit_sample_rate[e->drum_kit];
   }
   e->drum_aux_to=mixed*(.28f/128.f);
 }
 e->drum_subsample=(phase+1u)&3u;
 return e->drum_aux_from+(e->drum_aux_to-e->drum_aux_from)*((phase+1u)*.25f);
}
static inline __attribute__((always_inline)) float drum_sample(sr_engine *e){
  /* Silence is a common path in the default generator. Skip all oscillators,
   * noise and kit inspection when no percussion envelope is active. */
  if(e->kick_env<=.0003f&&e->hand_env<=.0003f&&e->shaker_env<=.0003f){
    e->kick_env=e->kick_previous=e->kick_attack=e->hand_env=e->shaker_env=0.f;
    return drum_aux(e)*e->drum_level;
  }
  const sr_drum_kit *kit=&sr_drum_kits[e->drum_kit];
  for(unsigned part=0;part<3;part++){
    float target=drum_lane_level(e,part);
    float gain=e->drum_part_gain[part];
    if(gain!=target)e->drum_part_gain[part]=clampf(gain+clampf(target-gain,-.004f,.004f),0.f,1.f);
  }
  float sound=0.f;
  unsigned fm=e->drum_kit>=15;
  if(e->kick_env>.0003f){
    float hz=kit->root+45.f*e->kick_env;
    e->kick_phase+=hz/(float)SR_RATE;if(e->kick_phase>=1.f)e->kick_phase-=1.f;
    float tone=fm?sine(1.f+e->kick_phase+.15f*e->kick_env*sine(e->kick_phase*2.f)):
                  sine(e->kick_phase)*.82f+sine(e->kick_phase*2.f)*.18f;
    /* Short, brightness-aware noise attack: makes soft 50-70Hz kicks
     * perceptible on the FM-1 speaker without boosting the whole mix. */
    /* Each triggered hit gets an attack, including quiet velocity hits.
     * The previous >.91 envelope threshold skipped almost every generated
     * kick, whose initial envelope is usually much smaller than .91. */
    if(e->kick_env>e->kick_previous+.003f)e->kick_attack=e->kick_env;
    float transient=(rnd(e)*2.f-1.f)*e->kick_attack*(.055f+kit->skin*.000045f);
    e->kick_attack*=.982f;
    sound+=((tone*.43f+sine(e->kick_phase*3.f)*.035f)*e->kick_env+transient)*kit->kick*e->drum_part_gain[0];
    e->kick_env*=kit->decay;
  }else e->kick_env=0.f;
  e->kick_previous=e->kick_env;
  if(e->hand_env>.0003f){
    float noise=rnd(e)*2.f-1.f;
    e->hand_noise_lp+=(noise-e->hand_noise_lp)*kit->brush;
    e->hand_phase+=kit->skin*(1.f/SR_RATE);if(e->hand_phase>=1.f)e->hand_phase-=1.f;
    float tap=fm?sine(1.f+e->hand_phase+.21f*e->hand_env*sine(e->hand_phase*3.f)):
                 sine(e->hand_phase);
    float brightness=.25f+kit->skin*.0006f;
    if(brightness>.58f)brightness=.58f;
    float attack=e->hand_env>.84f?(e->hand_env-.84f)*6.25f:0.f;
    float bright=(noise-e->hand_noise_lp)*brightness+noise*attack*.2f;
    float body=tap*(.78f-brightness*.5f)+e->hand_noise_lp*.19f;
    sound+=(body+bright)*e->hand_env*kit->hand*.43f*e->drum_part_gain[1];
    e->hand_env*=.99925f;
  }else e->hand_env=0.f;
  if(e->shaker_env>.0003f){
    float noise=rnd(e)*2.f-1.f;
    e->shaker_lp+=(noise-e->shaker_lp)*(.20f+kit->brush*.3f);
    float brightness=1.f+kit->skin*.0006f;
    sound+=(noise-e->shaker_lp)*e->shaker_env*kit->shaker*.28f*brightness*e->drum_part_gain[2];
    e->shaker_env*=.9967f;
  }else e->shaker_env=0.f;
  sound=(sound+drum_aux(e))*1.14f;
  e->drum_meter=fmaxf(e->drum_meter*.9993f,fabsf(sound));
  return sound*e->drum_level;
}
/* Female VocalSet vowels, arranged as an original E5–B4–A4 signature.
 * The short stereo taps are local to boot and cannot claim sequencer voices. */
static float startup_vowel(unsigned frame)
{
  static const unsigned starts[]={SR_RATE/5u,SR_RATE*17u/10u,SR_RATE*31u/10u};
  static const unsigned lengths[]={SR_RATE*17u/10u,SR_RATE*16u/10u,SR_RATE*27u/10u};
  static const uint8_t zones[]={4,3,5};
  static const float steps[]={659.255f/525.26014f*.25f,493.883f/525.21703f*.25f,
                            440.f/525.81201f*.25f};
  float mix=0.f;
  for(unsigned note=0;note<3;note++){
    if(frame<starts[note]||frame-starts[note]>=lengths[note])continue;
    unsigned age=frame-starts[note];
    unsigned zone=zones[note],loop=vocal_zones[zone].loop;
    float pos=age*steps[note];
    unsigned index=(unsigned)pos;
    float fraction=pos-index;
    if(index>=VOCAL_LENGTH)index=loop+(index-loop)%(VOCAL_LENGTH-loop);
    unsigned next=index+1u<VOCAL_LENGTH?index+1u:loop;
    float sample=vocal_flash[zone][index]+
        (vocal_flash[zone][next]-vocal_flash[zone][index])*fraction;
    float attack=clampf(age*(14.f/SR_RATE),0.f,1.f);
    float release=clampf((lengths[note]-age)*(3.8f/SR_RATE),0.f,1.f);
    mix+=sample*attack*release;
  }
  return mix;
}

void sr_startup_vocal(unsigned frame,float *left,float *right)
{
  static const unsigned taps[]={1323,3137,6615,13892,22535,37620,57065,79710};
  static const uint8_t gains[]={6,5,4,3,3,2,2,1};
  float dry=startup_vowel(frame)*21.f;
  *left=*right=dry;
  for(unsigned tap=0;tap<sizeof(gains);tap++)if(frame>=taps[tap]){
    float wet=startup_vowel(frame-taps[tap])*gains[tap];
    if(tap&1u){*right+=wet;*left+=wet*.45f;}
    else {*left+=wet;*right+=wet*.45f;}
  }
}

static inline __attribute__((always_inline)) float advance(float *phase,float hz){
  *phase+=hz/(float)SR_RATE;
  if(*phase>=1.f)*phase-=1.f;
  return sine(*phase);
}
typedef struct { float target,step_l,step_r,gain; int steady; } binaural_block;
static inline __attribute__((always_inline)) binaural_block prepare_binaural(const sr_engine *e){
  binaural_block b;
  b.target=(e->running && e->binaural_enabled && !e->speaker_enabled && e->solo==SR_TRACKS)?1.f:0.f;
  b.steady=e->binaural_env==b.target && e->carrier_now==e->binaural_carrier &&
           e->beat_now==e->binaural_rate;
  b.step_l=(e->carrier_now-e->beat_now*.5f)*(1.f/(float)SR_RATE);
  b.step_r=(e->carrier_now+e->beat_now*.5f)*(1.f/(float)SR_RATE);
  b.gain=e->binaural_level*e->binaural_env;
  return b;
}
/* Float smoothing eventually stalls short of its target. Snap only at that
   representational limit; this also makes the exact steady path reachable. */
static inline __attribute__((always_inline)) float binaural_smooth(float now,float target,float rate){
  float next=now+(target-now)*rate;
  return next==now?target:next;
}
static inline __attribute__((always_inline)) void render_binaural(sr_engine *e,const binaural_block *b,float *left,float *right){
  // Each ear gets a separate sinusoid, mixed AFTER shared FX to preserve
  // the ear-specific frequency difference. Never collapse to mono.
  if(b->steady){
    e->binaural_phase_l+=b->step_l;e->binaural_phase_r+=b->step_r;
    if(e->binaural_phase_l>=1.f)e->binaural_phase_l-=1.f;
    if(e->binaural_phase_r>=1.f)e->binaural_phase_r-=1.f;
    *left+=sine(e->binaural_phase_l)*b->gain;
    *right+=sine(e->binaural_phase_r)*b->gain;
    return;
  }
  e->binaural_env=binaural_smooth(e->binaural_env,b->target,.0009f);
  if(e->binaural_env<.00003f && !b->target){e->binaural_env=0;return;}
  e->carrier_now=binaural_smooth(e->carrier_now,e->binaural_carrier,.0005f);
  e->beat_now=binaural_smooth(e->beat_now,e->binaural_rate,.0005f);
  float hz_left=e->carrier_now-e->beat_now*.5f;
  float hz_right=e->carrier_now+e->beat_now*.5f;
  *left+=advance(&e->binaural_phase_l,hz_left)*e->binaural_level*e->binaural_env;
  *right+=advance(&e->binaural_phase_r,hz_right)*e->binaural_level*e->binaural_env;
}
static const uint8_t ornament_track[]={SR_PIANO,SR_GUITAR,SR_LEAD,SR_HARMONY};
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
    e->ornaments[slot].beats=beats;
  }else sr_note_on(e,ornament_track[slot],midi,velocity,beats);
}
/* Fixed-capacity control-rate event queue. Overflow drops optional detail.
 * Bass, direct keys and base drum pulse never use this queue. */
static unsigned schedule(sr_engine *e,unsigned role,unsigned note,unsigned velocity,float beats,unsigned delay)
{
 for(unsigned i=0;i<12;i++)if(!e->scheduled[i].active){
  e->scheduled[i].due=e->frame_count+delay;e->scheduled[i].role=role;
  e->scheduled[i].note=note;e->scheduled[i].velocity=velocity;
  e->scheduled[i].beats=beats;e->scheduled[i].active=1;return 1;
 }
 e->scheduled_overflows++;return 0;
}
/* Match physical plucked-string patches by their append-only patch IDs. */
static unsigned string_roll_gap(unsigned patch,unsigned mode)
{
 static const uint8_t guitar[SR_MUSIC_MODES]={180,205,190,180,180};
 static const uint8_t harp[SR_MUSIC_MODES]={200,230,215,190,195};
 if(patch==9u||(patch>=36u&&patch<=39u))return guitar[mode];
 if(patch==10u||patch==27u||(patch>=40u&&patch<=42u))return harp[mode];
 return 0;
}

static unsigned chord_voice(sr_engine *e,unsigned step,unsigned flourish)
{
 if(e->chord_mode==3||e->frequency_lock)return 0;
 unsigned role=e->music_mode==SR_NEOCLASSICAL?SR_PIANO:SR_GUITAR;
 if(e->tracks[role].mute||(e->solo<SR_TRACKS&&e->solo!=role)||e->role_target[role]<.01f||e->tracks[role].volume<=0)return 0;
 /* The committed score is independent of instantaneous DSP voice tails.
  * The existing priority allocator bounds sounding polyphony to 16. */
 unsigned count=e->music_mode==SR_CINEMATIC?2u:3u;
 unsigned degree=phrase_chord(e,step/16u),inversion=e->chord_inversion;
 if(!inversion)inversion=1u+song_draw(e->song_seed,e->phrase+step/16u)%3u;
 int notes[3];const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 for(unsigned i=0;i<count;i++){
  unsigned d=degree+i*2u+(flourish?2u:0u);notes[i]=sr_tonic(e)+12+scale[d%7u]+12*(d/7u);
  if(i<inversion-1u)notes[i]+=12;
  if(e->chord_previous[i]){
   while(notes[i]-e->chord_previous[i]>7)notes[i]-=12;
   while(e->chord_previous[i]-notes[i]>7)notes[i]+=12;
  }
 }
 for(unsigned i=0;i<count;i++)for(unsigned j=i+1;j<count;j++)if(notes[j]<notes[i]){int n=notes[i];notes[i]=notes[j];notes[j]=n;}
 unsigned auto_gap=string_roll_gap(e->tracks[role].patch,e->music_mode);
 unsigned gap=e->chord_mode==2?e->chord_gap:e->chord_mode==0?auto_gap:0;
 unsigned reverse=e->chord_direction==2||(e->chord_direction==3&&(e->chord_alternate++&1u));
 if(!e->chord_direction&&auto_gap)reverse=((step/16u+e->form_loop)&1u)!=0u;
 unsigned performed=0;
 for(unsigned i=0;i<count;i++){
  unsigned index=reverse?count-1u-i:i;
  e->chord_previous[index]=notes[index];
  unsigned delay=(i*gap*SR_RATE+500u)/1000u;
  if(e->chord_mode==0)delay/=(count-1u); /* AUTO sets the entire stroke span. */
  if(delay)performed+=schedule(e,role,notes[index],58-i*4,2.8f,delay);
  else {unsigned before=e->tracks[role].events;sr_note_on(e,role,notes[index],62-i*4,2.8f);performed+=e->tracks[role].events!=before;}
 }
 return performed;
}
static unsigned chord_phrase(sr_engine *e,unsigned step)
{
 /* In AUTO, the Neoclassical piano has its own hand voicings. A plucked
  * string patch assigned to the piano role still receives its roll. */
 if(e->music_mode==SR_NEOCLASSICAL&&e->chord_mode==0&&
    !string_roll_gap(e->tracks[SR_PIANO].patch,e->music_mode))return 0;
 return chord_voice(e,step,0);
}
static unsigned transition_fill(sr_engine *e,unsigned step,int gesture)
{
 if(e->freeze||!e->drums_enabled||!e->drum_density||e->fill_character==3||
    e->tracks[SR_DRUMS].mute||(e->solo<SR_TRACKS&&e->solo!=SR_DRUMS)||e->tracks[SR_DRUMS].volume<=0||e->drum_level<=0||e->role_target[SR_DRUMS]<.1f)return 0;
 unsigned current=form_index(e,0),next=form_index(e,1);
 unsigned preparing=(sr_section_remaining(e)==1u)&&current!=next;
 unsigned style=current%5u;
 unsigned cadence=(step&15u)==15u;
 if(!gesture&&(!cadence||(!preparing&&(e->phrase+style)%5u!=4u)))return 0;
 unsigned count=gesture?3u:preparing?2u+sr_sections[next].pulse:2u;
 if(e->music_mode==SR_NEOCLASSICAL&&count>2u)count=2u;
 unsigned performed=0;for(unsigned i=0;i<count;i++)
   performed+=schedule(e,SR_DRUMS,(const uint8_t[]){10,9,8,14,7}[(i+style)%5u],
       34u+i*7u+(preparing?7u:0u),0,(i+1u)*e->tick_length/(count+1u));
 return performed;
}
static int phrase_expression(unsigned step){
  static const int8_t arc[8]={-4,0,4,6,2,0,-3,-7};
  return arc[(step&15u)/2u];
}
/* The bass owns the low register. Place the pad above it, favouring
 * a common tone through the upcoming change and stepwise motion from
 * the previous chord. These decisions depend on the score, even in solo. */
static unsigned support_degree(const sr_engine *e,unsigned chord,unsigned next,
                               unsigned reference,int moving)
{
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 unsigned best=chord,best_cost=100u;
 for(unsigned voice=0;voice<3;voice++){
  unsigned degree=(chord+2u*voice)%7u;
  int distance=(scale[degree]%12)-(scale[reference]%12);
  if(distance<0)distance=-distance;
  if(distance>6)distance=12-distance;
  unsigned common=degree==next||degree==(next+2u)%7u||degree==(next+4u)%7u;
  unsigned cost=2u*distance+3u*!common+3u*(voice==0)+8u*(moving&&degree==reference);
  if(cost<best_cost){best_cost=cost;best=degree;}
 }
 return best;
}

static void accompaniment(sr_engine *e,unsigned bar,unsigned chord)
{
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 const int tonic=sr_tonic(e);
 unsigned previous=bar?phrase_chord(e,bar-1u):chord;
 unsigned next=phrase_chord(e,bar+1u);
 unsigned inner=support_degree(e,chord,next,(previous+2u)%7u,0);
 int note=tonic+12+scale[inner];
 /* A root kick uses this same harmony in bass_groove. Match its
  * register when moving the pad above a high tonic override. */
 int bass=tonic+scale[chord];
 while(bass<33)bass+=12;
 while(bass>60)bass-=12;
 while(note<bass+10||note<55)note+=12;
 unsigned common=inner==next||inner==(next+2u)%7u||inner==(next+4u)%7u;
 sr_note_on(e,SR_DRONE,note,e->music_mode==SR_CINEMATIC?58:49,common?4.0f:3.15f);
 /* Open the voicing at a crest without layering another bass root. */
 if((song_section(e)==2u&&e->current_energy>=.35f&&e->music_mode<=SR_CINEMATIC)||
    (e->current_energy>.78f&&song_section(e)==4u)){
  unsigned upper=(chord+(inner==(chord+4u)%7u?2u:4u))%7u;
  int upper_note=tonic+12+scale[upper];
  while(upper_note<note+3)upper_note+=12;
  sr_note_on(e,SR_DRONE,upper_note,39,2.65f);
 }
}

/* Real, pitched human takes use the dedicated vocal lane and shared voice pool.
 * Fit each chord tone into one of two vocal registers before transposition. */
/* Recorded samples already have a calibrated root. Snap played chromatic
 * notes toward the nearest scale degree, retaining pitch bend at low mix. */
static float vocal_pitch_target(const sr_engine *e,const sr_voice *voice)
{
  int played=voice->midi, tonic=sr_tonic(e)%12;
  /* A nonzero legacy value remains an explicit setting. Bit 3 also locks
     an intentional zero; AUTO reset clears both. No bank format changes. */
  float tune=e->vocal_tune;
  if(!(e->vocal_fx_manual&8u))
    tune=(e->music_mode==SR_IDM?72.f:e->music_mode==SR_NEOCLASSICAL?18.f:36.f)
      +intention_score[e->scene][0]*2.f+(song_section(e)==2u?8.f:0.f);
  if(!tune)return voice->freq;
  const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
  int best=played, distance=13;
  for(int delta=-2;delta<=2;delta++){
    int candidate=played+delta;
    if(candidate<0||candidate>127)continue;
    unsigned degree=(candidate-tonic+12)%12;
    int separation=delta<0?-delta:delta;
    for(unsigned i=0;i<7;i++)if((unsigned)scale[i]%12u==degree&&separation<distance){
      best=candidate;
      distance=separation;
    }
  }
  float corrected=sr_note_frequency(e,best);
  return voice->freq+(corrected-voice->freq)*(tune*.01f);
}

static sr_voice *recorded_voice(sr_engine *e,int note,unsigned zone,unsigned velocity,float beats,unsigned live)
{
 if(!e->vocals_enabled||zone>VOCAL_LAST_ZONE)return 0;
 unsigned occupied=0,manual=0;
 for(unsigned i=0;i<SR_VOICES;i++){
   sr_voice *v=&e->voices[i];
   if(v->active&&v->sample_zone){occupied++;manual+=v->gate&&(v->key||v->external);}
 }
 sr_voice *replacement=0;
 if(occupied>=2u){
   for(unsigned i=0;i<SR_VOICES;i++){
     sr_voice *v=&e->voices[i];
     if(!v->active||!v->sample_zone)continue;
     unsigned held=v->gate&&(v->key||v->external);
     if(held&&manual<occupied)continue;
     /* AUTO+Play Along can borrow one slot when both are held by hand. */
     if(held&&!live&&e->vocal_play==1u)continue;
     if(!replacement||(!v->gate&&replacement->gate)||
        (v->gate==replacement->gate&&e->frame_count-v->born>e->frame_count-replacement->born))
       replacement=v;
   }
   if(!replacement){e->vocal_sample_denied++;e->vocal_rejected++;return 0;}
 }
 float source_pitch=zone>=VOCAL_DAY_ZONE?vocal_words[zone-VOCAL_DAY_ZONE].hz:vocal_zones[zone].hz;
 int root=zone>=VOCAL_SHINE_ZONE?64:zone==VOCAL_DREAMS_ZONE?57:source_pitch<400.f?60:72;
 while(note>root+6)note-=12;
 while(note<root-6)note+=12;
 sr_voice *voice=allocate_note(e,SR_CHOIR,note,velocity,beats,1,replacement);
 if(!voice){e->vocal_budget_denied++;e->vocal_rejected++;return 0;}
 if(replacement)e->vocal_reclaimed++;
 voice->sample_zone=zone+1u;
 e->vocal_started++;
 voice->freq=vocal_pitch_target(e,voice);
 voice->pitch=voice->freq;
 if(e->vocal_param_now[VF_GLIDE]>=1.f&&e->vocal_pitch_last>0.f){
   float ratio=e->vocal_pitch_last/voice->freq;
   if(ratio>.74f&&ratio<1.35f)voice->pitch=e->vocal_pitch_last;
 }
 e->vocal_pitch_last=voice->freq;
 voice->phase2=voice->freq*(VOCAL_RATE/(float)SR_RATE)/source_pitch;
 voice->attack_coef=zone>=VOCAL_DAY_ZONE?.004f:.00065f;
 voice->release_coef=zone==VOCAL_DAY_ZONE||zone==VOCAL_YOU_ZONE||zone>=VOCAL_LOVE_ZONE?.00010f:.00095f;
 return voice;
}

static sr_voice *recorded_note(sr_engine *e,int note,unsigned zone,unsigned velocity,float beats)
{
 return recorded_voice(e,note,zone,velocity,beats,0);
}
static sr_voice *recorded_live_note(sr_engine *e,int note,unsigned zone,unsigned velocity,float beats)
{
 return recorded_voice(e,note,zone,velocity,beats,1);
}

/* Follow the existing per-song section route. Presence changes verse
 * density while a crest and reprise keep the same recognizable hook. */
unsigned sr_vocal_status(const sr_engine *e)
{
 if(!e->vocals_enabled)return SR_VOCAL_OFF;
 if(!e->running&&!e->preview_frames&&!e->external_audio)return SR_VOCAL_STOPPED;
 if(e->frequency_lock)return SR_VOCAL_LOCKED;
 if(!e->vocal_level)return SR_VOCAL_LEVEL;
 if(e->tracks[SR_CHOIR].mute)return SR_VOCAL_MUTED;
 if(e->solo<SR_TRACKS&&e->solo!=SR_CHOIR)return SR_VOCAL_SOLO;
 if(e->isolate!=SR_ISOLATE_MIX&&e->isolate!=SR_ISOLATE_TRACK1+SR_CHOIR)return SR_VOCAL_ISOLATED;
 if(e->vocal_play==1u&&!e->vocal_reprise)return SR_VOCAL_MANUAL;
 return SR_VOCAL_READY;
}

static unsigned featured_vocal_section(const sr_engine *e,unsigned step)
{
 if(!e->vocals_enabled||!e->vocal_presence||(e->vocal_play==1u&&!e->vocal_reprise)||e->frequency_lock||
    !e->vocal_level||e->tracks[SR_CHOIR].mute||
    (e->isolate!=SR_ISOLATE_MIX&&e->isolate!=SR_ISOLATE_TRACK1+SR_CHOIR)||
    (e->solo<SR_TRACKS&&e->solo!=SR_CHOIR))return 0;
 if(e->vocal_reprise)return 2u;
 if(e->vocal_entry_pending)return e->vocal_presence>=3u?2u:1u;
 unsigned bar=score_bar(e,step);
 unsigned family=song_section(e);
 unsigned id=form_index(e,0);
 if(!family||family==3u){
   /* Sparse humming in spacious sections, with entire bars left to breathe.
      Low user presence retains the original instrumental intro/bridge. */
   unsigned breathing=2u+(intention_score[e->scene][1]&1u);
   return e->vocal_presence>=3u&&bar%breathing==0u?1u:0u;
 }
 if(id==9u)return (bar&1u)?1u:0u; /* Bridge: alternate voice and band. */
 if(family==2u||family==4u)return e->vocal_presence>=2u?2u:(bar&1u?0u:2u);
 return (bar&3u)<e->vocal_presence?1u:0u;
}

/* A song establishes a primary delivery. Sections choose articulations
 * within it, and a recalled hook restores its actual vowel and onset. */
static unsigned vocal_character(const sr_engine *e)
{
 unsigned draw=song_draw(e->song_seed,922u)+intention_score[e->scene][0]*7u+intention_score[e->scene][2]*3u;
 if(e->music_mode==SR_CINEMATIC)return draw%3u==0u?1u:0u;
 if(e->music_mode==SR_NEOCLASSICAL)return draw%2u?1u:0u;
 if(e->music_mode==SR_IDM)return 2u+draw%3u;
 static const uint8_t characters[]={0,1,2,3,4,5};
 return characters[draw%6u];
}

/* Deterministic section development around a stable song identity. No RNG,
 * sample allocation, or changes to user presence/eligibility in this plan. */
static unsigned vocal_delivery(const sr_engine *e,unsigned section)
{
 unsigned identity=vocal_character(e);
 unsigned mood=intention_score[e->scene][1];
 if(section==0u||section==3u)return 1u; /* breathing intro/bridge */
 if(e->music_mode==SR_CINEMATIC)return section==2u?0u:identity;
 if(e->music_mode==SR_NEOCLASSICAL)return section==2u&&mood%3u==0u?0u:identity;
 if(section==2u)return e->music_mode==SR_IDM?3u:4u;
 if(section==4u)return identity; /* outro quotes the original delivery */
 return identity;
}

static unsigned vocal_flow_for(const sr_engine *e,unsigned section)
{
 if(e->vocal_flow)return e->vocal_flow;
 unsigned mood=intention_score[e->scene][1];
 /* The tick is one sixteenth note. 8583 samples is about 77 BPM. */
 if(section==0u||section==3u||section==4u)return SR_VOCAL_HALF;
 if(section==2u&&(e->music_mode==SR_IDM||
      (e->current_energy>.72f&&e->tick_length>=8583u&&mood>=4u)))
   return SR_VOCAL_DOUBLE;
 if((e->music_mode==SR_CINEMATIC||e->music_mode==SR_NEOCLASSICAL)&&
    (mood<4u||e->tick_length>=11025u))return SR_VOCAL_HALF;
 return SR_VOCAL_NORMAL;
}

static unsigned sung_refrain(const sr_engine *e,unsigned section,unsigned slot)
{
 static const uint8_t couplets[][2]={
   {VOCAL_SHINE_ZONE,VOCAL_TONIGHT_ZONE}, {VOCAL_FLY_ZONE,VOCAL_AWAY_ZONE},
   {VOCAL_STAY_ZONE,VOCAL_HOME_ZONE}, {VOCAL_STAY_ZONE,VOCAL_ALIVE_ZONE},
   {VOCAL_FOREVER_ZONE,VOCAL_FREE_ZONE}, {0,VOCAL_HOPE_ZONE}
 };
 if(slot==1u||section==0u||section==3u)return 0;
 unsigned mood=intention_score[e->scene][1];
 unsigned style=e->music_mode;
 unsigned draw=song_draw(e->song_seed,958u);
 unsigned phrase=(mood+draw%6u)%6u;
 if(style==SR_CINEMATIC||style==SR_NEOCLASSICAL)
   phrase=(mood+draw)%3u==0u?5u:(mood+draw)&1u?2u:4u;
 if(style==SR_IDM)
   phrase=(mood+draw+!!(e->tick_length<8583u))%3u;
 return couplets[phrase][slot==2u];
}

static int recorded_vocal_phrase(sr_engine *e,unsigned step,unsigned chord)
{
 unsigned part=featured_vocal_section(e,step);
 if(!part)return 0;
 unsigned section=song_section(e),bar=score_bar(e,step),pos=step&15u,mode=e->music_mode;
 unsigned flow=vocal_flow_for(e,section);
 unsigned opening=e->vocal_entry_pending?pos:0u, answer=6u, sustain=11u;
 if(flow==SR_VOCAL_HALF)sustain=10u;
 unsigned start=step-pos;
 if(part==2u&&mode!=SR_CINEMATIC){
   answer=5u+song_draw(e->song_seed,762u+(bar&1u))%3u;
   if(groove_kick(e,start+answer)||
      ((e->drum_manual_mask&2u)&&(e->drum_pattern[1]&(1u<<answer)))){
      unsigned next=answer==7u?5u:answer+1u;
      if(!groove_kick(e,start+next)&&
         (!(e->drum_manual_mask&2u)||!(e->drum_pattern[1]&(1u<<next))))answer=next;
   }
 }
 if((mode==SR_IDM||e->drum_manual_mask)&&groove_kick(e,start+sustain))sustain=13u;
 unsigned extra=flow==SR_VOCAL_DOUBLE&&pos!=opening&&pos!=sustain&&pos!=answer&&
      (pos==3u||pos==9u||pos==14u)&&!groove_kick(e,step)&&part==2u&&e->vocal_presence>=3u;
 unsigned answer_note=pos==answer&&part==2u&&e->vocal_presence>=3u&&flow!=SR_VOCAL_HALF;
 if(pos!=opening&&pos!=sustain&&!answer_note&&!extra)return 0;

 int call=(int)chord;
 if(part==2u){
   /* Reprises and climaxes quote the original song theme, even after
      the instrumental motif has developed. Cadences keep a chord tone. */
   unsigned hook_step=pos==opening?0u:pos==answer?4u:8u;
   if(e->motif_home[hook_step]>=0)call=e->motif_home[hook_step];
 }else{
   for(unsigned back=1;back<=4u&&back<=step;back++){
     int degree=motif_degree(e,step-back);
     if(degree>=0){call=degree;break;}
   }
 }
 unsigned hook_note=pos==opening?0u:pos==answer?1u:2u;
 if(part==2u&&e->vocal_hook[hook_note])call=e->vocal_hook[hook_note]-1u;
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 unsigned target=phrase_chord(e,step/16u+1u);
 unsigned cadence=pos==sustain&&sr_section_remaining(e)==1u;
 /* Once the refrain is established, answer its opening tone with motion
  * on a crest. The saved opening and closing anchors still identify it. */
 unsigned lift=part==2u&&section==2u&&pos==answer&&pos!=opening&&
               (bar&1u)&&e->vocal_hook[0];
 if(lift)call=e->vocal_hook[0]-1u;
 unsigned tone=cadence?chord:support_degree(e,chord,target,(unsigned)call,lift);
 /* Recorded zones are A, O, U in each register. */
 unsigned character=vocal_delivery(e,section);
 unsigned phrase=song_draw(e->song_seed,923u);
 unsigned vowel=pos==opening?2u:pos==sustain?0u:((phrase>>2u)&1u);
 unsigned zone=vowel+(mode==SR_CINEMATIC||((phrase>>3u)&1u)?3u:0u);
 unsigned attack=0u,transition=0u;
 unsigned velocity=(pos==answer?63u:part==2u?81u:72u)+4u*e->vocal_presence;
 float held=pos==opening?(part==2u?1.42f:1.78f):pos==answer?.68f:1.05f;
 if(pos==answer&&pos!=opening){
   velocity+=mode==SR_IDM?9u:3u;
   if(e->drum_manual_mask&4u)
      velocity+=(e->drum_pattern[2]&(1u<<pos))?4u:0u;
 }
 if(cadence)held=(mode==SR_CINEMATIC||mode==SR_NEOCLASSICAL)?1.55f:1.27f;
 if(lift)velocity+=5u;
 /* Phrase length and accent follow the intention's breathing profile and
    the actual bass/drum pulse; these do not increase the presence cap. */
 float breath=(float)intention_score[e->scene][1]/7.f;
 held*=mode==SR_CINEMATIC?1.18f+breath*.18f:
       mode==SR_NEOCLASSICAL?1.04f+breath*.12f:
       mode==SR_IDM?.76f+breath*.12f:.92f+breath*.16f;
 if(groove_kick(e,step))velocity+=3u;
 velocity+=(unsigned)(clampf(e->current_energy,0.f,1.f)*5.f);
 if(flow==SR_VOCAL_HALF)held*=1.75f;
 if(flow==SR_VOCAL_DOUBLE)held*=extra?.42f:.69f;
 float until_bar=(16u-pos)*.25f;
 if(held>until_bar)held=until_bar;
 if(extra)velocity-=8u;
 if(velocity>120u)velocity=120u;
 unsigned lexical=character==2u&&section!=0u&&section!=3u;
 if(lexical){
   static const uint8_t words[][3]={
     {VOCAL_BABY_ZONE,VOCAL_LOVE_ZONE,VOCAL_YOU_ZONE},
     {VOCAL_YOU_ZONE,VOCAL_LOVE_ZONE,VOCAL_ME_ZONE},
     {VOCAL_LOVE_ZONE,VOCAL_YOU_ZONE,VOCAL_YOU_ZONE}
   };
   zone=words[phrase%3u][hook_note];
 }else if(character==1u||section==0u){
   zone=VOCAL_HUM_ZONE;
   if(section==2u&&pos==sustain){zone=5u;transition=VOCAL_EE_ZONE+1u;}
 }else if(character==3u){
   /* The same syllable figure returns throughout this song. */
   static const uint8_t vowels[][3]={{1,VOCAL_EH_ZONE,VOCAL_EE_ZONE},{0,0,0},{0,0,0},{0,1,VOCAL_EH_ZONE}};
   static const uint8_t attacks[][3]={{4,5,1},{2,2,2},{3,3,3},{6,4,7}};
   zone=vowels[phrase%4u][hook_note];
   attack=attacks[phrase%4u][hook_note];
   held*=mode==SR_IDM?.62f:.82f;
 }else if(character==4u&&part==2u){
   zone=hook_note==1u?0u:1u;
   /* Two-note OH/AH refrain, resolved to a chord tone at its cadence. */
   if(hook_note==1u)tone=(chord+2u)%7u;
   else if(hook_note==0u)tone=(chord+4u)%7u;
 }else if(character==5u){
   zone=hook_note==1u?VOCAL_EH_ZONE:VOCAL_EE_ZONE;
   attack=hook_note==0u?7u:hook_note==1u?8u:2u;
   if(pos==sustain){zone=2u;transition=VOCAL_EE_ZONE+1u;}
 }
 /* A sustained OH/AH or MM/OO transition belongs to the phrase cadence. */
 if(!lexical&&!attack&&pos==sustain&&section==2u&&!transition){
   zone=1u;
   transition=1u; /* AH */
 }
 if(!lexical&&part==2u&&e->vocal_presence==4u&&((bar+e->scene)&7u)==0u&&
    character==0u&&pos==sustain){
   zone=VOCAL_DAY_ZONE;
   transition=0u;
 }
 if(e->vocal_articulation)zone=vocal_articulation(e->vocal_articulation,hook_note,&attack,&transition);
 if(!e->vocal_articulation&&part==2u&&(e->vocal_reprise||section==4u)&&e->vocal_hook_vowel[hook_note]){
   zone=e->vocal_hook_vowel[hook_note]-1u;
   attack=e->vocal_hook_attack[hook_note];
   transition=e->vocal_hook_transition[hook_note];
 }
 if(!e->vocal_articulation){
   if(zone<3u)zone+=3u;
   if(transition&&transition<=3u)transition+=3u;
 }
 /* Refrains use two anchored words. Syncopated intervening notes use a
    short vowel and do not replace the phrase's saved melodic anchors. */
 if(extra&&!e->vocal_articulation){zone=4u;attack=0;transition=0;}
 if(!e->vocal_articulation&&!extra&&part==2u&&e->vocal_presence>=3u&&
     (section==2u||lexical||section==4u)){
   unsigned sung=sung_refrain(e,section,hook_note);
   if(sung){zone=sung;attack=0;transition=0;}
 }
 /* A recorded word can decorate a Dreaming cadence without overwriting
    the reusable vowel hook or introducing a new sample ID into saved banks. */
 if(!e->vocal_articulation&&part==2u&&e->scene==4u&&e->vocal_presence>=3u&&
    cadence&&mode!=SR_IDM&&!attack){
   zone=VOCAL_DREAMS_ZONE;
   transition=0;
 }
 unsigned word=zone==VOCAL_DAY_ZONE||zone==VOCAL_YOU_ZONE||zone>=VOCAL_LOVE_ZONE;
 sr_voice *lead=recorded_note(e,sr_tonic(e)+scale[tone],zone,velocity,held);
 if(lead){
   lead->vocal_consonant=attack;
   lead->vocal_vowel_to=transition;
   lead->vocal_morph_rate=1.f/(float)lead->remain;
   if(part==2u&&!extra&&zone<VOCAL_DREAMS_ZONE&&(section==2u||section==4u)&&!e->vocal_hook_vowel[hook_note]){
     e->vocal_hook_vowel[hook_note]=zone+1u;
     e->vocal_hook_attack[hook_note]=attack;
     e->vocal_hook_transition[hook_note]=transition;
   }
 }
 if(lead&&!word&&!attack&&zone<6u&&pos==opening&&e->vocal_presence>=3u&&((bar+e->scene)&1u)==0u){
   /* Move through two nearby scale tones on the same vowel and land
      back on the chord tone before the response. Keep the sample phase. */
   int direction=tone>=2u?-1:1;
   int first=scale[(int)tone+direction]-scale[tone];
   int second=scale[(int)tone+2*direction]-scale[tone];
   float reference=sr_note_frequency(e,60);
   lead->vocal_turn_first=sr_note_frequency(e,60+first)/reference;
   lead->vocal_turn_second=sr_note_frequency(e,60+second)/reference;
   lead->vocal_turn_rate=1.f/(float)lead->remain;
 }
 int emitted=lead!=0;
 if(emitted){
   e->vocal_entry_pending=0;
   if(!extra&&!e->vocal_hook[hook_note])e->vocal_hook[hook_note]=tone+1u;
   if(pos==sustain)e->vocal_reprise=0;
   if(pos!=sustain)e->vocal_call=tone+1u;
 }
 if(!word&&pos==sustain&&section==2u&&e->vocal_presence>=2u&&mode!=SR_EXPERIMENTAL){
   unsigned harmony=tone==chord?(chord+2u)%7u:chord;
   emitted|=!!recorded_note(e,sr_tonic(e)+scale[harmony],zone<3u?zone+3u:zone<6u?zone-3u:zone,
                           velocity-21u,held);
 }
 return emitted;
}

static void pluck_answer(sr_engine *e,unsigned step,unsigned chord,int lead_degree)
{
  unsigned pos=step&15u;
  unsigned bar=score_bar(e,step);
  if(lead_degree>=0||pos==0u||pos==15u)return;
  const uint8_t *profile=intention_score[e->scene];
  unsigned lag=e->music_mode==SR_CINEMATIC?3u:2u;
  unsigned offset=1u+((profile[1]+bar+song_draw(e->song_seed,356))&1u);
  if(pos<lag||((pos+offset)&1u))return;
  if(pos<8u&&song_section(e)==3u)return;
  int call=motif_degree(e,step-lag);
  if(call<0&&pos>=lag+1u)call=motif_degree(e,step-lag-1u);
  if(call<0||motif_degree(e,step-1u)>=0)return;
  const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
  unsigned patch=e->tracks[SR_GUITAR].patch;
  const float body=pluck_decay[patch].body;
  float duration=body>0.f?.25f+body:.82f;
  int answer=fit_degree(call+(pos<8u?1:-1),chord);
  if(answer<0)answer=(int)chord;
  int root=sr_tonic(e)+12;
  ornament(e,1,root+scale[answer],pos<8u?62:55,duration,step,e->variation>0.f&&!e->freeze);
  unsigned roll=string_roll_gap(patch,e->music_mode);
  if(roll&&pos>=10u&&pos<=13u&&song_section(e)==2u&&
     e->chord_mode==0&&!e->frequency_lock&&motif_degree(e,step+1u)<0){
    unsigned delay=(roll*SR_RATE+500u)/1000u;
    if(delay<e->tick_length)
      schedule(e,SR_GUITAR,root+12+scale[chord],46,duration*.70f,delay);
  }
}

static float melody_duration(const sr_engine *e,unsigned step)
{
  const sr_patch *patch=&sr_patches[e->tracks[SR_LEAD].patch];
  unsigned gap=1;
  while(gap<5u&&(step&15u)+gap<16u&&motif_degree(e,step+gap)<0)gap++;
  unsigned instrument=e->tracks[SR_LEAD].patch;
  float duration=(gap-.35f)*.25f;
  if(instrument>=48u&&instrument<=55u){
    unsigned position=step&15u;
    if(position>=8u&&gap>=3u)duration+=instrument<=49u?.40f:.26f;
    if(e->music_mode==SR_CINEMATIC)duration+=.40f;
    if(instrument==50u||instrument==55u)duration*=.84f;
  }else{
    if(patch->attack>.08f)duration+=.30f;
    if(e->music_mode==SR_CINEMATIC)duration+=.55f;
  }
  if((step&15u)>=13u)duration+=.5f;
  return duration;
}

/* Lift the dampers together just before the next harmony. Key gates
 * remain short on muted sections and on non-piano patches. The generated
 * gate is also the duration seen by the score preview and MIDI scheduler. */
static float piano_gate(const sr_engine *e,unsigned pos,float finger_gate)
{
 unsigned patch=e->tracks[SR_PIANO].patch;
 if(patch<6u||patch>8u||e->frequency_lock||song_section(e)==3u)return finger_gate;
 unsigned lift=e->current_energy>.7f?14u:13u;
 if(pos>=lift)return finger_gate;
 float pedal=(lift-pos)*.25f;
 return fmaxf(finger_gate,pedal);
}

static unsigned lead_pitch(const sr_engine *e,unsigned degree)
{
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 unsigned patch=e->tracks[SR_LEAD].patch;
 int note=sr_tonic(e)+24+scale[degree];
 if((patch==49u||patch==52u)&&note>65)note-=12;
 if(patch==54u&&note>76)note-=12;
 if(patch==55u&&note<72)note+=12;
 return (unsigned)note;
}

static void piano_phrase(sr_engine *e,unsigned step,unsigned chord,int lead,int expression)
{
 static const uint8_t rhythm[SR_MUSIC_MODES][2][4]={
  {{2,7,11,14},{3,6,10,13}},
  {{2,6,10,14},{3,7,11,14}},
  {{1,5,9,13},{2,6,10,14}},
  {{3,6,11,14},{2,7,10,13}},
  {{1,6,9,15},{2,5,11,14}}
 };
 const unsigned pos=step&15u,bar=step/16u,mode=e->music_mode;
 const unsigned section=song_section(e);
 const unsigned answer=(score_bar(e,step)+(song_draw(e->song_seed,390)&1u))&1u;
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 const int root=sr_tonic(e)+12;
 if(mode==SR_NEOCLASSICAL&&e->chord_mode!=3&&!e->frequency_lock&&pos==0u)
  sr_note_on(e,SR_PIANO,root+scale[chord],54+expression,piano_gate(e,pos,2.25f));

 const uint8_t *phrase=rhythm[mode][answer];
 unsigned count=mode==SR_NEOCLASSICAL||e->current_energy>=.78f?4u:3u;
 if(e->current_energy<.32f)count=e->current_energy<.16f?1u:2u;
 for(unsigned i=0;i<count;i++){
  if(pos!=phrase[i])continue;
  if(mode!=SR_NEOCLASSICAL&&(lead>=0||(section==3u&&i==1u)))return;
  int call=-1;
  for(unsigned back=1u;back<=4u&&back<=pos&&call<0;back++)
   call=motif_degree(e,step-back);
  unsigned next=phrase_chord(e,bar+1u);
  unsigned reference=call>=0?(unsigned)call:(chord+(answer?4u:2u))%7u;
  unsigned target=i+1u==count?next:chord;
  unsigned degree=support_degree(e,chord,target,reference,i==1u&&section!=3u);
  /* Let the middle of an exposed piano phrase connect the two chord
   * tones with a short scale step, then land on a consonant response. */
  if(mode==SR_NEOCLASSICAL&&i==1u&&call<0&&section!=3u)degree=(degree+6u)%7u;
  int velocity=(lead>=0?48:57)+expression+(i==0u?4:0);
  float beats=i==0u?1.15f:i+1u==count?.65f:.38f;
  if(mode==SR_CINEMATIC)beats+=.45f;
  sr_note_on(e,SR_PIANO,root+(mode==SR_NEOCLASSICAL?12:0)+scale[degree],
      velocity,piano_gate(e,pos,beats));
  return;
 }
}

static void mode_phrase(sr_engine *e,unsigned s,uint32_t seed)
{
  const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
  unsigned chord=composition_chord(e,s),pos=s&15u;
  int tonic=sr_tonic(e),degree=motif_degree(e,s);
  unsigned draw=song_draw(seed,score_bar(e,s)*16u+pos);
  unsigned dynamic=6u+((draw>>4)&3u);
  int expression=phrase_expression(s)+(int)(8.f*(e->current_energy-.5f));
  if(e->music_mode==SR_IDM)expression+=(pos&3u)==2u?6:(pos&3u)==0u?-3:0;
  drum_trigger(e,s,e->music_mode==SR_CINEMATIC?2:1);
  bass_groove(e,s);
  if(e->music_mode==SR_IDM&&section_harmony(form_index(e,0))>=1u&&
     e->current_energy>.55f&&sr_section_remaining(e)==1u&&pos==14u&&
     !e->freeze&&e->drums_enabled&&e->drum_density>=5u&&e->fill_character!=3u){
    schedule(e,SR_DRUMS,2,64,0.f,e->tick_length/3u);
    schedule(e,SR_DRUMS,2,78,0.f,e->tick_length*2u/3u);
  }
  if(pos==0u)accompaniment(e,s/16u,chord);
  unsigned vocal_section=featured_vocal_section(e,s);
  /* Instrumental choir answers in the space between phrases. Its FM
   * voices retain the track fader, independently of the sampled singer. */
  if(pos==8u&&(e->music_mode==SR_CINEMATIC||(score_bar(e,s)&1u))){
    unsigned inner=support_degree(e,chord,phrase_chord(e,s/16u+1u),
                                  degree>=0?(unsigned)degree:(chord+2u)%7u,0);
    int note=tonic+24+scale[inner];
    int previous=e->tracks[SR_CHOIR].last_note;
    if(previous){
      while(note-previous>7)note-=12;
      while(previous-note>7)note+=12;
    }
    while(note<55)note+=12;
    sr_note_on(e,SR_CHOIR,note,vocal_section?33:49,e->music_mode==SR_CINEMATIC?2.7f:e->music_mode==SR_IDM?.44f:1.8f);
  }
  if(vocal_section){
    recorded_vocal_phrase(e,s,chord);
    /* The voice yields a beat before the cadence. The band answers there
       instead of queuing ornamental lead notes over the held vowels. */
    if(pos==9u&&e->vocal_call){
      unsigned response=support_degree(e,chord,phrase_chord(e,s/16u+1u),e->vocal_call-1u,0);
      ornament(e,1,tonic+12+scale[response],60+expression,.65f,s,0);
      e->vocal_call=0;
    }else if(pos==9u)pluck_answer(e,s,chord,-1);
    if(pos==10u&&!learned_active(e)){
      if(degree>=0)sr_note_on(e,SR_LEAD,lead_pitch(e,degree),56+expression,.21f);
      else if((score_bar(e,s)&3u)==2u&&e->vocal_hook[0]&&
              (song_section(e)==1u||song_section(e)==4u)){
        unsigned reply=support_degree(e,chord,phrase_chord(e,s/16u+1u),e->vocal_hook[0]-1u,1);
        sr_note_on(e,SR_LEAD,lead_pitch(e,reply),54+expression,.31f);
      }
    }
    if(pos==0u||pos==8u)piano_phrase(e,s,chord,-1,expression-6);
  }else{
    if(e->current_energy>=.25f||(score_bar(e,s)&1u)==0u)pluck_answer(e,s,chord,degree);
    if(!learned_active(e)&&degree>=0)
      ornament(e,2,lead_pitch(e,degree),68-dynamic+expression,melody_duration(e,s),s,e->variation>0.f&&!e->freeze);
    piano_phrase(e,s,chord,degree,expression);
  }
  if((pos==12u&&e->current_energy>.33f&&(song_section(e)==2u||song_section(e)==4u))||
     (pos==15u&&e->current_energy>.83f&&song_section(e)==2u&&degree<0)){
    unsigned next=phrase_chord(e,s/16u+1u);
    unsigned inner=support_degree(e,chord,next,(chord+4u)%7u,0);
    sr_note_on(e,SR_HARMONY,tonic+24+scale[inner],44+expression,.85f);
  }
}
static void musical_detail(sr_engine *e,unsigned step)
{
  if(e->freeze||e->variation<=0.f||e->current_energy<.2f)return;
  unsigned id=form_index(e,0);
  unsigned preparing=sr_section_remaining(e)==1u;
  int tonic=sr_tonic(e);const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];

  /* A sparse answer in the final bar, coordinated with prepared arrival. */
  if(step+3u==e->loop_length&&(e->form_loop+id)%3u==1u){
    unsigned chord=phrase_chord(e,step/16u)%7u;
    sr_note_on(e,SR_HARMONY,tonic+36+scale[(chord+4u)%7u],42,1.3f);
    schedule(e,SR_PIANO,tonic+36+scale[(chord+2u)%7u],37,1.f,e->tick_length/2u);
  }
  /* Suspended response resolves into the next crest, with low velocity
   * and current sounds/tails, never a reference-tone or key change. */
  if(preparing&&(step&15u)==12u&&sr_sections[form_index(e,1)].family==2u){
    unsigned chord=phrase_chord(e,step/16u)%7u;
    sr_note_on(e,SR_HARMONY,tonic+24+scale[(chord+2u)%7u],42,2.5f);
  }
}
static void trigger(sr_engine *e){
    /* Continuous expression changes future notes on the musical grid. */
  e->current_energy+=clampf(e->energy-e->current_energy,-.08f,.08f);
  /* The separate mode control remains quantized. Intention selection is
   * committed by the app at the audio callback boundary. */
  unsigned song_changed=0;
  if((e->sequence_step&3u)==0u){
    if(e->pending_mode){
      e->profile_applying=1;
      e->music_mode=e->pending_mode-1u;
      e->pending_mode=0;
      sr_set_scene(e,e->scene);song_changed=1;
      e->profile_applying=0;
    }
  }
  unsigned eligible=sr_vocal_status(e)==SR_VOCAL_READY;
  if(eligible&&!e->vocal_auto_ready)e->vocal_entry_pending=1;
  e->vocal_auto_ready=eligible;
  response_tick(e);
  if(!e->loop_pos){
    if(e->learn_pending&&!e->freeze){e->learned=e->capture;e->capture.count=0;e->learn_pending=0;e->learn_origin=e->sequence_step;}
    if(e->rhythm_pending){sr_set_rhythm_style(e,e->rhythm_pending-1u);e->rhythm_pending=0;}
    if(e->pending_length){e->loop_length=e->pending_length;e->pending_length=0;e->play_queued=0;}
    if(e->pending_key){
      memset(e->scheduled,0,sizeof e->scheduled);
      e->tonic_override=36+e->pending_key-1;
      e->pending_key=e->pending_physical_key=0;
      for(unsigned i=0;i<SR_VOICES;i++){
        sr_voice *v=&e->voices[i];
        release_voice(e,v);v->remain=0;v->release_coef=.002f;
      }
    }
  }
  if(!song_changed&&!(e->loop_pos&15u)&&e->sequence_step&&!e->freeze){arrange_advance(e,!e->loop_pos);plan_roles(e);}
  uint32_t absolute=e->sequence_step++;
  uint32_t s=e->loop_pos;
  unsigned idx=e->loop_pos++;
  if(e->loop_pos>=e->loop_length)e->loop_pos=0;
  s=idx;
  /* The absolute counter survives phrase wrap; idx is only the loop position. */
  unsigned pace=e->evolve_pace?1u<<e->evolve_pace:(e->loop_length==16?2u:1u);
  if(!song_changed&&absolute&&idx==0&&!e->freeze&&(e->evolve_next||
      ((absolute/e->loop_length)%pace==0&&e->variation>0.f&&(song_draw(e->song_seed,absolute+913u)>>8)*(1.f/16777216.f)<e->variation))){
    e->evolve_next=0;
    e->phrase++;
    remember_motif(e);
    if(e->phrase%4u==0){
      if(e->motif_count)memcpy(e->melody,e->motif_memory[(e->phrase/4u)%e->motif_count],16);
      else memcpy(e->melody,e->motif_home,16);
    }else develop_motif(e);
    plan_roles(e);
  }
  int k=motif_degree(e,idx);
  if((s&15u)==0)chord_phrase(e,s);
  transition_fill(e,s,0);
  int learned=learned_index(e,s);
  if(learned>=0&&k>=0)sr_note_on(e,SR_LEAD,lead_pitch(e,k),74,e->learned.length[learned]*.25f);
  musical_detail(e,s);
  mode_phrase(e,s,e->song_seed);
}
/* Pure event-level score expansion. The future uses the SAME trigger and
 * offbeat grammar as playback, with no DSP samples, audio RNG or shared state
 * advanced. Storage lives in the pool; never place an engine copy on stack. */
static void score_advance(sr_engine *e,uint32_t frame){
 uint32_t elapsed=frame-e->frame_count;
 if(e->vocals_enabled)for(unsigned i=0;i<SR_VOICES;i++){
   sr_voice *v=&e->voices[i];
   if(!v->active||!v->sample_zone)continue;
   if(v->gate&&elapsed<v->remain){v->remain-=elapsed;continue;}
   if(v->gate){
     v->gate=0;
     v->remain+=v->sample_zone>=VOCAL_DAY_ZONE+1u?SR_RATE/2u:SR_RATE/6u;
   }
   if(elapsed>=v->remain)v->active=0;
   else v->remain-=elapsed;
 }
 e->frame_count=frame;
}
static unsigned preview_collect(sr_engine *e,sr_event *out,unsigned cap,unsigned count,unsigned before){
 for(unsigned i=before;i!=e->timeline_write;i=(i+1u)%96u){
  if(count<cap)out[count++]=e->timeline[i];
 }
 return count;
}
void sr_score_snapshot(sr_engine *scratch,const sr_engine *engine){
 /* The score never renders audio, so leave the FX buffers untouched. */
 memcpy(scratch,engine,offsetof(sr_engine,reverb));
 memcpy(&scratch->loop_length,&engine->loop_length,sizeof(*engine)-offsetof(sr_engine,loop_length));
}
unsigned sr_score_preview(sr_engine *e,sr_event *out,unsigned cap,unsigned ticks){
 if(!e->running||e->external_sync)return 0;
 e->timeline_enabled=1;e->midi_enabled=0;
 unsigned count=0;
 uint32_t next=e->frame_count+(e->tick_phase<e->tick_length?e->tick_length-e->tick_phase:0u);
 for(unsigned tick=0;tick<ticks;tick++){
  /* Drain real offbeat events before the next score tick. */
  for(;;){
   uint32_t due=next;int kind=-1;unsigned index=0;
   for(unsigned i=0;i<12;i++)if(e->scheduled[i].active&&(int32_t)(e->scheduled[i].due-due)<0){due=e->scheduled[i].due;kind=0;index=i;}
   for(unsigned i=0;i<4;i++)if(e->ornaments[i].phase){
    uint32_t f=e->frame_count-e->tick_phase+e->ornaments[i].phase;
    if((int32_t)(f-due)<0){due=f;kind=1;index=i;}
   }
   if(kind<0)break;
   if((int32_t)(due-e->frame_count)<0)due=e->frame_count;
   unsigned before=e->timeline_write;
   uint32_t gap=due-e->frame_count;score_advance(e,due);e->tick_phase+=gap;
   if(kind==1){
    e->ornaments[index].phase=0;
    sr_note_on(e,ornament_track[index],e->ornaments[index].midi,e->ornaments[index].velocity,e->ornaments[index].beats);
   }else{
    e->scheduled[index].active=0;
    unsigned role=e->scheduled[index].role;
    if(role==SR_DRUMS){
     if(e->drums_enabled&&!e->freeze&&!e->tracks[role].mute){unsigned j=timeline_slot(e);
      e->timeline[j]=(sr_event){due,e->tick_length/2u,SR_DRUMS,sr_drum_notes[e->scheduled[index].note],e->scheduled[index].velocity,0};}
    }else sr_note_on(e,role,e->scheduled[index].note,e->scheduled[index].velocity,e->scheduled[index].beats);
   }
   count=preview_collect(e,out,cap,count,before);
  }
  score_advance(e,next);e->tick_phase=0;memset(e->ornaments,0,sizeof e->ornaments);
  unsigned before=e->timeline_write;trigger(e);
  count=preview_collect(e,out,cap,count,before);
  next=e->frame_count+e->tick_length;
 }
 return count;
}
static inline __attribute__((always_inline)) float nature_noise(sr_engine *e)
{
  e->nature_rng=e->nature_rng*1664525u+1013904223u;
  return (float)(e->nature_rng>>8)*(2.f/16777216.f)-1.f;
}

static inline __attribute__((always_inline)) float nature_call(sr_engine *e,float noise,unsigned type)
{
  static const struct {
    uint16_t pitch, length;
    int16_t sweep;
    uint8_t notes, noise_percent;
  } calls[]={
    {1840,2800,950,3,9},{3200,1600,-1050,2,6},{2250,5100,-640,1,8},{2800,2000,620,4,8},
    {350,3100,-160,3,28},{2950,880,100,5,22},{710,620,440,1,78},{190,9100,-55,1,92},
    /* Droplet, gurgle, leaf gust, cricket, night frog, rising birdsong,
     * distant rumble, long surf wash, falling rain burst, dry canopy ticks. */
    {1080,440,-910,1,57},{460,1630,-310,3,63},{260,6200,85,1,97},
    {3520,720,-270,4,42},{290,2900,-115,2,47},{1620,4400,1140,2,11},
    {73,21000,-30,1,87},{115,15800,25,1,99},{540,6500,220,1,95},
    {1940,1060,-650,3,79}
  };
  static const uint8_t calls_by_bed[SR_BEDS][8]={
    {0},
    {10,10,17,7,6,13,10,17}, /* Wind */
    {8,9,9,6,2,8,16,4},     /* Water */
    {16,8,16,6,8,17,16,7},  /* Rain */
    {0,13,2,3,10,17,11,4},  /* Forest */
    {15,15,7,16,9,15,13,7}, /* Ocean */
    {0,1,2,3,13,1,13,2},   /* Birds */
    {17,10,17,6,11,0,5,10},/* Leaves */
    {12,11,4,12,5,11,9,1}, /* Florida Night */
    {0,11,13,4,5,10,3,12}, /* Twilight Meadow */
    {8,9,8,2,6,13,9,0},    /* Crystal Creek */
    {16,16,8,7,14,10,6,16},/* Mountain Rain */
    {9,8,0,2,13,9,17,6},   /* Woodland Brook */
    {14,7,15,16,14,10,8,16}/* Distant Storm */
  };
  if(!e->bird_left){
    if(e->bird_wait){e->bird_wait--;return 0;}
    if(!e->bird_note){
      e->bird_phrase=calls_by_bed[type][(e->nature_rng>>20)&7u];
      e->bird_pan=noise*.52f;
    }
    unsigned call=e->bird_phrase;
    e->bird_left=calls[call].length/2u;
    e->bird_env=1.f/e->bird_left;
    e->bird_pitch=calls[call].pitch*(1.f+.07f*noise)+e->bird_note*(call<4u?147.f:21.f);
    if(++e->bird_note==calls[call].notes){
      e->bird_note=0;
      e->bird_wait=SR_RATE/2u*(5u-e->nature_activity)+((e->nature_rng>>12)&0x1ffffu);
    }else e->bird_wait=(call==5u||call==11u?420u:1200u)+((e->nature_rng>>19)&1023u);
  }
  unsigned call=e->bird_phrase;
  float progress=e->bird_left*e->bird_env;
  float env=progress*(1.f-progress)*4.f;
  e->bird_left--;
  float hz=fmaxf(28.f,e->bird_pitch+calls[call].sweep*progress*progress);
  float fundamental=advance(&e->bird_phase,2.f*hz);
  float texture=calls[call].noise_percent*.01f;
  return ((fundamental*.077f+sine(e->bird_phase*2.f)*.014f)*(1.f-texture)
          +noise*.11f*texture)*env*env;
}

static inline __attribute__((always_inline)) float bed_color(unsigned type,float wind,float rain,float air,float swell,float bird,float *pan)
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
   case 12:return (rain*.65f+air*.063f)*(.75f+swell*.32f)+wind*.27f;
   case 13:return (wind*1.15f+rain*.53f)*(.40f+.60f*swell*swell)+air*.028f;
   default:return 0.f;
  }
}

static inline __attribute__((always_inline)) float bed(sr_engine *e,float *side)
{
  if(e->nature_blend>=1.f&&e->bed_type!=e->nature_active){
    e->nature_from=e->nature_active;
    e->nature_active=e->bed_type;
    e->nature_blend=0;
  }
  if(!e->nature_active&&e->nature_blend>=1.f){
    e->bird_left=e->bird_note=0;
    return 0;
  }
  float n=nature_noise(e);
  e->wind_lp+=(n-e->wind_lp)*.0016f;
  e->rain_lp+=(n-e->rain_lp)*.1164f;
  float wind=e->wind_lp*3.5f;
  float rain=(e->rain_lp-e->wind_lp)*.26f;
  float air=n-e->rain_lp;
  float swell=.55f+.45f*advance(&e->bed_phase,.24f);
  unsigned character=e->nature_active?e->nature_active:e->nature_from;
  float event=nature_call(e,n,character)*(1.f-.15f*e->nature_distance);
  float pan=e->bird_pan;
  float stereo=e->nature_active!=0;
  float value=bed_color(e->nature_active,wind,rain,air,swell,event,&pan);
  if(e->nature_blend<1.f){
    float previous_pan=e->bird_pan;
    float previous_stereo=e->nature_from!=0;
    stereo=previous_stereo+(stereo-previous_stereo)*e->nature_blend;
    float previous=bed_color(e->nature_from,wind,rain,air,swell,event,&previous_pan);
    value=previous+(value-previous)*e->nature_blend;
    e->nature_blend+=2.f/(.5f*SR_RATE);
  }
  *side=(event*pan*.34f+air*(swell-.55f)*.018f)*stereo;
  return value+event*.24f*stereo;
}
static inline __attribute__((always_inline)) float fx(sr_engine *e,unsigned mask,float dry,float repeats,float shimmer,float *wide){
  float wet=0.f;
  if(mask&1u)for(int i=0;i<4;i++){
    uint16_t p=e->rev_pos[i];float delayed=e->rev_history>=rv_len[i]?(float)e->reverb[i][p]*(1.f/32768.f):0;
    float next=clampf((dry+shimmer)*.34f+delayed*e->fx_decay_now,-.97f,.97f);
    e->reverb[i][p]=(int16_t)(next*32767.f);
    e->rev_pos[i]=(uint16_t)(p+1u==rv_len[i]?0:p+1u);wet+=delayed*.25f;
  }
  if((mask&1u)&&e->rev_history<1789)e->rev_history++;
  float d=0;
  if(mask&2u){
  uint16_t p=e->delay_pos;
  unsigned period=e->tick_length;
  if(period>16384u)period=16384u;
  unsigned read=(p+16384u-period)&16383u;
  d=e->delay_history>=period?(float)e->delay[read]*(1.f/32768.f):0;
  if(e->delay_history<16384)e->delay_history++;
  e->fx_delay_low+=(d-e->fx_delay_low)*.12f;
  float next=clampf(repeats*.63f+(d+(e->fx_delay_low-d)*e->fx_damping_now)*e->fx_feedback_now,-.97f,.97f);
  e->delay[p]=(int16_t)(next*32767.f);e->delay_pos=(uint16_t)((p+1u)&16383u);
  }
  *wide=(wet*.56f-d*.12f)*e->reverb_wet*e->fx_width_now;
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
static inline __attribute__((always_inline)) unsigned slicer_lane(unsigned role)
{
    return role<=SR_BASS?role:role==SR_DRUMS?2u:3u;
}

static inline __attribute__((always_inline)) unsigned slicer_pattern(const sr_engine *e)
{
    static const uint16_t patterns[7][3]={
        {0x7777,0x7f77,0x777f}, {0x3333,0x5733,0x3375},
        {0x5b35,0x3753,0x355b}, {0x295b,0x571b,0x3575},
        {0x9249,0x4925,0x2493}, {0x5fff,0x35ff,0x15ff},
        {0x7fff,0x77ff,0x7ff7}
    };
    static const uint8_t auto_style[SR_MUSIC_MODES]={2,7,1,4,3};
    unsigned section=song_section(e);
    unsigned style=e->slicer_style;
    if(!style){
      style=auto_style[e->music_mode];
      if(!section||section==3u)style=e->music_mode==SR_CINEMATIC?7u:1u;
      else if(e->current_energy>.62f&&sr_section_remaining(e)==1u)style=6u;
      else if(section==4u&&e->drum_density>=4u)style=3u;
    }
    unsigned variation=(e->scene+section+e->form_loop+((e->sequence_step?(e->sequence_step-1u):0u)>>4u))%3u;
    unsigned pattern=patterns[style-1u][variation]|1u;
    /* Keep the current kick and bass anchor intact, including edited kicks. */
    if(e->drum_steps[e->drum_playhead]&1u)pattern|=1u<<(e->drum_playhead&15u);
    return pattern;
}
unsigned sr_slicer_pattern(const sr_engine *e){return slicer_pattern(e);}


/* CPU0 is the sole writer of engine, MIDI, filter state and mix buses. The
 * secondary core sees only explicit oscillator inputs and its private output. */
typedef struct {
  sr_voice *voice;
  float cutoff, gain, step, left, right, send, echo_send;
  float fm_ratio, fm_depth, fm_envelope, body_decay, breath, vocal, morph, other_step, drive;
  sr_wave wave;
  uint8_t track;
} voice_config;
typedef struct {
  voice_config active[SR_VOICES];
  float left[SR_TRACKS],right[SR_TRACKS],send[SR_TRACKS],echo_send[SR_TRACKS],slicer_target[4];
  float bass_hz,target_bass;
  unsigned count,scope_track,smooth_slicer;
} voice_block;
typedef struct {
  float phase,mod_phase,increment,step,ratio,depth,index_envelope;
  float env,peak,attack,decay,vocal;
} fm_input;
enum { FM_JOB_VOICES=SR_VOICES/2+1, FM_JOB_FRAMES=64 };
typedef struct {
  fm_input inputs[FM_JOB_VOICES];
  float samples[FM_JOB_FRAMES][FM_JOB_VOICES];
  unsigned count,frames;
} fm_job;
static fm_job remote_fm,local_fm;
static const sr_fm_worker *fm_worker;

void sr_set_fm_worker(const sr_fm_worker *worker){fm_worker=worker;}

const char *const sr_vocal_patch_names[SR_VOCAL_PATCHES]={
  "Angelic Halo","Velvet Soul","Crystal Dream","Celestial Cathedral","Aurora Pop",
  "Starlight Echo","Moonlit Whisper","Neon IDM","Golden Harmony","Infinite Bloom"
};

/* Relative to the Vocal Director: chorus, room, echo, space and direct tone.
 * Bias only automatic controls so every manual knob remains authoritative. */
static const struct {
  int8_t chorus,room,echo,decay,width,shimmer,presence,damping,depth,feedback;
} vocal_chars[SR_VOCAL_PATCHES]={
  { 13, 16, -8, 12,  8, 18,  4, -5,  4, -5},
  {-14,-18,-17, -8,-22, -8, 10, 16, -8, -8},
  {  6,  7,  7,  6, 21, 32, 12,-10,  8,  1},
  {  4, 26,  2, 29, 12, 14, -5,  4, -2, -3},
  { 10,-18, -3,-16,  4, -6, 20, -3,  7, -5},
  { -8, -6, 28, -4, 23, -3,  8,  2, -5, 17},
  {-18,-23,-16,-13,-24,-10, -4, 11, -9,-11},
  {  6,-12, 15,-10, 13,  9, 10, -6, 22,  8},
  { 26, -2, -9,  1, 20,  6,  1,  7, 23, -5},
  { 12, 21, 12, 24, 18, 24, -2,  1, 13,  3}
};

static void vocal_production(sr_engine *e,size_t frames)
{
  static const uint8_t profiles[SR_MUSIC_MODES][3]={
    {28,59,32},{34,78,42},{20,57,25},{44,48,54},{32,42,48}
  };
  unsigned section=song_section(e);
  float variation=(float)(song_draw(e->song_seed,927u)&7u);
  float warmth=(float)intention_score[e->scene][0]/5.f;
  float breath=(float)intention_score[e->scene][1]/7.f;
  float color=(float)intention_score[e->scene][2]/5.f;
  float energy=clampf(e->current_energy,0.f,1.f);
  float bloom=section==2u?10.f:section==3u?-6.f:section==0u?-3.f:0.f;
  unsigned flow=vocal_flow_for(e,section);
  float space=flow==SR_VOCAL_HALF?1.f:0.f;
  float rapid=flow==SR_VOCAL_DOUBLE?1.f:0.f;
  float cinematic=e->music_mode==SR_CINEMATIC?1.f:0.f;
  float organic=e->music_mode==SR_NEOCLASSICAL?1.f:0.f;
  float rhythmic=e->music_mode==SR_IDM?1.f:e->music_mode==SR_EXPERIMENTAL?.65f:0.f;
  const uint8_t *profile=profiles[e->music_mode];
  static const uint8_t auto_patches[SR_MUSIC_MODES][4]={
    {0,1,4,8},{3,0,8,9},{1,6,0,8},{2,5,0,9},{7,5,4,2}
  };
  unsigned character=e->vocal_patch?e->vocal_patch-1u:
      auto_patches[e->music_mode][(song_draw(e->song_seed,928u)+
      (unsigned)e->scene+intention_score[e->scene][2]+sr_scenes[e->scene].scale)&3u];
  float chorus=e->vocal_fx_manual&1u?e->vocal_chorus:
      clampf(profile[0]+vocal_chars[character].chorus+variation+warmth*5.f+energy*4.f+bloom*.2f,0.f,100.f);
  float room=e->vocal_fx_manual&2u?e->vocal_reverb:
      clampf(profile[1]+vocal_chars[character].room+bloom+variation+breath*4.f+5.f*space-7.f*rapid,0.f,100.f);
  float echo=e->vocal_fx_manual&4u?e->vocal_delay:
      clampf(profile[2]+vocal_chars[character].echo+bloom+color*6.f+energy*4.f-6.f*rapid,0.f,100.f);
  float blend=clampf((float)frames*.00045f,0.f,1.f);
  e->vocal_chorus_now+=(chorus-e->vocal_chorus_now)*blend;
  e->vocal_room_now+=(room-e->vocal_room_now)*blend;
  e->vocal_echo_now+=(echo-e->vocal_echo_now)*blend;
  /* All exposed automatic controls have musical targets. Delay TIME selects
     the existing tempo divisions; smoothing and per-control locks stay last. */
  float targets[VF_COUNT]={
    42.f+cinematic*22.f+breath*8.f+bloom,
    28.f+organic*12.f+warmth*12.f-energy*5.f+bloom*.2f,
    60.f+cinematic*14.f+color*8.f+bloom*.4f,
    32.f+rhythmic*36.f+breath*20.f+(section==2u?12.f:0.f),
    24.f+rhythmic*14.f+color*12.f+bloom*.4f,
    48.f+energy*12.f+color*10.f-organic*8.f+bloom*.2f,
    22.f+rhythmic*18.f+warmth*10.f+energy*6.f+bloom*.2f,
    rhythmic*(10.f+color*18.f+energy*6.f)+bloom*.15f,
    rhythmic*(8.f+warmth*16.f+energy*8.f)+bloom*.12f,
    35.f+warmth*12.f+rhythmic*8.f+bloom*.3f,
    22.f+cinematic*14.f+breath*10.f+energy*5.f+bloom*.4f,
    34.f+organic*18.f+breath*12.f-rhythmic*12.f-bloom*.3f,
    8.f+cinematic*22.f+warmth*12.f-organic*6.f+bloom*.5f,
    44.f+energy*12.f+color*8.f-organic*8.f+bloom*.2f,
    48.f+rhythmic*16.f+energy*10.f+warmth*5.f+bloom*.2f,
    45.f+organic*4.f+color*6.f+energy*4.f+bloom*.1f,
    5.f+cinematic*22.f+color*8.f+bloom*.5f,
    28.f+rhythmic*20.f+breath*12.f+bloom*.3f,
    12.f+rhythmic*16.f+color*8.f+energy*5.f+bloom*.2f,
    36.f+cinematic*14.f+warmth*12.f+energy*6.f+bloom*.3f
  };
  targets[VF_TIME]+=17.f*space-20.f*rapid;
  targets[VF_VIB_WAIT]+=9.f*space-7.f*rapid;
  targets[VF_VIB_DEPTH]+=5.f*space-6.f*rapid;
  targets[VF_GLIDE]+=6.f*space-8.f*rapid;
  targets[VF_DECAY]+=vocal_chars[character].decay+5.f*space-8.f*rapid;
  targets[VF_WIDTH]+=vocal_chars[character].width;
  targets[VF_SHIMMER]+=vocal_chars[character].shimmer;
  targets[VF_PRESENCE]+=vocal_chars[character].presence;
  targets[VF_DAMPING]+=vocal_chars[character].damping;
  targets[VF_CHORUS_DEPTH]+=vocal_chars[character].depth;
  targets[VF_FEEDBACK]+=vocal_chars[character].feedback;
  for(unsigned id=0;id<VF_COUNT;id++){
    float target=e->vocal_param_lock&(1u<<id)?e->vocal_param[id]:clampf(targets[id],0.f,100.f);
    e->vocal_param_now[id]+=(target-e->vocal_param_now[id])*blend;
  }
  e->vocal_send_now=1.f;
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void prepare_voices(sr_engine *e,voice_block *block,size_t frames){
  float *left=block->left,*right=block->right,*send=block->send;
  int locked=e->frequency_lock;
  int audition=!locked&&e->nature_audition>0;
  unsigned preview=e->preview_frames&&!e->running;
  unsigned source=audition?SR_ISOLATE_NATURE:preview?SR_ISOLATE_TRACK1+e->preview_role:e->isolate;
  e->nature_peak*=1.f-(float)frames*.0005f;
  for(unsigned b=0;b<SR_BUS_COUNT;b++)e->bus_meter[b]*=1.f-(float)frames*.0005f;
  int full=source==SR_ISOLATE_MIX;
  unsigned scope_track=e->scope_track;
  unsigned gate_mask=e->slicer_mask;
  unsigned slicer_beat=e->sequence_step?(e->sequence_step-1u)&15u:0u;
  unsigned pattern=gate_mask&&e->running&&!locked?slicer_pattern(e):0xffffu;
  float *slicer_target=block->slicer_target;
  for(unsigned lane=0;lane<4;lane++){
    unsigned bit=1u<<((slicer_beat+(lane==0?2u:0u))&15u);
    slicer_target[lane]=(gate_mask&(1u<<lane))&&!(pattern&bit)?
      (e->music_mode==SR_CINEMATIC?.60f:.18f):1.f;
  }
  unsigned smooth_slicer=gate_mask||e->slicer_gain[0]!=1.f||
    e->slicer_gain[1]!=1.f||e->slicer_gain[2]!=1.f||e->slicer_gain[3]!=1.f;
  float cutoff=e->filter_cutoff*(.22f/15.f);
  voice_config *active=block->active;
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
  float backing=0.f;
  for(int t=0;t<SR_TRACKS;t++){
    sr_track *track=&e->tracks[t];
    float level=(track->mute||(e->solo<SR_TRACKS&&e->solo!=t))?0:track->volume;
    /* Predict the nominal peak from the patch calibration and the fixed
     * role budget. Silence and transients never change the target. The
     * limiter handles the combined peak of overlaps, drums and wet buses. */
    static const float track_peak[SR_TRACKS]={.22f,.29f,.17f,.16f,.20f,.19f,.17f,.31f};
    float reference=t==SR_DRUMS?.27f:sr_patches[track->patch].gain;
    float target=e->auto_level&&!locked?
      clampf(track_peak[t]/fmaxf(reference,.04f),.62f,1.48f):1.f;
    e->gain_trim[t]+=(target-e->gain_trim[t])*clampf((float)frames*.00022f,0.f,1.f);
    if(!locked)level*=e->gain_trim[t];
    if(t==SR_DRUMS)level*=1.f+((int)e->drum_presence-50)*.006f;
    float send_level=track->send,echo_ratio=1.f;
    if(!(e->sound_locks&SR_SOUND_FX)&&!(e->track_send_manual&(1u<<t))&&
       !(e->sound_locks&(1u<<t))){
      static const float role_space[SR_TRACKS]={.88f,.36f,1.26f,.96f,1.05f,1.14f,1.25f,.86f};
      static const float mode_space[SR_MUSIC_MODES]={.91f,1.18f,.94f,1.09f,.88f};
      float room=mode_space[e->music_mode]*role_space[t];
      if(t!=SR_BASS&&t!=SR_DRUMS){
        room*=.88f+.21f*e->current_energy;
        room*=.95f+.025f*((e->song_seed>>(t*3u))&7u);
      }
      static const float role_echo[SR_TRACKS]={1.20f,.18f,.43f,.75f,.52f,1.41f,.46f,.65f};
      static const float mode_echo[SR_MUSIC_MODES]={.85f,.62f,.58f,1.08f,1.18f};
      echo_ratio=role_echo[t]*mode_echo[e->music_mode]*(.85f+.22f*e->current_energy);
      send_level*=room;
    }
    float send_blend=clampf((float)frames*.00045f,0.f,1.f);
    e->track_send_now[t]+=(send_level-e->track_send_now[t])*send_blend;
    send[t]=level*e->track_send_now[t];
    e->track_echo_now[t]+=(send_level*echo_ratio-e->track_echo_now[t])*send_blend;
    block->echo_send[t]=level*e->track_echo_now[t];
    if(!full&&source!=SR_ISOLATE_TRACK1+(unsigned)t)level=0;
    if(full&&level>0.f&&t!=SR_CHOIR)backing+=track->meter;
    left[t]=level*(.5f-.31f*track->pan);
    right[t]=level*(.5f+.31f*track->pan);
  }
  float human_phrase=0.f;
  if(!e->vocal_level_manual&&e->vocal_direct_peak>.014f){
    float target=full?clampf((.13f+backing*.31f)*e->vocal_level_smoothed/
        e->vocal_direct_peak,.76f,1.25f):1.f;
    e->vocal_auto_gain+=(target-e->vocal_auto_gain)*
        clampf((float)frames*(1.f/(SR_RATE*2.f)),0.f,1.f);
  }
  float vocal_target=e->vocal_level_manual||e->vocal_level!=100u?e->vocal_level*.01f:e->vocal_auto_gain;
  e->vocal_level_smoothed+=(vocal_target-e->vocal_level_smoothed)*
      clampf((float)frames*.004f,0.f,1.f);
  if(fabsf(vocal_target-e->vocal_level_smoothed)<.001f)
    e->vocal_level_smoothed=vocal_target;
  float vocal_level=!locked&&!e->tracks[SR_CHOIR].mute&&
      (e->solo>=SR_TRACKS||e->solo==SR_CHOIR)&&
      (full||source==SR_ISOLATE_TRACK1+SR_CHOIR)?e->vocal_level_smoothed:0.f;
  for(int j=0;j<SR_VOICES;j++){
    sr_voice *v=&e->voices[j];
    if(!v->active)continue;
    if(v->sample_zone&&(!isfinite(v->phase)||!isfinite(v->phase2)||
       !isfinite(v->env)||!isfinite(v->pitch)||v->sample_zone>VOCAL_LAST_ZONE+1u||
       (v->gate&&!v->remain))){
      release_voice(e,v);
      v->active=0;
      e->vocal_recovered++;
      e->vocal_entry_pending=e->vocals_enabled;
      continue;
    }
    if(v->sample_zone&&vocal_level>0.f)
      human_phrase=fmaxf(human_phrase,(v->gate?fmaxf(.24f,v->env):v->env)*vocal_level);
    if(!v->external&&e->frame_count-v->born>SR_RATE*30u&&v->release_coef<.003f){
      release_voice(e,v);v->remain=0;v->release_coef=.003f;e->watchdogs++;
    }
    if(v->track==SR_BASS&&v->gate){
      bass_envelope=fmaxf(bass_envelope,v->env);
      uint32_t age=e->frame_count-v->born;
      if(age<=bass_age){bass_age=age;bass_pitch=v->pitch;}
    }
    voice_config *config=&active[count++];
    sr_track *track=&e->tracks[v->track];
    const sr_patch *patch=&sr_patches[v->patch];
    config->voice=v;
    config->track=v->track;
    config->wave=patch->wave;

    float brightness=patch_tone(patch,track);
    config->body_decay=1.f;
    if(patch->wave==SR_PLUCK){
      const float body=pluck_decay[v->patch].body;
      const float transient=pluck_decay[v->patch].attack_color;
      if(body>0.f){
        float register_factor=clampf(v->freq*(1.f/440.f),.45f,2.4f);
        config->body_decay=1.f-(.7f+.3f*register_factor)/(SR_RATE*body);
        float age=(e->frame_count-v->born)*(1.f/SR_RATE);
        float color=transient/(transient+age*(.7f+.3f*register_factor));
        brightness*=.43f+.57f*color;
      }
    }
    config->breath=0.f;config->vocal=0.f;config->drive=0.f;
    if(v->patch>=48u&&v->patch<=55u){
      float age=(e->frame_count-v->born)*(1.f/SR_RATE);
      float onset=.045f/(.045f+age);
      config->breath=(v->patch>=53u?.08f:.039f)*(.22f+.78f*onset);
      brightness*=.72f+.28f*onset;
    }else if(v->patch>=56u&&v->patch<=59u){
      config->vocal=v->patch==57u?.14f:.22f;
    }
    unsigned op=v->patch;
    float fm_brightness=brightness*(v->freq>3000.f?.15f:1.f);
    config->fm_ratio=fm_operators[op].ratio+fm_operators[op].detune;
    config->fm_depth=fm_operators[op].depth*(.25f+.75f*fm_brightness);
    config->fm_envelope=fm_operators[op].envelope;
    if(op>=SR_LEGACY_PATCHES)
      config->fm_depth*=.85f+.25f*v->peak;
    config->cutoff=e->filter_cutoff?.008f+cutoff:.028f+brightness*.24f;
    config->gain=v->sample_zone?.72f:calibrated_patch_gain(v->patch);
    if(!v->sample_zone&&!locked&&!(e->sound_locks&SR_SOUND_FX)&&
       !(e->sound_locks&(1u<<v->track))){
      static const float warmth[SR_TRACKS]={.08f,.24f,.02f,.03f,.015f,.04f,.02f,0.f};
      config->drive=warmth[v->track]*(.75f+.35f*e->current_energy);
      if(!e->filter_cutoff&&(v->track==SR_DRONE||v->track==SR_HARMONY))
        config->cutoff*=.70f+.30f*e->current_energy;
    }
    if(v->sample_zone&&!e->filter_cutoff)config->cutoff=.16f+e->vocal_param_now[VF_PRESENCE]*.0044f;
    config->left=left[v->track];
    config->right=right[v->track];
    config->send=send[v->track]*.5f;
    config->echo_send=block->echo_send[v->track]*.5f;
    if(v->sample_zone){
      config->left=config->right=vocal_level*.5f;
      config->send=config->echo_send=0.f;
    }
    float modulation=0;
    if(v->track<=SR_BASS){
      sr_motion *m=&e->motion[v->track];
      float amount=m->glide_mode&&m->glide_ms?clampf((float)frames*(1000.f/SR_RATE)/m->glide_ms,0,1):1;
      v->pitch+=(v->freq-v->pitch)*amount;
      modulation=vibrato[v->track];
      if(!(e->motion_manual&(1u<<v->track))){
        unsigned age=e->frame_count-v->born;
        /* Delayed, shallow expression only in sustained phrase answers;
         * no per-sample policy work or modulation on reference-locked audio. */
        if(v->track==SR_BASS||age<SR_RATE/2u||age+v->remain<SR_RATE||
           patch->attack<.03f||((v->born/e->tick_length)&7u)<4u||e->frequency_lock)
          modulation=0;
      }
    }
    if(v->patch>=48u&&v->patch<=55u&&!e->frequency_lock&&
       (v->track>SR_BASS||!(e->motion_manual&(1u<<v->track)))){
      unsigned wind=v->patch-48u;
      float age=(e->frame_count-v->born)*(1.f/SR_RATE);
      float depth=clampf((age-wind_motion[wind].delay)*3.f,0.f,1.f);
      const float style[SR_MUSIC_MODES]={.72f,1.f,.83f,.60f,.68f};
      modulation=depth*wind_motion[wind].depth*style[e->music_mode]*.0005776f*
                 sine(age*wind_motion[wind].rate);
    }
    if(v->sample_zone){
      float age=(e->frame_count-v->born)*(1.f/SR_RATE);
      const float *fx=e->vocal_param_now;
      float depth=clampf((age-fx[VF_VIB_WAIT]*.0052f)*2.1f,0.f,1.f);
      float rate=3.2f+fx[VF_VIB_RATE]*.041f;
      v->pitch+=(v->freq-v->pitch)*clampf((float)frames/(1.f+fx[VF_GLIDE]*160.f),0.f,1.f);
      float turn=1.f;
      if(v->vocal_turn_rate>0.f){
        float progress=(float)(e->frame_count-v->born)*v->vocal_turn_rate;
        float lift=clampf((progress-.23f)*26.f,0.f,1.f);
        float move=clampf((progress-.44f)*26.f,0.f,1.f);
        float settle=clampf((progress-.66f)*22.f,0.f,1.f);
        turn=1.f+(1.f-settle)*(lift*(v->vocal_turn_first-1.f)+
                              move*(v->vocal_turn_second-v->vocal_turn_first));
      }
      config->vocal=(v->pitch/v->freq)*turn*(1.f+fx[VF_VIB_DEPTH]*.00029f*depth*sine(age*rate));
      config->step=v->phase2*config->vocal;
      config->morph=clampf(age*SR_RATE*v->vocal_morph_rate*2.f-.55f,0.f,1.f);
      if(v->vocal_vowel_to){
        unsigned target=v->vocal_vowel_to-1u;
        float root=target<VOCAL_DAY_ZONE?vocal_zones[target].hz:vocal_words[target-VOCAL_DAY_ZONE].hz;
        config->other_step=v->freq*config->vocal*(VOCAL_RATE/(float)SR_RATE)/root;
      }
    }else config->step=(v->pitch*(1+modulation)*(1.f/SR_RATE)-v->increment)*reciprocal;
  }
  /* Give the featured singer room in the midrange while leaving the
   * bass and drums in place. Follow the note envelope for smooth release. */
  float priority=clampf(human_phrase*3.f,0.f,1.f);
  float coefficient=(float)frames*(priority>e->vocal_priority?.0010f:.00022f);
  e->vocal_priority+=(priority-e->vocal_priority)*coefficient;
  if(e->vocal_priority>.001f){
    for(unsigned i=0;i<count;i++){
      voice_config *voice=&active[i];
      if(voice->voice->sample_zone)continue;
      float reduction=0.f;
      switch(voice->track){
        case SR_LEAD:case SR_GUITAR:reduction=.59f;break;
        case SR_DRONE:case SR_CHOIR:reduction=.43f;break;
        case SR_PIANO:case SR_HARMONY:reduction=.30f;break;
      }
      if(reduction==0.f)continue;
      float gain=1.f-reduction*e->vocal_priority;
      voice->left*=gain;
      voice->right*=gain;
      voice->send*=gain;
      voice->echo_send*=gain;
    }
  }else e->vocal_priority=0.f;
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
  block->count=count;
  block->bass_hz=bass_hz;
  block->target_bass=target_bass;
  block->scope_track=scope_track;
  block->smooth_slicer=smooth_slicer;
}

static inline __attribute__((always_inline)) float fm_sample(float phase,float mod_phase,
                                                         float index,float vocal)
{
    float mod=sine(mod_phase);
    float sample=sine(phase+index*mod+1.f);
    return vocal>0.f?sample*(1.f-vocal)+vocal*mod:sample;
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void fm_job_render(fm_job *job)
{
    for(unsigned a=0;a<job->count;a++){
        fm_input input=job->inputs[a];
        for(unsigned i=0;i<job->frames;i++){
            input.env+=(input.peak-input.env)*input.attack;
            input.increment+=input.step;
            input.phase+=input.increment;
            if(input.phase>=1.f)input.phase-=1.f;
            input.mod_phase+=input.increment*input.ratio;
            if(input.mod_phase>=1.f)input.mod_phase-=1.f;
            float index=input.depth*(1.f-input.index_envelope*(1.f-input.env));
            job->samples[i][a]=fm_sample(input.phase,input.mod_phase,index,input.vocal);
            input.peak*=input.decay;
        }
        job->inputs[a]=input;
    }
}

/* Called through a function pointer from XIP, with the same SRAM oscillator
 * kernel used on both cores. No worker job points into the audio stack. */
static void (*volatile synthesize_fm)(fm_job *)=fm_job_render;
static void remote_fm_render(uint32_t unused)
{
    (void)unused;
    synthesize_fm(&remote_fm);
}

/* Shared quarter-rate history for independent vocal chorus, room and echo.
 * The dry vocal path retains full sample rate. */
static inline __attribute__((always_inline)) float vocal_tap(const sr_engine *e,unsigned offset)
{
  return e->vocal_fx_history>=offset?
      e->vocal_fx_buffer[(e->vocal_fx_pos-offset)&8191u]*(1.f/32768.f):0.f;
}

static inline __attribute__((always_inline)) float vocal_fractional_tap(const sr_engine *e,float delay)
{
  float offset=clampf(delay,1.f,8190.f);
  unsigned first=(unsigned)offset;
  float value=vocal_tap(e,first);
  return value+(vocal_tap(e,first+1u)-value)*(offset-first);
}

static inline __attribute__((always_inline)) void vocal_effects(sr_engine *e,float dry,
    unsigned frame,float *left,float *right)
{
  float chorus=e->vocals_enabled?e->vocal_chorus_now:e->vocal_chorus;
  float room=e->vocals_enabled?e->vocal_room_now:e->vocal_reverb;
  float echo=e->vocals_enabled?e->vocal_echo_now:e->vocal_delay;
  if(!(chorus||room||echo)){e->vocal_fx_history=0;e->vocal_fx_tail=0;e->vocal_fx_input=0;return;}
  if(fabsf(dry)>.0001f)e->vocal_fx_tail=SR_RATE*(3u+(unsigned)e->vocal_param_now[VF_DECAY]/12u);
  if(!e->vocal_fx_tail){e->vocal_fx_history=0;return;}
  e->vocal_fx_tail--;
  e->vocal_fx_input+=dry;
  if(!(frame&3u)){
    const float *fx=e->vocal_param_now;
    e->vocal_fx_phase+=.00002f+fx[VF_RATE]*.000003f;
    if(e->vocal_fx_phase>=1.f)e->vocal_fx_phase-=1.f;
    float motion=e->vocal_fx_phase<.5f?e->vocal_fx_phase*2.f:2.f-e->vocal_fx_phase*2.f;
    e->vocal_flange_phase+=.000016f+fx[VF_FLANGE_RATE]*.0000028f;
    if(e->vocal_flange_phase>=1.f)e->vocal_flange_phase-=1.f;
    float flange_motion=1.f-fabsf(2.f*e->vocal_flange_phase-1.f);
    float chorus_depth=20.f+fx[VF_CHORUS_DEPTH]*.86f;

    float a=chorus?vocal_fractional_tap(e,105.f+motion*chorus_depth)*(chorus*.0028f):0.f;
    float b=chorus?vocal_fractional_tap(e,190.f-motion*chorus_depth)*(chorus*.0028f):0.f;
    float room_a=room?.52f*vocal_tap(e,499u)+.29f*vocal_tap(e,1093u)+
                     .13f*vocal_tap(e,1777u)+.06f*vocal_tap(e,3001u):0.f;
    float room_b=room?.52f*vocal_tap(e,787u)+.29f*vocal_tap(e,1399u)+
                     .13f*vocal_tap(e,2017u)+.06f*vocal_tap(e,3253u):0.f;
    /* The ring runs at one quarter of the audio rate. Choose binary
       tempo divisions, folding long values into the available history. */
    float delay=e->tick_length*.25f;
    unsigned division=(unsigned)fx[VF_TIME]/26u;
    delay*=division==0u?.5f:division==1u?1.f:division==2u?1.5f:2.f;
    while(delay>5400.f)delay*=.5f;
    float repeat=echo?vocal_fractional_tap(e,delay):0.f;
    e->vocal_fx_tone+=(repeat-e->vocal_fx_tone)*(.1f+fx[VF_TONE]*.008f);
    float second=echo?vocal_fractional_tap(e,delay*1.5f):0.f;
    float width=fx[VF_WIDTH]*.01f;
    float input=e->vocal_fx_input*.25f;
    float shimmer=0.f;
    if(fx[VF_SHIMMER]>.01f){
      unsigned phase=(frame>>2u)&511u;
      unsigned other=(phase+256u)&511u;
      float fade=(phase<256u?phase:512u-phase)*(1.f/256.f);
      shimmer=(vocal_tap(e,640u-phase)*fade+vocal_tap(e,640u-other)*(1.f-fade))
          *fx[VF_SHIMMER]*.0026f;
    }
    float flange=fx[VF_FLANGER]>.01f?(vocal_fractional_tap(e,9.f+flange_motion*31.f)-input)*fx[VF_FLANGER]*.0022f:0.f;
    float phase=0.f;
    if(fx[VF_PHASER]>.01f){
      float coefficient=.11f+.77f*motion;
      float shifted=e->vocal_phaser_in+coefficient*(input-e->vocal_phaser_out);
      e->vocal_phaser_in=input;e->vocal_phaser_out=shifted;
      phase=(shifted-input)*fx[VF_PHASER]*.0022f;
    }
    float damping=1.f-fx[VF_DAMPING]*.005f;
    float average=(room_a+room_b)*.5f;
    e->vocal_fx_left=a+(average+(room_a-average)*width)*(room*.0070f)*damping+
        e->vocal_fx_tone*(echo*.0060f)+phase+flange;
    e->vocal_fx_right=b+(average+(room_b-average)*width)*(room*.0070f)*damping+
        (e->vocal_fx_tone*(1.f-width)+second*width)*(echo*.0058f)+phase-flange;
    float room_feedback=.18f+room*.0018f+fx[VF_DECAY]*.0046f;
    float echo_feedback=echo*.0010f+fx[VF_FEEDBACK]*.0030f;
    float feedback_gain=.86f/fmaxf(.86f,room_feedback+echo_feedback+
        fx[VF_SHIMMER]*.0026f+fx[VF_FLANGER]*.0006f);
    float next=input+feedback_gain*(average*room_feedback*damping+
        e->vocal_fx_tone*echo_feedback+
        flange*(.12f+fx[VF_FLANGE_FEEDBACK]*.004f)+shimmer);
    e->vocal_fx_buffer[e->vocal_fx_pos]=sampleout(next);
    e->vocal_fx_pos=(e->vocal_fx_pos+1u)&8191u;
    if(e->vocal_fx_history<8192u)e->vocal_fx_history++;
    e->vocal_fx_input=0;
  }
  *left+=e->vocal_fx_left;
  *right+=e->vocal_fx_right;
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void mix_voices(sr_engine *e,const voice_block *block,float *dry,size_t frames,
                       const uint8_t *precomputed)
{
  const float *left=block->left,*right=block->right,*send=block->send;
  const voice_config *active=block->active;
  unsigned count=block->count,scope_track=block->scope_track;
  unsigned smooth_slicer=block->smooth_slicer;
  const float *slicer_target=block->slicer_target;
  float bass_hz=block->bass_hz,target_bass=block->target_bass;
  int locked=e->frequency_lock;
  float lock_hz=locked?sr_frequencies[e->frequency].hz:0.f;
  int audition=!locked&&e->nature_audition>0;
  unsigned preview=e->preview_frames&&!e->running;
  unsigned source=audition?SR_ISOLATE_NATURE:preview?SR_ISOLATE_TRACK1+e->preview_role:e->isolate;
  int full=source==SR_ISOLATE_MIX;
  const float bed_target=sr_limit_level(e->bed_level,SR_NATURE_MAX);
  const float reference_level=sr_limit_level(e->tone_level,SR_FIXED_TONE_MAX);
  float resonance=e->filter_resonance*(.78f/15.f);
  int vocal_visible=!locked&&!e->tracks[SR_CHOIR].mute&&e->vocals_enabled&&
      (e->solo>=SR_TRACKS||e->solo==SR_CHOIR)&&
      (full||source==SR_ISOLATE_TRACK1+SR_CHOIR);
  int vocal_fx=vocal_visible&&e->vocal_level;
  float vocal_send=e->vocal_send_now;
  if(!vocal_fx){
    e->vocal_fx_tail=0;e->vocal_fx_history=0;
    e->vocal_fx_left=e->vocal_fx_right=e->vocal_fx_input=0.f;
    e->vocal_fx_tone=e->vocal_phaser_in=e->vocal_phaser_out=0.f;
  }
  /* Track meters are display-only. Decay once per render chunk. */
  float meter_decay=1.f-(float)frames*.00097f;
  for(int t=0;t<SR_TRACKS;t++)e->tracks[t].meter*=meter_decay;
  float vocal_pose_peak=0.f;
  float vocal_direct_peak=0.f;
  for(size_t i=0;i<frames;i++){
    float l=0.f,r=0.f,mix=0.f,repeats=0.f,scope=0.f,vocal=0.f;
    if(smooth_slicer)for(unsigned lane=0;lane<4;lane++){
      float gain=e->slicer_gain[lane];
      float target=slicer_target[lane];
      if(gain!=target)e->slicer_gain[lane]=gain<target?clampf(gain+.008f,0.f,target):clampf(gain-.008f,target,1.f);
    }
    unsigned air_hash=(e->frame_count+i)*0x9e3779b9u;
    float breath_noise=((float)((air_hash^(air_hash>>13))&1023u)-512.f)*(1.f/512.f);
    int capture=scope_track<SR_TRACKS&&((e->frame_count+i)&3u)==0;
    float locked_sample=locked?advance(&e->lock_phase,lock_hz):0;
    for(unsigned a=0;a<count;a++){
      const voice_config *config=&active[a];
      sr_voice *v=config->voice;
      if(!v->active)continue;
      if(v->remain>0&&--v->remain==0&&v->gate){
        release_voice(e,v);
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
        if(capture&&scope_track==config->track)scope+=z*(config->left+config->right);
        continue;
      }
      unsigned cached=precomputed?precomputed[a]:0u;
      float s;
      unsigned articulation=0;
      if(v->sample_zone){
        unsigned zone=v->sample_zone-1u;
        unsigned index=(unsigned)v->phase;
        if(zone>=VOCAL_DAY_ZONE){
          unsigned word=zone-VOCAL_DAY_ZONE;
          if(index>=vocal_words[word].length-1u){v->active=0;continue;}
          const int8_t *pcm=vocal_words[word].pcm;
          s=(pcm[index]+(pcm[index+1u]-pcm[index])*(v->phase-index))*(1.f/128.f);
          unsigned release=vocal_words[word].release;
          articulation=vocal_words[word].mouth[index<vocal_words[word].attack?0:index<release?1:2];
          v->phase+=config->step;
          if(v->gate&&v->phase>=release)v->phase-=release-vocal_words[word].loop;
        }else{
          const int8_t *pcm=vocal_pcm[zone];
          unsigned next=index+1u<VOCAL_LENGTH?index+1u:vocal_zones[zone].loop;
          s=(pcm[index]+(pcm[next]-pcm[index])*(v->phase-index))*(1.f/128.f);
          articulation=1u+zone%3u;
          v->phase+=config->step;
          if(v->phase>=VOCAL_LENGTH)v->phase-=VOCAL_LENGTH-vocal_zones[zone].loop;
        }
        if(v->vocal_vowel_to){
          unsigned target=v->vocal_vowel_to-1u;
          const int8_t *pcm=target<VOCAL_DAY_ZONE?vocal_pcm[target]:vocal_words[target-VOCAL_DAY_ZONE].pcm;
          unsigned end=target<VOCAL_DAY_ZONE?VOCAL_LENGTH:vocal_words[target-VOCAL_DAY_ZONE].release;
          unsigned loop=target<VOCAL_DAY_ZONE?vocal_zones[target].loop:vocal_words[target-VOCAL_DAY_ZONE].loop;
          unsigned index=(unsigned)v->vocal_other_phase;
          unsigned next=index+1u<end?index+1u:loop;
          float second=(pcm[index]+(pcm[next]-pcm[index])*(v->vocal_other_phase-index))*(1.f/128.f);
          s+=(second-s)*config->morph;
          v->vocal_other_phase+=config->other_step;
          if(v->vocal_other_phase>=end)v->vocal_other_phase-=end-loop;
          if(config->morph>.5f)articulation=target<VOCAL_DAY_ZONE?1u+target%3u:
              vocal_words[target-VOCAL_DAY_ZONE].mouth[1];
        }
        unsigned age=e->frame_count+(unsigned)i-v->born;
        if(v->vocal_consonant==8u)s=s*.30f+v->filter*.70f;
        if(v->vocal_consonant&&v->vocal_consonant<8u&&age<SR_RATE/14u){
          float entrance=age*(14.f/SR_RATE);
          if(v->vocal_consonant==1u||v->vocal_consonant==3u){
            s=s*.22f+v->filter*.78f;
            if(entrance<.4f)articulation=4u;
          }else if(v->vocal_consonant==7u){
            s=s*entrance+breath_noise*.10f*(1.f-entrance);
          }else if(v->vocal_consonant==4u||v->vocal_consonant==6u){
            float burst=clampf(1.f-entrance*4.f,0.f,1.f);
            s=s*(1.f-burst)+breath_noise*.15f*burst;
            if(burst>.5f)articulation=v->vocal_consonant==6u?4u:6u;
          }else{
            s*=.55f+.45f*entrance;
            if(entrance<.3f)articulation=6u;
          }
        }
      }else if(cached){
        s=cached>FM_JOB_VOICES?local_fm.samples[i][cached-FM_JOB_VOICES-1u]:
          remote_fm.samples[i][cached-1u];
      }else{
        v->increment+=config->step;
        v->phase+=v->increment;
        if(v->phase>=1.f)v->phase-=1.f;
        v->phase2+=v->increment*config->fm_ratio;
        if(v->phase2>=1.f)v->phase2-=1.f;
        float index=config->fm_depth*(1.f-config->fm_envelope*(1.f-v->env));
        s=fm_sample(v->phase,v->phase2,index,config->vocal);
      }
      if(!v->sample_zone)s+=config->breath*breath_noise;
      if(!v->sample_zone&&config->wave==SR_PLUCK)v->peak*=config->body_decay;
      if(config->drive>0.f)s=s*(1.f+config->drive)/(1.f+config->drive*fabsf(s));
      if(resonance>0.f){
        s+=(v->filter-v->filter_tail)*resonance;
        v->filter_tail+=(v->filter-v->filter_tail)*config->cutoff;
      }
      v->filter+=(s-v->filter)*config->cutoff;
      float z=v->filter*v->env*config->gain;
      if(v->sample_zone){
        float peak=fabsf(z);
        float ratio=.8f-e->vocal_param_now[VF_COMP]*.007f;
        if(peak>.15f)z*=ratio+(.15f*(1.f-ratio))/peak;
        z*=.7f+e->vocal_param_now[VF_MAKEUP]*.006f;
      }
      z+=v->tail;
      if(smooth_slicer){
        if(v->sample_zone)z*=e->slicer_gain[3];
        else if(config->track<=SR_BASS)z*=e->slicer_gain[slicer_lane(config->track)];
      }
      v->tail*=.98f;
      l+=z*config->left;
      r+=z*config->right;
      mix+=z*config->send;
      repeats+=z*config->echo_send;
      if(vocal_fx&&v->sample_zone)vocal+=z*(config->left+config->right)*.5f;
      if(capture&&scope_track==config->track)scope+=z*(config->left+config->right);
      float raw=fabsf(z);
      if(v->sample_zone&&vocal_visible){
        float direct=raw*(config->left+config->right);
        if(direct>vocal_direct_peak)vocal_direct_peak=direct;
      }
      float mag=raw*(config->left+config->right);
      sr_track *track=&e->tracks[config->track];
      if(mag>track->meter)track->meter=mag;
      if(vocal_visible&&articulation&&mag>vocal_pose_peak){
        vocal_pose_peak=mag;
        e->vocal_face_mouth=articulation;
        e->vocal_face_midi=v->midi;
        e->vocal_face_pitch_ratio=config->vocal;
        e->vocal_face_sustain=v->gate&&e->frame_count+i-v->born>SR_RATE/4u;
      }
    }
    if(locked&&!e->lock_textures){
      if(capture){
        e->scope[e->scope_write]=sampleout(scope);
#ifdef PLAT_HOST
      e->scope_samples++;
#endif
        e->scope_write=(e->scope_write+1u)&127u;
      }
      e->tracks[SR_DRUMS].meter=0;
      dry[4*i]=l;dry[4*i+1]=r;dry[4*i+2]=dry[4*i+3]=0;
      continue;
    }
    if(vocal_fx)vocal_effects(e,vocal*vocal_send,e->frame_count+i,&l,&r);
    float drums=drum_sample(e);
    if(smooth_slicer)drums*=e->slicer_gain[2];
    if(capture){
      if(scope_track==SR_DRUMS)scope=drums*(left[SR_DRUMS]+right[SR_DRUMS]);
      e->scope[e->scope_write]=sampleout(scope);
#ifdef PLAT_HOST
      e->scope_samples++;
#endif
      e->scope_write=(e->scope_write+1u)&127u;
    }
    l+=drums*left[SR_DRUMS];r+=drums*right[SR_DRUMS];
    mix+=drums*send[SR_DRUMS]*.5f;
    repeats+=drums*block->echo_send[SR_DRUMS]*.5f;
    e->tracks[SR_DRUMS].meter=e->drum_meter*e->drum_level*(left[SR_DRUMS]+right[SR_DRUMS]);
    e->transport_env+=((e->running||audition||e->external_audio?1.f:0.f)-e->transport_env)*.0008f;
    e->nature_gain_now+=(bed_target-e->nature_gain_now)*.001f;
    float bed_gain=audition?e->nature_gain_now*clampf(((float)e->nature_audition-i)*(1.f/1102.5f),0,1):e->nature_gain_now;
    float nature=0, nature_side=0;
    if(!preview&&(audition||e->solo==SR_TRACKS)&&bed_gain>0&&
       (e->bed_type||e->nature_active||e->nature_blend<1.f||e->nature_mono||e->nature_side)){
      if((e->frame_count+i)&1u){
        nature=e->nature_mono;
        nature_side=e->nature_side;
      }else{
        float previous_mono=e->nature_mono;
        float previous_side=e->nature_side;
        e->nature_side=0;
        e->nature_mono=bed(e,&e->nature_side);
        nature=.5f*(e->nature_mono+previous_mono);
        nature_side=.5f*(e->nature_side+previous_side);
      }
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
      dry[4*i]=l;dry[4*i+1]=r;dry[4*i+2]=dry[4*i+3]=0;
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
    /* Phase-continuous, 28ms reference glide and gain ramp. A separate exact
     * reference oscillator; sacred bass remains explicitly octave-folded. */
    float target_hz=sr_frequencies[e->frequency].hz;
    if(e->reference_hz_now<=0.f)e->reference_hz_now=target_hz;
    e->reference_hz_now+=(target_hz-e->reference_hz_now)*.0008f;
    if(fabsf(target_hz-e->reference_hz_now)<.08f)e->reference_hz_now=target_hz;
    float target_gain=e->solo==SR_TRACKS?reference_level*e->transport_env:0.f;
    e->reference_gain_now+=(target_gain-e->reference_gain_now)*.001f;
    float reference=advance(&e->tone_phase,e->reference_hz_now)*e->reference_gain_now;
    if(full||source==SR_ISOLATE_FIXED){l+=reference;r+=reference;if(!(i&7u))bus_meter(e,SR_BUS_REFERENCE,reference);}
    if(full||source==SR_ISOLATE_SACRED){l+=sacred*.6f;r+=sacred*.6f;if(!(i&7u))bus_meter(e,SR_BUS_SACRED,sacred*.6f);}
    dry[4*i]=l; dry[4*i+1]=r; dry[4*i+2]=mix; dry[4*i+3]=repeats+nature*.2f;
  }
  e->vocal_face_peak=vocal_pose_peak;
  e->vocal_direct_peak=vocal_direct_peak;
  if(vocal_direct_peak>.0001f){
    e->vocal_audio_frames+=(uint32_t)frames;
    e->vocal_last_pcm_frame=e->frame_count;
  }
  if(precomputed)for(unsigned a=0;a<count;a++){
    unsigned cached=precomputed[a];
    if(!cached)continue;
    const fm_input *result=cached>FM_JOB_VOICES?&local_fm.inputs[cached-FM_JOB_VOICES-1u]:
        &remote_fm.inputs[cached-1u];
    sr_voice *voice=active[a].voice;
    voice->phase=result->phase;
    voice->phase2=result->mod_phase;
    voice->increment=result->increment;
  }
}

static void (*volatile prepare_block)(sr_engine *,voice_block *,size_t)=prepare_voices;
static void (*volatile mix_block)(sr_engine *,const voice_block *,float *,size_t,const uint8_t *)=mix_voices;

static void render_samples(sr_engine *e,float *dry,size_t frames)
{
    voice_block block;
    uint8_t route[SR_VOICES]={0};
    prepare_block(e,&block,frames);
    const sr_fm_worker *worker=fm_worker;
    if(!worker||!worker->available()||e->frequency_lock||frames<32||frames>FM_JOB_FRAMES||block.count<4){
        mix_block(e,&block,dry,frames,0);
        return;
    }
    remote_fm.count=local_fm.count=0;
    remote_fm.frames=local_fm.frames=frames;
    for(unsigned a=0;a<block.count;a++){
        const voice_config *c=&block.active[a];
        sr_voice *v=c->voice;
        /* Key-off and release tails stay local so event order and lifecycle
         * cannot diverge across the cores. */
        if(v->sample_zone||!v->gate||(v->remain&&v->remain<=frames))continue;
        unsigned local=local_fm.count<remote_fm.count+2u;
        fm_job *job=local?&local_fm:&remote_fm;
        unsigned slot=job->count++;
        job->inputs[slot]=(fm_input){v->phase,v->phase2,v->increment,c->step,c->fm_ratio,
            c->fm_depth,c->fm_envelope,v->env,v->peak,v->attack_coef,
            c->wave==SR_PLUCK?c->body_decay:1.f,c->vocal};
        route[a]=(uint8_t)(slot+1u+(local?FM_JOB_VOICES:0u));
    }
    if(local_fm.count<2||!worker->start(remote_fm_render,0)){
        mix_block(e,&block,dry,frames,0);
        return;
    }
    synthesize_fm(&local_fm);
    /* One readiness check, never a wait on an outstanding audio job. */
    if(!worker->finish())
        for(unsigned a=0;a<block.count;a++)if(route[a]<=FM_JOB_VOICES)route[a]=0;
    mix_block(e,&block,dry,frames,route);
}

static inline __attribute__((always_inline)) float modulation_tap(const sr_engine *e,float tap)
{
  unsigned lag=(unsigned)tap;
  if(e->delay_history<=lag+1u)return 0.f;
  unsigned read=(e->delay_pos+16383u-lag)&16383u;
  float a=e->delay[read],b=e->delay[(read+16383u)&16383u];
  return (a+(b-a)*(tap-lag))*(1.f/32768.f);
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void apply_fx(sr_engine *e,float *dry,size_t frames)
{
  unsigned mask=e->fx_mask;
  int audible=e->isolate==SR_ISOLATE_MIX||e->isolate==SR_ISOLATE_FX;
  float depth=(e->sound_locks&SR_SOUND_FX)?1.f:.7f+.3f*e->current_energy;
  float reverb=e->reverb_amount*depth;
  float delay=e->delay_amount*depth;
  float decay=.89f+((int)e->fx_decay-50)*.0016f;
  float feedback=.46f+((int)e->fx_feedback-50)*.006f;
  float damping=e->fx_damping*.01f;
  float stereo=e->fx_width*.02f;
  if(!(e->sound_locks&SR_SOUND_FX)){
    float bloom=.012f*e->current_energy;
    if(e->music_mode==SR_CINEMATIC){decay+=.022f+bloom;stereo*=1.1f;}
    if(e->music_mode==SR_EXPERIMENTAL||e->music_mode==SR_IDM)feedback+=.06f+bloom;
    decay=clampf(decay,0.f,.975f);
    feedback=clampf(feedback,0.f,.84f);
  }
  float chorus=e->fx_chorus*.01f, pan=e->fx_autopan*.01f;
  float filter=e->fx_filter*.01f, crush=e->fx_crush*.01f;
  float flange=e->fx_flanger*.01f, trem=e->fx_tremolo*.01f;
  float drive=e->fx_drive*.01f, stutter=e->fx_stutter*.01f;
  float texture_level=e->fx_chorus_now+e->fx_pan_now+e->fx_filter_now+e->fx_crush_now+
                      e->fx_flanger_now+e->fx_tremolo_now+e->fx_drive_now+e->fx_stutter_now;
  int texture=chorus||pan||filter||crush||flange||trem||drive||stutter||texture_level>.0001f;
  float shimmer=e->fx_shimmer*.01f, granular=e->fx_granular*.01f, duck=e->fx_duck*.01f;
  int shimmer_active=shimmer||e->fx_shimmer_now>.0001f;
  int granular_active=granular||e->fx_granular_now>.0001f;
  int duck_active=duck||e->fx_duck_now>.0001f;
  if(!granular_active)e->fx_grain_written=e->fx_grain_phase=0;
  if(!duck_active){e->fx_duck_env=0.f;e->fx_duck_gain=1.f;}
  if(shimmer_active)reverb=fmaxf(reverb,shimmer*.48f);
  for(size_t i=0;i<frames;i++){
    e->fx_decay_now+=(decay-e->fx_decay_now)*.002f;
    e->fx_feedback_now+=(feedback-e->fx_feedback_now)*.002f;
    e->fx_damping_now+=(damping-e->fx_damping_now)*.002f;
    e->fx_width_now+=(stereo-e->fx_width_now)*.002f;
    e->reverb_wet+=(reverb-e->reverb_wet)*.002f;
    e->delay_wet+=(delay-e->delay_wet)*.002f;
    float width=0;
    float shifted=0.f;
    if(shimmer_active){
      e->fx_shimmer_now+=(shimmer-e->fx_shimmer_now)*.002f;
      /* Two offset read heads advance twice as fast as the ring writes.
         A triangular crossfade hides their wrap, and the shifted signal
         feeds only the reverb input, outside the delay feedback loop. */
      unsigned phase=e->fx_shimmer_phase;
      unsigned offset=(phase+512u)&1023u;
      float blend=(phase<512u?phase:1024u-phase)*(1.f/512.f);
      if(e->delay_history>=1120u){
        unsigned a=(e->delay_pos+16384u-1120u+phase)&16383u;
        unsigned b=(e->delay_pos+16384u-1120u+offset)&16383u;
        shifted=(e->delay[a]*blend+e->delay[b]*(1.f-blend))*(.68f/32768.f)*e->fx_shimmer_now;
      }
      e->fx_shimmer_phase=(phase+1u)&1023u;
    }
    float wet=fx(e,mask,dry[4*i+2],dry[4*i+3],shifted,&width);
    if(granular_active){
      if(granular&&e->fx_grain_written<1024u){
        e->fx_grain[e->fx_grain_written++]=sampleout(dry[4*i+2]);
      }else if(e->fx_grain_written==1024u){
        e->fx_granular_now+=(granular-e->fx_granular_now)*.002f;
        /* Hold the captured send. Overlap two windowed 512-frame grains
           drawn from different offsets in the snapshot. */
        unsigned p=e->fx_grain_phase;
        unsigned q=(p+256u)&511u;
        float w=(p<256u?p:512u-p)*(1.f/256.f);
        float a=e->fx_grain[p]*w*(.55f/32768.f);
        float b=e->fx_grain[512u+q]*(1.f-w)*(.55f/32768.f);
        wet+=(a+b)*e->fx_granular_now;
        width+=(a-b)*(.35f*e->fx_granular_now);
        e->fx_grain_phase=(p+1u)&511u;
      }
    }
    if(texture){
      e->fx_chorus_now+=(chorus-e->fx_chorus_now)*.002f;
      e->fx_pan_now+=(pan-e->fx_pan_now)*.002f;
      e->fx_filter_now+=(filter-e->fx_filter_now)*.002f;
      e->fx_crush_now+=(crush-e->fx_crush_now)*.002f;
      e->fx_flanger_now+=(flange-e->fx_flanger_now)*.002f;
      e->fx_tremolo_now+=(trem-e->fx_tremolo_now)*.002f;
      e->fx_drive_now+=(drive-e->fx_drive_now)*.002f;
      e->fx_stutter_now+=(stutter-e->fx_stutter_now)*.002f;
      e->fx_mod_phase+=.35f/SR_RATE;
      if(e->fx_mod_phase>=1.f)e->fx_mod_phase-=1.f;
      float sweep=sine(e->fx_mod_phase);
      if(e->fx_chorus_now>.0001f){
        float voice=modulation_tap(e,530.f+180.f*sweep)*.48f*e->fx_chorus_now;
        wet+=voice;
        width+=voice*.5f;
      }
      if(e->fx_flanger_now>.0001f){
        float tap=modulation_tap(e,52.f+40.f*sweep);
        wet+=(dry[4*i+3]*.63f-tap)*.35f*e->fx_flanger_now;
      }
      if(e->fx_stutter_now>.0001f){
        if(!e->fx_repeat_left){
          unsigned slice=e->tick_length>>2u;
          if(slice<64u)slice=64u;
          if(slice>2048u)slice=2048u;
          if(e->delay_history>=slice){
            e->fx_repeat_start=(e->delay_pos+16384u-slice)&16383u;
            e->fx_repeat_length=slice;
            e->fx_repeat_pos=0;
            e->fx_repeat_left=4u*slice;
          }
        }
        if(e->fx_repeat_left){
          unsigned index=e->fx_repeat_pos;
          unsigned remain=e->fx_repeat_length-index;
          unsigned edge=index<remain?index:remain;
          float window=edge<24u?edge*(1.f/24.f):1.f;
          float sample=e->delay[(e->fx_repeat_start+index)&16383u]*(1.f/32768.f);
          wet+=sample*window*.52f*e->fx_stutter_now;
          e->fx_repeat_left--;
          if(++e->fx_repeat_pos==e->fx_repeat_length)e->fx_repeat_pos=0;
        }
      }else e->fx_repeat_left=0;
      if(e->fx_filter_now>.0001f){
        float coef=.42f-.404f*e->fx_filter_now;
        e->fx_filter_low+=(wet-e->fx_filter_low)*coef;
        wet+=(e->fx_filter_low-wet)*e->fx_filter_now;
        width*=1.f-.6f*e->fx_filter_now;
      }
      if(e->fx_crush_now>.0001f){
        if(!((e->frame_count+i)&7u))e->fx_crush_hold=(int)(wet*32.f)*(1.f/32.f);
        wet+=(e->fx_crush_hold-wet)*e->fx_crush_now;
      }
      if(e->fx_drive_now>.0001f){
        float shaped=clampf(wet*2.2f,-1.f,1.f);
        shaped=shaped*(1.f-.33f*shaped*shaped);
        wet+=(shaped-wet)*e->fx_drive_now;
      }
      if(e->fx_tremolo_now>.0001f){
        float gain=1.f-(.5f+.5f*sine(8.f*e->fx_mod_phase))*.9f*e->fx_tremolo_now;
        wet*=gain;
        width*=gain;
      }
      width+=wet*sweep*.85f*e->fx_pan_now;
    }
    if(duck_active){
      e->fx_duck_now+=(duck-e->fx_duck_now)*.002f;
      if(!(i&7u)){
        float key=fmaxf(fabsf(dry[4*i]),fabsf(dry[4*i+1]));
        float coef=key>e->fx_duck_env?.08f:.0012f;
        e->fx_duck_env+=(key-e->fx_duck_env)*coef;
      }
      float gain=1.f-.84f*e->fx_duck_now*clampf(e->fx_duck_env*7.f,0.f,1.f);
      e->fx_duck_gain+=(gain-e->fx_duck_gain)*.006f;
      wet*=e->fx_duck_gain;
      width*=e->fx_duck_gain;
    }
    if(audible){
      if(!(i&7u))bus_meter(e,SR_BUS_FX,fmaxf(fabsf(wet+width),fabsf(wet-width)));
      dry[4*i]+=wet+width;
      dry[4*i+1]+=wet-width;
    }
  }
}

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void output_samples(sr_engine *e,const float *dry,int16_t *out,size_t frames)
{
  const float ceiling=.78f;
  const float boost=e->auto_gain&&!e->frequency_lock?1.13f:1.f;
  float peak=e->master_peak*(1.f-(float)frames*.0005f);
  int binaural=!e->frequency_lock&&!e->nature_audition&&!e->preview_frames&&
     (e->binaural_env>=.00003f||(e->running&&e->binaural_enabled&&!e->speaker_enabled&&e->solo==SR_TRACKS));
  if(!binaural&&e->binaural_env<.00003f)e->binaural_env=0;
  binaural_block binaural_plan={0};
  if(binaural)binaural_plan=prepare_binaural(e);
  const int shape_speaker=e->speaker_enabled||e->speaker_mix>0.f;
  const float speaker_target=e->speaker_enabled?1.f:0.f;
  for(size_t i=0;i<frames;i++){
    float l=dry[4*i],r=dry[4*i+1];
    float binaural_l=0,binaural_r=0;
    if(binaural)render_binaural(e,&binaural_plan,&binaural_l,&binaural_r);
    if(binaural&&(e->isolate==SR_ISOLATE_MIX||e->isolate==SR_ISOLATE_BINAURAL)){
      if(!(i&7u))bus_meter(e,SR_BUS_BINAURAL,fmaxf(fabsf(binaural_l),fabsf(binaural_r)));
      l+=binaural_l;r+=binaural_r;
    }
    if(shape_speaker){
      /* Sub-bass attenuation near 135 Hz and mild presence above 1.6 kHz. */
      float mono=.5f*(l+r);
      e->speaker_low+=.019f*(mono-e->speaker_low);
      e->speaker_body+=.204f*(mono-e->speaker_body);
      float shaped=.84f*(mono-.88f*e->speaker_low+.19f*(mono-e->speaker_body));
      if(e->speaker_mix!=speaker_target)
        e->speaker_mix=clampf(e->speaker_mix+(speaker_target>0.f?.0016f:-.0016f),0.f,1.f);
      l+=(shaped-l)*e->speaker_mix;
      r+=(shaped-r)*e->speaker_mix;
    }
    // Soft-knee master saturation controls peaks without a dynamic allocation.
    float level=fmaxf(fabsf(l),fabsf(r));
    if(level>peak)peak=level;
    /* Fixed optional mastering lift followed by the common peak ceiling.
     * AUTO LEVEL has no influence on the release or the final ceiling. */
    float target=level*boost>ceiling?ceiling/level:boost;
    if(target<e->limiter || e->limiter==0.f)e->limiter=target;
    else e->limiter+=(target-e->limiter)*.00018f;
    float gain=e->limiter*e->master;
    if(e->preview_frames&&e->preview_frames-i<1102u)
      gain*=(e->preview_frames-i)*(1.f/1102.f);
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
 float dry[64*4];
 e->fx_ticks=0;
 while(frames){
  if(e->external_audio){
   e->external_audio=0;
   for(unsigned v=0;v<SR_VOICES;v++)if(e->voices[v].active&&e->voices[v].external)e->external_audio=1;
  }
  if(!e->external_audio&&!e->running&&!e->stop_frames&&!e->nature_audition&&!e->preview_frames){
    e->vocal_face_peak=e->vocal_direct_peak=0.f;
    memset(out,0,frames*2*sizeof(*out));
    return;
  }
  if(e->running&&e->tick_phase>=e->tick_length){e->tick_phase=0;memset(e->ornaments,0,sizeof e->ornaments);trigger(e);}
  size_t n=frames>64?64:frames;
  if(e->running)for(unsigned i=0;i<12;i++)if(e->scheduled[i].active){
    int32_t due=(int32_t)(e->scheduled[i].due-e->frame_count);
    if(due<=0){
      unsigned role=e->scheduled[i].role;e->scheduled[i].active=0;
      if(role==SR_DRUMS){
        if(!e->drums_enabled||e->freeze||e->tracks[role].mute)continue;
        drum_strike(e,e->scheduled[i].note,e->scheduled[i].velocity);
      }else sr_note_on(e,role,e->scheduled[i].note,e->scheduled[i].velocity,e->scheduled[i].beats);
    }else if(n>(unsigned)due)n=(unsigned)due;
  }
  if(e->running&&(e->ornaments[0].phase||e->ornaments[1].phase||
                 e->ornaments[2].phase||e->ornaments[3].phase)){
    for(unsigned slot=0;slot<sizeof ornament_track;slot++){
      unsigned phase=e->ornaments[slot].phase;
      if(!phase)continue;
      if(phase==e->tick_phase){
        e->ornaments[slot].phase=0;
        sr_note_on(e,ornament_track[slot],e->ornaments[slot].midi,
                   e->ornaments[slot].velocity,e->ornaments[slot].beats);
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

  if(e->running&&!e->external_sync&&n>e->tick_length-e->tick_phase)n=e->tick_length-e->tick_phase;
  if(e->stop_frames&&n>e->stop_frames)n=e->stop_frames;
  if(e->nature_audition&&n>e->nature_audition)n=e->nature_audition;
  if(e->preview_frames&&n>e->preview_frames)n=e->preview_frames;
  unsigned fading=e->stop_frames;
  unsigned audition=e->nature_audition;
  if(e->vocals_enabled&&!e->frequency_lock)vocal_production(e,n);
  render_block(e,dry,n);
  unsigned reverb=e->reverb_amount>0||e->reverb_wet>.00001f;
  unsigned delay=e->delay_amount>0||e->delay_wet>.00001f;
  unsigned short_delay=e->fx_chorus||e->fx_flanger||e->fx_stutter||
      e->fx_chorus_now+e->fx_flanger_now+e->fx_stutter_now>.0001f;
  unsigned shimmer=e->fx_shimmer||e->fx_shimmer_now>.0001f;
  unsigned freeze=e->fx_granular||e->fx_granular_now>.0001f;
  unsigned fx_mask=((reverb||shimmer)?1u:0u)|((reverb||delay||short_delay||shimmer)?2u:0u);
  if(!freeze)e->fx_grain_written=e->fx_grain_phase=0;
  if(e->fx_mask&~fx_mask){
    if(!(fx_mask&1)){e->rev_history=0;memset(e->rev_pos,0,sizeof(e->rev_pos));e->reverb_wet=0;}
    if(!(fx_mask&2)){e->delay_history=0;e->delay_pos=0;e->delay_wet=0;e->fx_repeat_left=0;}
  }
  e->fx_mask=(uint8_t)fx_mask;
  if((fx_mask||freeze)&&!e->frequency_lock&&!audition&&!e->preview_frames){
    uint32_t begin=e->clock_ticks?e->clock_ticks():0;
    process_fx(e,dry,n);
    if(e->clock_ticks)e->fx_ticks+=e->clock_ticks()-begin;
  }
  finish_block(e,dry,out,n);
  if(e->preview_frames)e->preview_frames-=(uint32_t)n;
  if(audition)e->nature_audition=audition>n?audition-(uint32_t)n:0;
  if(fading&&!e->stop_frames){
    unsigned restart=e->restart;
    clear_transport(e);
    e->restart=0;
    e->frequency_lock=e->pending_lock;
    if(restart){e->running=1;e->sequence_step=0;e->loop_pos=0;e->tick_phase=e->tick_length;e->external_clock_count=255;}
  }
  if(e->running&&!fading){
    e->tick_phase+=(uint32_t)n;
    if(e->external_sync&&e->tick_phase>=e->tick_length)e->tick_phase=e->tick_length-1u;
  }
  out+=n*2;frames-=n;
 }
}
