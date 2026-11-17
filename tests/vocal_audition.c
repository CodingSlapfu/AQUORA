#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

int main(void)
{
    sr_init(&engine, 42);
    engine.vocals_enabled=1;
    engine.tracks[SR_CHOIR].mute=0;
    engine.tracks[SR_CHOIR].volume=.7f;
    sr_start(&engine);
    sr_note_on(&engine,SR_LEAD,60,100,4.f);
    unsigned step=engine.sequence_step, solo=engine.solo;
    sr_audition_vocal(&engine);
    unsigned recorded=0, lead=0;
    for(unsigned i=0;i<SR_VOICES;i++){
        const sr_voice *voice=&engine.voices[i];
        recorded+=voice->active&&voice->sample_zone;
        lead+=voice->active&&voice->track==SR_LEAD;
    }
    assert(engine.running && !engine.preview_frames && engine.sequence_step==step);
    assert(engine.solo==solo && engine.isolate==SR_ISOLATE_MIX && recorded==1 && lead==1);
    sr_init(&engine, 42);
    engine.vocals_enabled=1;
    engine.tracks[SR_CHOIR].mute=0;
    engine.tracks[SR_CHOIR].volume=.7f;
    sr_audition_vocal(&engine);
    assert(!engine.running && engine.preview_frames);
    sr_start(&engine);
    assert(engine.running && !engine.preview_frames && engine.isolate==SR_ISOLATE_MIX);
    puts("PASS vocal audition preserves transport and mix");
}
