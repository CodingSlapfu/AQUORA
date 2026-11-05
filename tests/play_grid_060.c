#include "serenity.h"
#include <assert.h>
#include <stdio.h>

static sr_engine engine;
static int16_t pcm[256];

static int keyed(unsigned key)
{
    for(unsigned i=0;i<SR_VOICES;i++)
        if(engine.voices[i].gate&&engine.voices[i].key==key+1)
            return 1;
    return 0;
}

int main(void)
{
    for(unsigned grid=1;grid<=4;grid++){
        sr_init(&engine,43);
        sr_start(&engine);
        assert(engine.play_grid==3);
        engine.tick_length=grid==3?8000:128;
        engine.tick_phase=engine.tick_length;
        engine.play_grid=grid;
        sr_render(&engine,pcm,5);
        sr_key_down(&engine,SR_LEAD,6);
        sr_key_down(&engine,SR_LEAD,8);
        sr_key_up(&engine,8);
        unsigned target=grid==3?4000:grid==4?256:grid==2?64:128;
        for(unsigned i=5;i<target;i++)sr_render(&engine,pcm,1);
        assert(!keyed(6)&&!keyed(8));
        sr_render(&engine,pcm,1);
        assert(keyed(6)&&!keyed(8));
        unsigned taps=0;
        for(unsigned i=0;i<SR_VOICES;i++){
            sr_voice *voice=&engine.voices[i];
            taps+=voice->gate&&!voice->key&&voice->track==SR_LEAD&&
                  voice->midi==sr_key_note(&engine,8)&&voice->remain==engine.tick_length/4u;
        }
        assert(taps==1);
        sr_key_up(&engine,6);
        assert(!keyed(6));
        sr_key_down(&engine,SR_LEAD,2);
        sr_stop(&engine);
        sr_render(&engine,pcm,128);
        sr_render(&engine,pcm,128);
        assert(!engine.play_queued&&!keyed(2));
    }
    sr_init(&engine,43);
    sr_set_scene(&engine,1);
    sr_start(&engine);
    engine.tick_length=640;
    engine.tick_phase=640;
    engine.play_grid=1;
    engine.play_feel=1;
    engine.play_strength=50;
    sr_render(&engine,pcm,10);
    sr_key_down(&engine,SR_LEAD,6);
    assert(engine.play_due[6]-engine.frame_count==340);
    sr_key_down(&engine,SR_BASS,9);
    assert(engine.play_due[9]-engine.frame_count==315);
    engine.play_strength=0;
    sr_key_down(&engine,SR_LEAD,11);
    assert(keyed(11));
    puts("PASS Play Along snaps to the exact configured or automatic sample, plays short taps and clears on STOP");
}
