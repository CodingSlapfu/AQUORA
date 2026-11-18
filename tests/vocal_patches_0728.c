#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;

int main(void)
{
    sr_init(&engine,728);
    sr_set_vocals(&engine,1,4);
    engine.running=engine.external_sync=1;
    engine.master=.8f;
    engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    assert(recorded_note(&engine,57,VOCAL_DREAMS_ZONE,100,4.f));
    unsigned dream_blocks=0;
    for(unsigned block=0;block<256;block++){
        int16_t pcm[512];
        sr_render(&engine,pcm,256);
        dream_blocks+=engine.vocal_direct_peak>.001f;
    }
    assert(dream_blocks>40);
    printf("DREAMS direct PCM blocks=%u\n",dream_blocks);

    uint64_t hash[SR_VOCAL_PATCHES];
    for(unsigned patch=1;patch<=SR_VOCAL_PATCHES;patch++){
        sr_init(&engine,728);
        sr_set_vocals(&engine,1,4);
        engine.vocal_patch=patch;
        engine.running=engine.external_sync=1;
        engine.master=.8f;
        engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
        engine.vocal_fx_manual=0;
        for(unsigned i=0;i<100;i++)vocal_production(&engine,256);
        assert(recorded_note(&engine,73,VOCAL_DAY_ZONE,100,5.f));
        int16_t samples[512];
        uint64_t digest=14695981039346656037ull;
        unsigned peak=0, heard=0;
        for(unsigned block=0;block<512;block++){
            sr_render(&engine,samples,256);
            heard+=engine.vocal_direct_peak>.001f;
            for(unsigned i=0;i<512;i++){
                int x=samples[i];
                unsigned mag=x<0?-x:x;
                if(mag>peak)peak=mag;
                digest=(digest^(uint16_t)x)*1099511628211ull;
            }
        }
        assert(heard>40&&peak>100&&peak<30000);
        for(unsigned i=0;i<patch-1;i++)assert(digest!=hash[i]);
        hash[patch-1]=digest;
        printf("%-20s peak=%5u direct_blocks=%u PCM=%016llx\n",
               sr_vocal_patch_names[patch-1],peak,heard,(unsigned long long)digest);
    }
}
