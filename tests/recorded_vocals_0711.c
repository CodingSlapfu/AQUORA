#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine e;

static void check_scores(void)
{
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        unsigned notes=0,duets=0,zones=0,turns=0;
        for(unsigned scene=0;scene<SR_SCENES;scene++){
            sr_init(&e,711);
            sr_set_music_mode(&e,mode);
            sr_set_scene(&e,scene);
            assert(!e.vocals_enabled && e.vocal_presence==4);
            sr_set_vocals(&e,1,4);
            sr_start(&e);
            unsigned heard=0;
            for(unsigned n=0;n<4096;n++){
                score_advance(&e,e.frame_count+e.tick_length);
                trigger(&e);
                unsigned active=0,new_notes=0;
                for(unsigned i=0;i<SR_VOICES;i++){
                    const sr_voice *v=&e.voices[i];
                    if(!v->active||!v->sample_zone)continue;
                    active++;
                    if(v->born!=e.frame_count)continue;
                    assert(v->track==SR_CHOIR);
                    assert(v->phase2>(v->sample_zone==VOCAL_YOU_ZONE+1u?.15f:.16f)&&v->phase2<.38f);
                    zones|=1u<<(v->sample_zone-1u);
                    if(v->vocal_turn_rate>0.f){
                        assert((v->sample_zone-1u)%3u==2u);
                        assert(v->vocal_turn_first!=1.f && v->vocal_turn_second!=v->vocal_turn_first);
                        assert(v->remain>5u*e.tick_length);
                        turns++;
                    }
                    new_notes++;
                }
                assert(active<=2u);
                heard+=new_notes;
                duets+=new_notes==2;
            }
            assert(heard>0);
            notes+=heard;
        }
        assert(notes>50&&(zones&0x09u)&&(zones&0x24u)&&turns>0);
        if(mode!=SR_EXPERIMENTAL)assert(duets>0);
        printf("Mode %u: %u recorded notes, %u simultaneous harmonies, zones 0x%x\n",
               mode,notes,duets,zones);
    }
}

static void check_playback(void)
{
    int16_t pcm[512];
    sr_init(&e,3);
    sr_set_vocals(&e,1,4);
    e.running=1;
    e.solo=SR_CHOIR;
    e.external_sync=1;
    e.tick_phase=0;
    e.reverb_amount=e.delay_amount=0;
    recorded_note(&e,72,3,100,8.f);
    recorded_note(&e,64,0,75,8.f);
    recorded_note(&e,60,1,100,8.f);
    unsigned count=0;
    for(unsigned v=0;v<SR_VOICES;v++)count+=e.voices[v].sample_zone!=0;
    assert(count==2);
    unsigned audible=0;
    for(unsigned b=0;b<160;b++){
        sr_render(&e,pcm,256);
        for(unsigned j=0;j<512;j++)audible+=pcm[j]>50;
    }
    assert(audible>1000);
    sr_set_vocals(&e,0,4);
    for(unsigned v=0;v<SR_VOICES;v++)
        assert(!e.voices[v].sample_zone||!e.voices[v].gate);
    for(unsigned b=0;b<48;b++)sr_render(&e,pcm,256);
    for(unsigned v=0;v<SR_VOICES;v++)
        assert(!e.voices[v].sample_zone||!e.voices[v].active);
    sr_set_vocals(&e,1,4);
    e.sound_locks|=1u<<SR_CHOIR;
    for(unsigned n=0;n<512;n++)trigger(&e);
    unsigned locked_vocals=0;
    for(unsigned v=0;v<SR_VOICES;v++)locked_vocals+=e.voices[v].active&&e.voices[v].sample_zone;
    assert(locked_vocals>0);

}

int main(void)
{
    check_scores();
    check_playback();
    puts("PASS recorded vocals across intentions and modes, two-voice cap, release and independence from legacy choir lock");
}
