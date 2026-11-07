#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "../device/dsp/engine.c"
static sr_engine e;
static sr_voice *latest(unsigned role){for(unsigned i=0;i<16;i++)if(e.voices[i].active&&e.voices[i].track==role&&e.voices[i].born==e.frame_count)return &e.voices[i];return 0;}
int main(void){
 for(unsigned scene=0;scene<21;scene++)for(unsigned mode=0;mode<3;mode++){
  sr_init(&e,732);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);
  for(unsigned role=0;role<2;role++)assert(e.motion[role].glide_ms<=80&&e.motion[role].vibrato_depth<=4);
  e.motion[0]=(sr_motion){640,2,25,67};sr_motion_auto(&e,0,0);sr_set_scene(&e,(scene+1)%21);
  assert(e.motion[0].glide_ms==640&&e.motion[0].vibrato_depth==25);
  for(unsigned p=0;p<24;p++){e.phrase=p;plan_roles(&e);if(song_section(&e)==3){assert(e.role_target[SR_BASS]==.55f);assert(e.role_target[SR_DRONE]==1);}}
 }
 sr_init(&e,1);sr_set_scene(&e,1);e.preview_frames=SR_RATE;e.sequence_step=5;sr_note_on(&e,0,60,70,2);
 e.frame_count+=11025;e.sequence_step=6;sr_note_on(&e,0,62,70,2);sr_voice *v=latest(0);assert(v&&v->pitch!=v->freq);
 e.frame_count+=11025;e.sequence_step=7;sr_note_on(&e,0,64,70,2);v=latest(0);assert(v&&v->pitch==v->freq);
 e.frame_count+=11025;e.sequence_step=6;sr_note_on(&e,0,84,70,2);v=latest(0);assert(v&&v->pitch==v->freq);
 e.motion[0]=(sr_motion){640,2,25,67};sr_motion_auto(&e,0,0);e.frame_count+=11025;e.sequence_step=0;sr_note_on(&e,0,65,70,1);v=latest(0);assert(v&&v->pitch!=v->freq);
 puts("PASS 63 intention/style profiles, AUTO ceilings and MANUAL precedence; neighboring phrase-answer glide only; detached/large-leap bypass; hush tonal anchors");
}
