#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"
#include "../device/kira_face/vocal_pose.h"

static sr_engine engine;
static uint16_t neutral[240*240], rendered[240*240];

static void verify_phrase(void)
{
    int16_t pcm[512];
    unsigned heard=0, animated=0;
    for(unsigned block=0;block<96;block++){
        sr_render(&engine,pcm,256);
        unsigned peak=0;
        for(unsigned i=0;i<512;i++){
            unsigned mag=pcm[i]<0?-pcm[i]:pcm[i];
            if(mag>peak)peak=mag;
        }
        k9_face pose=k9_recorded_vocal_pose(&engine);
        heard+=peak>100;
        if(peak>100&&pose.open&&pose.mouth){
            for(unsigned y=0;y<240;y+=48)k9_face_render(rendered+y*240,y,48,&pose);
            animated+=memcmp(neutral,rendered,sizeof neutral)!=0;
        }
    }
    assert(heard>16&&animated>16);
}

static void verify_hidden(void)
{
    int16_t pcm[512];
    for(unsigned block=0;block<6;block++){
        k9_face pose=k9_recorded_vocal_pose(&engine);
        assert(!pose.mouth&&!pose.open&&!pose.jaw&&!pose.neck);
        for(unsigned y=0;y<240;y+=48)k9_face_render(rendered+y*240,y,48,&pose);
        assert(memcmp(neutral,rendered,sizeof neutral)==0);
        sr_render(&engine,pcm,256);
        for(unsigned i=0;i<512;i++)assert(pcm[i]==0);
    }
}

int main(void)
{
    const k9_face closed={0};
    for(unsigned y=0;y<240;y+=48)k9_face_render(neutral+y*240,y,48,&closed);
    sr_init(&engine,714);
    sr_set_vocals(&engine,1,4);
    engine.master=.8f;
    engine.running=1;
    engine.external_sync=1;
    engine.reverb_amount=engine.delay_amount=engine.tone_level=engine.bed_level=0.f;
    engine.reverb_wet=engine.delay_wet=0.f;
    engine.solo=SR_CHOIR;
    assert(recorded_note(&engine,69,0,100,32.f));
    verify_phrase();
    engine.tracks[SR_CHOIR].mute=1;
    verify_hidden();
    engine.tracks[SR_CHOIR].mute=0;
    verify_phrase();
    engine.isolate=SR_ISOLATE_TRACK1+SR_LEAD;
    verify_hidden();
    engine.isolate=SR_ISOLATE_TRACK1+SR_CHOIR;
    verify_phrase();
    engine.solo=SR_LEAD;
    verify_hidden();
    puts("PASS recorded vocal PCM and portrait bands follow mute, solo, and source isolation");
}
