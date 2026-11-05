#include "serenity.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static sr_engine engine;
static int16_t samples[512];

static unsigned peak(void)
{
    unsigned result=0;
    for(unsigned i=0;i<512;i++){
        unsigned magnitude=(unsigned)(samples[i]<0?-samples[i]:samples[i]);
        if(magnitude>result)result=magnitude;
    }
    return result;
}

static void require_silence(void)
{
    for(unsigned b=0;b<8;b++){
        sr_render(&engine,samples,256);
        assert(peak()==0);
    }
}

static void transport(void)
{
    sr_init(&engine,17);
    require_silence();
    engine.binaural_enabled=1;
    engine.tone_level=.08f;
    engine.reverb_amount=.8f;
    engine.delay_amount=.7f;
    engine.bed_level=1.f;
    engine.drum_density=3;
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        sr_set_scene(&engine,scene);
        sr_start(&engine);
        sr_key_down(&engine,SR_LEAD,7);
        sr_key_down(&engine,SR_BASS,3);
        for(unsigned b=0;b<220;b++)sr_render(&engine,samples,256);
        sr_stop(&engine);
        sr_key_down(&engine,SR_LEAD,8);
        sr_set_scene(&engine,(scene+1)%SR_SCENES);
        sr_render(&engine,samples,256);
        assert(!engine.stop_frames);
        require_silence();
        sr_start(&engine);
        sr_render(&engine,samples,256);
        sr_stop(&engine);
        sr_start(&engine);
        sr_render(&engine,samples,256);
        assert(engine.running&&!engine.restart&&engine.sequence_step==0);
        sr_stop(&engine);
        sr_render(&engine,samples,256);
        require_silence();
    }
    puts("PASS all scenes: loaded STOP, held keys, scene change, rapid restart, exact silence after 256 fade frames");
}

static void keys_and_motion(void)
{
    static const int roots[]={57,57,57,53,57,60,55,60,58,62,55,53,60};
    static const unsigned masks[]={0x295,0x4ad,0x295,0x4a9,0x2a5,0xab5,0x4a9,0xab5,0x4ad,0xab5,0x295,0x2a5,0xab5};
    sr_init(&engine,31);
    assert(engine.perfect_key);
    engine.play_grid=0; /* Inspect glide allocation before rendering any frames. */
    for(unsigned scene=0;scene<SR_SCENES;scene++){
        sr_set_scene(&engine,scene);
        int previous=0;
        for(unsigned key=0;key<27;key++){
            int note=sr_key_note(&engine,key);
            assert(note>previous);
            assert(masks[scene]&(1u<<((note-roots[scene])%12)));
            previous=note;
        }
    }
    sr_set_scene(&engine,0);
    sr_set_perfect_key(&engine,0);
    for(unsigned key=0;key<27;key++)assert(sr_key_note(&engine,key)==53+key);
    sr_set_perfect_key(&engine,1);
    sr_start(&engine);
    engine.tick_phase=0;
    engine.motion[SR_LEAD]=(sr_motion){60,1,23,44};
    engine.motion[SR_BASS]=(sr_motion){90,2,0,35};
    sr_key_down(&engine,SR_LEAD,0);
    sr_key_down(&engine,SR_LEAD,5);
    assert(engine.voices[0].pitch==engine.voices[1].pitch);
    assert(engine.voices[1].pitch<engine.voices[1].freq);
    for(unsigned b=0;b<70;b++)sr_render(&engine,samples,256);
    assert(fabsf(engine.voices[1].pitch-engine.voices[1].freq)<.5f);
    sr_key_down(&engine,SR_BASS,2);
    sr_key_up(&engine,2);
    sr_key_down(&engine,SR_BASS,4);
    unsigned found=0;
    for(unsigned i=0;i<SR_VOICES;i++)if(engine.voices[i].key==5){
        assert(engine.voices[i].pitch<engine.voices[i].freq);found++;
    }
    assert(found==1);
    sr_set_scene(&engine,7);
    sr_set_tuning_432(&engine,1);
    assert(engine.voices[1].midi==sr_key_note(&engine,5));
    assert(engine.voices[1].freq==sr_note_frequency(&engine,engine.voices[1].midi));
    sr_key_up(&engine,5);
    assert(!engine.voices[1].gate&&engine.voices[0].gate);
    puts("PASS all 351 mapped keys, chromatic mode, held retuning, Lead legato, Bass always glide and key release");
}

static void tracks(void)
{
    uint32_t signature[SR_DRUM_KITS];
    assert(SR_DRUMS==7&&SR_TRACKS==8);
    for(unsigned kit=0;kit<SR_DRUM_KITS;kit++){
        sr_init(&engine,47);
        engine.drum_kit=kit;
        engine.solo=SR_DRUMS;
        engine.bed_level=0;
        engine.reverb_amount=engine.delay_amount=0;
        sr_start(&engine);
        unsigned maximum=0;
        uint32_t hash=2166136261u;
        for(unsigned b=0;b<400;b++){
            sr_render(&engine,samples,256);
            unsigned level=peak();
            if(level>maximum)maximum=level;
            for(unsigned i=0;i<512;i++)hash=(hash^(uint16_t)samples[i])*16777619u;
        }
        assert(maximum>900&&maximum<30000);
        assert(engine.tracks[SR_DRUMS].events>=2);
        for(unsigned earlier=0;earlier<kit;earlier++)assert(hash!=signature[earlier]);
        signature[kit]=hash;
        engine.tracks[SR_DRUMS].mute=1;
        require_silence();
        printf("Kit %s: peak %u, distinct PCM and mute pass\n",sr_drum_kits[kit].name,maximum);
    }
    for(unsigned role=0;role<SR_DRUMS;role++){
        sr_init(&engine,55);
        engine.solo=role;
        engine.reverb_amount=engine.delay_amount=0;
        sr_start(&engine);
        sr_key_down(&engine,role,3);
        unsigned maximum=0;
        for(unsigned b=0;b<64;b++){sr_render(&engine,samples,256);if(peak()>maximum)maximum=peak();}
        assert(maximum>100);
        engine.tracks[role].mute=1;
        require_silence();
    }
    for(unsigned scene=6;scene<8;scene++){
        sr_init(&engine,19);
        sr_set_scene(&engine,scene);
        assert(engine.drum_kit==(scene==6?13:3));
        engine.solo=SR_BASS;
        engine.reverb_amount=engine.delay_amount=0;
        sr_start(&engine);
        for(unsigned b=0;b<100;b++)sr_render(&engine,samples,256);
        assert(engine.sacred_gain>.07f);
        assert(engine.tone_level==0);
        engine.sacred_mode=0;
        for(unsigned b=0;b<100;b++)sr_render(&engine,samples,256);
        assert(engine.sacred_gain<.00001f);
    }
    puts("PASS all eight solo/mute routes and independent Sacred Bass in both intentions");
}

int main(void)
{
    transport();
    keys_and_motion();
    tracks();
    return 0;
}
