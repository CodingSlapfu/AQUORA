#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

static void tempo_modes(void)
{
    for(unsigned mode=0; mode<SR_MUSIC_MODES; mode++){
        for(unsigned scene=0; scene<SR_SCENES; scene++){
            sr_init(&engine,13);
            sr_set_scene(&engine,scene);
            sr_set_music_mode(&engine,mode);
            unsigned first=0, varied=0;
            for(unsigned seed=1; seed<=16; seed++){
                sr_generate_song(&engine,seed,0);
                if(seed==1)first=engine.tick_length;
                varied|=engine.tick_length!=first;
            }
            assert(varied);
            engine.motion_manual|=4u;
            engine.tick_length=9900;
            sr_generate_song(&engine,17,0);
            assert(engine.tick_length==9900);
            unsigned seed=engine.song_seed;
            sr_generate_song(&engine,18,1);
            assert(engine.song_seed==seed&&engine.tick_length==9900);
        }
    }
}

static void vocal_priority(void)
{
    static voice_block block;
    sr_init(&engine,13);
    sr_note_on(&engine,SR_CHOIR,60,70,3.f);
    sr_note_on(&engine,SR_DRONE,65,70,3.f);
    prepare_voices(&engine,&block,64);
    float baseline=block.left[SR_CHOIR];
    recorded_note(&engine,66,0,80,1.f);
    for(unsigned i=0;i<80;i++)prepare_voices(&engine,&block,64);
    unsigned human=0, choir=0;
    for(unsigned i=0;i<block.count;i++){
        voice_config *current=&block.active[i];
        if(current->track!=SR_CHOIR)continue;
        if(current->voice->sample_zone){
            human++;
            assert(current->left==block.left[SR_CHOIR]);
        }else{
            choir++;
            assert(current->left<baseline*.65f);
        }
    }
    assert(human==1&&choir==1);
    engine.tracks[SR_CHOIR].mute=1;
    for(unsigned i=0;i<800;i++)prepare_voices(&engine,&block,64);
    assert(engine.vocal_priority==0.f);
}

int main(void)
{
    tempo_modes();
    vocal_priority();
    puts("PASS automatic and locked BPM across all 88 combinations, sound reroll, smooth vocal priority and mute release");
}
