#include "serenity.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static sr_engine engine;
static int16_t pcm[512];

int main(void)
{
    for(unsigned mode=0;mode<4;mode++){
        sr_init(&engine,5701);
        engine.auto_gain=mode&1u;
        engine.auto_level=mode>>1;
        engine.bed_level=1.f;
        engine.binaural_enabled=1;
        engine.reverb_amount=.8f;
        engine.delay_amount=.7f;
        engine.energy=1.f;
        engine.drum_density=3;
        engine.drum_hit=100;
        sr_start(&engine);
        unsigned peak=0;
        for(unsigned block=0;block<1400;block++){
            if(block%20==0)
                for(unsigned t=0;t<SR_DRUMS;t++)
                    sr_note_on(&engine,t,40+t*5,127,3.f);
            sr_render(&engine,pcm,256);
            for(unsigned i=0;i<512;i++){
                unsigned mag=pcm[i]<0?-pcm[i]:pcm[i];
                if(mag>peak)peak=mag;
            }
            for(unsigned t=0;t<SR_TRACKS;t++)
                assert(engine.gain_trim[t]>=.72f&&engine.gain_trim[t]<=1.28f);
        }
        assert(peak>1000&&peak<21000);
        sr_stop(&engine);
        sr_render(&engine,pcm,256);
        sr_render(&engine,pcm,256);
        for(unsigned i=0;i<512;i++)assert(!pcm[i]);
        printf("Auto Gain %u / Auto Level %u: peak %u, bounded trims, STOP silent\n",
            engine.auto_gain,engine.auto_level,peak);
    }
}
