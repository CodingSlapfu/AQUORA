#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static sr_engine engine;
static int16_t pcm[256];

int main(void)
{
    sr_init(&engine,973);
    sr_set_scene(&engine,1);
    sr_set_loop_length(&engine,32);
    engine.variation=1.f;
    engine.energy=1.f;
    sr_start(&engine);
    engine.tick_length=128;
    for(unsigned i=0;i<128;i++)sr_render(&engine,pcm,128);
    assert(engine.phrase==3);
    sr_render(&engine,pcm,128);
    assert(engine.phrase==4);
    assert(!memcmp(engine.melody,engine.motif_home,sizeof engine.melody));
    unsigned heard=0;
    for(unsigned step=0;step<32&&!heard;step++){
        sr_render(&engine,pcm,1);
        for(unsigned slot=0;slot<2;slot++){
            unsigned phase=engine.ornaments[slot].phase;
            if(!phase)continue;
            assert(phase==10);
            unsigned role=slot?SR_GUITAR:SR_PIANO;
            unsigned events=engine.tracks[role].events;
            sr_render(&engine,pcm,phase-1);
            assert(engine.tracks[role].events==events);
            sr_render(&engine,pcm,1);
            assert(engine.tracks[role].events==events+1);
            heard=1;
            break;
        }
        if(!heard)sr_render(&engine,pcm,127);
    }
    assert(heard);
    unsigned phrase=engine.phrase;
    engine.freeze=1;
    for(unsigned i=0;i<128;i++)sr_render(&engine,pcm,128);
    assert(engine.phrase==phrase);
    puts("PASS returning motif, sample-clock Flow State ornament offset and Freeze");
}
