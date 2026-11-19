#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static const char *const words[]={
    "PEACE","FAITH","HOPE","LIGHT","JOY","DIVINE","CONSOLE","UNDERSTAND",
    "RECEIVE","LIFE","WELCOME"
};
enum { WORDS=sizeof words/sizeof *words };
_Static_assert(WORDS==VOCAL_LAST_ZONE-VOCAL_PEACE_ZONE+1u,"Word bank coverage");

static void manual_words(const char *directory)
{
    for(unsigned i=0;i<WORDS;i++){
        sr_init(&engine,729);
        sr_set_vocals(&engine,1,4);
        engine.vocal_articulation=SR_VOCAL_WORDS_EXT;
        engine.vocal_play=1;
        engine.vocal_fx_manual=15;
        engine.vocal_tune=100;
        engine.vocal_delay=engine.vocal_reverb=engine.vocal_chorus=0;
        engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
        engine.master=.8f;
        sr_key_down(&engine,SR_CHOIR,i);
        sr_voice *voice=NULL;
        for(unsigned j=0;j<SR_VOICES;j++)
            if(engine.voices[j].key==i+1u)voice=&engine.voices[j];
        assert(voice&&voice->sample_zone==VOCAL_PEACE_ZONE+i+1u);
        unsigned word=voice->sample_zone-1u-VOCAL_DAY_ZONE;
        float phase=0.f;
        unsigned heard=0, coda=0, blocks=0;
        char path[512];
        int length=snprintf(path,sizeof path,"%s/%s.pcm",directory,words[i]);
        assert(length>0&&(unsigned)length<sizeof path);
        FILE *audio=fopen(path,"wb");
        assert(audio);
        while(voice->active&&blocks++<512u){
            int16_t pcm[256];
            sr_render(&engine,pcm,128);
            assert(fwrite(pcm,sizeof pcm,1,audio)==1u);
            assert(voice->phase>=phase);
            phase=voice->phase;
            heard+=engine.vocal_direct_peak>.001f;
            coda+=phase>vocal_words[word].release&&engine.vocal_direct_peak>.001f;
            for(unsigned j=0;j<256;j++)assert(pcm[j]!=32767&&pcm[j]!=-32768);
        }
        assert(fclose(audio)==0);
        assert(!voice->active&&heard>20&&coda>0);
        printf("Manual %-10s MIDI %u, audible blocks %u, coda %u\n",
               words[i],voice->midi,heard,coda);
    }
}

static void automatic_words(void)
{
    unsigned reached=0;
    const unsigned all=(1u<<WORDS)-1u;
    const unsigned positions[]={0u,8u,12u};

    /* Put the actual AUTO vocal director in a Bloom bar and exercise its
       chord/slot selection. Then render the selected voice to its finish. */
    for(unsigned seed=1;seed<=12&&reached!=all;seed++){
        for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
            for(unsigned scene=0;scene<SR_SCENES;scene++){
                sr_init(&engine,seed*501u+scene);
                sr_set_music_mode(&engine,mode);
                sr_set_scene(&engine,scene);
                sr_set_vocals(&engine,1,4);
                sr_start(&engine);
                engine.form_route[0]=12u;
                engine.form_loop=scene&1u;
                engine.loop_pos=1u;
                engine.vocal_entry_pending=0;
                engine.external_sync=1;
                engine.tick_phase=0;
                engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
                engine.master=.8f;
                for(unsigned slot=0;slot<3u;slot++){
                    unsigned step=positions[slot];
                    unsigned zone=auto_word_phrase(&engine,engine.form_loop,2u,slot,vocal_flow_for(&engine,2u));
                    if(zone<VOCAL_PEACE_ZONE||reached&(1u<<(zone-VOCAL_PEACE_ZONE)))continue;
                    engine.frame_count=2000u+step*engine.tick_length;
                    engine.vocal_phrase_note=0;
                    unsigned chord=composition_chord(&engine,step);
                    recorded_vocal_phrase(&engine,step,chord);
                    for(unsigned v=0;v<SR_VOICES;v++){
                        sr_voice *voice=&engine.voices[v];
                        if(!voice->active||voice->sample_zone!=zone+1u)continue;
                        unsigned heard=0, blocks=0;
                        while(voice->active&&blocks++<512u){
                            int16_t pcm[256];
                            sr_render(&engine,pcm,128);
                            heard+=engine.vocal_direct_peak>.001f;
                        }
                        assert(!voice->active&&heard>20u);
                        reached|=1u<<(zone-VOCAL_PEACE_ZONE);
                        printf("AUTO %-10s seed pass %u, scene %u, mode %u, step %u, audible blocks %u\n",
                               words[zone-VOCAL_PEACE_ZONE],seed,scene,mode,step,heard);
                    }
                    memset(engine.voices,0,sizeof engine.voices);
                }
            }
        }
    }
    assert(reached==all);
}

static void glide_at_each_tune_setting(void)
{
    for(unsigned tune=0;tune<=100;tune+=50){
        sr_init(&engine,729);
        sr_set_vocals(&engine,1,4);
        engine.vocal_fx_manual=15;
        engine.vocal_tune=tune;
        engine.vocal_param_lock=1u<<VF_GLIDE;
        engine.vocal_param[VF_GLIDE]=80;
        engine.vocal_param_now[VF_GLIDE]=80;
        engine.external_sync=1;
        sr_start(&engine);
        sr_voice *voice=recorded_note(&engine,64,VOCAL_HELLO_ZONE,100,4);
        assert(voice);
        const float from=10.f*256.f;
        int16_t pcm[256];
        voice->phase=from;
        sr_render(&engine,pcm,128);
        float normal=voice->phase-from;
        voice->phase=from;
        voice->pitch=voice->freq*.88f;
        sr_render(&engine,pcm,128);
        float glide=voice->phase-from;
        assert(glide>normal*.85f&&glide<normal*.93f);
        printf("TUNE %u: PCM advance %.3f flat, %.3f in glide\n",tune,normal,glide);
    }
}

int main(int argc,char **argv)
{
    assert(argc==2);
    manual_words(argv[1]);
    glide_at_each_tune_setting();
    fflush(stdout);
    automatic_words();
}
