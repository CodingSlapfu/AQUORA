#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static sr_engine engine;
static int16_t pcm[128];

int main(void)
{
    for(unsigned steps=16;steps<=64;steps*=2){
        sr_init(&engine,701);
        sr_set_loop_length(&engine,steps);
        engine.variation=1.f;
        sr_start(&engine);
        engine.tick_length=64;
        unsigned boundary=steps==16?32:steps;
        int8_t original[sizeof engine.melody];
        memcpy(original,engine.melody,sizeof original);
        for(unsigned i=0;i<boundary;i++)sr_render(&engine,pcm,64);
        assert(!engine.phrase&&!memcmp(original,engine.melody,sizeof original));
        sr_render(&engine,pcm,64);
        assert(engine.phrase==1&&memcmp(original,engine.melody,sizeof original));
        memcpy(original,engine.melody,sizeof original);
        engine.freeze=1;
        for(unsigned i=0;i<boundary*3;i++)sr_render(&engine,pcm,64);
        assert(engine.phrase==1&&!memcmp(original,engine.melody,sizeof original));
    }
    puts("PASS motif evolves on the scheduled 16/32/64-step boundary and Freeze prevents changes");
}
