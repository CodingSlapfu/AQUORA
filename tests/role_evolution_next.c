#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "serenity.h"
static sr_engine e;
static int16_t pcm[512];
int main(void)
{
    unsigned supported=0,changing=0;
    for(unsigned scene=0;scene<SR_SCENES;scene++)
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++)
    for(unsigned loop=16;loop<=64;loop*=2){
        sr_init(&e,0x1234u+scene*313u+mode*17u+loop);
        sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
        sr_set_loop_length(&e,loop);e.variation=1.f;
        sr_start(&e);
        unsigned first=0,changed=0,previous_phrase=e.phrase;
        float last[SR_TRACKS];for(unsigned t=0;t<SR_TRACKS;t++)last[t]=e.role_gain[t];
        while(e.phrase<5){
            e.evolve_next=1;
            sr_render(&e,pcm,256);
            unsigned mask=0;
            for(unsigned t=0;t<SR_TRACKS;t++){
                assert(isfinite(e.role_gain[t]));
                assert(e.role_gain[t]>=0&&e.role_gain[t]<=1);
                assert(fabsf(e.role_gain[t]-last[t])<=256.f/SR_RATE+.00001f);
                last[t]=e.role_gain[t];
                if(e.role_target[t]>.5f)mask|=1u<<t;
                if(e.tracks[t].meter>.00001f)supported|=1u<<t;
                if(e.tracks[t].mute)assert(e.tracks[t].meter<.1f);
            }
            if(!first)first=mask+256u;
            if(mask!=(first&255u))changed=1;
            if(e.phrase!=previous_phrase){
                printf("ROLE %u %u %u %u %u",scene,mode,loop,e.phrase,mask);
                for(unsigned t=0;t<SR_TRACKS;t++)printf(" %.6f/%u",e.tracks[t].meter,e.tracks[t].events);
                puts("");previous_phrase=e.phrase;
            }
        }
        changing+=changed;
        e.freeze=1;e.evolve_next=0;
        float targets[SR_TRACKS];for(unsigned t=0;t<SR_TRACKS;t++)targets[t]=e.role_target[t];
        for(unsigned n=0;n<20;n++)sr_render(&e,pcm,256);
        for(unsigned t=0;t<SR_TRACKS;t++)assert(e.role_target[t]==targets[t]);
        sr_stop(&e);sr_render(&e,pcm,256);sr_render(&e,pcm,256);
        for(unsigned i=0;i<512;i++)assert(pcm[i]==0);
    }
    assert(supported==255u);
    assert(changing>100u);
    printf("PASS 117 intention/mode/loop cases; %u changing sets; all eight real post-gain roles observed; bounded fades, Freeze, STOP\n",changing);
}
