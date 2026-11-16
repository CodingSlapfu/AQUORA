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
      if(section==2u&&e->music_mode==SR_EXPERIMENTAL)degree--;
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
  static const uint8_t mode[SR_MUSIC_MODES][4]={{4,2,2,3},{2,5,2,4},{3,1,7,1},{2,3,1,7}};
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
      static const uint8_t mode_kits[4][5]={{0,3,7,10,15},{1,5,12,16,18},{2,4,8,13,17},{6,9,11,14,18}};
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
      e->tracks[SR_DRUMS].send=.08f+.035f*palette[scene].space+(e->music_mode==SR_CINEMATIC?.06f:0.f);
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
    e->reverb_amount=.12f+.045f*palette[scene].space+.01f*(song_draw(seed,9)%8u);
    e->delay_amount=.06f+.025f*palette[scene].space+.01f*(song_draw(seed,10)%6u);
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
  static const uint8_t ranges[SR_MUSIC_MODES][3][2]={
    {{36,48},{49,60},{61,72}},{{30,40},{41,52},{53,64}},
    {{52,68},{69,92},{93,118}},{{42,64},{65,94},{95,120}}
  };
  unsigned group=song_draw(seed,201)%10u;
  group=group<(e->music_mode<=SR_CINEMATIC?6u:2u)?0u:group<8u?1u:2u;
  unsigned lo=ranges[e->music_mode][group][0],hi=ranges[e->music_mode][group][1];
  unsigned midpoint=(lo+hi)/2u;
  unsigned preferred=(midpoint*2u+sr_scenes[e->scene].bpm)/3u;
  int bpm=(int)preferred+(int)(song_draw(seed,202)%11u)-5;
  if(bpm<(int)lo)bpm=(int)lo;
  if(bpm>(int)hi)bpm=(int)hi;
  e->tick_length=(SR_RATE*15u+bpm/2u)/bpm;
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
void sr_set_track_patch(sr_engine *e,uint8_t t,uint8_t patch){if(t<SR_TRACKS&&patch<SR_PATCHES)e->tracks[t].patch=patch;}
void sr_init(sr_engine *e,uint32_t seed){
  memset(e,0,sizeof(*e)); e->rng=seed?seed:0x73ac05u;
  if(!lut_ready){
    for(int i=0;i<=LUT;i++)sin_table[i]=sinf((float)i*2.f*PI/(float)LUT);
    lut_ready=1;
  }
  e->auto_gain=e->auto_level=1;
  e->chord_gap=190; e->master=.78f; e->reverb_amount=.32f; e->delay_amount=.18f; e->bed_level=.18f;
  e->learn_strength=2;e->drums_enabled=1;e->drum_level=1.f;e->drum_presence=100;e->groove_strength=100;e->air_track=SR_TRACKS;e->air_depth=100;e->drum_density=SR_DENSITY_MAX;e->drum_hit=25;e->drum_rng=e->rng^0x6a09e667u;e->drum_kit=0;
  for(unsigned part=0;part<3;part++)e->drum_part_gain[part]=e->slicer_gain[part]=1.f;
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
    e->external_audio=0;
    e->play_queued=0;
    e->play_released=0;
    memset(e->ornaments,0,sizeof e->ornaments);
    for(unsigned i=0;i<SR_VOICES;i++)release_voice(e,&e->voices[i]);
    memset(e->voices,0,sizeof(e->voices));
    memset(e->rev_pos,0,sizeof(e->rev_pos));
    e->delay_pos=0;
    e->rev_history=e->delay_history=0;
    e->reverb_wet=e->delay_wet=0;
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
    if(e->rhythm_pending){e->rhythm_style=e->rhythm_pending-1u;e->rhythm_pending=0;}
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

static sr_voice *note_on(sr_engine *e,uint8_t track,uint8_t midi,uint8_t velocity,float beats){
  if((!e->running&&!e->preview_frames&&!e->midi_suppress)||track>=SR_DRUMS||midi>127||!velocity||e->tracks[track].mute)return 0;
  int slot=-1;float quiet=1.e9f;
  for(int i=0;i<SR_VOICES;i++){
    if(!e->voices[i].active){slot=i;break;}
    if(e->voices[i].key&&e->voices[i].gate)continue;
    float score=e->voices[i].env+(e->voices[i].track==SR_BASS?.35f:e->voices[i].key&&e->voices[i].gate?.25f:0);
    if(score<quiet){quiet=score;slot=i;}
  }
  if(slot<0)return 0;
  sr_voice *v=&e->voices[slot];release_voice(e,v);e->note_ons++;float tail=v->active?v->filter*v->env*calibrated_patch_gain(v->patch):0.f;memset(v,0,sizeof(*v));v->tail=tail;v->active=v->gate=1;v->track=track;v->patch=e->tracks[track].patch;
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
 sr_voice *v=note_on(e,(uint8_t)role,(uint8_t)note,(uint8_t)velocity,1.f);
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
    if(track<SR_DRUMS&&e->running&&!e->preview_frames){
      if(e->solo>=SR_TRACKS&&e->role_target[track]==0.f)return;
      if(e->role_target[track]>0.f)
        velocity=(uint8_t)(velocity*(.55f+.45f*e->role_target[track])+.5f);
    }
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
      e->role_target[SR_DRUMS]=1.f;
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
    if(!e->running){e->preview_role=track;e->preview_frames=SR_RATE*10u;play_key(e,track,key,0);return;}
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
 unsigned density=groove_density(e,step);
 unsigned beat=step&15u,bar=score_bar(e,step);

 if(!beat||(beat==8u&&density>=2u))return 1.f;
 unsigned groove=(intention_score[e->scene][1]+e->music_mode+song_draw(e->song_seed,370))&3u;
 static const uint8_t reply[4][2]={{6,10},{11,7},{14,6},{10,13}};
 if(density>=4u&&beat==reply[groove][bar&1u])return .54f;
 if(e->rhythm_style==1u&&density>=2u&&(beat==3u||beat==11u))return .65f;
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
 unsigned pickup=(beat==7u||beat==15u)&&e->energy>.55f&&
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
static void drum_trigger(sr_engine *e,uint32_t step,uint8_t energy){
  e->drum_playhead=step&63u;
  e->drum_steps[e->drum_playhead]=0;
  if(!e->drums_enabled||!e->drum_density||e->drum_level<=0.f)return;
  unsigned density=groove_density(e,step);
  uint32_t beat=step&15u;
  unsigned bar=e->loop_length>16?step/16u:0;

  unsigned hits=0,parts=0;
  uint32_t stable_draw=song_draw(e->song_seed+e->scene+bar*13u,beat);
  float kick=groove_kick(e,step);if(kick){e->kick_env=kick;parts|=1u;hits++;}
  if((beat==4||beat==12) && (density>=2 || (stable_draw&3u)==0)){
    e->hand_env=bar&&(beat==4?bar&1u:bar&2u)?.78f:1.f;parts|=2u;hits++;
  }
  if(density>=3 && (beat&1u)){e->shaker_env=.72f;parts|=4u;hits++;}
  else if(density==2 && (beat&3u)==2){e->shaker_env=.65f;parts|=4u;hits++;}
  else if(density==1 && beat==8 && (stable_draw&1u)==0){e->shaker_env=.45f;parts|=4u;hits++;}
  if(density>=4&&(beat&3u)==2){e->shaker_env=.56f;parts|=4u;hits++;}
  if(density>=5&&beat==(bar&1u?10u:6u)){e->hand_env=.48f;parts|=2u;hits++;}
  if(density>=7&&(beat==2||beat==14)){e->hand_env=.42f;parts|=2u;hits++;}
  if(density>=8&&(beat==3||beat==15)){e->hand_env=bar&1u?.52f:.37f;parts|=2u;hits++;}
  /* A bloom grows within the musical phrase, without global gain ramps.
   * Longer phrases have more gradual development and a small final answer. */
  if(!e->freeze&&e->fill_character!=3&&song_section(e)==2u&&e->loop_length>16){
    unsigned bar_progress=step/16u;
    if(bar_progress&&(beat&3u)==2u){e->shaker_env=.25f+.12f*bar_progress;parts|=4u;hits++;}
    if(bar_progress==e->loop_length/16u-1u&&beat==14u){e->hand_env=.43f;parts|=2u;hits++;}
  }
  /* Optional rhythm character: the downbeat is always preserved. */
  if(e->rhythm_style==1&&density>=2){
    if(beat==7||beat==14){e->hand_env=.48f;parts|=2u;hits++;}
    if((beat&1u)&&((bar+beat)%3u)){e->shaker_env=.38f+.08f*(bar&1u);parts|=4u;hits++;}
  }else if(e->rhythm_style==2){
    if(beat==6||beat==13){e->hand_env=.42f;parts|=2u;hits++;}
  }
  /* Authored section percussion: current kit, no generator gating at
   * boundaries. Sparse functions keep the downbeat/tonal anchor. */
  if(!e->freeze&&e->variation>0.f){
    unsigned id=form_index(e,0),pulse=sr_sections[id].pulse;
    unsigned character=id%8u;
    if(pulse&&bar%2u&&beat==(character&1u?14u:6u)){e->hand_env=.28f+.03f*(character%3u);parts|=2u;hits++;}
    if(id==20u&&(beat&3u)==3u){e->shaker_env=.26f+.09f*(step/16u);parts|=4u;hits++;}
    if(id==30u&&(beat%3u)==2u){e->shaker_env=.32f;parts|=4u;hits++;}
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
  if(!hits&&eligible){
    e->drum_rng=e->drum_rng*1664525u+1013904223u;
    unsigned percent=e->drum_hit>SR_HIT_MAX?SR_HIT_MAX:e->drum_hit;
    unsigned allowance=e->music_mode==SR_CINEMATIC?22u:e->music_mode==SR_NEOCLASSICAL?34u:55u;
    if((stable_draw>>16)%100u<percent*allowance/100u){
      if(beat==11||(bar&&(beat==5||beat==13))){e->hand_env=.55f;parts|=2u;}
      else {e->shaker_env=.52f;parts|=4u;}
      e->optional_hits++;
      hits++;
    }
  }
  unsigned accents=(e->kick_env>=.75f?1u:0u)|(e->hand_env>=.75f?2u:0u)|(e->shaker_env>=.75f?4u:0u);
  e->drum_steps[e->drum_playhead]=parts|((parts&accents)<<3u);
  e->tracks[SR_DRUMS].events+=hits;
  if(e->timeline_enabled)for(unsigned voice=0;voice<3;voice++)if(parts&(1u<<voice)){
   unsigned n=timeline_slot(e);
   e->timeline[n].frame=e->frame_count;e->timeline[n].duration=e->tick_length/2u;
   e->timeline[n].role=SR_DRUMS;e->timeline[n].note=voice==0?36:voice==1?38:42;
   float level=voice==0?e->kick_env:voice==1?e->hand_env:e->shaker_env;
   e->timeline[n].velocity=(uint8_t)(level*127.f+.5f);
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

static inline __attribute__((always_inline)) float drum_sample(sr_engine *e){
  /* Silence is a common path in the default generator. Skip all oscillators,
   * noise and kit inspection when no percussion envelope is active. */
  if(e->kick_env<=.0003f&&e->hand_env<=.0003f&&e->shaker_env<=.0003f){
    e->kick_env=e->kick_previous=e->kick_attack=e->hand_env=e->shaker_env=0.f;
    return 0.f;
  }
  const sr_drum_kit *kit=&sr_drum_kits[e->drum_kit];
  for(unsigned part=0;part<3;part++){
    float target=(e->drum_mutes&(1u<<part))?0.f:1.f;
    float gain=e->drum_part_gain[part];
    if(gain!=target)e->drum_part_gain[part]=clampf(gain+(target>0.f?.004f:-.004f),0.f,1.f);
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
  sound*=1.14f;
  e->drum_meter=fmaxf(e->drum_meter*.9993f,fabsf(sound));
  return sound*e->drum_level;
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
 static const uint8_t guitar[SR_MUSIC_MODES]={180,205,190,180};
 static const uint8_t harp[SR_MUSIC_MODES]={200,230,215,190};
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
   performed+=schedule(e,SR_DRUMS,((i+style)%3u)?1:2,
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
 if(song_section(e)==2u&&e->music_mode<=SR_CINEMATIC){
  unsigned upper=(chord+(inner==(chord+4u)%7u?2u:4u))%7u;
  int upper_note=tonic+12+scale[upper];
  while(upper_note<note+3)upper_note+=12;
  sr_note_on(e,SR_DRONE,upper_note,39,2.65f);
 }
}

static void choir_answer(sr_engine *e,unsigned step,unsigned chord)
{
 const unsigned pos=step&15u,bar=step/16u,section=song_section(e),mode=e->music_mode;
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 const int tonic=sr_tonic(e);
 /* The choir enters after the lead has left space. In Cinematic,
  * pick up a longer phrase from the most recent lead note in the bar. */
 if(pos<5u||pos>13u||motif_degree(e,step)>=0||motif_degree(e,step-1u)>=0)return;
 if(section==3u||(section==0u&&mode!=SR_CINEMATIC))return;
 int call=motif_degree(e,step-2u);
 if(call<0&&mode==SR_CINEMATIC&&(pos==7u||pos==12u))
  for(unsigned back=3u;back<=5u&&call<0;back++)call=motif_degree(e,step-back);
 if(call<0)return;
 unsigned answer=support_degree(e,chord,phrase_chord(e,bar+1u),(unsigned)call,1);
 int note=tonic+24+scale[answer];
 if(note>=tonic+24+scale[call])note-=12;
 while(note<53)note+=12;
 float gate=(15u-pos)*.25f;
 if(section!=2u&&mode!=SR_CINEMATIC&&gate>1.4f)gate=1.4f;
 /* Avoid a zero-age choir attack being displaced by a piano or guitar
  * note on the same sequencer tick when the voice pool is full. */
 schedule(e,SR_CHOIR,note,(section==2u?59:49)+(pos<9u?3:0),gate,
          e->tick_length/3u);

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
  {{3,6,11,14},{2,7,10,13}}
 };
 const unsigned pos=step&15u,bar=step/16u,mode=e->music_mode;
 const unsigned section=song_section(e);
 const unsigned answer=(score_bar(e,step)+(song_draw(e->song_seed,390)&1u))&1u;
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 const int root=sr_tonic(e)+12;
 if(mode==SR_NEOCLASSICAL&&e->chord_mode!=3&&!e->frequency_lock&&pos==0u)
  sr_note_on(e,SR_PIANO,root+scale[chord],54+expression,piano_gate(e,pos,2.25f));

 const uint8_t *phrase=rhythm[mode][answer];
 unsigned count=mode==SR_NEOCLASSICAL?4u:3u;
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
  int expression=phrase_expression(s);
  drum_trigger(e,s,e->music_mode==SR_CINEMATIC?2:1);
  bass_groove(e,s);
  if(pos==0u)accompaniment(e,s/16u,chord);
  pluck_answer(e,s,chord,degree);
  if(!learned_active(e)&&degree>=0)
    ornament(e,2,lead_pitch(e,degree),68-dynamic+expression,melody_duration(e,s),s,e->variation>0.f&&!e->freeze);
  piano_phrase(e,s,chord,degree,expression);
  choir_answer(e,s,chord);
  if(pos==12u&&(song_section(e)==2u||song_section(e)==4u)){
    unsigned next=phrase_chord(e,s/16u+1u);
    unsigned inner=support_degree(e,chord,next,(chord+4u)%7u,0);
    sr_note_on(e,SR_HARMONY,tonic+24+scale[inner],44+expression,.85f);
  }
}
static void musical_detail(sr_engine *e,unsigned step)
{
  if(e->freeze||e->variation<=0.f)return;
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
    sr_note_on(e,SR_CHOIR,tonic+24+scale[(chord+2u)%7u],48,2.5f);
  }
}
static void trigger(sr_engine *e){
  unsigned requested=0;
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
  response_tick(e);
  if(!e->loop_pos){
    if(e->learn_pending&&!e->freeze){e->learned=e->capture;e->capture.count=0;e->learn_pending=0;e->learn_origin=e->sequence_step;}
    if(e->rhythm_pending){e->rhythm_style=e->rhythm_pending-1u;e->rhythm_pending=0;}
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
  if(!e->loop_pos&&e->air_request&&!e->freeze&&e->sequence_step){
    requested=e->air_request;e->air_request=0;
    if(requested==1)e->evolve_next=1;
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
  if(requested){
    unsigned performed=requested==1?1:requested==2?transition_fill(e,s,1):chord_voice(e,s,1);
    e->air_result=(uint8_t)(performed?requested:128u+requested);
    if(performed)e->air_actions++;
  }
  if((s&15u)==0&&requested!=3)chord_phrase(e,s);
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
      e->timeline[j]=(sr_event){due,e->tick_length/2u,SR_DRUMS,e->scheduled[index].note==0?36:e->scheduled[index].note==1?38:42,e->scheduled[index].velocity,0};}
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
static inline __attribute__((always_inline)) unsigned slicer_lane(unsigned role)
{
    return role<=SR_BASS?role:role==SR_DRUMS?2u:3u;
}

static inline __attribute__((always_inline)) unsigned slicer_pattern(const sr_engine *e)
{
    static const uint16_t patterns[4][3]={
        {0x3377,0x7733,0x5777},{0x7fff,0x77ff,0x7f77},
        {0x3777,0x7377,0x7b77},{0x3355,0x5733,0x5557}
    };
    unsigned function=song_section(e);
    unsigned variation=(e->scene+function)%3u;
    return patterns[e->music_mode][variation]|0x1111u;
}

/* CPU0 is the sole writer of engine, MIDI, filter state and mix buses. The
 * secondary core sees only explicit oscillator inputs and its private output. */
typedef struct {
  sr_voice *voice;
  float cutoff, gain, step, left, right, send;
  float fm_ratio, fm_depth, fm_envelope, body_decay, breath, vocal;
  sr_wave wave;
  uint8_t track;
} voice_config;
typedef struct {
  voice_config active[SR_VOICES];
  float left[SR_TRACKS],right[SR_TRACKS],send[SR_TRACKS],slicer_target[3];
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
  for(unsigned lane=0;lane<3;lane++){
    unsigned bit=1u<<((slicer_beat+(lane==0?2u:0u))&15u);
    slicer_target[lane]=(gate_mask&(1u<<lane))&&!(pattern&bit)?
      (e->music_mode==SR_CINEMATIC?.60f:.18f):1.f;
  }
  unsigned smooth_slicer=gate_mask||e->slicer_gain[0]!=1.f||
    e->slicer_gain[1]!=1.f||e->slicer_gain[2]!=1.f;
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
  e->air_wet+=(e->air_target-e->air_wet)*((float)frames*.0004f);
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
    float send_level=track->send;
    if(e->air_track==SR_TRACKS)
      send_level+=(1.f-send_level)*e->air_wet*(e->air_depth*.01f);
    else if(e->air_track==t)
      send_level+=.5f*e->air_wet*(e->air_depth*.01f); /* shared FX bus: audible role focus */
    send[t]=level*send_level;
    if(!full&&source!=SR_ISOLATE_TRACK1+(unsigned)t)level=0;
    left[t]=level*(.5f-.31f*track->pan);
    right[t]=level*(.5f+.31f*track->pan);
  }
  for(int j=0;j<SR_VOICES;j++){
    sr_voice *v=&e->voices[j];
    if(!v->active)continue;
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
    config->breath=0.f;config->vocal=0.f;
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
    float air_focus=(e->air_track>=SR_TRACKS||e->air_track==v->track)?e->air_wet*e->air_depth*.01f:0;
    if(e->air_fx==3)config->cutoff*=1.f-.88f*air_focus;
    config->gain=calibrated_patch_gain(v->patch);
    config->left=left[v->track];
    config->right=right[v->track];
    if(e->air_mode==4){float swell=1.f+.12f*air_focus;config->left*=swell;config->right*=swell;}
    if(e->air_fx==4){
      float pulse=1.f-air_focus*(.22f+.22f*sine((e->frame_count%(SR_RATE/4u))*(4.f/SR_RATE)));
      config->left*=pulse;config->right*=pulse;
    }
    config->send=send[v->track]*.5f;
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
      const float style[SR_MUSIC_MODES]={.72f,1.f,.83f,.60f};
      modulation=depth*wind_motion[wind].depth*style[e->music_mode]*.0005776f*
                 sine(age*wind_motion[wind].rate);
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
  /* Track meters are display-only. Decay once per render chunk. */
  float meter_decay=1.f-(float)frames*.00097f;
  for(int t=0;t<SR_TRACKS;t++)e->tracks[t].meter*=meter_decay;
  for(size_t i=0;i<frames;i++){
    float l=0.f,r=0.f,mix=0.f,scope=0.f;
    if(smooth_slicer)for(unsigned lane=0;lane<3;lane++){
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
      if(cached){
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
      s+=config->breath*breath_noise;
      if(config->wave==SR_PLUCK)v->peak*=config->body_decay;
      if(resonance>0.f){
        s+=(v->filter-v->filter_tail)*resonance;
        v->filter_tail+=(v->filter-v->filter_tail)*config->cutoff;
      }
      v->filter+=(s-v->filter)*config->cutoff;
      float z=v->filter*v->env*config->gain+v->tail;
      if(smooth_slicer&&config->track<=SR_BASS)z*=e->slicer_gain[slicer_lane(config->track)];
      v->tail*=.98f;
      l+=z*config->left;
      r+=z*config->right;
      mix+=z*config->send;
      if(capture&&scope_track==config->track)scope+=z*(config->left+config->right);
      float raw=fabsf(z);
      float mag=raw*(config->left+config->right);
      sr_track *track=&e->tracks[config->track];
      if(mag>track->meter)track->meter=mag;
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
      dry[3*i]=l;dry[3*i+1]=r;dry[3*i+2]=0;
      continue;
    }
    float drums=drum_sample(e);
    if(smooth_slicer)drums*=e->slicer_gain[2];
    if(e->air_track>=SR_TRACKS||e->air_track==SR_DRUMS){
      float focus=e->air_wet*e->air_depth*.01f;
      if(e->air_fx==3){e->drum_air_lp+=(drums-e->drum_air_lp)*.12f;drums+=(e->drum_air_lp-drums)*focus;}
      if(e->air_fx==4)drums*=1.f-.35f*focus*e->kick_env;
      if(e->air_mode==4)drums*=1.f+.08f*focus;
    }
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
    dry[3*i]=l; dry[3*i+1]=r; dry[3*i+2]=mix;
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
        if(!v->gate||(v->remain&&v->remain<=frames))continue;
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

#if defined(SR_DEVICE) && !defined(PLAT_HOST)
__attribute__((section(".ram_text"), noinline))
#endif
static void apply_fx(sr_engine *e,float *dry,size_t frames)
{
  unsigned mask=e->fx_mask;
  int audible=e->isolate==SR_ISOLATE_MIX||e->isolate==SR_ISOLATE_FX;
  float depth=(e->sound_locks&SR_SOUND_FX)?1.f:.7f+.3f*e->current_energy;
  float reverb=e->reverb_amount*depth;
  float air=e->air_wet*(e->air_depth*.01f);
  if(e->air_fx==0||e->air_fx==2)reverb+=(.8f-reverb)*air;
  float delay=e->delay_amount*depth;
  if(e->air_fx==1||e->air_fx==2)delay+=(.7f-delay)*air;
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
    float l=dry[3*i],r=dry[3*i+1];
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
 float dry[64*3];
 e->fx_ticks=0;
 while(frames){
  if(e->external_audio){
   e->external_audio=0;
   for(unsigned v=0;v<SR_VOICES;v++)if(e->voices[v].active&&e->voices[v].external)e->external_audio=1;
  }
  if(!e->external_audio&&!e->running&&!e->stop_frames&&!e->nature_audition&&!e->preview_frames){
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
        float velocity=e->scheduled[i].velocity*(1.f/127.f);
        if(e->timeline_enabled){unsigned j=timeline_slot(e);
          e->timeline[j].frame=e->frame_count;e->timeline[j].duration=e->tick_length/2u;
          e->timeline[j].role=SR_DRUMS;e->timeline[j].note=e->scheduled[i].note==0?36:e->scheduled[i].note==1?38:42;
          e->timeline[j].velocity=e->scheduled[i].velocity;}
        if(e->scheduled[i].note==0)e->kick_env=velocity;
        else if(e->scheduled[i].note==1)e->hand_env=velocity;else e->shaker_env=velocity;
        unsigned part=e->scheduled[i].note;
        if(part<3u){
          unsigned bit=1u<<part;
          e->drum_steps[e->drum_playhead]|=bit|(velocity>=.75f?bit<<3u:0u);
        }
        e->tracks[role].events++;
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
  render_block(e,dry,n);
  unsigned air_active=e->air_depth&&e->air_wet>.00001f;
  unsigned reverb=e->reverb_amount>0||e->reverb_wet>.00001f||(air_active&&e->air_fx!=1);
  unsigned delay=e->delay_amount>0||e->delay_wet>.00001f||(air_active&&e->air_fx!=0);
  unsigned fx_mask=(reverb?1u:0u)|((reverb||delay)?2u:0u);
  if(e->fx_mask&~fx_mask){
    if(!(fx_mask&1)){e->rev_history=0;memset(e->rev_pos,0,sizeof(e->rev_pos));e->reverb_wet=0;}
    if(!(fx_mask&2)){e->delay_history=0;e->delay_pos=0;e->delay_wet=0;}
  }
  e->fx_mask=(uint8_t)fx_mask;
  if(fx_mask&&!e->frequency_lock&&!audition&&!e->preview_frames){
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
