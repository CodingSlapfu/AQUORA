/* v0.7.14: real PCM differentiation for all 19 drum kits/16 parts.
 * Self-contained deterministic host test; no special fixtures or app UI. */
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine e;
static int16_t pcm[512];

static uint64_t one_shot(unsigned kit,unsigned lane,unsigned *peak)
{
    sr_init(&e,714);
    sr_set_drum_kit(&e,kit);
    e.bed_level=e.tone_level=e.reverb_amount=e.delay_amount=0.f;
    e.sacred_mode=0;
    e.binaural_enabled=0;
    e.solo=SR_DRUMS;
    e.isolate=SR_ISOLATE_TRACK1+SR_DRUMS;
    sr_audition_drum(&e,lane);
    assert(e.preview_frames>0);
    uint64_t hash=UINT64_C(14695981039346656037);
    *peak=0;
    for(unsigned block=0;block<40;block++){
        sr_render(&e,pcm,256);
        for(unsigned i=0;i<512;i++){
            int v=pcm[i];
            unsigned magnitude=(unsigned)(v<0?-v:v);
            if(magnitude>*peak)*peak=magnitude;
            hash^=(uint16_t)pcm[i];
            hash*=UINT64_C(1099511628211);
        }
    }
    return hash;
}

int main(void)
{
    uint64_t hashes[SR_DRUM_KITS]={0};
    for(unsigned kit=0;kit<SR_DRUM_KITS;kit++){
        unsigned peak=0;
        hashes[kit]=one_shot(kit,3,&peak); /* Kick 2: first recorded one-shot */
        fprintf(stderr,"kit %02u sampled kick2 hash=%016" PRIx64 " peak=%u\n",kit,hashes[kit],peak);
        assert(peak>8);
        for(unsigned other=0;other<kit;other++)assert(hashes[kit]!=hashes[other]);
    }
    for(unsigned lane=0;lane<SR_DRUM_PARTS;lane++){
        unsigned peak=0;
        (void)one_shot(0,lane,&peak);
        fprintf(stderr,"lane %02u one-shot peak=%u\n",lane,peak);
        assert(peak>8);
    }
    puts("PASS all nineteen sampled kit timbres are PCM-distinct; all sixteen lanes audibly render");
    return 0;
}
