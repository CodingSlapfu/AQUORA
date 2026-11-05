#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static sr_engine original, edited;
static int16_t a[512], b[512];

int main(void)
{
    sr_init(&original,43);
    sr_start(&original);
    original.tick_phase=0;
    original.solo=SR_LEAD;
    original.reverb_amount=original.delay_amount=0;
    original.bed_level=0;
    edited=original;
    sr_track *track=&edited.tracks[SR_LEAD];
    track->tone=6;
    track->attack=5;
    track->release=-5;
    const sr_track *neutral=&original.tracks[SR_LEAD];
    assert(sr_patch_preview(track,.125f)!=sr_patch_preview(neutral,.125f));
    assert(sr_patch_envelope(track,.1f,0)<sr_patch_envelope(neutral,.1f,0));
    assert(sr_patch_envelope(track,.5f,1)<sr_patch_envelope(neutral,.5f,1));
    sr_note_on(&original,SR_LEAD,65,105,1.f);
    sr_note_on(&edited,SR_LEAD,65,105,1.f);
    assert(edited.voices[0].attack_coef<original.voices[0].attack_coef);
    assert(edited.voices[0].release_coef>original.voices[0].release_coef);
    sr_render(&original,a,256);
    sr_render(&edited,b,256);
    assert(memcmp(a,b,sizeof a)!=0);
    puts("PASS preview follows DSP, envelope coefficients change and edited PCM differs");
}
