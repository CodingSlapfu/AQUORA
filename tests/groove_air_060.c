#include "serenity.h"
#include <assert.h>
#include <stdio.h>

static sr_engine engine;
static int16_t pcm[256];

static unsigned offset(unsigned mode,unsigned strength)
{
    sr_init(&engine,973);
    sr_set_scene(&engine,1);
    engine.groove_mode=mode;
    engine.groove_strength=strength;
    engine.variation=1.f;
    engine.phrase=1;
    engine.energy=1.f;
    sr_start(&engine);
    engine.tick_length=128;
    unsigned piano=engine.tracks[SR_PIANO].events;
    for(unsigned step=0;step<16;step++){
        sr_render(&engine,pcm,1);
        unsigned phase=engine.ornaments[0].phase;
        if(phase)return phase;
        sr_render(&engine,pcm,127);
    }
    assert(engine.tracks[SR_PIANO].events>piano);
    return 0;
}

int main(void)
{
    assert(offset(0,100)==10);
    assert(offset(0,50)==5);
    assert(offset(0,0)==0);
    assert(offset(1,100)==0);
    assert(offset(2,100)==12);
    sr_init(&engine,42);
    sr_start(&engine);
    engine.air_track=SR_DRUMS;
    float send=engine.tracks[SR_DRUMS].send;
    engine.air_target=1.f;
    sr_render(&engine,pcm,128);
    assert(engine.air_wet>0&&engine.air_wet<.1f);
    for(unsigned i=0;i<200;i++)sr_render(&engine,pcm,128);
    assert(engine.air_wet>.9f&&engine.air_wet<=1.f);
    engine.air_target=0.f;
    for(unsigned i=0;i<300;i++)sr_render(&engine,pcm,128);
    assert(engine.air_wet<.001f&&engine.tracks[SR_DRUMS].send==send);
    puts("PASS Intention/Straight/Swing offsets and strength; AIR ramps and returns without altering saved send");
}
