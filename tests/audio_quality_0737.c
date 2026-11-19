#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

static void tune_check(unsigned zone)
{
    const int8_t *map=human_word_pitch[zone-VOCAL_GO_ZONE];
    unsigned bin=1;
    while(bin+1<sizeof human_word_pitch[0]&&abs(map[bin])<3)bin++;
    assert(bin+1<sizeof human_word_pitch[0]);
    float speed[3];
    for(unsigned i=0;i<3;i++){
        sr_init(&engine,123);
        sr_set_vocals(&engine,1,4);
        sr_start(&engine);
        engine.external_sync=1;
        engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
        engine.vocal_fx_manual=15;
        engine.vocal_tune=i*50u;
        sr_voice *voice=recorded_note(&engine,64,zone,100,3.f);
        assert(voice);
        voice->phase=bin*256.f;
        int16_t pcm[256];
        sr_render(&engine,pcm,128);
        assert(voice->active);
        speed[i]=(voice->phase-bin*256.f)/(voice->phase2*128.f);
        assert(fabsf(speed[i]-(1.f+map[bin]*(float)i*.00275f))<.012f);
    }
    assert((speed[1]-speed[0])*map[bin]>0.f);
    assert((speed[2]-speed[1])*map[bin]>0.f);
    printf("PCM zone %u, phase advance at TUNE 0/50/100: %.4f %.4f %.4f\n",
           zone,speed[0],speed[1],speed[2]);
}

int main(void)
{
    tune_check(VOCAL_COLORS_ZONE);
    tune_check(VOCAL_HELLO_ZONE);
    tune_check(VOCAL_YES_ZONE);
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
            sr_init(&engine, 37u+scene);
            sr_set_music_mode(&engine,mode);
            sr_set_scene(&engine,scene);
            unsigned unique=0;
            int8_t previous[16]={0};
            for(unsigned visit=0;visit<8;visit++){
                sr_set_scene(&engine,(scene+1)%SR_SCENES);
                sr_set_scene(&engine,scene);
                assert(engine.music_mode==mode);
                unique+=memcmp(previous,engine.melody,sizeof previous)!=0;
                memcpy(previous,engine.melody,sizeof previous);
            }
            assert(unique==8);
        }
    }
    puts("PASS 22 intentions x 5 modes, eight distinct adjacent A-B-A motif realizations per pair");
}
