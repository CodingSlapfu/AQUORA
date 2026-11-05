#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static sr_engine engine;
static int16_t pcm[512];
static unsigned bits(uint64_t mask)
{
    unsigned n=0;
    for(;mask;mask&=mask-1)n++;
    return n;
}

int main(void)
{
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        uint64_t patches[SR_DRUMS]={0}, kits=0,beds=0;
        uint32_t hashes[8];
        for(unsigned seed=0;seed<128;seed++){
            sr_init(&engine,821);
            sr_set_scene(&engine,scene);
            sr_generate_song(&engine,seed,0);
            for(unsigned t=0;t<SR_DRUMS;t++)patches[t]|=UINT64_C(1)<<engine.tracks[t].patch;
            kits|=1u<<engine.drum_kit;
            beds|=1u<<engine.bed_type;
            assert(engine.frequency==sr_scenes[scene].frequency);
            if(seed>=8)continue;
            sr_start(&engine);
            uint32_t h=2166136261u;
            for(unsigned i=0;i<1800;i++){
                sr_render(&engine,pcm,256);
                for(unsigned j=0;j<512;j++)h=(h^(uint16_t)pcm[j])*16777619u;
            }
            for(unsigned i=0;i<seed;i++)assert(hashes[i]!=h);
            hashes[seed]=h;
        }
        for(unsigned t=0;t<SR_DRUMS;t++)assert(bits(patches[t])>=3);
        assert(bits(kits)==3&&bits(beds)==3);
        printf("%s: 8 distinct 10-second PCM renders; 3 kits, 3 beds, >=3 patches per role\n",sr_scenes[scene].name);
    }
    sr_init(&engine,17);
    engine.sound_locks=1u<<SR_LEAD|1u<<SR_DRUMS|SR_SOUND_NATURE|SR_SOUND_FX;
    engine.bed_type=0;
    unsigned lead=engine.tracks[SR_LEAD].patch,kit=engine.drum_kit;
    float reverb=engine.reverb_amount,delay=engine.delay_amount;
    int8_t motif[sizeof(engine.melody)];
    memcpy(motif,engine.melody,sizeof(motif));
    uint32_t rng=engine.rng, drum_rng=engine.drum_rng;
    sr_generate_song(&engine,441,1);
    assert(!memcmp(motif,engine.melody,sizeof(motif)));
    assert(engine.rng==rng&&engine.drum_rng==drum_rng);
    sr_generate_song(&engine,778,0);
    sr_set_scene(&engine,8);
    assert(engine.tracks[SR_LEAD].patch==lead&&engine.drum_kit==kit);
    assert(!engine.bed_type&&engine.reverb_amount==reverb&&engine.delay_amount==delay);
    puts("PASS REROLL preserves motif/RNG, locked sounds survive generation and intention changes");
}
