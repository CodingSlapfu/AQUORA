#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static double impulse(unsigned effect)
{
    sr_init(&engine,714);
    engine.vocal_reverb=0;
    if(effect==0)engine.vocal_chorus=80;
    if(effect==1)engine.vocal_reverb=80;
    if(effect==2)engine.vocal_delay=80;
    double energy=0;
    for(unsigned frame=0;frame<SR_RATE*12u;frame++){
        float left=0,right=0;
        vocal_effects(&engine,frame<8?.12f:0.f,frame,&left,&right);
        assert(fabsf(left)<1.f&&fabsf(right)<1.f);
        energy+=left*left+right*right;
    }
    assert(!engine.vocal_fx_tail);
    return energy;
}

static void verify_auto_profiles(void)
{
    sr_init(&engine,714);
    sr_set_vocals(&engine,1,4);
    engine.music_mode=SR_STANDARD;
    for(unsigned n=0;n<400;n++)vocal_production(&engine,64);
    float standard=engine.vocal_room_now;
    engine.music_mode=SR_CINEMATIC;
    for(unsigned n=0;n<400;n++)vocal_production(&engine,64);
    assert(engine.vocal_room_now>standard+15.f);
    engine.vocal_fx_manual=2;
    engine.vocal_reverb=30;
    for(unsigned n=0;n<400;n++)vocal_production(&engine,64);
    assert(fabsf(engine.vocal_room_now-30.f)<.01f);
    engine.vocal_fx_send=0;
    for(unsigned n=0;n<400;n++)vocal_production(&engine,64);
    assert(fabsf(engine.vocal_send_now-.65f)<.01f);
    puts("PASS vocal profiles, manual room lock and processed minimum send");
}

int main(void)
{
    for(unsigned effect=0;effect<3;effect++)assert(impulse(effect)>0.0001);
    assert(impulse(3)==0);
    sr_init(&engine,714);
    engine.vocals_enabled=1;
    engine.running=1;
    int chromatic=sr_tonic(&engine)+25;
    sr_voice *voice=recorded_note(&engine,chromatic,0,100,1.f);
    assert(voice&&voice->sample_zone);
    float dry=voice->freq;
    engine.vocal_tune=100;
    assert(vocal_pitch_target(&engine,voice)!=dry);
    verify_auto_profiles();
    puts("PASS independent vocal impulse responses, bypass and chromatic pitch correction");
}
