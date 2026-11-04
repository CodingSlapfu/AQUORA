#include "serenity.h"
#include <assert.h>
#include <stdio.h>

static sr_engine a,b;
static int16_t pcm[512];

static void block(sr_engine *e){sr_render(e,pcm,256);}
static void silence(void)
{
    for(unsigned i=0;i<512;i++)assert(pcm[i]==0);
}

static void stopped_voices(void)
{
    sr_init(&a,731);
    a.bed_level=0;
    a.reverb_amount=a.delay_amount=0;
    a.drums_enabled=0;
    sr_start(&a);
    a.tick_length=44100u*100;
    a.tick_phase=0;
    sr_key_down(&a,SR_BASS,4);
    unsigned count=a.note_ons;
    sr_key_down(&a,SR_LEAD,4);
    assert(a.note_ons==count);
    for(unsigned i=0;i<800;i++)block(&a);
    assert(a.sacred_gain>.001f);
    sr_key_up(&a,4);
    for(unsigned i=0;i<70;i++)block(&a);
    assert(a.sacred_gain<.00001f);
    sr_key_down(&a,SR_LEAD,1);
    for(unsigned i=0;i<1560;i++)block(&a);
    assert(a.watchdogs==1);
    for(unsigned i=0;i<SR_VOICES;i++)assert(!a.voices[i].gate);

    for(unsigned scene=0;scene<SR_SCENES;scene++){
        sr_set_scene(&a,scene);
        a.freeze=1;
        a.drums_enabled=1;
        a.binaural_enabled=1;
        a.bed_level=1;
        a.reverb_amount=.8f;
        a.delay_amount=.7f;
        a.tone_level=.1f;
        for(unsigned round=0;round<4;round++){
            sr_start(&a);
            for(unsigned key=0;key<27;key++)sr_key_down(&a,key%7,key);
            for(unsigned i=0;i<20;i++)block(&a);
            sr_stop(&a);
            block(&a);
            block(&a);silence();
            assert(!a.rev_history&&!a.delay_history);
            for(unsigned v=0;v<SR_VOICES;v++)assert(!a.voices[v].active);
            sr_start(&a);
            block(&a);
            for(unsigned v=0;v<SR_VOICES;v++)assert(!a.voices[v].key);
            sr_stop(&a);block(&a);
        }
    }
    puts("PASS duplicate held key, missed-release watchdog, Sacred Bass release, 32 saturated STOP/restart cycles, no stale FX or keyed voices");
}

static void hit_cap(void)
{
    sr_init(&a,3131);
    sr_init(&b,3131);
    sr_start(&a);sr_start(&b);
    a.drum_density=b.drum_density=2;
    a.drum_hit=70;b.drum_hit=255;
    a.tick_length=b.tick_length=32;
    for(unsigned i=0;i<3500;i++){
        block(&a);block(&b);
        assert(a.optional_hits==b.optional_hits);
    }
    unsigned eligible=a.sequence_step/4;
    assert(a.optional_hits*100>eligible*65&&a.optional_hits*100<eligible*75);
    printf("PASS 70%% equals corrupted 255%% input: %u optional strikes in about %u eligible subdivisions\n",a.optional_hits,eligible);
    sr_init(&a,1);sr_start(&a);a.drum_hit=0;a.tick_length=32;
    for(unsigned i=0;i<500;i++)block(&a);
    assert(!a.optional_hits&&a.tracks[SR_DRUMS].events>0&&a.drums_enabled);
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        sr_init(&a,901);sr_set_scene(&a,scene);sr_start(&a);
        float peak=0;
        for(unsigned i=0;i<760;i++){
            block(&a);
            if(a.tracks[SR_DRUMS].meter>peak)peak=a.tracks[SR_DRUMS].meter;
        }
        assert(a.tracks[SR_DRUMS].events>0&&peak>.07f);
        printf("Scene %u drum peak %.3f, %u strikes\n",scene,peak,a.tracks[SR_DRUMS].events);
    }
    puts("PASS HIT zero retains pulse and all eight scenes produce audible-range drum PCM");
}
int main(void){stopped_voices();hit_cap();return 0;}
