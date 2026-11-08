#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static void prepare(void){sr_init(&e,732);e.variation=0;sr_start(&e);e.sequence_step=16;e.loop_pos=0;e.solo=SR_GUITAR;e.chord_mode=1;}
int main(void){
 prepare();e.air_request=3;trigger(&e);assert(e.air_actions==1&&e.air_result==3&&e.tracks[SR_GUITAR].events==3);
 prepare();e.chord_mode=3;e.air_request=3;trigger(&e);assert(e.air_actions==0&&e.air_result==131);
 prepare();e.tracks[SR_GUITAR].mute=1;e.air_request=3;trigger(&e);assert(e.air_actions==0&&e.air_result==131);
 prepare();e.solo=SR_DRUMS;e.drums_enabled=0;e.air_request=2;trigger(&e);assert(e.air_actions==0&&e.air_result==130);
 prepare();e.solo=SR_DRUMS;e.air_request=2;trigger(&e);assert(e.air_actions==1&&e.air_result==2);
 prepare();e.air_request=1;unsigned p=e.phrase;trigger(&e);assert(e.air_actions==1&&e.phrase==p+1);
 prepare();e.air_request=3;e.freeze=1;trigger(&e);assert(e.air_actions==0&&e.air_request==3);
 prepare();e.rhythm_pending=3;trigger(&e);assert(e.rhythm_style==2&&!e.rhythm_pending);
 prepare();e.rhythm_pending=2;sr_stop(&e);assert(e.rhythm_style==1&&!e.rhythm_pending);
 puts("PASS real phrase evolution/fill/harmony, no duplicate chord; OFF/mute block reported honestly; Freeze protects; rhythm commit at phrase and STOP");
}
