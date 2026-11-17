#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

int main(void)
{
    sr_init(&engine,718);
    sr_start(&engine);
    assert(!featured_vocal_section(&engine,0));
    sr_set_vocals(&engine,1,4);
    engine.form_route[0]=8; /* Suspension leaves a full instrumental break. */
    for(unsigned step=0;step<16;step++)
        assert(!recorded_vocal_phrase(&engine,step,composition_chord(&engine,step)));
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
}
