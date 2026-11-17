#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static uint32_t audition(unsigned kind)
{
    sr_init(&e,723);
    sr_start(&e);
    sr_set_vocals(&e,1,4);
    e.vocal_play=1;
    e.vocal_articulation=kind;
    e.play_grid=0;
    e.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    int16_t pcm[512];
    uint32_t hash=2166136261u;
    unsigned heard=0;
    for(unsigned note=0;note<3;note++){
        sr_key_down(&e,SR_LEAD,note);
        for(unsigned block=0;block<192;block++){
            sr_render(&e,pcm,256);
            heard+=e.vocal_direct_peak>.0001f;
            for(unsigned i=0;i<512;i++)hash=(hash^(uint16_t)pcm[i])*16777619u;
        }
        sr_key_up(&e,note);
        for(unsigned block=0;block<40;block++)sr_render(&e,pcm,256);
    }
    assert(heard>30);
    return hash;
}
int main(void)
{
    uint32_t hashes[19];
    for(unsigned kind=1;kind<19;kind++){
        hashes[kind]=audition(kind);
        for(unsigned previous=1;previous<kind;previous++)
            if(hashes[previous]==hashes[kind]){
                fprintf(stderr,"Identical articulations %u and %u\n",previous,kind);
                assert(0);
            }
    }
    unsigned characters=0,words=0;
    for(unsigned seed=1;seed<200;seed++){
        sr_init(&e,seed);
        sr_generate_song(&e,seed,0);
        unsigned identity=vocal_character(&e);
        characters|=1u<<identity;
        sr_start(&e);
        sr_set_vocals(&e,1,4);
        e.form_route[0]=12;
        e.vocal_entry_pending=0;
        for(unsigned step=0;step<16;step++){
            score_advance(&e,e.frame_count+e.tick_length);
            recorded_vocal_phrase(&e,step,0);
            assert(vocal_character(&e)==identity);
            for(unsigned i=0;i<SR_VOICES;i++){
                sr_voice *voice=&e.voices[i];
                if(voice->active&&voice->sample_zone>=VOCAL_DAY_ZONE+1u)
                    words|=1u<<(voice->sample_zone-1u);
            }
        }
        uint8_t vowels[3],attacks[3];
        memcpy(vowels,e.vocal_hook_vowel,3);
        memcpy(attacks,e.vocal_hook_attack,3);
        e.vocal_reprise=1;
        for(unsigned step=16;step<32;step++){
            score_advance(&e,e.frame_count+e.tick_length);
            recorded_vocal_phrase(&e,step,0);
        }
        assert(!memcmp(vowels,e.vocal_hook_vowel,3));
        assert(!memcmp(attacks,e.vocal_hook_attack,3));
    }
    assert(characters==63u);
    assert(words&(1u<<VOCAL_BABY_ZONE));
    assert(words&(1u<<VOCAL_LOVE_ZONE));
    assert(words&(1u<<VOCAL_ME_ZONE));
    puts("PASS 18 distinct audible articulations, six stable song characters, recorded multiword vocabulary and hook articulation recall");
}
