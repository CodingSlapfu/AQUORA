/* SPDX-License-Identifier: MPL-2.0
 * Original Sound Healing application. Audio owns engine state; UI produces a bounded queue. */
#include "serenity.h"
#include "fastmath.h"
#include "air_sensor.h"
static sr_engine engine PLAT_POOL;
static int16_t pcm[APP_BLOCK*2] PLAT_POOL;
typedef struct {int kind,value; unsigned track;} command;
static command queue[64];
static volatile unsigned qread,qwrite,state_revision;
static unsigned displayed_revision;
static unsigned dropped,page,selected,group,oldkeys,oldbuttons,lastdraw,dirty=1,debug;
static float output_gain;
typedef struct { unsigned max, count, sum, bins[64]; } timing;
typedef struct { unsigned mean, p95, p99, peak; } timing_summary;
static timing audio_timing, fx_timing, callback_timing;
static timing_summary audio_stats, fx_stats, callback_stats;
static unsigned window_ms;
static volatile unsigned requested_play, stop_generation, physical_keys;
static unsigned applied_stop;
static unsigned saved_status, graphics_us, blocked_keys, voice_slot;
static unsigned mix_tab,mix_reset,mix_bus,mix_focus,song_tab,song_nonce,song_arm;
static unsigned drum_tab, setup_tab, display_subtab, sound_picker, fullscreen, last_control_ms;
static unsigned ui_theme, ui_font_size=1, ui_background, preferences_dirty, preferences_changed;
enum { PAGE_PLAY, PAGE_SOUND, PAGE_EVOLVE, PAGE_MIX, PAGE_DRUMS, PAGE_SETUP, PAGE_AIR };
static unsigned air_on, air_hold, air_poll;
static aq_air_sensor air_sensor;
static unsigned air_packets, air_packet_window, air_packet_rate, air_last_packet_ms, air_packet_total;
static unsigned air_window_start;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
static unsigned air_sent;
#endif
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
extern unsigned plat_air_sample(unsigned *packets);
extern void plat_air_enable(int on);
#endif
static unsigned quick_item;
static const char *quick_names[]={"Drums","Mixer","Motion","Groove","AIR","Banks","Performance","Play Along","FX / Nature",
 "Patch Shape","Frequency","Binaural","Sacred Bass","New Song","Display / LEDs","About","Source Isolate","Nature Style","Evolution","Master FX","Track Routing"};
static const uint8_t sound_groups[]={0,6,1,2,3,5,7,12,11,13,14};

static volatile unsigned intro_active;
static unsigned intro_start, intro_samples, intro_master;
static volatile unsigned intro_master_moved;
static unsigned intro_inputs_seen;
static float intro_gain;
static float intro_sine_table[256], intro_fm_phase[6];
/* Small original 3-voice FM startup signature. Fixed table / phase updates;
 * never calls libc sin/exp or the full effects bus in the audio interrupt. */
static inline float intro_sine(float phase)
{
    float p=phase*256.f;
    int index=(int)p;
    float f=p-(float)index;
    float a=intro_sine_table[index&255];
    return a+(intro_sine_table[(index+1)&255]-a)*f;
}
static inline float intro_note(unsigned elapsed)
{
    float t=elapsed*(1.f/SR_RATE);
    static const float increment[6]={220.f/SR_RATE,440.f/SR_RATE,392.f/SR_RATE,
                                      784.f/SR_RATE,659.f/SR_RATE,988.5f/SR_RATE};
    for(unsigned k=0;k<6;k++){
        intro_fm_phase[k]+=increment[k];
        if(intro_fm_phase[k]>=1.f)intro_fm_phase[k]-=1.f;
    }
    float low=fm_clampf((t-.12f)*1.7f,0,1)*fm_clampf((8.f-t)*.38f,0,1);
    float halo=fm_clampf((t-1.1f)*2.1f,0,1)*fm_clampf((7.7f-t)*.42f,0,1);
    float crystal=fm_clampf((t-2.7f)*2.0f,0,1)*fm_clampf((7.8f-t)*.34f,0,1);
    float warm=intro_sine(intro_fm_phase[0]+.10f*intro_sine(intro_fm_phase[1]));
    float bloom=intro_sine(intro_fm_phase[2]+.29f*intro_sine(intro_fm_phase[3]));
    float glass=intro_sine(intro_fm_phase[4]+.31f*intro_sine(intro_fm_phase[5]));
    return (warm*440.f*low+bloom*460.f*halo+glass*245.f*crystal);
}
static uint32_t evolve_events[SR_TRACKS], evolve_step;
static uint8_t evolve_history[64];
static unsigned led_test, led_step;
static int battery_level=-1;
static unsigned battery_poll_ms, battery_overlay_ms, battery_fade, battery_dirty;
#ifndef PLAT_HOST
extern void plat_aquora_led_test_log(unsigned slot);
#endif
static unsigned lighting, fps_auto, frames, missed, shown_fps10, shown_missed;
static unsigned last_xruns, xrun_delta, frame_us;
static unsigned knob_focus, knob_until, redraw_ui_ms, shown_step;
enum { GROUP_PERC=4, GROUP_HIT=8, GROUP_VISUAL=9, GROUP_BANKS=10, GROUP_ISOLATE=11, GROUP_KEYS=12, GROUP_COUNT=15 };
static const uint8_t sound_controls[GROUP_COUNT][4]={
 {2,13,24,29},{21,5,54,78},{4,22,48,45},{8,7,23,6},{16,17,10,9},
 {30,31,36,22},{32,33,34,35},{13,24,29,28},
 {37,17,10,9},{38,56,0,0},{39,40,41,38},{43,4,22,31},{47,63,73,74},{55,14,48,45},{79,85,86,54}
};
static const uint8_t page_controls[4][4]={
 {1,18,22,5},{0,0,0,0},{19,20,25,46},{13,24,29,49}
};
#include "arrangement.c"
typedef struct {
    uint32_t version;
    sr_motion motion[2];
    uint8_t perfect_key, lighting, fps_auto;
} aq_settings_legacy;
typedef struct {
    uint32_t version;
    sr_motion motion[2];
    uint8_t perfect_key, lighting, fps_auto;
    uint8_t visual, theme, font_size, background;
} aq_settings;

/* Never write flash in the callback. All preference edits are coalesced and
 * committed only after the transport has stopped. The setting object is
 * separate from arrangement banks and preserves the pre-0.6.2 schema. */
static int save_preferences(void)
{
    aq_settings settings={0};
    unsigned flags=plat_lock();
    settings.version=0x41513033u;
    settings.motion[0]=engine.motion[0];settings.motion[1]=engine.motion[1];
    settings.perfect_key=engine.perfect_key;
    settings.lighting=lighting;settings.fps_auto=fps_auto;
    settings.visual=visual_mode;settings.theme=ui_theme;
    settings.font_size=ui_font_size;settings.background=ui_background;
    plat_unlock(flags);
    int result=plat_store_save(OBJ_SETTINGS,&settings,sizeof(settings));
    if(!result)preferences_dirty=0;
    return result;
}
static void preferences_changed_now(void)
{
    preferences_dirty=1;
    preferences_changed=plat_ms();
    dirty=1;
}

static void measure(timing *t,unsigned us)
{
    if(us>t->max)t->max=us;
    t->sum+=us;
    t->count++;
    t->bins[us/100<63?us/100:63]++;
}

static timing_summary summarize(const timing *t)
{
    timing_summary result={0,0,0,t->max};
    if(!t->count)return result;
    result.mean=t->sum/t->count;
    unsigned cumulative=0;
    for(unsigned i=0;i<64;i++){
      cumulative+=t->bins[i];
      if(!result.p95&&cumulative*100>=t->count*95)result.p95=(i+1)*100;
      if(!result.p99&&cumulative*100>=t->count*99)result.p99=(i+1)*100;
    }
    return result;
}
#include "visual_worker.c"
static int mix_visible(unsigned channel)
{
 if(mix_tab!=3)return 1;
 if(channel<SR_TRACKS){
   const sr_track *track=&engine.tracks[channel];
   return !track->mute&&track->volume>0&&(channel!=SR_DRUMS||engine.drums_enabled);
 }
 switch(channel-SR_TRACKS){
 case SR_BUS_NATURE:return engine.bed_level>0;
 case SR_BUS_SACRED:return engine.sacred_mode&&engine.sacred_level>0;
 case SR_BUS_REFERENCE:return engine.tone_level>0;
 case SR_BUS_BINAURAL:return engine.binaural_enabled&&engine.binaural_level>0;
 default:return engine.reverb_amount>0||engine.delay_amount>0||engine.air_wet>0;
 }
}
static void mix_select(int delta)
{
 int next=(int)mix_focus;
 int direction=delta>0?1:-1;
 unsigned moves=delta>0?(unsigned)delta:(unsigned)-delta;
 while(moves){
   next+=direction;
   if(next<0||next>=(mix_tab==5?SR_TRACKS:SR_TRACKS+SR_BUS_COUNT))break;
   if(!mix_visible((unsigned)next))continue;
   mix_focus=(unsigned)next;
   moves--;
 }
 if(mix_focus<SR_TRACKS)selected=mix_focus;
 else mix_bus=mix_focus-SR_TRACKS;
}
static unsigned control_for(unsigned knob)
{
 if(debug)return knob==0?(debug==3?42:debug==1?57:0):0;
 if(page==PAGE_AIR){static const uint8_t controls[]={66,67,68,69};return controls[knob];}
 if(page==PAGE_DRUMS){
   static const uint8_t controls[][4]={{9,10,37,17},{16,13,24,29},{49,28,62,0}};
   return controls[drum_tab][knob];
 }
 if(page==PAGE_SETUP){
   if(setup_tab==0&&display_subtab){
     static const uint8_t appearance[]={90,91,92,38};
     return appearance[knob];
   }
   if(setup_tab==3)return knob==0?71:knob==1?72:0;
   return setup_tab==2?0:(!setup_tab&&knob>=2?80u+knob-2u:sound_controls[setup_tab?GROUP_BANKS:GROUP_VISUAL][knob]);
 }
 if(page==PAGE_SOUND&&sound_picker)return 0;
 if(page==PAGE_EVOLVE&&song_tab){
   static const uint8_t ids[][4]={{58,59,60,61},{64,65,46,20},{82,19,83,84}};
   return ids[song_tab-1][knob];
 }
 if(page==PAGE_MIX&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))){
   static const uint8_t controls[SR_BUS_COUNT][4]={{5,21,54,79},{30,31,36,0},
      {22,4,48,45},{23,6,7,8},{11,12,50,51}};
   return controls[mix_bus][knob];
 }
 if(page==PAGE_MIX&&(mix_tab==1||mix_tab==2)){
   static const uint8_t master_controls[]={11,12,50,51};
   static const uint8_t reset_controls[]={52,0,0,0};
   return mix_tab==1?master_controls[knob]:reset_controls[knob];
 }
 if(page==PAGE_SOUND&&group==0&&selected<SR_DRUMS){
   static const uint8_t shape[]={2,75,76,77};
   return shape[knob];
 }
 return page==1?sound_controls[group][knob]:page_controls[page][knob];
}
#include "quick_find.h"
static int switched(int state,int delta)
{
 return delta==0?!state:delta>0;
}
static void drums_enable(int enabled)
{
 sr_set_drums(&engine,enabled);
 if(enabled){
   engine.tracks[SR_DRUMS].mute=0;
   if(engine.solo<SR_TRACKS&&engine.solo!=SR_DRUMS)engine.solo=SR_TRACKS;
   if(!engine.drum_density)engine.drum_density=2;
   if(engine.drum_level<.40f)engine.drum_level=.66f;
   if(engine.tracks[SR_DRUMS].volume<.50f)engine.tracks[SR_DRUMS].volume=1.f;
 }
}
static int bounded(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static float adjust(float x,int d,float step,float hi){return fm_clampf(x+d*step,0,hi);}
static void send(int kind,int value){unsigned next=(qwrite+1)&63;if(next==qread){dropped++;return;}queue[qwrite]=(command){kind,value,page==PAGE_DRUMS&&kind!=3?SR_DRUMS:selected};__asm__ volatile("":::"memory");qwrite=next;}
static void consume(void){
 unsigned stop=stop_generation;
 unsigned play=requested_play;
 unsigned interrupted=stop!=applied_stop;
 if(interrupted){applied_stop=stop;sr_stop(&engine);}
 unsigned count=0;while(qread!=qwrite&&count++<64){command c=queue[qread];__asm__ volatile("":::"memory");qread=(qread+1)&63;sr_track *t=&engine.tracks[c.track];int d=c.value;
 state_revision++;
 switch(c.kind){

 case 1:{
   unsigned next=bounded(engine.scene+d,0,SR_SCENES-1);
   if(next!=engine.scene)sr_set_scene(&engine,next);
   break;
 }
 case 2:case 16:{
   unsigned role=c.kind==16?SR_DRUMS:c.track;
   engine.sound_locks|=1u<<role;
   if(role==SR_DRUMS)sr_set_drum_kit(&engine,bounded(engine.drum_kit+d,0,SR_DRUM_KITS-1));
   else sr_set_track_patch(&engine,role,bounded(t->patch+d,0,SR_PATCHES-1));
   sr_preview_sound(&engine,role);
   break;
 }
 case 3:if(!interrupted&&play&&((physical_keys&(1u<<d))||(engine.play_grid&&engine.play_strength&&!engine.keyboard_mode)))sr_key_down(&engine,c.track,(unsigned)d);break;
 case 4:sr_set_frequency(&engine,bounded(engine.frequency+d,0,SR_FREQUENCIES-1));break;
 case 5:engine.nature_level_manual=1;engine.bed_level=adjust(engine.bed_level,d,.02f,1);break;
 case 6:sr_set_binaural(&engine,switched(engine.binaural_enabled,d));break;
 case 7:sr_set_binaural_rate(&engine,fm_clampf(engine.binaural_rate+d*.5f,.5f,40));break;
 case 8:sr_set_binaural_carrier(&engine,fm_clampf(engine.binaural_carrier+d*5.f,80,1000));break;
 case 9:drums_enable(switched(engine.drums_enabled,d));break;
 case 10:sr_set_drum_density(&engine,bounded(engine.drum_density+d,0,SR_DENSITY_MAX));break;
 case 11:engine.sound_locks|=SR_SOUND_FX;engine.reverb_amount=adjust(engine.reverb_amount,d,.02f,.8f);break;
 case 12:engine.sound_locks|=SR_SOUND_FX;engine.delay_amount=adjust(engine.delay_amount,d,.02f,.7f);break;
 case 13:t->volume=adjust(t->volume,d,.03f,1);break;
 case 14:sr_set_tuning_432(&engine,switched(engine.tuning_432,d));break;
 case 15:t->mute=!t->mute;break;
 case 17:sr_set_drum_level(&engine,engine.drum_level+d*.02f);break;
 case 18:engine.energy=adjust(engine.energy,d,.05f,1);break;
 case 82:{
   static const float levels[]={0,.25f,.55f,.85f};
   int level=engine.variation==0?0:engine.variation<.4f?1:engine.variation<.7f?2:3;
   engine.variation=levels[bounded(level+d,0,3)];
   break;
 }
 case 83:engine.evolve_pace=bounded(engine.evolve_pace+d,0,3);break;
 case 84:if(d>0&&!engine.freeze)engine.evolve_next=1;break;
 case 19:engine.variation=adjust(engine.variation,d,.05f,1);break;
 case 20:engine.freeze=switched(engine.freeze,d);break;
 case 21:engine.bed_type=bounded(engine.bed_type+d,0,SR_BEDS-1);engine.sound_locks|=SR_SOUND_NATURE;break;
 case 22:engine.tone_level=adjust(engine.tone_level,d,.005f,.10f);break;
 case 23:engine.binaural_level=adjust(engine.binaural_level,d,.005f,.10f);break;
 case 24:t->pan=fm_clampf(t->pan+d*.05f,-1,1);break;
 case 25:engine.play_queued=0;engine.tick_length=bounded((int)engine.tick_length-d*80,5512,22050);break;
 case 27:sr_set_perfect_key(&engine,!engine.perfect_key);break;
 case 28:if(d<0)engine.solo=SR_TRACKS;else engine.solo=d>0?c.track:engine.solo==c.track?SR_TRACKS:c.track;break;
 case 29:t->send=adjust(t->send,d,.05f,1);break;
 case 30:engine.sacred_level=adjust(engine.sacred_level,d,.01f,.20f);break;
 case 31:engine.sacred_mode=bounded(engine.sacred_mode+d,0,2);break;
 case 32:if(c.track<=SR_BASS)engine.motion[c.track].glide_mode=bounded(engine.motion[c.track].glide_mode+d,0,2);break;
 case 33:if(c.track<=SR_BASS)engine.motion[c.track].glide_ms=bounded(engine.motion[c.track].glide_ms+10*d,10,800);break;
 case 34:if(c.track<=SR_BASS)engine.motion[c.track].vibrato_rate=bounded(engine.motion[c.track].vibrato_rate+d,5,80);break;
 case 36:engine.sacred_harmonics=adjust(engine.sacred_harmonics,d,.025f,1);break;
 case 37:engine.drum_hit=bounded(engine.drum_hit+d,0,SR_HIT_MAX);break;
 case 46:{
   unsigned length=engine.pending_length?engine.pending_length:engine.loop_length;
   sr_set_loop_length(&engine,d>0?(length<64?length*2:64):(length>16?length/2:16));
   break;
 }
 case 47:
   engine.play_queued=0;
   engine.keyboard_mode=switched(engine.keyboard_mode,d);
   if(!engine.keyboard_mode)engine.pending_key=engine.pending_physical_key=0;
   sr_set_perfect_key(&engine,1);
   break;
 case 49:t->mute=switched(t->mute,d);break;
 case 50:engine.auto_gain=switched(engine.auto_gain,d);break;
 case 51:engine.auto_level=switched(engine.auto_level,d);break;
 case 55:engine.lock_textures=switched(engine.lock_textures,d);break;
 case 58:case 59:
   if(!engine.running)engine.nature_audition=0;
   if(d>0)sr_generate_song(&engine,plat_ticks24()^++song_nonce,c.kind==59);
   break;
 case 60:if(d>0)engine.sound_locks|=1u<<c.track;else engine.sound_locks&=~(1u<<c.track);break;
 case 64:engine.groove_mode=bounded(engine.groove_mode+d,0,2);break;
 case 65:engine.groove_strength=bounded(engine.groove_strength+d*5,0,100);break;
 case 67:engine.air_track=bounded(engine.air_track+d,0,SR_TRACKS);break;
 case 70:engine.air_target=bounded(d,0,100)*.01f;break;
 case 75:case 76:case 77:
   if(c.track<SR_DRUMS){
     int8_t *edit=c.kind==75?&t->tone:c.kind==76?&t->attack:&t->release;
     *edit=bounded(*edit+d,-7,7);
   }
   break;
 case 73:engine.play_strength=bounded(engine.play_strength+d*5,0,100);engine.play_queued=0;break;
 case 74:engine.play_feel=switched(engine.play_feel,d);engine.play_queued=0;break;
 case 63:engine.play_grid=bounded(engine.play_grid+d,0,4);engine.play_queued=0;break;
 case 62:engine.drum_presence=bounded(engine.drum_presence+d,0,100);break;
 case 61:if(d>0)engine.sound_locks|=SR_SOUND_NATURE|SR_SOUND_FX;else engine.sound_locks&=~(engine.bed_type?(SR_SOUND_NATURE|SR_SOUND_FX):SR_SOUND_FX);break;
 case 78:
   if(d>0){engine.sound_locks&=~SR_SOUND_NATURE;sr_select_nature(&engine,engine.rng);}
   else{engine.bed_type=0;engine.sound_locks|=SR_SOUND_NATURE;}
   break;
 case 79:engine.nature_style=bounded(engine.nature_style+d,0,3);sr_select_nature(&engine,engine.rng);break;
 case 85:engine.nature_activity=bounded(engine.nature_activity+d,0,4);break;
 case 86:engine.nature_distance=bounded(engine.nature_distance+d,0,4);break;
 case 54:
   if(d>0&&!engine.pending_lock&&engine.bed_type){engine.preview_frames=0;engine.nature_audition=SR_RATE*4u;}
   else if(engine.nature_audition>1102u)engine.nature_audition=1102u;
   break;
 case 53:
   engine.solo=SR_TRACKS;
   engine.reverb_amount=.32f;engine.delay_amount=.18f;
   engine.auto_gain=engine.auto_level=0;
   for(unsigned i=0;i<SR_TRACKS;i++){
     sr_track *track=&engine.tracks[i];
     track->volume=i==SR_BASS?.88f:i==SR_DRUMS?1.f:.68f;
     track->pan=((int)(i%4)-1)*.13f;track->send=1.f;track->mute=0;
   }
   break;
 case 48:sr_set_frequency_lock(&engine,switched(engine.pending_lock,d));break;
 case 45:sr_set_frequency_auto(&engine,switched(engine.frequency_auto,d));break;
 case 44:engine.isolate=SR_ISOLATE_MIX;engine.nature_audition=0;break;
 case 43:engine.isolate=bounded(engine.isolate+d,SR_ISOLATE_MIX,SR_ISOLATE_FX);break;
 case 35:if(c.track<=SR_BASS)engine.motion[c.track].vibrato_depth=bounded(engine.motion[c.track].vibrato_depth+d,0,50);break;
 }
 }
 engine.play_released|=engine.play_queued&~physical_keys;
 for(unsigned i=0;i<SR_VOICES;i++){
   sr_voice *v=&engine.voices[i];
   if(v->gate&&v->key&&!(physical_keys&(1u<<(v->key-1)))){engine.note_offs+=v->gate!=0;v->gate=0;}
 }
 if(play&&!engine.running&&!engine.restart){sr_start(&engine);state_revision++;}
 if(!play&&engine.running){sr_stop(&engine);state_revision++;}
}
void app_init(void)
{
    song_nonce=plat_ticks24();
#ifdef PLAT_HOST
    if(getenv("AQUORA_TEST_SEED"))song_nonce=7301u;
#endif
    sr_init(&engine,song_nonce^7301u);
    sr_generate_song(&engine,song_nonce,0);
    engine.clock_ticks=plat_ticks24;
    aq_settings settings={0};
    int prefs_size=plat_store_size(OBJ_SETTINGS);
    if((prefs_size==(int)sizeof(aq_settings_legacy)||prefs_size==(int)sizeof(settings))&&
       plat_store_load(OBJ_SETTINGS,&settings,sizeof(settings))==prefs_size&&
       (settings.version==0x41513032u||
        (settings.version==0x41513033u&&prefs_size==(int)sizeof(settings)))){
      for(unsigned i=0;i<2;i++){
        sr_motion m=settings.motion[i];
        if(m.glide_mode<=2&&m.glide_ms>=10&&m.glide_ms<=800&&
           m.vibrato_rate>=5&&m.vibrato_rate<=80&&m.vibrato_depth<=50)engine.motion[i]=m;
      }
      engine.perfect_key=!!settings.perfect_key;
      lighting=settings.lighting<=3?settings.lighting:0;
      fps_auto=!!settings.fps_auto;
      if(settings.version==0x41513033u){
        ui_theme=settings.theme<=4?settings.theme:0;
        ui_font_size=settings.font_size<=2?settings.font_size:1;
        ui_background=settings.background<=1?settings.background:0;
      }
    }
    arrangement_init();
    /* arrangement_init resets its bank cursor and visual slot. Restore the
     * saved global visualizer before first intro frame, independent of banks. */
    if(settings.version==0x41513033u&&prefs_size==(int)sizeof(settings))
      visual_mode=settings.visual<=1?settings.visual:0;
    visual_init();
    plat_cpu2_set(1);
    output_gain=0;
    intro_active=intro_samples=intro_master_moved=intro_inputs_seen=0;
    memset(intro_fm_phase,0,sizeof intro_fm_phase);
    for(unsigned i=0;i<256;i++)
      intro_sine_table[i]=fm_sinf(i*(6.28318530718f/256.f));
    intro_master=~0u;
    intro_gain=0;
    evolve_step=0;
    memset(evolve_events,0,sizeof evolve_events);
    memset(evolve_history,0,sizeof evolve_history);
    qread=qwrite=0;
    requested_play=stop_generation=applied_stop=physical_keys=0;
    page=selected=group=debug=display_subtab=0;
    preferences_dirty=preferences_changed=0;
    air_packet_window=air_packet_rate=air_packet_total=air_last_packet_ms=0;
    air_window_start=plat_ms();
    oldkeys=oldbuttons=blocked_keys=voice_slot=knob_until=redraw_ui_ms=0;
    dirty=1;
}
void app_panic(void)
{
    requested_play=0;
    engine.running=0;
    engine.stop_frames=0;
    engine.restart=0;
    engine.master=0;
    intro_active=0;
    intro_gain=0;
    output_gain=0;
}
void app_render(int32_t *lr,uint32_t n,uint32_t master)
{
    unsigned begin=plat_ticks24();
    consume();
    if(n>APP_BLOCK)n=APP_BLOCK;
    sr_render(&engine,pcm,n);
    float target=(float)master*(1.f/4096.f);
    if(!master)output_gain=0;
    if(intro_active){
      /* Sample startup ADC settling before taking a master-knob skip edge.
       * Ordinary 12-bit ADC noise must not abort the introduction. */
      if(intro_master==~0u||intro_samples<SR_RATE/20u)intro_master=master;
      else if((master>intro_master?master-intro_master:intro_master-master)>160u)
        intro_master_moved=1;
    }
    unsigned intro=intro_samples;
    unsigned playing_intro=intro_active;
    unsigned chime=intro<SR_RATE*8u&&(playing_intro||intro_gain>.0001f);
    for(unsigned i=0;i<n;i++){
      float bell=0;
      if(chime){
        intro_gain+=((float)playing_intro-intro_gain)*.003f;
        /* Master zero remains real silence. The start of the synth timeline
         * is the first ready audio callback, not an early boot wall clock. */
        if(master)bell=intro_note(intro+i)*intro_gain;
      }
      output_gain+=(target-output_gain)*.002f;
      lr[2*i]=(int32_t)((pcm[2*i]+bell)*256.f*output_gain);
      lr[2*i+1]=(int32_t)((pcm[2*i+1]+bell)*256.f*output_gain);
    }
    if(chime)intro_samples+=n;
    if(!master)intro_gain=0;
    unsigned total=plat_ticks24()-begin;
    unsigned fx=engine.fx_ticks<total?engine.fx_ticks:total;
    measure(&audio_timing,(total-fx)/24);
    measure(&fx_timing,fx/24);
    measure(&callback_timing,total/24);
}
void app_say(const char*a,const char*b){(void)a;(void)b;dirty=1;}
void app_input_only(void){unsigned b=plat_buttons(),k=plat_keys(),press=b&~oldbuttons;oldbuttons=b;
 int enc[7]={plat_enc(EN_ALGO),plat_enc(EN_SELECT),plat_enc(EN_PRESET),
             plat_enc(EN_K1),plat_enc(EN_K1+1),plat_enc(EN_K1+2),plat_enc(EN_K1+3)};
 int control=press!=0;
 for(unsigned i=0;i<7;i++)control|=enc[i]!=0;
 if(intro_active){
   unsigned now=plat_ms();
   /* First sample after power-on is a baseline, not a user input edge. */
   if(!intro_inputs_seen){intro_inputs_seen=1;oldkeys=k;return;}
   if(control||(k&~oldkeys)||intro_master_moved||now-intro_start>=8000u){
     intro_active=0;
     last_control_ms=now;
     blocked_keys|=k;
     dirty=1;
   }
   oldkeys=k;
   return;
 }
 if(control){
   last_control_ms=plat_ms();
   if(fullscreen){
     fullscreen=0;dirty=1;
     press=0;
     for(unsigned i=0;i<7;i++)enc[i]=0;
   }
 }
 if((press&(1u<<B_EDIT))&&(b&(1u<<B_HOME))){
   page=PAGE_SETUP;setup_tab=3;debug=0;bank_confirm=0;dirty=1;
   press&=~((1u<<B_EDIT)|(1u<<B_HOME));
 }
 if(debug==3 && (press&(1u<<B_SEQ))){
   unsigned flags=plat_lock();
   requested_play=0;
   stop_generation++;
   blocked_keys|=k;
   plat_unlock(flags);
 }
 if(press&(1u<<B_PLAY)){
   unsigned flags=plat_lock();
   blocked_keys|=k;
   bank_confirm=0;
   song_arm=0;
   requested_play=!requested_play;
   if(!requested_play)stop_generation++;
   plat_unlock(flags);
 }
 if(press&(1u<<B_HOME)){page=0;debug=0;bank_confirm=song_arm=0;send(44,0);}
 if((press&(1u<<B_SEL))&&!(page==PAGE_MIX&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))))send(15,0);
 if(press&(1u<<B_ARP)){bank_confirm=0;page=PAGE_DRUMS;drum_tab=0;debug=0;}
 if(press&(1u<<B_GLO))debug=(debug+1)%4;
 if(press&(1u<<B_REC))plat_cpu2_set(0);
 if(press&(1u<<B_ENV))send(14,0);
 if(press&(1u<<B_FX)){
   if(page==PAGE_MIX&&!debug){
     mix_tab=mix_tab==5?0:5;
     if(mix_focus>=SR_TRACKS)mix_focus=selected;
     dirty=1;
   }else send(6,0);
 }
 if(debug!=3&&(press&(1u<<B_SEQ)))send(20,0);
 if(press&(1u<<B_EDIT)){
   if(!debug&&page==0)send(47,0);
   if(!debug&&page==2)song_tab=(song_tab+1)%4;
   if(page==PAGE_SOUND)sound_picker=!sound_picker;
   if(page==PAGE_DRUMS)drum_tab=(drum_tab+1)%3;
   if(page==PAGE_SETUP)setup_tab=(setup_tab+1)%4;
   if(page==3){/* Preserve legacy EDIT cycle; FX enters the dedicated focus strip. */
     mix_tab=mix_tab>=4?0:mix_tab+1;mix_reset=0;}
   bank_confirm=song_arm=0;bank_message=BANK_IDLE;
 }
 /* OCT- opens the arrangement page. OCT+ jumps between percussion and HIT. */
 if((press&(1u<<B_OCTDN))&&!(b&(1u<<B_OCTUP))){page=PAGE_SETUP;setup_tab=1;debug=0;}
 if((press&(1u<<B_OCTUP))&&!(b&(1u<<B_OCTDN))){bank_confirm=0;drum_tab=page==PAGE_DRUMS?(drum_tab+1)%3:0;page=PAGE_DRUMS;debug=0;}
 if((press&(1u<<B_LFO))&&!(page==PAGE_MIX&&!debug&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))))send(debug?27:28,0);
 if((press&(1u<<B_SAVE))&&page==3&&mix_tab==2&&!debug){
    if(mix_reset)send(53,0);
    mix_reset=0;
 }else if((press&(1u<<B_SAVE))&&page==PAGE_EVOLVE&&song_tab==1&&!debug){
    if(song_arm&&!requested_play&&!engine.running&&!engine.stop_frames)send(song_arm,1);
    song_arm=0;
 }else if((press&(1u<<B_SAVE))&&page==PAGE_SETUP&&setup_tab==1&&!debug){
    arrangement_execute();
    blocked_keys|=k;
 }else if((press&(1u<<B_SAVE))&&!requested_play&&!engine.running&&!engine.stop_frames){
    saved_status=save_preferences()==0?1:2;
 }
 blocked_keys&=k;
 physical_keys=k&~blocked_keys;
 for(unsigned i=0;i<NKEYS;i++){
   if(!(press&(1u<<B_PLAY))&&requested_play&&((physical_keys&~oldkeys)&(1u<<i)))send(3,i);
 }
 oldkeys=k;
 int d=enc[0];
 if(d&&!debug){page=bounded((int)page+d,PAGE_PLAY,PAGE_AIR);sound_picker=0;mix_reset=0;bank_confirm=song_arm=0;knob_until=0;dirty=1;}
 d=enc[1];
 if(d&&!debug){
   if(page==PAGE_SOUND&&sound_picker){
     int index=0;
     for(unsigned i=0;i<sizeof(sound_groups);i++)if(sound_groups[i]==group)index=(int)i;
     group=sound_groups[bounded(index+d,0,sizeof(sound_groups)-1)];
   }else if(page==PAGE_MIX&&mix_tab==4)mix_bus=bounded((int)mix_bus+d,0,SR_BUS_COUNT-1);
   else if(page==PAGE_MIX&&(mix_tab==0||mix_tab==3||mix_tab==5))mix_select(d);
   else if(page==PAGE_SETUP&&setup_tab==0)display_subtab=bounded((int)display_subtab+d,0,1);
   else if(page!=PAGE_DRUMS)selected=bounded((int)selected+d,0,7);
   knob_until=0;dirty=1;
 }
 d=enc[2];
 if(d&&!debug&&page!=PAGE_SETUP&&page!=PAGE_AIR&&!(page==PAGE_SOUND&&sound_picker)){send(page==0?1:page==PAGE_DRUMS?16:2,d>0?1:-1);dirty=1;}
 for(unsigned i=0;i<4;i++){
    d=enc[3+i];
    unsigned control=control_for(i);
    if(!d||!control)continue;
    if(control==71)quick_item=bounded((int)quick_item+d,0,QUICK_COUNT-1u);
    else if(control==72&&d>0){
      if(quick_item>=QUICK_CATEGORIES)open_parameter(quick_item-QUICK_CATEGORIES);
      else switch(quick_item){
      case 0:page=PAGE_DRUMS;drum_tab=0;break;
      case 1:page=PAGE_MIX;mix_tab=0;break;
      case 2:page=PAGE_SOUND;group=6;sound_picker=0;break;
      case 3:page=PAGE_EVOLVE;song_tab=2;break;
      case 4:page=PAGE_AIR;break;
      case 5:page=PAGE_SETUP;setup_tab=1;break;
      case 6:debug=1;break;
      case 7:page=PAGE_SOUND;group=GROUP_KEYS;sound_picker=0;break;
      case 8:page=PAGE_SOUND;group=1;sound_picker=0;break;
      case 9:page=PAGE_SOUND;group=0;sound_picker=0;break;
      case 10:page=PAGE_SOUND;group=2;sound_picker=0;break;
      case 11:page=PAGE_SOUND;group=3;sound_picker=0;break;
      case 12:page=PAGE_SOUND;group=5;sound_picker=0;break;
      case 13:page=PAGE_EVOLVE;song_tab=1;break;
      case 14:setup_tab=0;break;
      case 15:setup_tab=2;break;
      case 16:page=PAGE_SOUND;group=GROUP_ISOLATE;sound_picker=0;break;
      case 17:page=PAGE_SOUND;group=14;sound_picker=0;break;
      case 18:page=PAGE_EVOLVE;song_tab=3;break;
      case 19:page=PAGE_MIX;mix_tab=1;break;
      case 20:page=PAGE_SOUND;group=7;sound_picker=0;break;
      }
    }else if(control==66){
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
      air_on=d>0;
      air_packets=air_sent=air_hold=0;
      air_packet_window=air_packet_rate=air_packet_total=air_last_packet_ms=0;
      air_window_start=plat_ms();
      air_sensor=(aq_air_sensor){0};
      if(!air_on)send(70,0);
      plat_air_enable(air_on);
#endif
    }else if(control==68){
      if(d>0&&air_on)aq_air_calibrate(&air_sensor,plat_ms());
    }else if(control==69)air_hold=d>0;
    else if(control==38){visual_mode=d>0;preferences_changed_now();}
    else if(control==39){bank_slot=bounded((int)bank_slot+d,0,3);bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==40){bank_action=d>0;bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==41)bank_confirm=d>0;
    else if(control==52)mix_reset=d>0;
    else if(control==58||control==59)song_arm=d>0&&!requested_play&&!engine.running?control:0;
    else if(control==80||control==81){
      if(control==80){led_test=d>0;led_step=0;}
      else if(led_test)led_step=bounded((int)led_step+d,0,NKEYS+NB);
#ifndef PLAT_HOST
      if(led_test)plat_aquora_led_test_log(led_step);
#endif
    }
    else if(control==56){lighting=bounded((int)lighting+d,0,3);preferences_changed_now();}
    else if(control==57){fps_auto=switched(fps_auto,d);preferences_changed_now();}
    else if(control==90){ui_theme=bounded((int)ui_theme+d,0,4);preferences_changed_now();}
    else if(control==91){ui_font_size=bounded((int)ui_font_size+d,0,2);preferences_changed_now();}
    else if(control==92){ui_background=bounded((int)ui_background+d,0,1);preferences_changed_now();}
    else if(control==42)voice_slot=bounded((int)voice_slot+d,0,SR_VOICES-1);
    else send(control,bounded(d,-8,8));
    if(control!=72){knob_focus=i;knob_until=plat_ms()+900;}
    dirty=1;
 }
 if(press)dirty=1;
 unsigned buttons=(engine.running?LED_PLAY_GREEN:0)|(engine.freeze?(1u<<B_SEQ):0);
 unsigned pending=engine.pending_physical_key;
 unsigned lights=k, dim_keys=0, dim_buttons=0;
 const unsigned all_keys=(1u<<NKEYS)-1u;
 if(pending&&pending<=NKEYS)lights|=1u<<(pending-1);
 if(lighting==0){
   dim_keys=all_keys;
   dim_buttons=((1u<<NB)-1u)&~((1u<<B_PLAY)|(1u<<B_REC));
 }else if(lighting==1)buttons=0;
 else if(lighting==2)buttons=lights=0;
 else if(engine.running){
   static unsigned seen[SR_TRACKS], pulse_until[SR_TRACKS];
   unsigned now=plat_ms();
   for(unsigned t=0;t<SR_TRACKS;t++){
     unsigned event=engine.tracks[t].events;
     if(event!=seen[t]){seen[t]=event;pulse_until[t]=now+140u;}
     if(!engine.tracks[t].mute&&(engine.solo>=SR_TRACKS||engine.solo==t)&&
        (int32_t)(pulse_until[t]-now)>0)
       lights|=1u<<((event+t*3u)%NKEYS);
   }
 }
 if(led_test&&page==PAGE_SETUP&&!setup_tab){
   buttons=led_step>=NKEYS?1u<<(led_step-NKEYS):0;
   lights=led_step<NKEYS?1u<<led_step:0;
   dim_buttons=dim_keys=0;
 }
 static unsigned old_b=~0u,old_k=~0u,old_db=~0u,old_dk=~0u;
 if(buttons!=old_b||lights!=old_k||dim_buttons!=old_db||dim_keys!=old_dk){
   plat_lights(buttons,lights,dim_buttons,dim_keys,1);
   old_b=buttons;old_k=lights;old_db=dim_buttons;old_dk=dim_keys;
 }
}
#include "ui.c"
enum { FRAME_MIN_MS = 50 };
static int battery_read(void)
{
#if defined(PLAT_HOST) && !defined(PLAT_WEB)
    const char *fixture=getenv("AQUORA_TEST_BATTERY");
    if(fixture)return atoi(fixture);
#endif
    return plat_battery();
}
void app_ui_init(void){
    intro_inputs_seen=0;
    intro_start=redraw_ui_ms=lastdraw=last_control_ms=battery_poll_ms=plat_ms();
    battery_level=battery_read();
    battery_fade=4;
    battery_dirty=1;
#if !defined(PLAT_HOST) || defined(PLAT_WEB)
    intro_active=1;
#else
    intro_active=getenv("AQUORA_TEST_INTRO")!=0;
#endif
    draw(1);
}
void app_frame(void)
{
    app_input_only();
    unsigned now=plat_ms();
    if(preferences_dirty&&!engine.running&&!requested_play&&!engine.stop_frames&&
       now-preferences_changed>=1200u)save_preferences();
    unsigned scope_track=page==PAGE_SOUND&&!sound_picker&&!debug&&group==0?selected:SR_TRACKS;
    if(scope_track!=engine.scope_track){
      unsigned flags=plat_lock();
      engine.scope_track=scope_track;
      engine.scope_write=0;
      memset(engine.scope,0,sizeof engine.scope);
      plat_unlock(flags);
    }
    /* The event timeline is display-only telemetry: never maintain it when
     * EVOLVE is hidden. Musical evolution continues independently in DSP. */
    static unsigned evolve_was_visible;
    unsigned evolve_visible=page==PAGE_EVOLVE&&!debug&&!intro_active&&!fullscreen;
    if(evolve_visible&&!evolve_was_visible){
      memset(evolve_history,0,sizeof evolve_history);
      for(unsigned t=0;t<SR_TRACKS;t++)
        evolve_events[t]=engine.tracks[t].events;
      evolve_step=engine.sequence_step;
    }
    evolve_was_visible=evolve_visible;
    if(evolve_visible){
      unsigned step=engine.sequence_step;
      if(step!=evolve_step){
        if(step<evolve_step)memset(evolve_history,0,sizeof evolve_history);
        unsigned events=0;
        for(unsigned t=0;t<SR_TRACKS;t++){
          uint32_t count=engine.tracks[t].events;
          if(count!=evolve_events[t])events|=1u<<t;
          evolve_events[t]=count;
        }
        evolve_history[step&63u]=(uint8_t)events;
        evolve_step=step;
      }
    }
    if(air_on&&now-air_poll>=80u){
      air_poll=now;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
      unsigned raw=plat_air_sample(&air_packets);
      air_packet_window+=air_packets;
      air_packet_total+=air_packets;
      if(air_packets)air_last_packet_ms=now;
      unsigned target=aq_air_update(&air_sensor,raw,air_packets,now);
      if((!air_hold||!air_sensor.raw)&&target!=air_sent){send(70,target);air_sent=target;}
#endif
      if(now-air_window_start>=2000u){
        unsigned span=now-air_window_start;
        air_packet_rate=span?air_packet_window*1000u/span:0;
        air_packet_window=0;
        air_window_start=now;
      }
      if(page==PAGE_AIR)dirty=1;
    }
    if(now-window_ms>=2000){
      timing audio,fx,callback;
      unsigned flags=plat_lock();
      audio=audio_timing;fx=fx_timing;callback=callback_timing;
      memset(&audio_timing,0,sizeof(audio_timing));
      memset(&fx_timing,0,sizeof(fx_timing));
      memset(&callback_timing,0,sizeof(callback_timing));
      plat_unlock(flags);
      audio_stats=summarize(&audio);
      fx_stats=summarize(&fx);
      callback_stats=summarize(&callback);
      unsigned span=now-window_ms;
      if(!debug&&page==0){shown_fps10=frames*10000u/span;shown_missed=missed;}
      frames=missed=0;
      unsigned xruns=plat_xruns();
      xrun_delta=xruns-last_xruns;
      last_xruns=xruns;
      window_ms=now;
      if(debug||(page==1&&(group==1||group==GROUP_PERC||group==GROUP_HIT)))dirty=1;
    }
    if(displayed_revision!=state_revision)dirty=1;
    if((page==2||(page==1&&group==GROUP_KEYS))&&shown_step!=engine.sequence_step){
      shown_step=engine.sequence_step;
      dirty=1;
    }
    if(knob_until&&(int32_t)(now-knob_until)>=0){knob_until=0;dirty=1;}
    if(!intro_active&&!fullscreen&&!debug&&page==PAGE_PLAY&&now-last_control_ms>=10000u){
      fullscreen=1;battery_overlay_ms=now;dirty=1;
    }
    if(now-battery_poll_ms>=500u){
      battery_poll_ms=now;
      int level=battery_read();
      if(level!=battery_level){
        if(fullscreen&&level==0&&battery_level>0&&battery_level<4)
          battery_overlay_ms=now;
        battery_level=level;
        if(!fullscreen||battery_fade)battery_dirty=1;
      }
    }
    unsigned fade=4;
    if(fullscreen){
      unsigned age=now-battery_overlay_ms;
      fade=age>=2000u?0:age<=1200u?4:4-(age-1200u)/200u;
    }
    if(fade!=battery_fade){battery_fade=fade;battery_dirty=1;}
    if(now-redraw_ui_ms<FRAME_MIN_MS)return;
    unsigned elapsed=now-lastdraw;
    /* Only pages with actually displayed live content schedule animation.
     * Static SOUND category pickers, stopped timelines and hidden mixer
     * views do not schedule recurrent blits. */
    int live_sound=page==PAGE_SOUND&&!sound_picker&&(engine.running||engine.preview_frames);
    int live_mix=page==PAGE_MIX&&engine.running;
    int live_evolve=page==PAGE_EVOLVE&&engine.running;
    int animate=!debug&&(intro_active||page==PAGE_PLAY||live_mix||live_sound||live_evolve)&&elapsed>=FRAME_MIN_MS;
    if(animate&&page==0){
      missed+=elapsed/FRAME_MIN_MS-1u;
      if(callback_stats.p95>=(fps_auto?5500u:5700u)){
        animate=0;missed++;lastdraw=now;
      }
    }
    if(animate){
      lastdraw=now;
      unsigned begin=plat_ticks24();
      if(page==PAGE_PLAY)prepare_geometry();
      graphics_us=(plat_ticks24()-begin)/24;
    }
    if(animate||dirty||battery_dirty){
      redraw_ui_ms=now;
      unsigned draw_start=plat_ticks24();
      draw(animate);
      frame_us=(plat_ticks24()-draw_start)/24;
      if(animate&&page==0)frames++;
    }
}
#ifdef PLAT_HOST
int app_expect(const char *what,int *got){
 const sr_track *track=&engine.tracks[selected];
 if(!strcmp(what,"auto_gain"))*got=engine.auto_gain;
 else if(!strcmp(what,"auto_level"))*got=engine.auto_level;
 else if(!strcmp(what,"mute"))*got=track->mute;
 else if(!strcmp(what,"solo"))*got=engine.solo;
 else if(!strcmp(what,"send_percent"))*got=(int)(track->send*100.f+.5f);
 else if(!strcmp(what,"song_armed"))*got=song_arm;
 else if(!strcmp(what,"directory_ok")){
   *got=1;
   for(unsigned i=0;i<sizeof parameter_routes/sizeof parameter_routes[0];i++){
     open_parameter(i);
     char buffer[16];
     const char *name;
     unsigned id=control_for(parameter_routes[i].knob);
     knob_value(id,&name,buffer);
     if(id!=parameter_routes[i].id||!*name)*got=0;
   }
   page=PAGE_PLAY;song_tab=debug=selected=0;dirty=1;
 }
 else if(!strcmp(what,"directory_edits")){
   *got=1;
   /* UI-local actions and the passive scanner are exercised in their own
    * panel and radio fixtures. Here verify engine-queued edits and labels. */
   static const uint8_t local[]={38,39,40,41,42,52,56,57,58,59,66,68,69,80,81,90,91,92};
   for(unsigned i=0;i<sizeof parameter_routes/sizeof parameter_routes[0];i++){
     unsigned id=parameter_routes[i].id, skip=0;
     for(unsigned j=0;j<sizeof local;j++)skip|=id==local[j];
     if(skip)continue;
     open_parameter(i);
     char old[64], buffer[16];
     const char *name;
     if(id==54){engine.bed_type=1;engine.pending_lock=0;}
     if(id==84)engine.freeze=0;
     if(id==46)engine.loop_length=16;
     const char *before=knob_value(id,&name,buffer);
     unsigned j=0;
     while(before[j]&&j+1<sizeof old){old[j]=before[j];j++;}
     old[j]=0;
     send(id,1);
     consume();
     if(!strcmp(old,knob_value(id,&name,buffer))){
       send(id,-1);
       consume();
     }
     if(!strcmp(old,knob_value(id,&name,buffer))){
       printf("Parameter edit did not change %u (%s)\n",id,name);
       *got=0;
     }
   }
   page=PAGE_PLAY;song_tab=debug=selected=0;dirty=1;
 }
 else if(!strcmp(what,"battery_level"))*got=battery_level;
 else if(!strcmp(what,"battery_fade"))*got=battery_fade;
 else if(!strcmp(what,"intro"))*got=intro_active;
 else if(!strcmp(what,"nature_bed"))*got=engine.bed_type;
 else if(!strcmp(what,"nature_style"))*got=engine.nature_style;
 else if(!strcmp(what,"lighting"))*got=lighting;
 else if(!strcmp(what,"air_rate"))*got=air_packet_rate;
 else if(!strcmp(what,"air_packets_total"))*got=air_packet_total;
 else if(!strcmp(what,"ui_theme"))*got=ui_theme;
 else if(!strcmp(what,"ui_font"))*got=ui_font_size;
 else if(!strcmp(what,"ui_background"))*got=ui_background;
 else if(!strcmp(what,"display_subtab"))*got=display_subtab;
 else if(!strcmp(what,"led_slot"))*got=led_step;
 else if(!strcmp(what,"fullscreen"))*got=fullscreen;
 else if(!strcmp(what,"setup_tab"))*got=setup_tab;
 else if(!strcmp(what,"drum_tab"))*got=drum_tab;
 else if(!strcmp(what,"sound_group"))*got=group;
 else if(!strcmp(what,"key_pending"))*got=engine.pending_physical_key;
 else if(!strcmp(what,"key_onset"))*got=engine.play_queued!=0;
 else if(!strcmp(what,"key_held")){
   *got=0;
   for(unsigned i=0;i<SR_VOICES;i++)*got|=engine.voices[i].key&&engine.voices[i].gate;
 }
 else if(!strcmp(what,"groove_mode"))*got=engine.groove_mode;
 else if(!strcmp(what,"groove_strength"))*got=engine.groove_strength;
 else if(!strcmp(what,"air_track"))*got=engine.air_track;
 else if(!strcmp(what,"air_on"))*got=air_on;
 else if(!strcmp(what,"mix_focus"))*got=mix_focus;
 else if(!strcmp(what,"mix_bus"))*got=mix_bus;
 else if(!strcmp(what,"mix_tab"))*got=mix_tab;
 else if(!strcmp(what,"volume_percent"))*got=(int)(engine.tracks[selected].volume*100.f+.5f);
 else if(!strcmp(what,"pan_percent"))*got=(int)(engine.tracks[selected].pan*100.f+(engine.tracks[selected].pan<0?-.5f:.5f));
 else if(!strcmp(what,"isolation"))*got=engine.isolate;
 else if(!strcmp(what,"frequency_lock"))*got=engine.frequency_lock;
 else if(!strcmp(what,"reverb_percent"))*got=(int)(engine.reverb_amount*100.f+.5f);
 else if(!strcmp(what,"delay_percent"))*got=(int)(engine.delay_amount*100.f+.5f);
 else if(!strcmp(what,"page"))*got=page;else if(!strcmp(what,"running"))*got=engine.running;else if(!strcmp(what,"binaural"))*got=engine.binaural_enabled;else if(!strcmp(what,"track"))*got=selected;else if(!strcmp(what,"perfect"))*got=engine.perfect_key;else if(!strcmp(what,"tracks"))*got=SR_TRACKS;else if(!strcmp(what,"lead_glide"))*got=engine.motion[SR_LEAD].glide_ms;else if(!strcmp(what,"bass_glide"))*got=engine.motion[SR_BASS].glide_ms;else if(!strcmp(what,"lead_vibrato"))*got=engine.motion[SR_LEAD].vibrato_depth;else if(!strcmp(what,"bass_vibrato"))*got=engine.motion[SR_BASS].vibrato_depth;else if(!strcmp(what,"drums"))*got=engine.drums_enabled;else if(!strcmp(what,"density"))*got=engine.drum_density;else if(!strcmp(what,"hit"))*got=engine.drum_hit;else if(!strcmp(what,"scene"))*got=engine.scene;else if(!strcmp(what,"visual"))*got=visual_mode;else if(!strcmp(what,"bank"))*got=bank_slot;else if(!strcmp(what,"bank_message"))*got=bank_message;else if(!strcmp(what,"knobs_valid")){
 *got=1;
 for(unsigned i=0;i<4;i++){
   unsigned id=control_for(i);
   if(id){
     char buffer[16];const char *name;
     const char *value=knob_value(id,&name,buffer);
     if(!*name||!*value||text_pixels(&FONT_XS,value)>108)*got=0;
   }
 }
 }else if(!strcmp(what,"bank_exact")){
 arrangement current,stored;
 arrangement_snapshot(&current);
 *got=arrangement_read(bank_slot,&stored)&&current.crc==stored.crc;
 }else if(!strcmp(what,"bank_guard")){
 arrangement bad;
 arrangement_snapshot(&bad);
 bad.melody[0]^=1;
 int corrupt=!arrangement_valid(&bad);
 bad.scene=255;bad.crc=arrangement_crc(&bad);
 *got=corrupt&&!arrangement_valid(&bad);
 }else if(!strcmp(what,"bank_mask")){*got=0;for(unsigned i=0;i<AQ_BANKS;i++)*got|=(bank_status[i]==1)<<i;}else if(!strcmp(what,"worker"))*got=plat_cpu2_on();else if(!strcmp(what,"fallbacks"))*got=worker_fallbacks;else if(!strcmp(what,"drops"))*got=dropped;else return 0;return 1;}
#endif
