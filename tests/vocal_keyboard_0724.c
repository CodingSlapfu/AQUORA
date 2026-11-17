#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
int main(void)
{
    for(unsigned enabled=0;enabled<2;enabled++)for(unsigned mode=0;mode<3;mode++){
        sr_init(&e,724);
        sr_start(&e);
        sr_set_vocals(&e,enabled,4);
        e.vocal_play=mode;
        e.play_grid=0;
        sr_key_down(&e,SR_CHOIR,3);
        unsigned held=0;
        for(unsigned i=0;i<SR_VOICES;i++){
            const sr_voice *v=&e.voices[i];
            if(v->active&&v->key==4){
                assert((v->sample_zone!=0)==(enabled&&mode));
                assert(v->track==SR_CHOIR);
                held++;
            }
        }
        assert(held==1);
        sr_key_up(&e,3);
        for(unsigned i=0;i<SR_VOICES;i++)
            assert(e.voices[i].key!=4||!e.voices[i].gate);
    }
    puts("PASS instrumental choir and sampled singer keyboard routing for OFF/AUTO/PLAY ALONG/AUTO+PLAY ALONG, including note-off");
}
