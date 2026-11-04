/* SPDX-License-Identifier: MPL-2.0
 * Original Sound Healing application. Audio owns engine state; UI produces a bounded queue. */
#include "serenity.h"
#include "fastmath.h"
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
static unsigned mix_tab,mix_reset, song_tab, song_nonce;
static unsigned drum_tab, setup_tab, sound_picker;
enum { PAGE_PLAY, PAGE_SOUND, PAGE_EVOLVE, PAGE_MIX, PAGE_DRUMS, PAGE_SETUP };
static const uint8_t sound_groups[]={0,6,1,2,3,5,7,11,13};

static unsigned lighting, fps_auto, frames, missed, shown_fps10, shown_missed;
static unsigned last_xruns, xrun_delta, frame_us;
static unsigned knob_focus, knob_until, redraw_ui_ms, shown_step;
enum { GROUP_PERC=4, GROUP_HIT=8, GROUP_VISUAL=9, GROUP_BANKS=10, GROUP_ISOLATE=11, GROUP_KEYS=12, GROUP_COUNT=14 };
static const uint8_t sound_controls[GROUP_COUNT][4]={
 {2,13,24,29},{21,5,54,12},{4,22,48,45},{8,7,23,6},{16,17,10,9},
 {30,31,36,22},{32,33,34,35},{13,24,29,28},
 {37,17,10,9},{38,56,0,0},{39,40,41,38},{43,4,22,31},{47,46,0,0},{55,14,48,45}
};
static const uint8_t page_controls[4][4]={
 {1,18,22,5},{0,0,0,0},{19,20,25,46},{13,24,29,49}
};
#include "arrangement.c"
typedef struct {
    uint32_t version;
    sr_motion motion[2];
    uint8_t perfect_key, lighting, fps_auto;
} aq_settings;

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
static unsigned control_for(unsigned knob)
{
 if(debug)return knob==0?(debug==3?42:debug==1?57:0):0;
 if(page==PAGE_DRUMS){
   static const uint8_t controls[][4]={{9,10,37,17},{16,13,24,29},{49,28,0,0}};
   return controls[drum_tab][knob];
 }
 if(page==PAGE_SETUP)return sound_controls[setup_tab?GROUP_BANKS:GROUP_VISUAL][knob];
 if(page==PAGE_SOUND&&sound_picker)return 0;
 if(page==2&&song_tab){static const uint8_t ids[]={58,59,60,61};return ids[knob];}
 if(page==3&&mix_tab){
   static const uint8_t master_controls[]={11,12,50,51};
   static const uint8_t reset_controls[]={52,0,0,0};
   return mix_tab==1?master_controls[knob]:reset_controls[knob];
 }
 return page==1?sound_controls[group][knob]:page_controls[page][knob];
}
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
   if(next!=engine.scene){
     sr_set_scene(&engine,next);
     sr_generate_song(&engine,plat_ticks24()^++song_nonce,0);
   }
   break;
 }
 case 2:if(c.track==SR_DRUMS)sr_set_drum_kit(&engine,bounded(engine.drum_kit+d,0,SR_DRUM_KITS-1));else sr_set_track_patch(&engine,c.track,bounded(t->patch+d,0,SR_PATCHES-1));break;
 case 3:if(!interrupted&&play&&(physical_keys&(1u<<d)))sr_key_down(&engine,c.track,(unsigned)d);break;
 case 4:sr_set_frequency(&engine,bounded(engine.frequency+d,0,SR_FREQUENCIES-1));break;
 case 5:engine.bed_level=adjust(engine.bed_level,d,.02f,1);break;
 case 6:sr_set_binaural(&engine,switched(engine.binaural_enabled,d));break;
 case 7:sr_set_binaural_rate(&engine,fm_clampf(engine.binaural_rate+d*.5f,.5f,40));break;
 case 8:sr_set_binaural_carrier(&engine,fm_clampf(engine.binaural_carrier+d*5.f,80,1000));break;
 case 9:drums_enable(switched(engine.drums_enabled,d));break;
 case 10:sr_set_drum_density(&engine,bounded(engine.drum_density+d,0,SR_DENSITY_MAX));break;
 case 11:engine.reverb_amount=adjust(engine.reverb_amount,d,.02f,.8f);break;
 case 12:engine.delay_amount=adjust(engine.delay_amount,d,.02f,.7f);break;
 case 13:t->volume=adjust(t->volume,d,.03f,1);break;
 case 14:sr_set_tuning_432(&engine,switched(engine.tuning_432,d));break;
 case 15:t->mute=!t->mute;break;
 case 16:sr_set_drum_kit(&engine,bounded(engine.drum_kit+d,0,SR_DRUM_KITS-1));break;
 case 17:sr_set_drum_level(&engine,engine.drum_level+d*.02f);break;
 case 18:engine.energy=adjust(engine.energy,d,.05f,1);break;
 case 19:engine.variation=adjust(engine.variation,d,.05f,1);break;
 case 20:engine.freeze=switched(engine.freeze,d);break;
 case 21:engine.bed_type=bounded(engine.bed_type+d,0,7);engine.sound_locks|=SR_SOUND_NATURE;break;
 case 22:engine.tone_level=adjust(engine.tone_level,d,.005f,.10f);break;
 case 23:engine.binaural_level=adjust(engine.binaural_level,d,.005f,.10f);break;
 case 24:t->pan=fm_clampf(t->pan+d*.05f,-1,1);break;
 case 25:engine.tick_length=bounded((int)engine.tick_length-d*80,5512,22050);break;
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
 case 61:if(d>0)engine.sound_locks|=SR_SOUND_NATURE|SR_SOUND_FX;else engine.sound_locks&=~(SR_SOUND_NATURE|SR_SOUND_FX);break;
 case 54:engine.nature_audition=engine.running&&!engine.pending_lock&&d>0?SR_RATE*4u:0;break;
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
    aq_settings settings;
    if(plat_store_load(OBJ_SETTINGS,&settings,sizeof(settings))==(int)sizeof(settings)&&settings.version==0x41513032u){
      for(unsigned i=0;i<2;i++){
        sr_motion m=settings.motion[i];
        if(m.glide_mode<=2&&m.glide_ms>=10&&m.glide_ms<=800&&
           m.vibrato_rate>=5&&m.vibrato_rate<=80&&m.vibrato_depth<=50)engine.motion[i]=m;
      }
      engine.perfect_key=!!settings.perfect_key;
      lighting=settings.lighting<=3?settings.lighting:0;
      fps_auto=!!settings.fps_auto;
    }
    arrangement_init();
    visual_init();
    plat_cpu2_set(1);
    output_gain=0;
    qread=qwrite=0;
    requested_play=stop_generation=applied_stop=physical_keys=0;
    page=selected=group=debug=0;
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
    for(unsigned i=0;i<n;i++){
      output_gain+=(target-output_gain)*.002f;
      lr[2*i]=(int32_t)(pcm[2*i]*256.f*output_gain);
      lr[2*i+1]=(int32_t)(pcm[2*i+1]*256.f*output_gain);
    }
    unsigned total=plat_ticks24()-begin;
    unsigned fx=engine.fx_ticks<total?engine.fx_ticks:total;
    measure(&audio_timing,(total-fx)/24);
    measure(&fx_timing,fx/24);
    measure(&callback_timing,total/24);
}
void app_say(const char*a,const char*b){(void)a;(void)b;dirty=1;}
void app_input_only(void){unsigned b=plat_buttons(),k=plat_keys(),press=b&~oldbuttons;oldbuttons=b;
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
   requested_play=!requested_play;
   if(!requested_play)stop_generation++;
   plat_unlock(flags);
 }
 if(press&(1u<<B_HOME)){page=0;debug=0;bank_confirm=0;send(44,0);}
 if(press&(1u<<B_SEL))send(15,0);
 if(press&(1u<<B_ARP)){bank_confirm=0;page=PAGE_DRUMS;drum_tab=0;debug=0;}
 if(press&(1u<<B_GLO))debug=(debug+1)%4;
 if(press&(1u<<B_REC))plat_cpu2_set(0);
 if(press&(1u<<B_ENV))send(14,0);
 if(press&(1u<<B_FX))send(6,0);
 if(debug!=3&&(press&(1u<<B_SEQ)))send(20,0);
 if(press&(1u<<B_EDIT)){
   if(!debug&&page==0)send(47,0);
   if(!debug&&page==2)song_tab=!song_tab;
   if(page==PAGE_SOUND)sound_picker=!sound_picker;
   if(page==PAGE_DRUMS)drum_tab=(drum_tab+1)%3;
   if(page==PAGE_SETUP)setup_tab=!setup_tab;
   if(page==3){mix_tab=(mix_tab+1)%3;mix_reset=0;}
   bank_confirm=0;bank_message=BANK_IDLE;
 }
 /* OCT- opens the arrangement page. OCT+ jumps between percussion and HIT. */
 if((press&(1u<<B_OCTDN))&&!(b&(1u<<B_OCTUP))){page=PAGE_SETUP;setup_tab=1;debug=0;}
 if((press&(1u<<B_OCTUP))&&!(b&(1u<<B_OCTDN))){bank_confirm=0;drum_tab=page==PAGE_DRUMS?(drum_tab+1)%3:0;page=PAGE_DRUMS;debug=0;}
 if(press&(1u<<B_LFO))send(debug?27:28,0);
 if((press&(1u<<B_SAVE))&&page==3&&mix_tab==2&&!debug){
    if(mix_reset)send(53,0);
    mix_reset=0;
 }else if((press&(1u<<B_SAVE))&&page==PAGE_SETUP&&setup_tab&&!debug){
    arrangement_execute();
    blocked_keys|=k;
 }else if((press&(1u<<B_SAVE))&&!requested_play&&!engine.running&&!engine.stop_frames){
    aq_settings settings={0};
    unsigned flags=plat_lock();
    settings.version=0x41513032u;
    settings.motion[0]=engine.motion[0];settings.motion[1]=engine.motion[1];
    settings.perfect_key=engine.perfect_key;
    settings.lighting=lighting;settings.fps_auto=fps_auto;
    plat_unlock(flags);
    saved_status=plat_store_save(OBJ_SETTINGS,&settings,sizeof(settings))==0?1:2;
 }
 blocked_keys&=k;
 physical_keys=k&~blocked_keys;
 for(unsigned i=0;i<NKEYS;i++){
   if(!(press&(1u<<B_PLAY))&&requested_play&&((physical_keys&~oldkeys)&(1u<<i)))send(3,i);
 }
 oldkeys=k;
 int d=plat_enc(EN_ALGO);
 if(d&&!debug){page=bounded((int)page+d,PAGE_PLAY,PAGE_SETUP);sound_picker=0;mix_reset=0;bank_confirm=0;knob_until=0;dirty=1;}
 d=plat_enc(EN_SELECT);
 if(d&&!debug){
   if(page==PAGE_SOUND&&sound_picker){
     int index=0;
     for(unsigned i=0;i<sizeof(sound_groups);i++)if(sound_groups[i]==group)index=(int)i;
     group=sound_groups[bounded(index+d,0,sizeof(sound_groups)-1)];
   }else if(page!=PAGE_DRUMS)selected=bounded((int)selected+d,0,7);
   knob_until=0;dirty=1;
 }
 d=plat_enc(EN_PRESET);
 if(d&&!debug&&page!=PAGE_SETUP&&!(page==PAGE_SOUND&&sound_picker)){send(page==0?1:page==PAGE_DRUMS?16:2,d>0?1:-1);dirty=1;}
 for(unsigned i=0;i<4;i++){
    d=plat_enc(EN_K1+i);
    unsigned control=control_for(i);
    if(!d||!control)continue;
    if(control==38)visual_mode=d>0;
    else if(control==39){bank_slot=bounded((int)bank_slot+d,0,3);bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==40){bank_action=d>0;bank_confirm=0;bank_message=BANK_IDLE;}
    else if(control==41)bank_confirm=d>0;
    else if(control==52)mix_reset=d>0;
    else if(control==56)lighting=bounded((int)lighting+d,0,3);
    else if(control==57)fps_auto=switched(fps_auto,d);
    else if(control==42)voice_slot=bounded((int)voice_slot+d,0,SR_VOICES-1);
    else send(control,bounded(d,-8,8));
    knob_focus=i;
    knob_until=plat_ms()+900;
    dirty=1;
 }
 if(press)dirty=1;
 unsigned buttons=(engine.running?(1u<<B_PLAY):0)|(engine.freeze?(1u<<B_SEQ):0);
 unsigned pending=engine.pending_physical_key;
 unsigned lights=k;
 if(pending&&pending<=NKEYS)lights|=1u<<(pending-1);
 if(lighting==3&&engine.running)
   for(unsigned t=0;t<SR_TRACKS;t++)
     if(engine.tracks[t].meter>.045f)
       lights|=1u<<((engine.tracks[t].events+t*3u)%NKEYS);
 plat_leds(lighting==1||lighting==2?0:buttons,lighting==2?0:lights);
}
#include "ui.c"
enum { FRAME_MIN_MS = 50 };
void app_ui_init(void){draw(1);redraw_ui_ms=lastdraw=plat_ms();}
void app_frame(void)
{
    app_input_only();
    unsigned now=plat_ms();
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
    if(now-redraw_ui_ms<FRAME_MIN_MS)return;
    unsigned elapsed=now-lastdraw;
    int animate=!debug&&(page==PAGE_PLAY||page==PAGE_MIX)&&elapsed>=FRAME_MIN_MS;
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
    if(animate||dirty){
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
 else if(!strcmp(what,"drum_tab"))*got=drum_tab;
 else if(!strcmp(what,"sound_group"))*got=group;
 else if(!strcmp(what,"mix_tab"))*got=mix_tab;
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
