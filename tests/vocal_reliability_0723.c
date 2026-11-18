#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static void setup(void)
{
    sr_init(&e,723);
    sr_start(&e);
    sr_set_vocals(&e,1,4);
    e.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    e.master=.8f;
}
static unsigned audio_window(unsigned frames)
{
    int16_t pcm[512];
    unsigned peak=0, direct=0;
    while(frames){
        unsigned chunk=frames>256?256:frames;
        sr_render(&e,pcm,chunk);
        for(unsigned i=0;i<chunk*2;i++){
            unsigned magnitude=pcm[i]<0?-(int)pcm[i]:pcm[i];
            if(magnitude>peak)peak=magnitude;
        }
        if(e.vocal_direct_peak>.0001f)direct=1;
        frames-=chunk;
    }
    return direct&&peak>32;
}

static void stress(void)
{
    setup();
    unsigned heard=0;
    for(unsigned change=0;change<1200;change++){
        sr_set_scene(&e,change%SR_SCENES);
        sr_set_music_mode(&e,(change/SR_SCENES)%SR_MUSIC_MODES);
        e.freeze=(change&1u);
        e.vocal_presence=1u+change%4u;
        e.vocal_level=25u+25u*(change%4u);
        e.vocal_fx_send=change%101u;
        e.vocal_param_lock=(1u<<VF_COUNT)-1u;
        for(unsigned param=0;param<VF_COUNT;param++)
            e.vocal_param[param]=(change*13u+param*7u)%101u;
        e.slicer_mask=change%16u;
        e.slicer_style=change%8u;
        switch(change%6u){
        case 0:
            sr_set_vocals(&e,0,e.vocal_presence);
            assert(sr_vocal_status(&e)==SR_VOCAL_OFF);
            audio_window(SR_RATE/8u);
            sr_set_vocals(&e,1,e.vocal_presence);
            break;
        case 1:
            e.vocal_play=1;
            audio_window(SR_RATE/8u);
            e.vocal_play=2;
            sr_key_down(&e,SR_LEAD,3);
            audio_window(SR_RATE/8u);
            sr_key_up(&e,3);
            break;
        case 2:
            e.tracks[SR_CHOIR].mute=1;
            assert(sr_vocal_status(&e)==SR_VOCAL_MUTED);
            audio_window(SR_RATE/8u);
            e.tracks[SR_CHOIR].mute=0;
            break;
        case 3:
            e.isolate=SR_ISOLATE_TRACK1+SR_BASS;
            audio_window(SR_RATE/8u);
            e.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
            break;
        case 4:
            sr_stop(&e);
            audio_window(SR_RATE/8u);
            sr_start(&e);
            break;
        default:
            e.solo=SR_LEAD;
            assert(sr_vocal_status(&e)==SR_VOCAL_SOLO);
            audio_window(SR_RATE/8u);
            e.solo=SR_TRACKS;
            break;
        }
        assert(sr_vocal_status(&e)==SR_VOCAL_READY);
        if(!audio_window(SR_RATE*3u)){
            fprintf(stderr,"No direct vocal PCM after change %u, started %u rejected %u status %u\n",
                    change,e.vocal_started,e.vocal_rejected,sr_vocal_status(&e));
            assert(0);
        }
        heard++;
        if(heard%200u==0u){printf("Vocal stress %u/1200 PCM windows passed\n",heard);fflush(stdout);}
    }
    unsigned recovering=e.vocal_recovered;
    sr_set_scene(&e,0);
    sr_voice *broken=recorded_note(&e,60,0,100,2.f);
    assert(broken);
    broken->phase=NAN;
    assert(audio_window(SR_RATE*3u));
    assert(e.vocal_recovered>recovering);
    printf("PASS %u setting-change windows with isolated output PCM and direct-vocal activity; %u reclaimed tails, %u invalid voices recovered\n",
           heard,e.vocal_reclaimed,e.vocal_recovered);
}

static void deadline_capped_entry(void)
{
    setup();
    /* Keep the protected channels full as a dense arrangement can at the
     * six-voice deadline floor. A sung entrance must replace a sequenced
     * lead rather than exceed the cap or cut a keyboard/bass voice. */
    for(unsigned i=0;i<6;i++){
        sr_voice *voice=note_on(&e,i<3?SR_BASS:SR_LEAD,48+i,90,8.f,0);
        assert(voice);
        if(i==3)voice->key=1;
        if(i==4)voice->external=1;
    }
    e.voice_limit=6;
    sr_set_music_mode(&e,SR_IDM);
    assert(recorded_vocal_phrase(&e,0,0));
    assert(e.voices[3].key&&e.voices[4].external);
    assert(e.voices[5].sample_zone);
    assert(audio_window(SR_RATE/8u));
}

static void repeated_mode_edits(void)
{
    static const uint8_t modes[]={SR_STANDARD,SR_IDM,SR_CINEMATIC,SR_IDM};
    setup();
    for(unsigned change=0;change<24;change++){
        unsigned mode=modes[change%4u];
        unsigned starting=e.vocal_started;
        unsigned heard=0;
        sr_set_music_mode(&e,mode);
        for(unsigned edit=0;edit<VF_COUNT;edit++){
            e.vocal_param_lock|=1u<<edit;
            e.vocal_param[edit]=(change*17u+edit*23u)%101u;
            e.vocal_level=30u+edit*3u;
            e.vocal_fx_send=(change+edit*7u)%101u;
            unsigned pcm=audio_window(SR_RATE/10u);
            if(e.music_mode==mode&&e.vocal_started>starting)heard|=pcm;
        }
        assert(e.music_mode==mode);
        assert(heard);
    }
    puts("PASS 24 mode transitions with concurrent VOCALS edits and direct PCM");
}

static void mode_tempo(void)
{
    setup();
    unsigned means[SR_MUSIC_MODES]={0};
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        e.music_mode=mode;
        for(unsigned scene=0;scene<SR_SCENES;scene++){
            sr_set_scene(&e,scene);
            unsigned initial=e.tick_length;
            uint32_t seed=e.song_seed;
            means[mode]+=(SR_RATE*15u+initial/2u)/initial;
            sr_set_scene(&e,(scene+1u)%SR_SCENES);
            sr_set_scene(&e,scene);
            assert(e.song_seed!=seed&&e.tick_length!=initial);
            unsigned previous=e.tick_length;
            sr_set_scene(&e,scene);
            assert(e.tick_length!=previous);
        }
    }
    assert(means[SR_CINEMATIC]<means[SR_STANDARD]);
    assert(means[SR_STANDARD]<means[SR_IDM]);
    unsigned manual=e.tick_length;
    sr_set_auto_tempo(&e,0);
    sr_set_scene(&e,1);
    assert(e.tick_length==manual);
    puts("PASS five mode tempo distributions, intention regeneration and manual lock");
}

int main(void)
{
    deadline_capped_entry();
    repeated_mode_edits();
    mode_tempo();
    setup();
    e.freeze=1;
    assert(recorded_vocal_phrase(&e,0,0));
    setup();
    assert(recorded_note(&e,60,VOCAL_DAY_ZONE,100,2.f));
    assert(recorded_note(&e,64,VOCAL_YOU_ZONE,100,2.f));
    sr_set_scene(&e,1);
    assert(recorded_vocal_phrase(&e,0,0));
    setup();
    e.vocal_play=2;
    sr_midi_note(&e,SR_CHOIR,0,60,100);
    sr_midi_note(&e,SR_CHOIR,0,64,100);
    assert(recorded_vocal_phrase(&e,0,0));
    assert(audio_window(SR_RATE));
    assert(e.vocal_reclaimed>0);
    sr_midi_panic(&e);
    stress();
    puts("PASS frozen arrangement keeps singer and scene changes reuse released sample slots");
}
