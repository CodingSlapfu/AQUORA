#include "serenity.h"
#include <assert.h>
#include <stdio.h>

static sr_engine engine;
static int16_t samples[128];

int main(void)
{
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        for(unsigned kit=0;kit<SR_DRUM_KITS;kit++){
            unsigned previous=0;
            for(unsigned density=0;density<=SR_DENSITY_MAX;density++){
                sr_init(&engine,841);
                sr_set_scene(&engine,scene);
                sr_set_drum_kit(&engine,kit);
                sr_set_loop_length(&engine,64);
                sr_set_drum_density(&engine,density);
                engine.drum_hit=0;
                sr_start(&engine);
                engine.tick_length=64;
                for(unsigned step=0;step<64;step++)sr_render(&engine,samples,64);
                unsigned count=engine.tracks[SR_DRUMS].events;
                if(density)assert(count>previous);
                else assert(!count);
                previous=count;
            }
        }
    }
    sr_init(&engine,4);
    sr_set_loop_length(&engine,32);
    engine.keyboard_mode=1;
    sr_start(&engine);
    engine.tick_length=64;
    sr_render(&engine,samples,64);
    sr_key_down(&engine,SR_LEAD,8);
    sr_key_up(&engine,8);
    assert(engine.pending_physical_key==9);
    sr_key_down(&engine,SR_LEAD,20);
    assert(engine.pending_physical_key==21);
    for(unsigned step=1;step<32;step++){
        sr_render(&engine,samples,64);
        assert(engine.pending_physical_key==21);
    }
    sr_render(&engine,samples,64);
    assert(!engine.pending_physical_key&&!engine.pending_key);
    puts("PASS increasing density 0–8 across 13 scenes and 15 kits; pending physical key replacement and loop commit");
}
