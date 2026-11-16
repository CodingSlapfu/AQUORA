#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"

static sr_engine reference,candidate;
static unsigned mode,starts,finishes;
static int available(void){return mode!=3;}
static int start(void (*job)(uint32_t),uint32_t arg)
{
    starts++;
    if(mode==2)return 0;
    if(mode==1){
        /* A dropped worker may have partially overwritten its own buffer. */
        memset(remote_fm.samples,0xff,sizeof remote_fm.samples);
    }else job(arg);
    return 1;
}
static int finish(void){finishes++;return mode==0;}
static const sr_fm_worker worker={available,start,finish};

int main(void)
{
    static int16_t expected[512],actual[512];
    for(mode=0;mode<4;mode++){
        starts=finishes=0;
        sr_init(&reference,710);
        sr_start(&reference);
        reference.midi_enabled=1;
        reference.filter_cutoff=9;
        reference.filter_resonance=11;
        reference.slicer_mask=7;
        for(unsigned voice=0;voice<SR_VOICES;voice++){
            unsigned role=voice%SR_DRUMS;
            sr_set_track_patch(&reference,role,(voice*13u)%SR_PATCHES);
            assert(note_on(&reference,role,45u+voice*2u,85u+voice%8u,.25f+voice*.30f));
        }
        candidate=reference;
        for(unsigned block=0;block<256;block++){
            sr_set_fm_worker(0);
            sr_render(&reference,expected,256);
            sr_set_fm_worker(&worker);
            sr_render(&candidate,actual,256);
            assert(memcmp(expected,actual,sizeof actual)==0);
            assert(memcmp(&reference,&candidate,sizeof reference)==0);
        }
        if(mode==3)assert(!starts&&!finishes);
        else {
            assert(starts>0);
            assert(finishes==(mode==2?0:starts));
        }
    }
    sr_set_fm_worker(0);
    puts("PASS exact PCM, voice and MIDI state with worker completion, dropped result, rejected launch and busy mailbox");
}
