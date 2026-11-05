#include "serenity.h"
#include "../device/air_sensor.h"
#include <assert.h>
#include <stdio.h>

static sr_engine engine;
static int16_t pcm[512];

static void profiles(void)
{
    unsigned beds=0, kits=0, patches=0;
    for(unsigned scene=0;scene<SR_SCENES;scene++)
      for(unsigned style=0;style<4;style++)
        for(unsigned seed=1;seed<=32;seed++){
            sr_init(&engine,seed);
            engine.nature_style=style;
            sr_set_scene(&engine,scene);
            sr_generate_song(&engine,seed,0);
            assert(engine.bed_type>0&&engine.bed_type<SR_BEDS);
            beds|=1u<<engine.bed_type;
            kits|=1u<<engine.drum_kit;
            for(unsigned t=0;t<SR_DRUMS;t++)
                patches|=1u<<engine.tracks[t].patch;
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
    assert((patches&0x3f000000u)==0x3f000000u);
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
    puts("PASS 13 intentions x 4 styles x 32 seeds, manual/off locks, new sounds selected and stereo beds fade to silence");
    return 0;
}
