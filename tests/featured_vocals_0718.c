#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

static void verify_vocal_gain(void)
{
    int16_t pcm[512];
    unsigned peak[3]={0};
    const unsigned levels[]={0,50,100};
    for(unsigned pass=0;pass<3;pass++){
        sr_init(&engine,718);
        sr_set_vocals(&engine,1,4);
        engine.tracks[SR_CHOIR].volume=0.f;
        engine.vocal_level=levels[pass];
        engine.vocal_level_smoothed=levels[pass]*.01f;
        sr_audition_vocal(&engine);
        for(unsigned block=0;block<32;block++){
            sr_render(&engine,pcm,256);
            for(unsigned i=0;i<512;i++){
                unsigned magnitude=pcm[i]<0?-pcm[i]:pcm[i];
                if(magnitude>peak[pass])peak[pass]=magnitude;
            }
        }
        assert(engine.tracks[SR_CHOIR].volume==0.f);
    }
    assert(peak[0]==0&&peak[1]>100&&peak[2]>peak[1]);
    printf("PASS isolated singer PCM gain: %u, %u, %u\n",peak[0],peak[1],peak[2]);
}

static void verify_slicer_styles(void)
{
    sr_init(&engine,718);
    unsigned masks[8]={0};
    for(unsigned style=1;style<8;style++){
        engine.slicer_style=style;
        masks[style]=sr_slicer_pattern(&engine);
        for(unsigned prev=1;prev<style;prev++)
            assert(masks[style]!=masks[prev]);
    }
    engine.slicer_style=0;
    unsigned first=sr_slicer_pattern(&engine);
    engine.form_loop++;
    assert(first!=sr_slicer_pattern(&engine));
    engine.drum_playhead=15;
    engine.drum_steps[15]=1;
    assert(sr_slicer_pattern(&engine)&(1u<<15));
    puts("PASS slicer style patterns, AUTO variation and kick protection");
}

int main(void)
{
    sr_init(&engine,718);
    sr_start(&engine);
    assert(!featured_vocal_section(&engine,0));
    sr_set_vocals(&engine,1,4);
    engine.form_route[0]=8; /* Suspension leaves a full instrumental break. */
    assert(recorded_vocal_phrase(&engine,7,composition_chord(&engine,7)));
    assert(!engine.vocal_entry_pending);
    unsigned presence=engine.vocal_presence;
    sr_set_scene(&engine,(engine.scene+1u)%SR_SCENES);
    assert(engine.vocals_enabled&&engine.vocal_presence==presence&&engine.vocal_entry_pending);
    assert(recorded_vocal_phrase(&engine,0,composition_chord(&engine,0)));
    for(unsigned i=0;i<SR_VOICES;i++)
        if(engine.voices[i].sample_zone)engine.voices[i].active=0;
    engine.form_route[0]=8;
    engine.vocal_presence=2; /* Low presence keeps spacious sections instrumental. */
    for(unsigned step=0;step<16;step++)
        assert(!recorded_vocal_phrase(&engine,step,composition_chord(&engine,step)));
    engine.vocal_presence=presence;
    engine.form_route[0]=12; /* Bloom uses the chorus hook. */
    assert(featured_vocal_section(&engine,0)==2);
    assert(recorded_vocal_phrase(&engine,0,composition_chord(&engine,0)));
    assert(!recorded_vocal_phrase(&engine,9,composition_chord(&engine,9)));
    sr_note_on(&engine,SR_LEAD,72,80,1.f);
    sr_note_on(&engine,SR_BASS,43,80,1.f);
    voice_block block;
    for(unsigned n=0;n<80;n++)prepare_block(&engine,&block,64);
    unsigned singer=0, lead=0, bass=0;
    for(unsigned n=0;n<block.count;n++){
        const voice_config *voice=&block.active[n];
        if(voice->voice->sample_zone){assert(voice->gain>.5f);singer++;}
        else if(voice->track==SR_LEAD){assert(voice->left<block.left[SR_LEAD]*.75f);lead++;}
        else if(voice->track==SR_BASS){assert(voice->left==block.left[SR_BASS]);bass++;}
    }
    assert(singer&&lead&&bass);
    assert(engine.vocal_priority>.6f);
    puts("PASS featured vocal break, recurring hook, forward gain and selective accompaniment ducking");
    verify_vocal_gain();
    verify_slicer_styles();
}
