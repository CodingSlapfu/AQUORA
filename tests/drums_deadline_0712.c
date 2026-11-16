#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine e;

static void test_sources(void)
{
    for(unsigned lane=0;lane<SR_DRUM_PARTS;lane++){
        sr_init(&e,712);
        e.drum_level=1.f;
        e.drum_solo=lane;
        drum_strike(&e,lane,100);
        assert(e.drum_steps[0]==((1u|65536u)<<lane));
        float energy=0.f;
        for(unsigned n=0;n<4096;n++)energy+=fabsf(drum_sample(&e));
        assert(energy>1.f);
        sr_init(&e,712);
        e.drum_level=1.f;
        e.drum_mutes=1u<<lane;
        if(lane<3u)e.drum_part_gain[lane]=0.f;
        drum_strike(&e,lane,100);
        for(unsigned n=0;n<2048;n++)assert(drum_sample(&e)==0.f);
        for(unsigned other=0;other<lane;other++)assert(sr_drum_notes[lane]!=sr_drum_notes[other]);
    }

    sr_init(&e,712);
    drum_strike(&e,5,100);
    float before=e.drum_amp[2];
    drum_strike(&e,2,90);
    assert(e.drum_amp[2]<before*.2f);
}

static void test_score(void)
{
    unsigned lanes=0;
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        sr_init(&e,712);
        sr_set_scene(&e,4);
        sr_set_music_mode(&e,mode);
        for(unsigned bar=0;bar<256;bar++){
            e.form_loop=bar;
            sr_plan_sections(&e);
            for(unsigned beat=0;beat<16;beat++){
                drum_trigger(&e,beat,2);
                lanes|=e.drum_steps[beat]&65535u;
                assert(!!(e.drum_steps[beat]&1u)==(groove_kick(&e,beat)>0.f));
            }
        }
    }
    assert(lanes==65535u);
}

static void test_deadlines(void)
{
    sr_init(&e,712);
    e.running=1;
    for(unsigned i=0;i<SR_VOICES;i++)assert(note_on(&e,i%7,58+i,80,2.f));
    sr_deadline_observe(&e,5000);
    assert(e.voice_limit==0);
    sr_deadline_observe(&e,5000);
    assert(e.voice_limit==15&&e.voices[0].gate&&e.voices[1].gate);
    for(unsigned i=0;i<32;i++)sr_deadline_observe(&e,6000);
    assert(e.voice_limit==6);
    for(unsigned i=0;i<800;i++)sr_deadline_observe(&e,2000);
    assert(e.voice_limit==0);
}

int main(void)
{
    test_sources();
    test_score();
    test_deadlines();
    puts("PASS 16 distinct audible parts and mutes, stable kick anchors, hat choke and deadline hysteresis");
}
