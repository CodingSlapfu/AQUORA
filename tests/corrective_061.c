#include "serenity.h"
#include "../device/air_sensor.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static sr_engine engine, reference;
static int16_t pcm[512];

static void profiles(void)
{
    unsigned beds=0, kits=0;
    uint64_t patches=0;
    for(unsigned mode=0;mode<3;mode++)
    for(unsigned scene=0;scene<SR_SCENES;scene++)
      for(unsigned style=0;style<4;style++)
        for(unsigned seed=1;seed<=32;seed++){
            sr_init(&engine,seed);
            sr_set_music_mode(&engine,mode);
            engine.nature_style=style;
            sr_set_scene(&engine,scene);
            sr_generate_song(&engine,seed,0);
            assert(engine.bed_type>0&&engine.bed_type<SR_BEDS);
            beds|=1u<<engine.bed_type;
            kits|=1u<<engine.drum_kit;
            for(unsigned t=0;t<SR_DRUMS;t++)
                patches|=UINT64_C(1)<<engine.tracks[t].patch;
            unsigned before=engine.bed_type;
            sr_select_nature(&engine,seed);
            assert(engine.bed_type==before);
            engine.sound_locks|=SR_SOUND_NATURE;
            engine.bed_type=6;
            sr_generate_song(&engine,seed+1,0);
            assert(engine.bed_type==6);
            engine.bed_type=0;
            sr_set_scene(&engine,(scene+1)%SR_SCENES);
            sr_generate_song(&engine,seed+2,0);
            assert(engine.bed_type==0);
        }
    assert((beds&0xf00u)==0xf00u);
    assert((kits&0x78000u)==0x78000u);
    assert((patches&UINT64_C(0xfff000000))==UINT64_C(0xfff000000));
}

static void nature_render(void)
{
    for(unsigned bed=1;bed<SR_BEDS;bed++){
        sr_init(&engine,820);
        engine.bed_type=bed;
        engine.bed_level=.85f;
        engine.isolate=SR_ISOLATE_NATURE;
        engine.reverb_amount=engine.delay_amount=0;
        sr_start(&engine);
        unsigned stereo=0, sounding=0;
        for(unsigned b=0;b<350;b++){
            sr_render(&engine,pcm,256);
            for(unsigned i=0;i<512;i+=2){
                stereo+=pcm[i]!=pcm[i+1];
                sounding+=pcm[i]!=0;
            }
        }
        assert(stereo>100&&sounding>100);
        engine.bed_type=0;
        for(unsigned b=0;b<100;b++)sr_render(&engine,pcm,256);
        for(unsigned i=0;i<512;i++)assert(pcm[i]==0);
    }
}

static void evolution(void)
{
    static const float intensity[]={0.f,.25f,.55f,.85f};
    for(unsigned scene=0;scene<SR_SCENES;scene++)
      for(unsigned length=16;length<=64;length*=2)
        for(unsigned seed=1;seed<=2;seed++){
          unsigned previous=0;
          for(unsigned level=0;level<4;level++){
            sr_init(&engine,seed);
            sr_set_scene(&engine,scene);
            sr_set_loop_length(&engine,length);
            engine.variation=intensity[level];
            engine.bed_level=engine.reverb_amount=engine.delay_amount=0;
            sr_start(&engine);
            engine.tick_length=16;
            uint32_t last[SR_TRACKS]={0};
            unsigned loops=96;
            for(unsigned loop=0;loop<loops;loop++){
              for(unsigned step=0;step<length;step++)sr_render(&engine,pcm,16);
              if(level==0){
                for(unsigned role=0;role<SR_TRACKS;role++){
                  unsigned count=engine.tracks[role].events;
                  if(loop==0)last[role]=count;
                  else assert(count==last[role]*(loop+1u));
                }
              }
            }
            if(level==0)assert(engine.phrase==0);
            else assert(engine.phrase>previous);
            previous=engine.phrase;
            unsigned phrase=engine.phrase;
            engine.freeze=1;
            for(unsigned step=0;step<length*8u;step++)sr_render(&engine,pcm,16);
            assert(engine.phrase==phrase);
          }
        }
}

static void telemetry(void)
{
    int16_t other[512];
    sr_init(&engine,777);
    sr_start(&engine);
    reference=engine;
    engine.scope_track=SR_BASS;
    unsigned visible=0;
    for(unsigned b=0;b<40;b++){
        sr_render(&engine,pcm,256);
        sr_render(&reference,other,256);
        assert(!memcmp(pcm,other,sizeof pcm));
        for(unsigned i=0;i<128;i++)visible+=engine.scope[i]!=0;
    }
    assert(visible>100);
    engine.tracks[SR_BASS].mute=1;
    for(unsigned b=0;b<4;b++)sr_render(&engine,pcm,256);
    for(unsigned i=0;i<128;i++)assert(!engine.scope[i]);
}

static void stopped_audition(void)
{
    for(unsigned role=0;role<SR_TRACKS;role++){
        sr_init(&engine,223);
        engine.tracks[role].patch=24u+role%6u;
        engine.drum_kit=15;
        sr_preview_sound(&engine,role);
        unsigned sounding=0;
        for(unsigned b=0;b<190;b++){
            sr_render(&engine,pcm,256);
            for(unsigned i=0;i<512;i++)sounding+=pcm[i]!=0;
            assert(!engine.running&&!engine.sequence_step);
        }
        assert(sounding>200&&!engine.preview_frames);
        for(unsigned i=0;i<512;i++)assert(!pcm[i]);
    }
}

static void air_signal(void)
{
    aq_air_sensor sensor={0};
    assert(!aq_air_calibrate(&sensor,0));
    assert(!aq_air_update(&sensor,200,5,80));
    assert(aq_air_calibrate(&sensor,80));
    unsigned target=0;
    for(unsigned t=160;t<=640;t+=80)target=aq_air_update(&sensor,182,7,t);
    assert(target>70);
    assert(!aq_air_update(&sensor,0,0,1440));
    assert(!aq_air_calibrate(&sensor,1440));
    assert(!aq_air_update(&sensor,200,6,1520));
}

int main(void)
{
    profiles();
    nature_render();
    air_signal();
    telemetry();
    evolution();
    stopped_audition();
    puts("PASS sound profiles, stereo beds, AIR filtering, role scope, evolution levels and stopped audition");
    return 0;
}
