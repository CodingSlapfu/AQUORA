#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine e;

static void setup(void)
{
    sr_init(&e,729);
    sr_start(&e);
    sr_set_vocals(&e,1,4);
    e.running=e.external_sync=1;
    e.master=.8f;
    e.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    e.form_route[0]=12;
}

int main(void)
{
    static const char *const names[]={
        "SHINE","FREE","FLY","STAY","AWAY","HOME","HOPE","ALIVE","TONIGHT","FOREVER"
    };
    uint32_t signatures[10];
    for(unsigned i=0;i<10;i++){
        setup();
        e.vocal_fx_manual=15;
        e.vocal_delay=e.vocal_reverb=e.vocal_chorus=0;
        e.vocal_param_lock=(1u<<VF_COUNT)-1u;
        sr_voice *v=recorded_note(&e,64,VOCAL_SHINE_ZONE+i,100,.85f);
        assert(v&&v->midi==64);
        unsigned audible=0,tail=0,peak=0;
        uint32_t hash=2166136261u;
        for(unsigned j=0;j<720;j++){
            int16_t pcm[256];
            sr_render(&e,pcm,128);
            audible+=e.vocal_direct_peak>.001f;
            if(v->active&&!v->gate&&v->phase>vocal_words[VOCAL_SHINE_ZONE+i-VOCAL_DAY_ZONE].release)
                tail+=e.vocal_direct_peak>.001f;
            for(unsigned n=0;n<256;n++){
                unsigned abs_sample=pcm[n]<0?-(int)pcm[n]:pcm[n];
                if(abs_sample>peak)peak=abs_sample;
                hash=(hash^(uint16_t)pcm[n])*16777619u;
            }
        }
        assert(audible>40&&tail>2&&peak>100&&peak<30000&&!v->active);
        for(unsigned j=0;j<i;j++)assert(hash!=signatures[j]);
        signatures[i]=hash;
        fprintf(stdout,"%s direct blocks=%u tail=%u peak=%u PCM=%08x\n",names[i],audible,tail,peak,hash);
    }
    unsigned count[4]={0},holds[4]={0},pitch[4]={0},tempo=0;
    for(unsigned flow=SR_VOCAL_HALF;flow<=SR_VOCAL_DOUBLE;flow++){
        setup();
        e.vocal_flow=flow;
        if(!tempo)tempo=e.tick_length;
        assert(e.tick_length==tempo);
        for(unsigned step=0;step<16;step++){
            unsigned before=e.vocal_started;
            recorded_vocal_phrase(&e,step,0);
            if(e.vocal_started==before)continue;
            count[flow]++;
            if(step==0){
                for(unsigned v=0;v<SR_VOICES;v++)if(e.voices[v].active&&e.voices[v].sample_zone){
                    holds[flow]=e.voices[v].remain;
                    pitch[flow]=e.voices[v].midi;
                    break;
                }
            }
        }
        printf("flow=%u onsets=%u opening_frames=%u midi=%u tick=%u\n",
               flow,count[flow],holds[flow],pitch[flow],e.tick_length);
    }
    assert(count[1]<count[2]&&count[2]<count[3]);
    assert(holds[1]>holds[2]&&holds[2]>holds[3]);
    assert(pitch[1]&&pitch[1]==pitch[2]&&pitch[2]==pitch[3]);
    unsigned seen=0;
    for(unsigned scene=0;scene<SR_SCENES;scene++)
      for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        setup();e.scene=scene;e.music_mode=mode;
        for(unsigned section=0;section<5;section++)
          for(unsigned tick=6500;tick<=15000;tick+=2000){
            e.tick_length=tick;
            seen|=1u<<vocal_flow_for(&e,section);
          }
      }
    assert(seen==14u);
    puts("PASS all 10 generated clips through the allocator and PCM mixer; distinct flow timing");
}
