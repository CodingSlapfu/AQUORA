/* Emit signed 16-bit stereo PCM for level-matched corrective comparisons. */
#include "serenity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sr_engine engine;
static int16_t pcm[512];

int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    unsigned id=(unsigned)atoi(argv[2]);
    unsigned seconds=20;
    sr_init(&engine,412);
    if(!strcmp(argv[1],"nature")){
        engine.isolate=SR_ISOLATE_NATURE;
        engine.bed_type=id;
        engine.bed_level=.8f;
        engine.reverb_amount=engine.delay_amount=0;
    }else if(!strcmp(argv[1],"patch")){
        engine.isolate=SR_ISOLATE_TRACK1+SR_PIANO;
        engine.tracks[SR_PIANO].patch=id;
        engine.bed_level=0;
        engine.reverb_amount=engine.delay_amount=0;
    }else if(!strcmp(argv[1],"drums")){
        engine.isolate=SR_ISOLATE_TRACK1+SR_DRUMS;
        engine.drums_enabled=1;
        engine.drum_kit=id;
        engine.drum_density=6;
        engine.drum_hit=72;
    }else if(!strcmp(argv[1],"evolve")){
        sr_set_scene(&engine,7);
        sr_generate_song(&engine,412,0);
        engine.variation=id*.28f;
        seconds=75;
    }else return 2;
    sr_start(&engine);
    for(unsigned block=0;block<seconds*SR_RATE/256u;block++){
        if(!strcmp(argv[1],"patch")&&block%345u==0)
            sr_note_on(&engine,SR_PIANO,60+block%7u,100,3.f);
        sr_render(&engine,pcm,256);
        if(fwrite(pcm,sizeof pcm,1,stdout)!=1)return 1;
    }
    return 0;
}
