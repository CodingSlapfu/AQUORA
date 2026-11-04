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
static unsigned saved_status, graphics_us;
typedef struct {
    uint32_t version;
    sr_motion motion[2];
    uint8_t perfect_key;
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
static int bounded(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static float adjust(float x,int d,float step,float hi){return fm_clampf(x+d*step,0,hi);}
static void send(int kind,int value){unsigned next=(qwrite+1)&63;if(next==qread){dropped++;return;}queue[qwrite]=(command){kind,value,selected};__asm__ volatile("":::"memory");qwrite=next;}
static void consume(void){
 unsigned stop=stop_generation;
 unsigned play=requested_play;
 unsigned interrupted=stop!=applied_stop;
 if(interrupted){applied_stop=stop;sr_stop(&engine);}
 unsigned count=0;while(qread!=qwrite&&count++<64){command c=queue[qread];__asm__ volatile("":::"memory");qread=(qread+1)&63;sr_track *t=&engine.tracks[c.track];int d=c.value;
 state_revision++;
 switch(c.kind){

 case 1:sr_set_scene(&engine,bounded(engine.scene+d,0,SR_SCENES-1));break;
 case 2:if(c.track==SR_DRUMS)sr_set_drum_kit(&engine,bounded(engine.drum_kit+d,0,SR_DRUM_KITS-1));else sr_set_track_patch(&engine,c.track,bounded(t->patch+d,0,SR_PATCHES-1));break;
 case 3:if(!interrupted&&play&&(physical_keys&(1u<<d)))sr_key_down(&engine,c.track,(unsigned)d);break;
 case 4:sr_set_frequency(&engine,bounded(engine.frequency+d,0,SR_FREQUENCIES-1));break;
 case 5:engine.bed_level=adjust(engine.bed_level,d,.02f,1);break;
 case 6:sr_set_binaural(&engine,!engine.binaural_enabled);break;
 case 7:sr_set_binaural_rate(&engine,fm_clampf(engine.binaural_rate+d*.5f,.5f,40));break;
 case 8:sr_set_binaural_carrier(&engine,fm_clampf(engine.binaural_carrier+d*5.f,80,1000));break;
 case 9:sr_set_drums(&engine,!engine.drums_enabled);break;
 case 10:sr_set_drum_density(&engine,bounded(engine.drum_density+d,0,3));break;
 case 11:engine.reverb_amount=adjust(engine.reverb_amount,d,.02f,.8f);break;
 case 12:engine.delay_amount=adjust(engine.delay_amount,d,.02f,.7f);break;
 case 13:t->volume=adjust(t->volume,d,.03f,1);break;
 case 14:sr_set_tuning_432(&engine,!engine.tuning_432);break;
 case 15:t->mute=!t->mute;break;
 case 16:sr_set_drum_kit(&engine,bounded(engine.drum_kit+d,0,SR_DRUM_KITS-1));break;
 case 17:sr_set_drum_level(&engine,engine.drum_level+d*.02f);break;
 case 18:engine.energy=adjust(engine.energy,d,.05f,1);break;
 case 19:engine.variation=adjust(engine.variation,d,.05f,1);break;
 case 20:engine.freeze=!engine.freeze;break;
 case 21:engine.bed_type=bounded(engine.bed_type+d,0,7);break;
 case 22:engine.tone_level=adjust(engine.tone_level,d,.005f,.10f);break;
 case 23:engine.binaural_level=adjust(engine.binaural_level,d,.005f,.10f);break;
 case 24:t->pan=fm_clampf(t->pan+d*.05f,-1,1);break;
 case 25:engine.tick_length=bounded((int)engine.tick_length-d*80,5512,22050);break;
 case 27:sr_set_perfect_key(&engine,!engine.perfect_key);break;
 case 28:if(d<0)engine.solo=SR_TRACKS;else engine.solo=engine.solo==c.track?SR_TRACKS:c.track;break;
 case 29:t->send=adjust(t->send,d,.05f,1);break;
 case 30:engine.sacred_level=adjust(engine.sacred_level,d,.01f,.20f);break;
 case 31:engine.sacred_mode=bounded(engine.sacred_mode+d,0,2);break;
 case 32:if(c.track<=SR_BASS)engine.motion[c.track].glide_mode=bounded(engine.motion[c.track].glide_mode+d,0,2);break;
 case 33:if(c.track<=SR_BASS)engine.motion[c.track].glide_ms=bounded(engine.motion[c.track].glide_ms+10*d,10,800);break;
 case 34:if(c.track<=SR_BASS)engine.motion[c.track].vibrato_rate=bounded(engine.motion[c.track].vibrato_rate+d,5,80);break;
 case 36:engine.sacred_harmonics=adjust(engine.sacred_harmonics,d,.025f,1);break;
 case 35:if(c.track<=SR_BASS)engine.motion[c.track].vibrato_depth=bounded(engine.motion[c.track].vibrato_depth+d,0,50);break;
 }
 }
 for(unsigned i=0;i<SR_VOICES;i++){
   sr_voice *v=&engine.voices[i];
   if(v->key&&!(physical_keys&(1u<<(v->key-1)))){v->gate=0;v->key=0;}
 }
 if(play&&!engine.running&&!engine.restart){sr_start(&engine);state_revision++;}
 if(!play&&engine.running){sr_stop(&engine);state_revision++;}
}
void app_init(void)
{
    sr_init(&engine,7301);
    engine.clock_ticks=plat_ticks24;
    aq_settings settings;
    if(plat_store_load(OBJ_SETTINGS,&settings,sizeof(settings))==(int)sizeof(settings)&&settings.version==0x41513032u){
      for(unsigned i=0;i<2;i++){
        sr_motion m=settings.motion[i];
        if(m.glide_mode<=2&&m.glide_ms>=10&&m.glide_ms<=800&&
           m.vibrato_rate>=5&&m.vibrato_rate<=80&&m.vibrato_depth<=50)engine.motion[i]=m;
      }
      engine.perfect_key=!!settings.perfect_key;
    }
    visual_init();
    plat_cpu2_set(1);
    output_gain=0;
    qread=qwrite=0;
    requested_play=stop_generation=applied_stop=physical_keys=0;
    page=selected=group=debug=0;
    oldkeys=oldbuttons=0;
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
 if(press&(1u<<B_PLAY)){
   unsigned flags=plat_lock();
   requested_play=!requested_play;
   if(!requested_play)stop_generation++;
   plat_unlock(flags);
 }
 if(press&(1u<<B_HOME)){page=0;debug=0;}
 if(press&(1u<<B_SEL))send(15,0);
 if(press&(1u<<B_ARP))send(9,0);
 if(press&(1u<<B_GLO))debug=(debug+1)%3;
 if(press&(1u<<B_REC))plat_cpu2_set(0);
 if(press&(1u<<B_ENV))send(14,0);
 if(press&(1u<<B_FX))send(6,0);
 if(press&(1u<<B_SEQ))send(20,0);
 if(press&(1u<<B_EDIT))group=(group+1)%8;
 if(press&(1u<<B_LFO))send(debug?27:28,0);
 if((press&(1u<<B_SAVE))&&!requested_play&&!engine.running&&!engine.stop_frames){
    aq_settings settings={0};
    unsigned flags=plat_lock();
    settings.version=0x41513032u;
    settings.motion[0]=engine.motion[0];settings.motion[1]=engine.motion[1];
    settings.perfect_key=engine.perfect_key;
    plat_unlock(flags);
    saved_status=plat_store_save(OBJ_SETTINGS,&settings,sizeof(settings))==0?1:2;
 }
 physical_keys=k;
 for(unsigned i=0;i<NKEYS;i++){
   if(!(press&(1u<<B_PLAY))&&requested_play&&((k&~oldkeys)&(1u<<i)))send(3,i);
 }
 oldkeys=k;
 int d=plat_enc(EN_ALGO);if(d){page=bounded((int)page+d,0,3);dirty=1;}
 d=plat_enc(EN_SELECT);if(d){selected=bounded((int)selected+d,0,7);dirty=1;}
 d=plat_enc(EN_PRESET);if(d){send(page==0?1:2,d>0?1:-1);dirty=1;}
 static const int sound[8][4]={{2,13,24,29},{21,5,11,12},{4,22,14,18},{8,7,23,6},{16,17,10,9},{30,31,36,22},{32,33,34,35},{13,24,29,28}};
 static const int controls[4][4]={{1,18,22,5},{0,0,0,0},{19,20,25,18},{13,24,11,12}};
 for(int i=0;i<4;i++){d=plat_enc(EN_K1+i);if(d){send(page==1?sound[group][i]:controls[page][i],bounded(d,-8,8));dirty=1;}}
 if(press)dirty=1;
 plat_leds((engine.running?(1u<<B_PLAY):0)|(engine.freeze?(1u<<B_SEQ):0),k);
}
#include "ui.c"
void app_ui_init(void){draw();}
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
      window_ms=now;
      if(debug)dirty=1;
    }
    if(now-lastdraw>=(debug?240u:plat_cpu_pct()>80?125u:plat_cpu_pct()>60?70u:34u)){
      lastdraw=now;
      if(displayed_revision!=state_revision)dirty=1;
      if(debug&&!dirty)return;
      unsigned begin=plat_ticks24();
      if(!debug)prepare_geometry();
      draw();
      graphics_us=(plat_ticks24()-begin)/24;
    }
}
#ifdef PLAT_HOST
int app_expect(const char *what,int *got){if(!strcmp(what,"page"))*got=page;else if(!strcmp(what,"running"))*got=engine.running;else if(!strcmp(what,"binaural"))*got=engine.binaural_enabled;else if(!strcmp(what,"track"))*got=selected;else if(!strcmp(what,"perfect"))*got=engine.perfect_key;else if(!strcmp(what,"tracks"))*got=SR_TRACKS;else if(!strcmp(what,"lead_glide"))*got=engine.motion[SR_LEAD].glide_ms;else if(!strcmp(what,"bass_glide"))*got=engine.motion[SR_BASS].glide_ms;else if(!strcmp(what,"lead_vibrato"))*got=engine.motion[SR_LEAD].vibrato_depth;else if(!strcmp(what,"bass_vibrato"))*got=engine.motion[SR_BASS].vibrato_depth;else if(!strcmp(what,"worker"))*got=plat_cpu2_on();else if(!strcmp(what,"fallbacks"))*got=worker_fallbacks;else if(!strcmp(what,"drops"))*got=dropped;else return 0;return 1;}
#endif
