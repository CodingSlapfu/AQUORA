#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "serenity.h"
#include "../device/midi_core.h"
static sr_engine e;
static int16_t pcm[512];
static unsigned on,off;
static void event(unsigned r,unsigned n,unsigned v){assert(r<8&&n<128);if(v)on++;else off++;}
static unsigned held(unsigned channel){unsigned n=0;for(unsigned i=0;i<SR_VOICES;i++)n+=e.voices[i].external&&e.voices[i].gate&&e.voices[i].channel==channel;return n;}
int main(void){
 aq_midi_core m;aq_midi_defaults(&m);m.settings.input=1;
 assert(aq_midi_accept(&m,aq_midi_packet(0x90,60,100),1,0));
 assert(!aq_midi_accept(&m,aq_midi_packet(0x90,60,100),2,1));
 assert(!aq_midi_accept(&m,aq_midi_packet(0x90,128,100),1,1));
 assert(!aq_midi_valid(0x00649008));
 m.settings.rx_channel=2;
 assert(!aq_midi_accept(&m,aq_midi_packet(0x90,60,100),1,1));
 assert(aq_midi_accept(&m,aq_midi_packet(0x91,60,100),1,1));
 m.settings.rx_channel=0;
 for(unsigned i=0;i<=24;i++)assert(aq_midi_accept(&m,aq_midi_packet(0xf8,0,0),1,i*20));
 assert(aq_midi_step_frames(&m,480)==5292);
 /* Eight-hour virtual 120-BPM event stream, including source-lock resets. */
 for(unsigned i=0;i<1382400;i++){
  assert(aq_midi_accept(&m,aq_midi_packet(0xf8,0,0),1,500+(uint32_t)((uint64_t)i*500u/24u)));
  (void)aq_midi_step_frames(&m,500+(uint32_t)((uint64_t)i*500u/24u));
  if(i%24000==0)aq_midi_release_source(&m);
 }
 sr_init(&e,66);e.bed_level=e.reverb_amount=e.delay_amount=0;e.midi_event=event;
 sr_midi_note(&e,SR_LEAD,0,60,100);sr_midi_note(&e,SR_LEAD,1,60,90);
 assert(held(0)==1&&held(1)==1&&on==0);
 sr_render(&e,pcm,256);unsigned nonzero=0;for(unsigned i=0;i<512;i++)nonzero+=pcm[i]!=0;
 assert(nonzero>10);
 sr_midi_note(&e,SR_LEAD,0,60,0);assert(!held(0)&&held(1)==1&&off==0);
 sr_midi_bend(&e,1,8191);
 for(unsigned i=0;i<SR_VOICES;i++)if(e.voices[i].external&&e.voices[i].channel==1)
  assert(fabsf(e.voices[i].freq/sr_note_frequency(&e,60)-1.12246f)<.001f);
 sr_midi_panic(&e);assert(!held(1)&&off==0);
 for(unsigned n=0;n<2000;n++)sr_render(&e,pcm,256);
 for(unsigned i=0;i<512;i++)assert(!pcm[i]);
 sr_start(&e);e.variation=0;
 for(unsigned n=0;n<500;n++)sr_render(&e,pcm,256);
 assert(on>0);sr_stop(&e);sr_render(&e,pcm,256);assert(on==off);
 sr_start(&e);e.external_sync=1;
 for(unsigned n=0;n<100;n++)sr_render(&e,pcm,256);
 assert(e.sequence_step==1);
 e.tick_phase=e.tick_length;sr_render(&e,pcm,256);assert(e.sequence_step==2);
 puts("PASS USB-neutral parser, channel/source policy, 8h virtual clock stream, idle received audio, channel note-off/bend, no echo, TX gate lifecycle and external phase hold");
}
