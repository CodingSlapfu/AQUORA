#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

int main(void)
{
    static const unsigned limits[][2]={{45,65},{45,65},{75,105},{75,105},{105,165}};
    sr_init(&engine,714);
    assert(engine.vocals_enabled);
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        sr_set_music_mode(&engine,mode);
        for(unsigned scene=0;scene<SR_SCENES;scene++){
            sr_set_scene(&engine,scene);
            unsigned previous=engine.tick_length;
            for(unsigned seed=1;seed<16;seed++){
                sr_generate_song(&engine,seed,0);
                unsigned bpm=(SR_RATE*15u+engine.tick_length/2u)/engine.tick_length;
                assert(bpm>=limits[mode][0]&&bpm<=limits[mode][1]);
                assert(engine.tick_length!=previous);
                previous=engine.tick_length;
            }
            sr_set_auto_tempo(&engine,0);
            sr_set_rhythm_style(&engine,(engine.rhythm_style+1u)%3u);
            assert(engine.tick_length==previous);
            sr_set_auto_tempo(&engine,1);
            assert(engine.tick_length!=previous);
        }
    }
    unsigned counts[2]={0,0}, lanes=0;
    for(unsigned pass=0;pass<2;pass++){
        for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
            sr_init(&engine,712);
            sr_set_scene(&engine,4);
            sr_set_music_mode(&engine,mode);
            engine.drum_hit=pass?100:0;
            for(unsigned bar=0;bar<256;bar++){
                engine.form_loop=bar;
                sr_plan_sections(&engine);
                for(unsigned beat=0;beat<16;beat++){
                    drum_trigger(&engine,beat,2);
                    unsigned mask=engine.drum_steps[beat]&65535u;
                    counts[pass]+=__builtin_popcount(mask);
                    if(pass)lanes|=mask;
                    assert(!!(mask&1u)==(groove_kick(&engine,beat)>0.f));
                }
            }
        }
    }
    printf("Drum events %u -> %u, lane mask %04x\n",counts[0],counts[1],lanes);
    assert(counts[1]>counts[0]*2);
    assert(lanes==65535u);
    puts("PASS five tempo ranges, manual locks, and all drum lanes at full HIT");
}
