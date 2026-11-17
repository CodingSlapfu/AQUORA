#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

static uint32_t render_word(unsigned parameter,unsigned level)
{
    int16_t pcm[256];
    sr_init(&engine,721);
    sr_set_vocals(&engine,1,4);
    engine.running=engine.external_sync=1;
    engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    if(parameter<VF_COUNT){
        engine.vocal_param_lock=1u<<parameter;
        if(parameter==VF_FLANGE_RATE||parameter==VF_FLANGE_FEEDBACK){
            engine.vocal_param_lock|=1u<<VF_FLANGER;
            engine.vocal_param[VF_FLANGER]=70;
        }
        engine.vocal_param[parameter]=level;
    }
    for(unsigned i=0;i<320;i++)vocal_production(&engine,64);
    assert(recorded_note(&engine,69,5,107,1.f));
    uint32_t hash=2166136261u;
    for(unsigned block=0;block<900;block++){
        if(block==150)assert(recorded_note(&engine,73,VOCAL_DAY_ZONE,125,1.8f));
        sr_render(&engine,pcm,128);
        for(unsigned i=0;i<256;i++)
            hash=(hash^(uint16_t)pcm[i])*16777619u;
    }
    return hash;
}

int main(void)
{
    for(unsigned param=0;param<VF_COUNT;param++){
        if(render_word(param,0)==render_word(param,100)){fprintf(stderr,"Parameter %u did not change PCM\n",param);return 1;}
    }
    sr_init(&engine,721);
    sr_start(&engine);
    sr_set_vocals(&engine,1,4);
    engine.vocal_play=1;
    assert(!featured_vocal_section(&engine,0));
    engine.play_grid=0;
    sr_key_down(&engine,SR_LEAD,3);
    unsigned manual=0;
    for(unsigned i=0;i<SR_VOICES;i++){
        sr_voice *voice=&engine.voices[i];
        if(voice->sample_zone&&voice->key==4){
            manual++;
            sr_key_up(&engine,3);
            assert(!voice->gate);
        }
    }
    assert(manual==1);
    engine.vocal_reprise=1;
    assert(featured_vocal_section(&engine,0)==2);
    engine.slicer_mask=8;
    engine.slicer_style=4;
    for(unsigned step=1;step<16;step++){
        voice_block block;
        engine.sequence_step=step+1;
        prepare_block(&engine,&block,64);
        assert(block.slicer_target[0]==1.f&&block.slicer_target[1]==1.f&&block.slicer_target[2]==1.f);
        if(!(sr_slicer_pattern(&engine)&(1u<<step)))assert(block.slicer_target[3]<1.f);
    }
    sr_init(&engine,721);
    voice_block instrumental;
    engine.current_energy=.7f;
    for(unsigned i=0;i<250;i++)prepare_block(&engine,&instrumental,64);
    assert(instrumental.echo_send[SR_GUITAR]>instrumental.send[SR_GUITAR]);
    assert(instrumental.echo_send[SR_BASS]<instrumental.send[SR_BASS]*.4f);
    engine.track_send_manual=1u<<SR_GUITAR;
    for(unsigned i=0;i<250;i++)prepare_block(&engine,&instrumental,64);
    assert(fabsf(instrumental.echo_send[SR_GUITAR]-instrumental.send[SR_GUITAR])<.001f);
    puts("PASS 20 vocal DSP control PCM responses, keyboard routing, recall and vocal-only gate target");
}
