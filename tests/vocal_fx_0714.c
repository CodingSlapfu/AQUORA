#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static double impulse(unsigned effect)
{
    sr_init(&engine,714);
    if(effect==0)engine.vocal_chorus=80;
    if(effect==1)engine.vocal_reverb=80;
    if(effect==2)engine.vocal_delay=80;
    double energy=0;
    for(unsigned frame=0;frame<SR_RATE*3u;frame++){
        float left=0,right=0;
        vocal_effects(&engine,frame<8?.12f:0.f,frame,&left,&right);
        assert(fabsf(left)<1.f&&fabsf(right)<1.f);
        energy+=left*left+right*right;
    }
    assert(!engine.vocal_fx_tail);
    return energy;
}

int main(void)
{
    for(unsigned effect=0;effect<3;effect++)assert(impulse(effect)>0.0001);
    assert(impulse(3)==0);
    sr_init(&engine,714);
    engine.running=1;
    int chromatic=sr_tonic(&engine)+25;
    sr_voice *voice=recorded_note(&engine,chromatic,0,100,1.f);
    assert(voice&&voice->sample_zone);
    float dry=voice->freq;
    engine.vocal_tune=100;
    assert(vocal_pitch_target(&engine,voice)!=dry);
    puts("PASS independent vocal impulse responses, bypass and chromatic pitch correction");
}
