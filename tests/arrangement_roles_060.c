#include "serenity.h"
#include <assert.h>
#include <stdio.h>

static sr_engine engine;
static int16_t pcm[128];

int main(void)
{
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        for(unsigned length=16;length<=64;length*=2){
            sr_init(&engine,430);
            sr_set_scene(&engine,scene);
            sr_set_loop_length(&engine,length);
            engine.variation=1.f;
            engine.energy=.95f;
            sr_start(&engine);
            engine.tick_length=64;
            unsigned initial_frequency=engine.frequency;
            for(unsigned step=0;step<64*length;step++)
                sr_render(&engine,pcm,64);
            assert(engine.frequency==initial_frequency);
            assert(engine.phrase==(length==16?31:63));
            for(unsigned role=0;role<SR_TRACKS;role++)
                assert(engine.tracks[role].events);
            unsigned phrase=engine.phrase;
            engine.freeze=1;
            for(unsigned step=0;step<4*length;step++)
                sr_render(&engine,pcm,64);
            assert(engine.phrase==phrase);
        }
    }
    puts("PASS 64 loops in all 13 intentions and 3 lengths, every role active, reference stable, Freeze holds phrase");
}
