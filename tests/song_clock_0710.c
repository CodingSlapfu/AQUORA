#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine songs[3];

int main(void)
{
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        for(unsigned scene=0;scene<SR_SCENES;scene++){
            for(unsigned i=0;i<3;i++){
                sr_engine *e=&songs[i];
                sr_init(e,830);
                sr_set_scene(e,scene);
                sr_set_music_mode(e,mode);
                sr_generate_song(e,30710,0);
                sr_set_loop_length(e,16u<<i);
                sr_start(e);
            }
            unsigned visited=0,changes=0;
            for(unsigned step=0;step<18000;step++){
                if(step==8000)sr_set_loop_length(&songs[2],32);
                unsigned before=sr_arrange_section(&songs[0],0);
                for(unsigned i=0;i<3;i++){
                    sr_engine *e=&songs[i];
                    trigger(e);
                    e->frame_count+=e->tick_length;
                    memset(e->scheduled,0,sizeof e->scheduled);
                    assert(sr_arrange_section(e,0)==sr_arrange_section(&songs[0],0));
                    assert(sr_section_remaining(e)==sr_section_remaining(&songs[0]));
                }
                unsigned section=sr_arrange_section(&songs[0],0);
                visited|=1u<<sr_sections[section].family;
                changes+=section!=before;
            }
            assert(songs[0].frame_count/SR_RATE>=30u*60u);
            assert(visited==31u&&changes>75u);
        }
    }
    puts("PASS 88 independently timed 30+ minute arrangement scores at 16/32/64 steps");
}
