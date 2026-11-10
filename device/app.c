/* SPDX-License-Identifier: MPL-2.0
 * Original Sound Healing application. Audio owns engine state; UI produces a bounded queue. */
#include "serenity.h"
#include "fastmath.h"
#include "air_sensor.h"
#include "midi_core.h"
#include "frame_policy.h"
#include "lighting_policy.h"
static sr_engine engine PLAT_POOL;
static int16_t pcm[APP_BLOCK*2] PLAT_POOL;
typedef struct {int kind,value; unsigned track, captured_ms;} command;
static command queue[64];
static volatile unsigned qread,qwrite,state_revision,command_enqueued,command_applied;
static unsigned displayed_revision, input_revision, select_revision;
static unsigned dropped,page,selected,group,oldkeys,oldbuttons,lastdraw,dirty=1,debug;
#ifdef PLAT_HOST
static unsigned geometry_jobs,history_updates,ui_blits,worker_jobs,worker_waits;
static unsigned test_led_black,test_led_white,test_led_bright;
#define AQ_VIS_COUNT(x) (++(x))
#else
#define AQ_VIS_COUNT(x) ((void)0)
#endif
static float output_gain;
typedef struct { unsigned max, count, sum, bins[64]; } timing;
typedef struct { unsigned mean, p95, p99, peak; } timing_summary;
static timing audio_timing, fx_timing, callback_timing;
/* Control histograms use 1ms bins (stored in 100-unit timing units).
 * Capture means main-loop acknowledgement, not the physical switch edge. */
static timing control_queue_timing[2],control_label_timing[2],control_poll_timing[2];
static unsigned input_changed_ms,labelled_input,input_poll_ms;
/* Per-page draw history uses 10us units: histogram covers 64ms, without audio work. */
static timing display_timing[7];
static timing_summary audio_stats, fx_stats, callback_stats;
static unsigned window_ms;
static volatile unsigned requested_play, stop_generation, physical_keys;
static unsigned applied_stop;
static unsigned saved_status, graphics_us, blocked_keys, voice_slot;
static unsigned mix_tab,mix_reset,mix_bus,mix_focus,song_tab,song_nonce,song_arm,arrange_browse;
static unsigned save_info,drum_tab, setup_tab, display_subtab, sound_picker, fullscreen, last_control_ms;
static unsigned preferences_saving, preferences_generation, preferences_saved_ms;
#ifndef PLAT_HOST
static unsigned preferences_snapshot_generation;
#endif
static unsigned ui_theme, ui_font_size=2, ui_background, preferences_dirty, preferences_changed;
enum { PAGE_PLAY, PAGE_SOUND, PAGE_EVOLVE, PAGE_MIX, PAGE_DRUMS, PAGE_SETUP, PAGE_AIR };
static unsigned air_on, air_hold, air_poll, air_tab;
static aq_air_sensor air_sensor;
static aq_air_gesture air_gesture;
static unsigned air_sensitivity=70,air_invert;
static unsigned boot_armed,boot_starts;
#ifndef PLAT_HOST
extern uint32_t plat_aquora_boot_seed(void);
#endif
static unsigned air_packets, air_packet_window, air_packet_rate, air_last_packet_ms, air_packet_total;
static unsigned air_window_start;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
static unsigned air_sent,air_calibrated_source;
extern unsigned plat_air_source_id(void);
extern unsigned plat_air_source_rejected(void);
#endif
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
extern unsigned plat_air_sample(unsigned *packets);
extern void plat_air_enable(int on);
#endif
static unsigned quick_item;
static const char *quick_names[]={"Drums","Mixer","Motion","Groove","AIR","Banks","Performance","Play Along","FX / Nature",
 "Patch Shape","Frequency","Binaural","Sacred Bass","New Song","Display / LEDs","About","Source Isolate","Nature Style","Evolution","Master FX","Track Routing"};
static const uint8_t sound_groups[]={0,6,15,1,2,3,5,7,12,11,13,14};

static volatile unsigned intro_active;
static unsigned intro_start, intro_samples, intro_master;
static volatile unsigned intro_master_moved;
static unsigned intro_inputs_seen;
static float intro_gain;
static float intro_right;
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
    /* Original suspended interval: A3, E5, B5, with inharmonic modulators.
     * Airy bell entrances coincide with the staged wordmark, not a borrowed
     * console melody or recording. Three two-operator voices only. */
    static const float increment[6]={220.f/SR_RATE,443.3f/SR_RATE,659.255f/SR_RATE,
                                      1313.9f/SR_RATE,987.767f/SR_RATE,1487.1f/SR_RATE};
    for(unsigned k=0;k<6;k++){
        intro_fm_phase[k]+=increment[k];
        if(intro_fm_phase[k]>=1.f)intro_fm_phase[k]-=1.f;
    }
    float low=fm_clampf((t-.12f)*1.3f,0,1)*fm_clampf((7.7f-t)*.30f,0,1);
    float halo=fm_clampf((t-2.4f)*7.f,0,1)*fm_clampf((7.5f-t)*.24f,0,1);
    float crystal=fm_clampf((t-3.7f)*6.f,0,1)*fm_clampf((7.7f-t)*.26f,0,1);
    float warm=intro_sine(intro_fm_phase[0]+.08f*intro_sine(intro_fm_phase[1]));
    float bloom=intro_sine(intro_fm_phase[2]+.26f*halo*intro_sine(intro_fm_phase[3]));
    float glass=intro_sine(intro_fm_phase[4]+.20f*crystal*intro_sine(intro_fm_phase[5]));
    intro_right=(warm*320.f*low+bloom*430.f*halo+glass*420.f*crystal)*3.f;
    return (warm*390.f*low+bloom*620.f*halo+glass*290.f*crystal)*3.f;
}
static uint32_t evolve_events[SR_TRACKS], evolve_step;
static uint8_t evolve_history[64];
static unsigned led_test, led_step;
static int battery_level=-1;
static unsigned battery_poll_ms, battery_overlay_ms, battery_fade, battery_dirty;
#ifndef PLAT_HOST
extern void plat_aquora_led_test_log(unsigned slot);
extern void plat_aquora_key_duty(unsigned black,unsigned white);
#endif
static unsigned startup_chime=2;
static unsigned lighting=2, keys_baseline=3, notes_lights=1, dance_lights=1;
static unsigned fps_auto, frames, missed, shown_fps10, shown_missed;
static unsigned last_xruns, xrun_delta, frame_us;
static unsigned knob_focus, knob_until, redraw_ui_ms, shown_step;
enum { GROUP_PERC=4, GROUP_HIT=8, GROUP_VISUAL=9, GROUP_BANKS=10, GROUP_ISOLATE=11, GROUP_KEYS=12, GROUP_COUNT=16 };
static const uint8_t sound_controls[GROUP_COUNT][4]={
 {2,13,24,29},{21,5,54,78},{4,22,48,45},{8,7,23,6},{16,17,10,9},
 {30,31,36,0},{32,33,34,35},{100,0,0,0},
 {37,17,10,9},{56,141,142,143},{39,40,41,0},{43,0,0,0},{47,63,73,74},{55,14,0,0},{79,85,86,0},{120,121,122,123}
};
static const uint8_t page_controls[4][4]={
 {18,145,22,5},{0,0,0,0},{0,20,25,46},{100,0,0,0}
};
#include "arrangement.c"
#include "quicksave.c"
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
    uint8_t air_fx, air_depth, active_bank, music_mode, motion_manual;
    uint8_t startup_sound;
    uint8_t air_mode,air_sensitivity,air_invert,air_calibrated;
    uint16_t air_far,air_near;
    uint8_t air_noise; /* formerly zero alignment padding, schema39 */
    uint32_t air_source;
    uint32_t tempo_ticks;
    uint8_t drum_presence;
    uint8_t auto_mix; /* schema40 alignment padding: 0x80 | gain | level<<1; zero legacy => defaults ON */
} aq_settings;
_Static_assert(sizeof(aq_settings)==offsetof(aq_settings,tempo_ticks)+8u,"Reuse schema40 padding without moving settings");

/* Never write flash in the callback. All preference edits are coalesced and
 * committed only after the transport has stopped. The setting object is
 * separate from arrangement banks and preserves the pre-0.6.2 schema. */

static int save_preferences(void)
{
    aq_settings settings={0};
    unsigned flags=plat_lock();
    settings.auto_mix=0x80u|engine.auto_gain|(engine.auto_level<<1u);
    settings.version=0x41513040u;settings.tempo_ticks=engine.tick_length;settings.drum_presence=engine.drum_presence;settings.air_noise=air_sensor.noise;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
    settings.air_source=air_calibrated_source;
#endif
    settings.air_mode=engine.air_mode;settings.air_sensitivity=air_sensitivity;settings.air_invert=air_invert;settings.air_calibrated=air_sensor.calibrated;settings.air_far=air_sensor.reference;settings.air_near=air_sensor.near_reference;
    settings.motion[0]=engine.motion[0];settings.motion[1]=engine.motion[1];
    settings.perfect_key=engine.perfect_key;
    settings.lighting=0x80u|lighting|(keys_baseline<<2u)|(notes_lights<<4u)|(dance_lights<<5u);settings.fps_auto=fps_auto;
    settings.visual=visual_mode;settings.theme=ui_theme;
    settings.font_size=ui_font_size;settings.background=ui_background;
    settings.active_bank=bank_slot;settings.music_mode=engine.music_mode;settings.motion_manual=engine.motion_manual;
    settings.air_fx=engine.air_fx;settings.air_depth=engine.air_depth;
    plat_unlock(flags);
    settings.startup_sound=startup_chime;
#ifdef PLAT_HOST
    int result=plat_store_save(OBJ_SETTINGS,&settings,sizeof settings);
    if(!result){preferences_dirty=0;preferences_saved_ms=plat_ms();}
#else
    int result=plat_aquora_save_begin(OBJ_SETTINGS,&settings,sizeof settings);
    if(!result){preferences_saving=1;preferences_snapshot_generation=preferences_generation;}
#endif
    return result;
}
static void preferences_changed_now(void)
{
    preferences_generation++;preferences_dirty=1;
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
 if(fullscreen)return knob==0?18:knob==1?145:0;
 if(debug)return knob==0?(debug==3?42:debug==1?57:0):0;
 if(page==PAGE_AIR){static const uint8_t controls[][4]={{66,67,68,69},{93,94,95,0},{124,125,126,128},{0,0,0,0}};return controls[air_tab][knob];}
 if(page==PAGE_DRUMS){
   static const uint8_t controls[][4]={{9,10,37,17},{16,13,24,29},{49,28,62,0},{129,130,0,0}};
   return controls[drum_tab][knob];
 }
 if(page==PAGE_SETUP){
   if(setup_tab==4)return knob==0?109:0;
   if(setup_tab==0&&display_subtab){
     static const uint8_t appearance[]={91,38,109,57};
     return appearance[knob];
   }
   if(setup_tab==3)return knob==0?71:knob==1?72:0;
   return setup_tab==2?0:sound_controls[setup_tab?GROUP_BANKS:GROUP_VISUAL][knob];
 }
 if(page==PAGE_SOUND&&sound_picker)return 0;
 if(page==PAGE_EVOLVE&&song_tab){
   static const uint8_t ids[][4]={{58,59,60,61},{64,65,0,0},{82,19,83,84},{96,97,98,0},{132,133,134,135},{137,138,139,140},{144,0,0,0}};
   return ids[song_tab-1][knob];
 }
 if(page==PAGE_MIX&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))){
   return knob==0?99:0;
 }
 if(page==PAGE_MIX&&(mix_tab==1||mix_tab==2)){
   static const uint8_t master_controls[]={11,12,50,51};
   static const uint8_t reset_controls[]={52,0,0,0};
   return mix_tab==1?master_controls[knob]:reset_controls[knob];
 }
 if(page==PAGE_MIX&&mix_tab==5){static const uint8_t strip[]={13,24,29,28};return strip[knob];}
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
static void send(int kind,int value){unsigned next=(qwrite+1)&63;if(next==qread){dropped++;return;}queue[qwrite]=(command){kind,value,page==PAGE_DRUMS&&kind!=3?SR_DRUMS:selected,plat_ms()};__asm__ volatile("":::"memory");qwrite=next;command_enqueued++;}
#include "usb_midi.h"
static void consume(void){
 unsigned stop=stop_generation;
 unsigned play=requested_play;
 unsigned interrupted=stop!=applied_stop;
 if(interrupted){applied_stop=stop;sr_stop(&engine);}
 unsigned count=0;while(qread!=qwrite&&count++<64){command c=queue[qread];__asm__ volatile("":::"memory");qread=(qread+1)&63;sr_track *t=&engine.tracks[c.track];int d=c.value;
 if(c.kind!=200||(((uint32_t)c.value>>8)&255u)!=0xf8)state_revision++;
 measure(&control_queue_timing[!!engine.running],(plat_ms()-c.captured_ms)*100u);
 command_applied++;
 switch(c.kind){
 case 200:midi_receive_audio((uint32_t)c.value);play=requested_play;break;
 case 201:midi_panic();sr_midi_panic(&engine);
  if(midi_usb_ready()&&midi.settings.sync==AQ_SYNC_MASTER&&midi.settings.usb_out){
   if(engine.running){midi_resync=1;midi_tx_running=0;}
   else midi_emit(0xfc,0,0);
  }break;
 case 101:case 102:case 103:case 104:case 105:case 106:case 107:
  midi_panic();sr_midi_panic(&engine);
  if(c.kind==101&&midi.settings.sync==AQ_SYNC_MASTER&&engine.running)midi_emit(0xfc,0,0);
  midi_tx_tick=midi_position();
  midi_cc_program=midi_cc_volume=midi_cc_pan=midi_cc_send=midi_cc_bend=-1;
  if(c.kind==101)midi.settings.sync=bounded((int)midi.settings.sync+d,0,2);
  if(c.kind==102)midi.settings.clock_out=bounded((int)midi.settings.clock_out+d,0,2);
  if(c.kind==103)midi.settings.source=bounded((int)midi.settings.source+d,0,SR_DRUMS-1);
  if(c.kind==104)midi.settings.tx_channel=bounded((int)midi.settings.tx_channel+d,0,15);
  if(c.kind==105)midi.settings.rx_channel=bounded((int)midi.settings.rx_channel+d,0,16);
  if(c.kind==106)midi.settings.usb_in=switched(midi.settings.usb_in,d);
  if(c.kind==107)midi.settings.usb_out=switched(midi.settings.usb_out,d);
  engine.external_sync=midi.settings.sync==AQ_SYNC_SLAVE;
  if(c.kind==101&&engine.external_sync){requested_play=play=0;sr_stop(&engine);}
  engine.external_clock_count=255;break;
 case 108:if(!midi_panic_pending)midi_panic_channel=midi.settings.tx_channel;midi_panic_pending=1;midi_panic();sr_midi_panic(&engine);break;
 case 110:midi.settings.controls_out=switched(midi.settings.controls_out,d);midi_cc_program=midi_cc_volume=midi_cc_pan=midi_cc_send=midi_cc_bend=-1;break;
 case 137:arrange_browse=bounded((int)arrange_browse+d,0,7);break;
 case 138:{unsigned id=engine.arrange_next?engine.arrange_next-1u:sr_arrange_section(&engine,1);sr_arrange_queue(&engine,bounded((int)id+d,0,31),engine.arrange_length?engine.arrange_length:2u+sr_scenes[engine.scene].energy%3u);break;}
 case 139:{unsigned len=engine.arrange_length?engine.arrange_length:2u+sr_scenes[engine.scene].energy%3u;sr_arrange_queue(&engine,engine.arrange_next?engine.arrange_next-1u:sr_arrange_section(&engine,1),bounded((int)len+d,1,16));break;}
 case 140:if(d>0)sr_arrange_auto(&engine);else if(d<0){unsigned id=sr_arrange_section(&engine,arrange_browse);if(id<32)sr_arrange_queue(&engine,id,2u+sr_scenes[engine.scene].energy%3u);}break;
 case 136:sr_learn_event(&engine,d&31u,!(d&32u),c.captured_ms);break;
 case 132:engine.learn_mode=bounded((int)engine.learn_mode+d,0,2);break;
 case 133:engine.learn_strength=bounded((int)engine.learn_strength+d,0,3);break;
 case 134:if(d>0)sr_learn_action(&engine,1);break;
 case 135:sr_learn_action(&engine,d>0?2:0);break;
 case 109:startup_chime=bounded((int)startup_chime+d,0,2);break;

 case 1:{
   unsigned next=bounded((int)(engine.pending_scene?engine.pending_scene-1u:engine.scene)+d,0,SR_SCENES-1);
   sr_set_scene(&engine,next);
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
 case 3:if(!interrupted&&(play||engine.learn_mode)&&((physical_keys&(1u<<d))||(engine.play_grid&&engine.play_strength&&!engine.keyboard_mode)))sr_key_down(&engine,c.track,(unsigned)d);break;
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
 case 144:engine.response_enabled=switched(engine.response_enabled,d);if(!engine.response_enabled){engine.response_pending=0;if(!engine.learn_mode)engine.capture.count=0;}break;
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
 case 25:engine.motion_manual|=4u;engine.play_queued=0;engine.tick_length=bounded((int)engine.tick_length-d*80,5512,22050);break;
 case 27:sr_set_perfect_key(&engine,!engine.perfect_key);break;
 case 28:if(d<0)engine.solo=SR_TRACKS;else engine.solo=d>0?c.track:engine.solo==c.track?SR_TRACKS:c.track;break;
 case 29:t->send=adjust(t->send,d,.05f,1);break;
 case 30:engine.sacred_level=adjust(engine.sacred_level,d,.01f,.20f);break;
 case 31:engine.sacred_mode=bounded(engine.sacred_mode+d,0,2);break;
 case 32:if(c.track<=SR_BASS)engine.motion_manual|=1u<<c.track;if(c.track<=SR_BASS)engine.motion[c.track].glide_mode=bounded(engine.motion[c.track].glide_mode+d,0,2);break;
 case 33:if(c.track<=SR_BASS)engine.motion_manual|=1u<<c.track;if(c.track<=SR_BASS)engine.motion[c.track].glide_ms=bounded(engine.motion[c.track].glide_ms+10*d,10,800);break;
 case 34:if(c.track<=SR_BASS)engine.motion_manual|=1u<<c.track;if(c.track<=SR_BASS)engine.motion[c.track].vibrato_rate=bounded(engine.motion[c.track].vibrato_rate+d,5,80);break;
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
 case 120:engine.chord_mode=bounded(engine.chord_mode+d,0,3);memset(engine.scheduled,0,sizeof engine.scheduled);break;
 case 121:if(engine.chord_mode==2)engine.chord_gap=bounded(engine.chord_gap+d*10,120,220);break;
 case 122:engine.chord_inversion=bounded(engine.chord_inversion+d,0,3);break;
 case 123:engine.chord_direction=bounded(engine.chord_direction+d,0,3);break;
 case 124:engine.air_mode=bounded(engine.air_mode+d,0,4);engine.air_request=0;break;
 case 127:if(engine.running&&!engine.freeze&&!engine.air_request)engine.air_request=bounded(d,1,3);break;
 case 128:if(d>0&&engine.running&&engine.air_mode&&engine.air_mode<4&&!engine.freeze)engine.air_request=engine.air_mode;break;
 case 131:engine.air_request=0;break;
 case 129:{unsigned next=bounded((int)(engine.rhythm_pending?engine.rhythm_pending-1u:engine.rhythm_style)+d,0,2);
   if(engine.running)engine.rhythm_pending=next+1u;else engine.rhythm_style=next;break;}
 case 130:engine.fill_character=bounded(engine.fill_character+d,0,3);break;
 case 93:engine.air_fx=bounded(engine.air_fx+d,0,4);break;
 case 94:engine.air_depth=bounded(engine.air_depth+d*5,0,100);break;
 case 96:sr_set_music_mode(&engine,bounded((engine.pending_mode?engine.pending_mode-1u:engine.music_mode)+d,0,SR_MUSIC_MODES-1));break;
 case 97:sr_motion_auto(&engine,SR_LEAD,d<0);break;
 case 98:sr_motion_auto(&engine,SR_BASS,d<0);break;
 case 95:engine.air_target=adjust(engine.air_target,d,.05f,1);break;
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
   engine.auto_gain=engine.auto_level=1;
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
 case 35:if(c.track<=SR_BASS)engine.motion_manual|=1u<<c.track;if(c.track<=SR_BASS)engine.motion[c.track].vibrato_depth=bounded(engine.motion[c.track].vibrato_depth+d,0,50);break;
 }
 }
 engine.play_released|=engine.play_queued&~physical_keys;
 for(unsigned i=0;i<SR_VOICES;i++){
   sr_voice *v=&engine.voices[i];
   if(v->gate&&v->key&&!(physical_keys&(1u<<(v->key-1)))){sr_key_up(&engine,v->key-1u);}
 }
 if(midi_rx_overrun){sr_midi_panic(&engine);memset(midi_sustain,0,sizeof midi_sustain);midi_rx_overrun=0;}
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
    midi_load();
    aq_settings settings={0};
    int prefs_size=plat_store_size(OBJ_SETTINGS);
    if((prefs_size==(int)sizeof(aq_settings_legacy)||prefs_size==24||prefs_size==28||prefs_size==40||prefs_size==(int)offsetof(aq_settings,tempo_ticks)||prefs_size==32||prefs_size==32||prefs_size==(int)sizeof(settings))&&
       plat_store_load(OBJ_SETTINGS,&settings,sizeof(settings))==prefs_size&&
       (settings.version==0x41513032u||
        (settings.version==0x41513033u&&prefs_size==24)||
        ((settings.version==0x41513034u||settings.version==0x41513035u||(settings.version==0x41513036u||(settings.version==0x41513037u||(settings.version==0x41513038u||(settings.version==0x41513039u||settings.version==0x41513040u)))))&&(prefs_size==28||prefs_size==32||prefs_size==40||prefs_size==(int)offsetof(aq_settings,tempo_ticks)||prefs_size==(int)sizeof(settings))))){
      for(unsigned i=0;i<2;i++){
        sr_motion m=settings.motion[i];
        if(m.glide_mode<=2&&m.glide_ms>=10&&m.glide_ms<=800&&
           m.vibrato_rate>=5&&m.vibrato_rate<=80&&m.vibrato_depth<=50)engine.motion[i]=m;
      }
      engine.perfect_key=!!settings.perfect_key;
      if(settings.lighting&0x80u){lighting=settings.lighting&3u;keys_baseline=(settings.lighting>>2u)&3u;notes_lights=(settings.lighting>>4u)&1u;dance_lights=(settings.lighting>>5u)&1u;}
      else {lighting=settings.lighting==2?0:2;keys_baseline=settings.lighting==0?3:0;notes_lights=settings.lighting!=2;dance_lights=settings.lighting==3;}
      fps_auto=!!settings.fps_auto;
      if(settings.version>=0x41513033u){
        ui_theme=settings.theme<=4?settings.theme:0;
        ui_font_size=settings.font_size<=2?settings.font_size:1;
        ui_background=0;
        if(settings.version>=0x41513034u){
          engine.air_fx=settings.air_fx<=4?settings.air_fx:0;
          engine.air_depth=settings.air_depth<=100?settings.air_depth:100;
        }
      }
    }
    if((settings.version==0x41513038u||(settings.version==0x41513039u||settings.version==0x41513040u))){
      engine.air_mode=settings.air_mode<=4?settings.air_mode:0;
      air_sensitivity=settings.air_sensitivity>=20&&settings.air_sensitivity<=100?settings.air_sensitivity:70;
      air_invert=!!settings.air_invert;
      air_sensor.reference=settings.air_far;air_sensor.near_reference=settings.air_near;
      air_sensor.calibrated=settings.air_calibrated<=2?settings.air_calibrated:0;
      air_sensor.noise=(settings.version==0x41513039u||settings.version==0x41513040u)&&settings.air_noise?settings.air_noise:3;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
      air_calibrated_source=(settings.version==0x41513039u||settings.version==0x41513040u)?settings.air_source:0;
#endif
    }
    arrangement_init();
    quick_pending=quick_started=quick_has_save=quick_finished_ms=0;
    /* arrangement_init resets its bank cursor and visual slot. Restore the
     * saved global visualizer before first intro frame, independent of banks. */
    if((settings.version==0x41513033u&&prefs_size==24)||
       ((settings.version==0x41513034u||settings.version==0x41513035u||(settings.version==0x41513036u||(settings.version==0x41513037u||(settings.version==0x41513038u||(settings.version==0x41513039u||settings.version==0x41513040u)))))&&(prefs_size==28||prefs_size==32||prefs_size==40||prefs_size==(int)offsetof(aq_settings,tempo_ticks)||prefs_size==(int)sizeof(settings))))
      visual_mode=settings.visual<=2?settings.visual:0;
    if(settings.version>=0x41513035u&&settings.active_bank<4)bank_slot=settings.active_bank;
    if((settings.version==0x41513036u||(settings.version==0x41513037u||(settings.version==0x41513038u||(settings.version==0x41513039u||settings.version==0x41513040u))))){
      sr_set_music_mode(&engine,settings.music_mode<SR_MUSIC_MODES?settings.music_mode:0);
      engine.motion_manual=settings.motion_manual&3u;
      for(unsigned t=0;t<2;t++)if(engine.motion_manual&(1u<<t))engine.motion[t]=settings.motion[t];
    }else if(settings.version){engine.motion_manual=sr_legacy_motion_manual(engine.motion,SR_SCENES);for(unsigned t=0;t<2;t++)if(!(engine.motion_manual&(1u<<t)))sr_motion_auto(&engine,t,1); }
    if((settings.version==0x41513037u||(settings.version==0x41513038u||(settings.version==0x41513039u||settings.version==0x41513040u)))&&settings.startup_sound<=2)startup_chime=settings.startup_sound;
    if(settings.version==0x41513040u&&prefs_size==(int)sizeof(settings)){
      if(settings.tempo_ticks>=5512u&&settings.tempo_ticks<=22050u&&(settings.motion_manual&4u)){
        engine.tick_length=settings.tempo_ticks;engine.motion_manual|=4u;
      }
      if(settings.drum_presence<=100u)engine.drum_presence=settings.drum_presence;
      if((settings.auto_mix&0xfcu)==0x80u){engine.auto_gain=settings.auto_mix&1u;engine.auto_level=(settings.auto_mix>>1u)&1u;}
    }
    /* Cold boot only. Hardware entropy comes from the BSP random source;
     * the test seed is explicit and never used by production firmware. */
    unsigned boot_seed=song_nonce;
#ifndef PLAT_HOST
    boot_seed^=plat_aquora_boot_seed();
#else
    boot_seed^=(unsigned)clock()^(unsigned)time(0);
    const char *boot_fixture=getenv("AQUORA_TEST_BOOT_SEED");
    if(boot_fixture)boot_seed=(unsigned)strtoul(boot_fixture,0,0);
#endif
    sr_set_music_mode(&engine,(boot_seed>>8)%SR_MUSIC_MODES);
    sr_set_scene(&engine,boot_seed%SR_SCENES);
    sr_generate_song(&engine,boot_seed^0x41513039u,0);
    engine.rhythm_style=(boot_seed>>16)%8u<4u?0u:1u+(boot_seed>>20)%2u;
    boot_armed=1;boot_starts=0;
#ifdef PLAT_HOST
    if(getenv("AQUORA_TEST_AUTOSTART")&&!strcmp(getenv("AQUORA_TEST_AUTOSTART"),"0"))boot_armed=0;
#endif
    preferences_saving=0;preferences_saved_ms=0;
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
    qread=qwrite=command_enqueued=command_applied=0;
    quick_started=quick_pending=quick_timeout=quick_finished_ms=0;
    requested_play=stop_generation=applied_stop=physical_keys=0;
    page=selected=group=debug=display_subtab=0;
    mix_tab=mix_reset=mix_focus=mix_bus=song_tab=song_arm=setup_tab=drum_tab=air_tab=save_info=0;
    preferences_dirty=preferences_changed=0;
    air_packet_window=air_packet_rate=air_packet_total=air_last_packet_ms=0;
    air_window_start=plat_ms();
    oldkeys=oldbuttons=blocked_keys=voice_slot=knob_until=redraw_ui_ms=0;
    dirty=1;
}
void app_panic(void)
{
    boot_armed=0;requested_play=0;
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
    midi_audio_before();
    sr_render(&engine,pcm,n);
    midi_audio_after(n);
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
      float bell=0,bell_right=0;
      if(chime){
        intro_gain+=((float)playing_intro-intro_gain)*.003f;
        /* Master zero remains real silence. The start of the synth timeline
         * is the first ready audio callback, not an early boot wall clock. */
        if(master&&startup_chime){float level=startup_chime==1?.35f:1.f;bell=intro_note(intro+i)*intro_gain*level;bell_right=intro_right*intro_gain*level;}
      }
      output_gain+=(target-output_gain)*.002f;
      lr[2*i]=(int32_t)((pcm[2*i]+bell)*256.f*output_gain);
      lr[2*i+1]=(int32_t)((pcm[2*i+1]+bell_right)*256.f*output_gain);
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
void app_input_only(void){
 unsigned poll_now=plat_ms();
 if(input_poll_ms)measure(&control_poll_timing[!!engine.running],(poll_now-input_poll_ms)*100u);
 input_poll_ms=poll_now;
 midi_poll();unsigned b=plat_buttons(),k=plat_keys(),press=b&~oldbuttons;oldbuttons=b;
 int enc[7]={plat_enc(EN_ALGO),plat_enc(EN_SELECT),plat_enc(EN_PRESET),
             plat_enc(EN_K1),plat_enc(EN_K1+1),plat_enc(EN_K1+2),plat_enc(EN_K1+3)};
 int control=press!=0;
 for(unsigned i=0;i<7;i++)control|=enc[i]!=0;
 if(control||k!=oldkeys){input_revision++;input_changed_ms=plat_ms();}
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
   if(fullscreen&&(press&(1u<<B_HOME)||enc[0])){fullscreen=0;dirty=1;enc[0]=0;press&=~(1u<<B_HOME);}
   /* Fullscreen retains transport, keys, PRESETS and contextual intensity. */
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
   song_arm=mix_reset=0;
   requested_play=!requested_play;
   if(!requested_play)stop_generation++;
   plat_unlock(flags);
 }
 if(press&(1u<<B_HOME)){page=0;debug=0;bank_confirm=song_arm=mix_reset=0;send(44,0);}
 if((press&(1u<<B_SEL))&&page==PAGE_SETUP&&setup_tab==0){display_subtab=!display_subtab;dirty=1;}
 else if((press&(1u<<B_SEL))&&(page==PAGE_PLAY||page==PAGE_SOUND||(page==PAGE_MIX&&mix_tab!=2))&&!(page==PAGE_MIX&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))))send(15,0);
 if((press&(1u<<B_SEL))&&page==PAGE_EVOLVE&&song_tab==0){selected=(selected+1u)%SR_TRACKS;dirty=1;}
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
   if(!debug&&page==2)song_tab=(song_tab+1)%8;
   if(page==PAGE_SOUND){
     unsigned index=0;
     for(unsigned i=0;i<sizeof(sound_groups);i++)if(sound_groups[i]==group)index=i;
     group=sound_groups[(index+1)%sizeof(sound_groups)];sound_picker=0;
   }
   if(page==PAGE_AIR)air_tab=(air_tab+1)%4;
   if(page==PAGE_DRUMS)drum_tab=(drum_tab+1)%4;
   if(page==PAGE_SETUP)setup_tab=(setup_tab+1)%5;
   if(page==3){/* Preserve legacy EDIT cycle; FX enters the dedicated focus strip. */
     mix_tab=mix_tab>=4?0:mix_tab+1;mix_reset=0;}
   bank_confirm=song_arm=0;if(!quick_started&&!quick_pending)bank_message=BANK_IDLE;
 }
 /* OCT- opens the arrangement page. OCT+ jumps between percussion and HIT. */
 if((press&(1u<<B_OCTDN))&&!(b&(1u<<B_OCTUP))){page=PAGE_SETUP;setup_tab=1;debug=0;}
 if((press&(1u<<B_OCTUP))&&!(b&(1u<<B_OCTDN))){bank_confirm=0;drum_tab=page==PAGE_DRUMS?(drum_tab+1)%4:0;page=PAGE_DRUMS;debug=0;}
 if((press&(1u<<B_LFO))&&!(page==PAGE_MIX&&!debug&&(mix_tab==4||((mix_tab==0||mix_tab==3)&&mix_focus>=SR_TRACKS))))send(debug?27:28,0);
 /* SAVE has exactly one meaning on every page, including fullscreen. */
 if(press&(1u<<B_SAVE))arrangement_quick_save();
 /* Explicit actions use SEL on their labelled action page, never SAVE. */
 if((press&(1u<<B_SEL))&&!debug){
   if(page==PAGE_SETUP&&setup_tab==1&&!quick_started&&!quick_pending){bank_action=0;arrangement_execute();if(bank_message==BANK_LOADED)preferences_changed_now();}
   if(page==PAGE_EVOLVE&&song_tab==1&&song_arm&&!requested_play&&!engine.running){send(song_arm,1);song_arm=0;}
   if(page==PAGE_MIX&&mix_tab==2&&mix_reset){send(53,0);preferences_changed_now();mix_reset=0;}
 }
 blocked_keys&=k;
 physical_keys=k&~blocked_keys;
 for(unsigned i=0;i<NKEYS;i++){
   if(!(press&(1u<<B_PLAY))&&(requested_play||engine.learn_mode)&&((physical_keys&~oldkeys)&(1u<<i)))send(3,i);
 }
 for(unsigned i=0;i<NKEYS;i++)if((engine.learn_mode||engine.response_enabled)&&((physical_keys^oldkeys)&(1u<<i)))
   send(136,i|((physical_keys&(1u<<i))?0:32));
 oldkeys=k;
 int d=enc[0];
 if(d&&!debug){page=bounded((int)page+d,PAGE_PLAY,PAGE_AIR);sound_picker=0;mix_reset=0;bank_confirm=song_arm=0;knob_until=0;dirty=1;}
 static unsigned role_page=~0u;
 if(page==PAGE_MIX&&role_page!=PAGE_MIX&&mix_tab!=4)mix_focus=selected;
 role_page=page;
 d=enc[1];
 if(d&&!debug){
   if(page==PAGE_SOUND&&sound_picker){
     int index=0;
     for(unsigned i=0;i<sizeof(sound_groups);i++)if(sound_groups[i]==group)index=(int)i;
     group=sound_groups[bounded(index+d,0,sizeof(sound_groups)-1)];
   }else if(page==PAGE_MIX&&mix_tab==4)mix_bus=bounded((int)mix_bus+d,0,SR_BUS_COUNT-1);
   else if(page==PAGE_MIX&&(mix_tab==0||mix_tab==3||mix_tab==5))mix_select(d);
   else if(page==PAGE_SETUP&&setup_tab==2)save_info=bounded((int)save_info+d,0,2);
   else if(page==PAGE_SETUP&&setup_tab==0)display_subtab=bounded((int)display_subtab+d,0,1);
   else if(page!=PAGE_DRUMS){selected=bounded((int)selected+d,0,7);if(page==PAGE_PLAY)select_revision++;}
   knob_until=0;dirty=1;
 }
 d=enc[2];
 if(d){send(1,bounded(d,-8,8));dirty=1;}
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
      case 20:page=PAGE_MIX;mix_tab=5;mix_focus=selected;break;
      }
    }else if(control==66){
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
      air_on=d>0;
      air_packets=air_sent=air_hold=0;
      air_packet_window=air_packet_rate=air_packet_total=air_last_packet_ms=0;
      air_window_start=plat_ms();
      unsigned far=air_sensor.reference,near=air_sensor.near_reference,cal=air_sensor.calibrated,noise=air_sensor.noise;
      air_sensor=(aq_air_sensor){0};air_sensor.reference=far;air_sensor.near_reference=near;air_sensor.calibrated=cal;air_sensor.noise=noise;
      air_gesture=(aq_air_gesture){0};
      if(!air_on){send(70,0);send(131,0);}
      plat_air_enable(air_on);
#endif
    }else if(control==68){
      if(d>0&&air_on&&aq_air_calibrate(&air_sensor,plat_ms())){
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
        air_calibrated_source=plat_air_source_id();
#endif
        preferences_changed_now();
      }
      if(d<0){air_sensor.calibrated=0;air_sensor.near_reference=0;send(70,0);}
    }else if(control==69)air_hold=d>0;
    else if(control==125){air_sensitivity=bounded((int)air_sensitivity+d*5,20,100);preferences_changed_now();}
    else if(control==126){air_invert=switched(air_invert,d);preferences_changed_now();}
    else if(control==109||(AQ_PERFORMANCE_MIDI&&control>=101&&control<=110)){send(control,d);preferences_changed_now();}
    else if(control==38){visual_mode=bounded((int)visual_mode+d,0,2);preferences_changed_now();}
    else if(control==39){bank_slot=bounded((int)bank_slot+d,0,3);preferences_changed_now();bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==40){bank_action=d>0;bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==41)bank_confirm=d>0;
    else if(control==99){
      if(d>0){page=PAGE_SOUND;group=mix_bus==0?1:mix_bus==1?5:mix_bus==2?2:3;
        if(mix_bus==4){page=PAGE_MIX;mix_tab=1;}sound_picker=0;}
    }
    else if(control==100){if(d>0){page=PAGE_MIX;mix_tab=5;mix_focus=selected;}}
    else if(control==52)mix_reset=d>0;
    else if(control==58||control==59)song_arm=d>0&&!requested_play&&!engine.running?control:0;
    else if(control==80||control==81){
      if(control==80){led_test=d>0;led_step=0;}
      else if(led_test)led_step=bounded((int)led_step+d,0,NKEYS+NB);
#ifndef PLAT_HOST
      if(led_test)plat_aquora_led_test_log(led_step);
#endif
    }
    else if(control==145){fullscreen=d>0;dirty=1;}
    else if(control==141){keys_baseline=bounded((int)keys_baseline+d,0,3);if(keys_baseline&&!lighting)lighting=1;preferences_changed_now();}
    else if(control==142){notes_lights=switched(notes_lights,d);preferences_changed_now();}
    else if(control==143){dance_lights=switched(dance_lights,d);preferences_changed_now();}
    else if(control==56){lighting=bounded((int)lighting+d,0,3);preferences_changed_now();}
    else if(control==57){fps_auto=switched(fps_auto,d);preferences_changed_now();}

    else if(control==91){ui_font_size=bounded((int)ui_font_size+d,0,2);preferences_changed_now();}

    else if(control==42)voice_slot=bounded((int)voice_slot+d,0,SR_VOICES-1);
    else {send(control,bounded(d,-8,8));if(control==25||control==50||control==51||control==62||control==93||control==94||control>=96||(control>=32&&control<=35))preferences_changed_now();}
    if(control>=120)preferences_changed_now();
    if(control!=72){knob_focus=i;knob_until=plat_ms()+900;}
    dirty=1;
 }
 if(press)dirty=1;
 unsigned clock_flags=plat_lock();
 unsigned play_pulse=aq_play_pulse(engine.running,engine.sequence_step,
                                  engine.tick_phase,engine.tick_length);
 plat_unlock(clock_flags);
 unsigned buttons=(play_pulse?LED_PLAY_GREEN:0)|(engine.freeze?(1u<<B_SEQ):0);
 unsigned pending=engine.pending_physical_key;
 unsigned lights=k, dim_keys=0, dim_buttons=0, black_duty=0,white_duty=0;
 if(pending&&pending<=NKEYS)lights|=1u<<(pending-1);
 static unsigned led_event[SR_TRACKS],led_until[SR_TRACKS],led_note[SR_TRACKS];
 static float dance_peak[2]={.0001f,.0001f},dance_env[2];
 static unsigned dance_ms;
 unsigned led_now=plat_ms(),dt=led_now-dance_ms;
 unsigned generated=0,tonal=0,drums=0;float energy[2]={0,0};
 unsigned flags=plat_lock();
 for(unsigned t=0;t<SR_TRACKS;t++){
   const sr_track *track=&engine.tracks[t];
   unsigned audible=(engine.running||engine.preview_frames)&&!track->mute&&track->volume>0&&
      (engine.solo>=SR_TRACKS||engine.solo==t)&&(t!=SR_DRUMS||(engine.drums_enabled&&engine.drum_level>0));
   if(track->events!=led_event[t]){
     led_event[t]=track->events;
     int note=track->last_note;while(note<53)note+=12;while(note>79)note-=12;
     led_note[t]=1u<<(note-53);led_until[t]=led_now+120u;
   }
   if(!audible){led_until[t]=0;continue;}
   if(track->meter>energy[t==SR_DRUMS])energy[t==SR_DRUMS]=track->meter;
   if((int32_t)(led_until[t]-led_now)>0){
     if(t==SR_DRUMS){drums=1;if(notes_lights)for(unsigned key=0;key<NKEYS;key++)if(aq_black_key(key))generated|=1u<<key;}else tonal|=led_note[t];
     if(notes_lights&&t<SR_DRUMS)generated|=led_note[t];
   }
 }
 for(unsigned v=0;v<SR_VOICES;v++){
   const sr_voice *voice=&engine.voices[v];unsigned t=voice->track;
   if(!voice->active||t>=SR_DRUMS||engine.tracks[t].mute||engine.tracks[t].volume<=0||
      (engine.solo<SR_TRACKS&&engine.solo!=t)||voice->env<.005f)continue;
   int note=voice->midi;while(note<53)note+=12;while(note>79)note-=12;
   tonal|=1u<<(note-53);if(notes_lights)generated|=1u<<(note-53);
 }
 if(!engine.running&&!engine.preview_frames){generated=tonal=drums=0;energy[0]=energy[1]=0;}
 plat_unlock(flags);
 if(dt>=20u){
   dance_ms=led_now;
   for(unsigned bus=0;bus<2;bus++){
     dance_peak[bus]*=.985f;if(dance_peak[bus]<.0001f)dance_peak[bus]=.0001f;
     if(energy[bus]>dance_peak[bus])dance_peak[bus]=energy[bus];
     float target=energy[bus]>.00001f?energy[bus]/dance_peak[bus]:0;
     dance_env[bus]+=(target-dance_env[bus])*(target>dance_env[bus]?.75f:.22f);
     if(!energy[bus]&&!(bus?drums:tonal))dance_env[bus]=0;
   }
 }
 unsigned baseline=0,dance_mid=0,dance_dim=0;
 for(unsigned key=0;key<NKEYS;key++){
   unsigned black=aq_black_key(key),bit=1u<<key;
   if(keys_baseline==3||(keys_baseline==2&&!black)||(keys_baseline==1&&(5u+key)%12u==0u))baseline|=bit;
   unsigned active=black?drums:tonal;
   if(dance_lights&&active){
     /* Tonal pitches illuminate their nearest white key; drum hits punch
      * all black keys together. No unrelated pseudo-random rotation. */
     unsigned near=tonal|((tonal<<1)&~tonal)|((tonal>>1)&~tonal);
     if(black||(near&bit)){
       float level=dance_env[black];
       if(level>.72f)lights|=bit;else if(level>.22f)dance_mid|=bit;else if(level>.015f)dance_dim|=bit;
     }
   }
 }
 unsigned level=keys_baseline&&!lighting?1u:lighting;
 buttons|=1u<<(page==PAGE_PLAY?B_HOME:page==PAGE_DRUMS?B_ARP:page==PAGE_EVOLVE?B_SEQ:B_SEL);
 unsigned state_buttons=buttons;
 unsigned idle_buttons=((1u<<NB)-1u)&~((1u<<B_PLAY)|(1u<<B_REC));
 if(lighting==3)buttons|=idle_buttons;
 else if(lighting==2)plat_lights_mid(idle_buttons,0);
 else if(lighting==1)dim_buttons|=idle_buttons;
 if(level==3)lights|=baseline;else if(level==2)dance_mid|=baseline;else if(level==1)dance_dim|=baseline;
 lights|=generated;
 dance_mid&=~lights;dance_dim&=~(lights|dance_mid);
 dim_keys=dance_dim;dim_buttons&=~state_buttons;
 plat_lights_mid(lighting==2?idle_buttons&~state_buttons:0,dance_mid);
 if(led_test&&page==PAGE_SETUP&&!setup_tab){
   buttons=led_step>=NKEYS?1u<<(led_step-NKEYS):0;
   lights=led_step<NKEYS?1u<<led_step:0;
   dim_buttons=dim_keys=0;
   black_duty=white_duty=0;
 }
#ifndef PLAT_HOST
 plat_aquora_key_duty(black_duty,white_duty);
#else
 test_led_black=test_led_white=0;test_led_bright=lights;
 for(unsigned key=0;key<NKEYS;key++)if((lights|dance_mid|dim_keys)&(1u<<key)){if(aq_black_key(key))test_led_black|=1u<<key;else test_led_white|=1u<<key;}
 /* Binary host/browser LED previews show the idle groups at mid level;
  * exact 70/50 scan duty is validated separately against the native path. */

#endif
 static unsigned old_b=~0u,old_k=~0u,old_db=~0u,old_dk=~0u;
 if(buttons!=old_b||lights!=old_k||dim_buttons!=old_db||dim_keys!=old_dk){
   plat_lights(buttons,lights,dim_buttons,dim_keys,1);
   old_b=buttons;old_k=lights;old_db=dim_buttons;old_dk=dim_keys;
 }
}
static unsigned visual_interval_ms=50;
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
    if(boot_armed&&!intro_active){
      boot_armed=0;boot_starts++;requested_play=1;last_control_ms=plat_ms();dirty=1;
    }
#ifndef PLAT_HOST
    if(preferences_saving){
      int result=plat_aquora_save_poll();
      if(result!=1){
        preferences_saving=0;
        if(!result&&preferences_generation==preferences_snapshot_generation){preferences_dirty=0;preferences_saved_ms=plat_ms();dirty=1;}
        if(result<0)preferences_changed=plat_ms();
      }
    }
#endif
    if(!preferences_saving)arrangement_service();
    unsigned now=plat_ms();
    /* HOME's header is otherwise static. Invalidate it when the committed
     * musical section changes, even with no encoder/button activity. */
    static uint32_t shown_form_key,shown_form_slot;
    if(page==PAGE_PLAY||page==PAGE_EVOLVE){
      unsigned flags=plat_lock();
      uint32_t key=engine.form_key,slot=engine.form_loop/(2u+sr_scenes[engine.scene].energy%3u);
      plat_unlock(flags);
      if(key!=shown_form_key||slot!=shown_form_slot){shown_form_key=key;shown_form_slot=slot;dirty=1;}
    }
    if(!quick_started&&!quick_pending&&!preferences_saving&&preferences_dirty&&
       now-preferences_changed>=1200u)save_preferences();
    if(quick_finished_ms&&now-quick_finished_ms>=3000u){quick_finished_ms=0;dirty=1;}
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
      memset(engine.timeline,0,sizeof engine.timeline);
      for(unsigned t=0;t<SR_TRACKS;t++)
        evolve_events[t]=engine.tracks[t].events;
      evolve_step=engine.sequence_step;
    }
    engine.timeline_enabled=evolve_visible;
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
        evolve_history[step&63u]=(uint8_t)events;AQ_VIS_COUNT(history_updates);
        evolve_step=step;
      }
    }
    if(air_on&&now-air_poll>=40u){
      air_poll=now;
#if FM1B_BLE_CENTRAL && !defined(PLAT_HOST)
      unsigned raw=plat_air_sample(&air_packets);
      air_packet_window+=air_packets;
      air_packet_total+=air_packets;
      if(air_packets)air_last_packet_ms=now;
      unsigned target=aq_air_update(&air_sensor,raw,air_packets,now);
      unsigned live=aq_air_live(&air_sensor,now)&&air_sensor.calibrated==2&&aq_air_span(&air_sensor)>=aq_air_min_span(&air_sensor);
      if(air_sensor.calibrated==2&&air_calibrated_source!=plat_air_source_id()){
        live=0;target=0;air_sensor.calibration_error=4;
      }
      unsigned gesture_target=target;
      if(air_invert&&live)target=100-target;
      if(aq_air_gesture_update(&air_gesture,gesture_target,live,air_packets,now,air_sensitivity)&&engine.air_mode&&engine.air_mode<4)
        send(127,engine.air_mode);
      if((!air_hold||!live)&&target!=air_sent){send(70,target);air_sent=target;}
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
    if(!intro_active&&!fullscreen&&!debug&&page==PAGE_PLAY&&!quick_started&&!quick_pending&&now-last_control_ms>=10000u){
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
    /* Never latch a frozen image: reduce optional cadence under sustained
     * pressure, then recover automatically. Audio ISR always owns priority. */
    unsigned load=callback_stats.p95;
#ifdef PLAT_HOST
    const char *pressure=getenv("AQUORA_TEST_CALLBACK_US");
    if(pressure)load=(unsigned)atoi(pressure); /* scheduling fixture only */
#endif
    unsigned visual_interval=aq_frame_interval(fps_auto,engine.running,
                                               intro_active,load,xrun_delta);
    visual_interval_ms=visual_interval;
    /* Only pages with actually displayed live content schedule animation.
     * Static SOUND category pickers, stopped timelines and hidden mixer
     * views do not schedule recurrent blits. */
    int live_sound=page==PAGE_SOUND&&!sound_picker&&(engine.running||engine.preview_frames||engine.external_audio);
    int live_mix=page==PAGE_MIX&&(engine.running||engine.external_audio);
    int live_evolve=page==PAGE_EVOLVE&&(engine.running||engine.external_audio);
    int live_air=page==PAGE_AIR&&(air_on||engine.air_target>.0001f||engine.air_wet>.0001f);
    int animate=!debug&&(intro_active||page==PAGE_PLAY||live_mix||live_sound||live_evolve||live_air)&&elapsed>=visual_interval;
    if(animate&&page==0){
      missed+=elapsed/FRAME_MIN_MS-1u;

    }
    if(animate){
      lastdraw=now;
      unsigned begin=plat_ticks24();
      if(page==PAGE_PLAY&&!intro_active&&((!quick_started&&!quick_pending)||quick_timeout))prepare_geometry();
      graphics_us=(plat_ticks24()-begin)/24;
    }
    if(animate||dirty||battery_dirty){
      redraw_ui_ms=now;
      unsigned draw_start=plat_ticks24();
      unsigned complete=draw(animate);
      frame_us=(plat_ticks24()-draw_start)/24;
      if(!debug&&!intro_active)measure(&display_timing[page],(frame_us+9u)/10u);
      if(complete&&animate&&page==0)frames++;
    }
}
#ifdef PLAT_HOST
int app_expect(const char *what,int *got){
 if(!strcmp(what,"large_text_overflow")){*got=large_text_overflow;return 1;}
 if(!strcmp(what,"large_font_used")){*got=!!(large_font_mask&1u);return 1;}
 if(!strcmp(what,"large_font_mask")){*got=large_font_mask;return 1;}
 if(!strcmp(what,"control_metrics")){
  for(unsigned run=0;run<2;run++){
   timing_summary q=summarize(&control_queue_timing[run]),l=summarize(&control_label_timing[run]),p=summarize(&control_poll_timing[run]);
   printf("CONTROL run=%u queue_p95_ms=%u queue_p99_ms=%u label_p95_ms=%u label_p99_ms=%u label_max_ms=%u poll_p99_ms=%u drops=%u\n",run,q.p95/100u,q.p99/100u,l.p95/100u,l.p99/100u,l.peak/100u,p.p99/100u,dropped);
  }*got=1;return 1;
 }
 if(!strcmp(what,"mix_focus")){*got=mix_focus;return 1;}
 if(!strcmp(what,"boot_starts")){*got=boot_starts;return 1;}
 if(!strcmp(what,"intensity")){*got=engine.energy*100.f+.5f;return 1;}
 if(!strcmp(what,"response_enabled")){*got=engine.response_enabled;return 1;}
 if(!strcmp(what,"keys_baseline")){*got=keys_baseline;return 1;}
 if(!strcmp(what,"notes_lights")){*got=notes_lights;return 1;}
 if(!strcmp(what,"dance_lights")){*got=dance_lights;return 1;}
 if(!strcmp(what,"learn_count")){*got=engine.capture.count;return 1;}
 if(!strcmp(what,"learn_applied")){*got=engine.learned.count;return 1;}
 if(!strcmp(what,"learn_bank_saved")){
   arrangement r;*got=arrangement_read(0,&r);if(*got){arrangement_apply(&r);*got=engine.learned.count==3;}return 1;
 }
 if(!strcmp(what,"learn_bank")){
   arrangement r;arrangement_snapshot(&r);engine.learned.count=0;arrangement_apply(&r);*got=engine.learned.count==3;return 1;
 }
 if(!strcmp(what,"bank_migration_matrix")){
  *got=1;arrangement record,loaded;arrangement_snapshot(&record);
  for(unsigned version=1;version<=8;version++){
   record.version=0x41514200u+version;
   size_t size=version==1?offsetof(arrangement,loop_length):version==2?offsetof(arrangement,motif_home):version==3?offsetof(arrangement,song_seed):version==4?offsetof(arrangement,chord_mode):version==5?offsetof(arrangement,learned):version==6?offsetof(arrangement,arrange_next):version==7?offsetof(arrangement,motif_memory):sizeof(record);
   uint32_t crc=arrangement_crc(&record);
   if(version==1)record.legacy_crc=crc;else if(version==2)record.v2_crc=crc;else if(version==3)record.crc=crc;else if(version==4)record.crc4=crc;else if(version==5)record.crc5=crc;else if(version==6)record.crc6=crc;else if(version==7)record.crc7=crc;else record.crc8=crc;
   for(unsigned slot=0;slot<4;slot++){
    if(plat_store_save(OBJ_BANK1+slot,&record,size)||!arrangement_read(slot,&loaded)){*got=0;return 1;}
    arrangement_apply(&loaded);
   }
  }
  return 1;
 }
 if(!strcmp(what,"bank_new_roundtrip")){
  arrangement record;*got=arrangement_read(0,&record)&&record.version==AQ_BANK_VERSION;
  if(*got){arrangement_apply(&record);*got=engine.chord_mode==2&&engine.chord_gap==110&&engine.rhythm_style==1&&engine.fill_character==2;}
  return 1;
 }
 if(!strcmp(what,"bank_v4_fixture")){
  arrangement record;arrangement_snapshot(&record);record.version=AQ_BANK_V4;record.crc4=arrangement_crc(&record);
  *got=plat_store_save(OBJ_BANK1,&record,offsetof(arrangement,chord_mode))==0;return 1;
 }
 if(!strcmp(what,"chord_mode")){*got=engine.chord_mode;return 1;}
 if(!strcmp(what,"chord_gap")){*got=engine.chord_gap;return 1;}
 if(!strcmp(what,"rhythm_pending")){*got=engine.rhythm_pending;return 1;}
 if(!strcmp(what,"rhythm_style")){*got=engine.rhythm_style;return 1;}
 if(!strcmp(what,"fill_character")){*got=engine.fill_character;return 1;}
 if(!strcmp(what,"air_mode")){*got=engine.air_mode;return 1;}
 if(!strcmp(what,"air_actions")){*got=engine.air_actions;return 1;}
 if(!strcmp(what,"air_request")){*got=engine.air_request;return 1;}
 const sr_track *track=&engine.tracks[selected];
 if(!strcmp(what,"auto_gain"))*got=engine.auto_gain;
 else if(!strcmp(what,"auto_level"))*got=engine.auto_level;
 else if(!strcmp(what,"mute"))*got=track->mute;
 else if(!strcmp(what,"solo"))*got=engine.solo;
 else if(!strcmp(what,"send_percent"))*got=(int)(track->send*100.f+.5f);
 else if(!strcmp(what,"music_mode"))*got=engine.music_mode;
 else if(!strcmp(what,"motion_manual"))*got=engine.motion_manual;
 else if(!strcmp(what,"visual_pitch_hz"))*got=(int)vis_input.pitch;
 else if(!strcmp(what,"node_count"))*got=vis_output.node_count;
 else if(!strcmp(what,"song_armed"))*got=song_arm;
 else if(!strcmp(what,"directory_ok")){
   *got=1;
   for(unsigned i=0;i<sizeof parameter_routes/sizeof parameter_routes[0];i++){
     open_parameter(i);
     char buffer[16];
     const char *name;
     unsigned id=control_for(parameter_routes[i].knob);
     knob_value(id,&name,buffer);
     if(id!=parameter_routes[i].id||!*name){printf("BAD ROUTE id %u actual %u\n",parameter_routes[i].id,id);*got=0;}
   }
   page=PAGE_PLAY;song_tab=debug=selected=0;dirty=1;
 }
 else if(!strcmp(what,"directory_edits")){
   *got=1;
   /* UI-local actions and the passive scanner are exercised in their own
    * panel and radio fixtures. Here verify engine-queued edits and labels. */
   static const uint8_t local[]={38,39,40,41,42,52,56,57,58,59,66,68,69,80,81,90,91,92,108};
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
 else if(!strcmp(what,"led_black_mask"))*got=test_led_black;
 else if(!strcmp(what,"led_white_mask"))*got=test_led_white;
 else if(!strcmp(what,"led_bright_mask"))*got=test_led_bright;
 else if(!strcmp(what,"page_draw_stats")){
   for(unsigned p=0;p<7;p++){timing_summary t=summarize(&display_timing[p]);printf("PAGE_DRAW %u %u mean%u p95%u p99%u max%u us\n",p,display_timing[p].count,t.mean*10,t.p95*10,t.p99*10,t.peak*10);}*got=1;
 }
 else if(!strcmp(what,"perf_sample")){
   printf("PERF %u %u %u %u %u %u\n",graphics_us,frame_us,callback_stats.p95,vis_output.node_count,worker_fallbacks,geometry_jobs);*got=1;
 }
 else if(!strcmp(what,"save_timeout"))*got=quick_timeout;
 else if(!strcmp(what,"save_polls"))*got=quick_polls;
 else if(!strcmp(what,"save_stage"))*got=quick_progress[0];
 else if(!strcmp(what,"bass_patch"))*got=engine.tracks[SR_BASS].patch;
 else if(!strcmp(what,"mix_reset_armed"))*got=mix_reset;
 else if(!strcmp(what,"font_family"))*got=AQUORA_FONT_FAMILY==0x41515335u;
 else if(!strcmp(what,"all_roles")){
   for(unsigned t=0;t<SR_DRUMS;t++)sr_note_on(&engine,t,48+t*2,90,5.f);
   engine.kick_env=engine.hand_env=1.f;*got=1;
 }
 else if(!strcmp(what,"remember_song")||!strcmp(what,"song_unchanged")){
   static uint32_t remembered;arrangement r;arrangement_snapshot(&r);
   if(!strcmp(what,"remember_song")){remembered=r.crc4;*got=1;}else *got=remembered==r.crc4;
 }
 else if(!strcmp(what,"remember_motif")||!strcmp(what,"motif_unchanged")){
   static int8_t remembered[16],home[16];static unsigned patches[8];
   if(!strcmp(what,"remember_motif")){memcpy(remembered,engine.melody,sizeof remembered);memcpy(home,engine.motif_home,sizeof home);for(unsigned t=0;t<8;t++)patches[t]=engine.tracks[t].patch;*got=1;}
   else{*got=!memcmp(remembered,engine.melody,sizeof remembered)&&!memcmp(home,engine.motif_home,sizeof home);for(unsigned t=0;t<8;t++)if((engine.sound_locks&(1u<<t))&&engine.tracks[t].patch!=patches[t])*got=0;}
 }
 else if(!strcmp(what,"route_unique")){
   unsigned saved_page=page,saved_group=group,saved_song=song_tab,saved_mix=mix_tab,saved_setup=setup_tab,saved_sub=display_subtab,saved_drum=drum_tab,saved_air=air_tab,saved_debug=debug,saved_focus=mix_focus;
   *got=1;debug=0;mix_focus=0;
   for(unsigned p=PAGE_SOUND;p<=PAGE_AIR;p++){
     unsigned seen[128]={0};page=p;
     unsigned count=p==PAGE_SOUND?sizeof(sound_groups):p==PAGE_EVOLVE?7:p==PAGE_MIX?6:p==PAGE_DRUMS?3:p==PAGE_SETUP?6:2;
     for(unsigned tab=0;tab<count;tab++){
       if(p==PAGE_SOUND)group=sound_groups[tab];
       if(p==PAGE_EVOLVE)song_tab=tab;
       if(p==PAGE_MIX)mix_tab=tab;
       if(p==PAGE_DRUMS)drum_tab=tab;
       if(p==PAGE_SETUP){setup_tab=tab==4?0:tab>=5?tab-1:tab;display_subtab=tab==4;}
       if(p==PAGE_AIR)air_tab=tab;
       for(unsigned k=0;k<4;k++){
         unsigned id=control_for(k);printf("ROUTE %u %u %u %u\n",p,tab,k,id);
         if(id&&id!=99&&id!=100&&id<128&&seen[id]++)*got=0;
       }
     }
   }
   page=saved_page;group=saved_group;song_tab=saved_song;mix_tab=saved_mix;setup_tab=saved_setup;display_subtab=saved_sub;drum_tab=saved_drum;air_tab=saved_air;debug=saved_debug;mix_focus=saved_focus;dirty=1;
 }
 else if(!strcmp(what,"geometry_jobs"))*got=geometry_jobs;
 else if(!strcmp(what,"history_updates"))*got=history_updates;
 else if(!strcmp(what,"ui_blits"))*got=ui_blits;
 else if(!strcmp(what,"worker_jobs"))*got=worker_jobs;
 else if(!strcmp(what,"worker_waits"))*got=worker_waits;
 else if(!strcmp(what,"scope_samples"))*got=engine.scope_samples;
 else if(!strcmp(what,"reset_visual_counters")){geometry_jobs=history_updates=ui_blits=worker_jobs=worker_waits=engine.scope_samples=0;*got=1;}
 else if(!strcmp(what,"nodal_profiles")||!strcmp(what,"crystal_profiles")){
   *got=1;uint32_t signatures[SR_SCENES];
   memset(&vis_input,0,sizeof vis_input);vis_input.water=vis_input.full=1;vis_input.frequency=528;
   vis_input.meter[SR_BASS]=.04f;nodal_level=1;
   for(unsigned scene=0;scene<SR_SCENES;scene++){
     vis_input.scene=scene;resonance_job();uint32_t hash=vis_output.node_count;
     if(!hash||hash>256)*got=0;
     for(unsigned j=0;j<vis_output.node_count;j++)for(unsigned k=0;k<4;k++){
       int xy=vis_output.nodes[j][k];if(xy<0||xy>239)*got=0;
       hash=(hash^(uint16_t)xy)*16777619u;
     }
     for(unsigned prior=0;prior<scene;prior++)if(signatures[prior]==hash)*got=0;
     signatures[scene]=hash;
   }
   int16_t before[256][4];memcpy(before,vis_output.nodes,sizeof before);
   vis_input.frequency=852;resonance_job();if(!memcmp(before,vis_output.nodes,sizeof before))*got=0;
   memcpy(before,vis_output.nodes,sizeof before);vis_input.meter[SR_BASS]=.2f;vis_input.onset[SR_DRUMS]=1.f;
   resonance_job();if(!memcmp(before,vis_output.nodes,sizeof before))*got=0;
   memset(&vis_input,0,sizeof vis_input);nodal_level=0;resonance_job();if(vis_output.node_count)*got=0;
   dirty=1;
 }
 else if(!strcmp(what,"battery_level"))*got=battery_level;
 else if(!strcmp(what,"battery_fade"))*got=battery_fade;
 else if(!strcmp(what,"intro"))*got=intro_active;
 else if(!strcmp(what,"nature_bed"))*got=engine.bed_type;
 else if(!strcmp(what,"nature_style"))*got=engine.nature_style;
 else if(!strcmp(what,"preferences_saved"))*got=!preferences_dirty&&!preferences_saving;
 else if(!strcmp(what,"pending_scene"))*got=engine.pending_scene;
 else if(!strcmp(what,"drum_presence"))*got=engine.drum_presence;
 else if(!strcmp(what,"boot_drums"))*got=engine.drum_density==8&&engine.drum_presence==100&&engine.drum_level==1.f;
 else if(!strcmp(what,"settings_066_fixture")){
   aq_settings prior={0};prior.version=0x41513036u;prior.visual=1;prior.lighting=3;prior.font_size=1;prior.background=1;
   *got=plat_store_save(OBJ_SETTINGS,&prior,28)==0;
 }
 else if(!strcmp(what,"visual_snapshot")){
   unsigned hash=2166136261u;
   for(unsigned t=0;t<SR_TRACKS;t++)hash=(hash^(unsigned)(vis_input.meter[t]*1000000.f))*16777619u;
   printf("VIS %u %u %u %u %.5f %.5f\n",visual_mode,vis_input.phase,hash,vis_output.node_count,nodal_level,plate_activity);*got=1;
 }
 else if(!strcmp(what,"lighting"))*got=lighting;
 else if(!strcmp(what,"frame_interval"))*got=visual_interval_ms;
 else if(!strcmp(what,"fps_auto"))*got=fps_auto;
 else if(!strcmp(what,"performance_midi"))*got=AQ_PERFORMANCE_MIDI;
 else if(!strcmp(what,"midi_sync"))*got=midi.settings.sync;
 else if(!strcmp(what,"startup_chime"))*got=startup_chime;
 else if(!strcmp(what,"midi_received"))*got=midi.received;
 else if(!strcmp(what,"midi_diag")){
  printf("MIDI %u %u in%u ch%u role%u q%u/%u run%u mute%u vol%u stop%u\n",midi.received,midi.rejected,midi.settings.usb_in,midi.settings.rx_channel,midi.settings.source,qread,qwrite,engine.running,engine.tracks[midi.settings.source].mute,(unsigned)(engine.tracks[midi.settings.source].volume*100),engine.stop_frames);*got=1;
 }
 else if(!strcmp(what,"midi_clock_out"))*got=midi_clock_sent;
 else if(!strcmp(what,"midi_clock_exact"))*got=midi_clock_sent==1u+midi_position();
 else if(!strcmp(what,"midi_rx_channel"))*got=midi.settings.rx_channel;
 else if(!strcmp(what,"midi_tx_channel"))*got=midi.settings.tx_channel+1;

 else if(!strcmp(what,"midi_external_voices")){
  *got=0;for(unsigned v=0;v<SR_VOICES;v++)*got+=engine.voices[v].external&&engine.voices[v].gate;
 }
 else if(!strcmp(what,"midi_tx_active")){
  *got=0;for(unsigned n=0;n<128;n++)*got+=midi_tx_notes[n]!=0;
 }
 else if(!strcmp(what,"midi_steps"))*got=engine.sequence_step;
 else if(!strcmp(what,"usb_unplug")){midi_test_online=0;*got=1;}
 else if(!strcmp(what,"usb_replug")){midi_test_online=1;*got=1;}

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
 else if(!strcmp(what,"air_fx"))*got=engine.air_fx;
 else if(!strcmp(what,"air_depth"))*got=engine.air_depth;
 else if(!strcmp(what,"air_percent"))*got=engine.air_target*100.f+.5f;
 else if(!strcmp(what,"air_tab"))*got=air_tab;
 else if(!strcmp(what,"sound_picker"))*got=sound_picker;
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
 else if(!strcmp(what,"page"))*got=page;else if(!strcmp(what,"running"))*got=engine.running;else if(!strcmp(what,"binaural"))*got=engine.binaural_enabled;else if(!strcmp(what,"track"))*got=selected;else if(!strcmp(what,"perfect"))*got=engine.perfect_key;else if(!strcmp(what,"tracks"))*got=SR_TRACKS;else if(!strcmp(what,"lead_glide"))*got=engine.motion[SR_LEAD].glide_ms;else if(!strcmp(what,"bass_glide"))*got=engine.motion[SR_BASS].glide_ms;else if(!strcmp(what,"lead_vibrato"))*got=engine.motion[SR_LEAD].vibrato_depth;else if(!strcmp(what,"bass_vibrato"))*got=engine.motion[SR_BASS].vibrato_depth;else if(!strcmp(what,"drums"))*got=engine.drums_enabled;else if(!strcmp(what,"density"))*got=engine.drum_density;else if(!strcmp(what,"hit"))*got=engine.drum_hit;else if(!strcmp(what,"scene"))*got=engine.scene;else if(!strcmp(what,"visual"))*got=visual_mode;else if(!strcmp(what,"bank"))*got=bank_slot;else if(!strcmp(what,"bank_message"))*got=bank_message;else if(!strcmp(what,"saving"))*got=quick_started||quick_pending;else if(!strcmp(what,"knobs_valid")){
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
 *got=arrangement_read(bank_slot,&stored)&&arrangement_crc(&current)==arrangement_crc(&stored);
 }else if(!strcmp(what,"bank_guard")){
 arrangement bad;
 arrangement_snapshot(&bad);
 bad.melody[0]^=1;
 int corrupt=!arrangement_valid(&bad);
 bad.scene=255;bad.crc8=arrangement_crc(&bad);
 *got=corrupt&&!arrangement_valid(&bad);
 }else if(!strcmp(what,"bank_mask")){*got=0;for(unsigned i=0;i<AQ_BANKS;i++)*got|=(bank_status[i]==1)<<i;}else if(!strcmp(what,"worker"))*got=plat_cpu2_on();else if(!strcmp(what,"fallbacks"))*got=worker_fallbacks;else if(!strcmp(what,"drops"))*got=dropped;else return 0;return 1;}
#endif
