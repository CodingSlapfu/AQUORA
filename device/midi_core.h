/* SPDX-License-Identifier: MPL-2.0
 * Transport-neutral MIDI 1.0 policy; independent of the instrument and radio. */
#ifndef AQUORA_MIDI_CORE_H
#define AQUORA_MIDI_CORE_H
#include <stdint.h>
#define AQ_MIDI_PROFILE_VERSION 1u
enum {AQ_SYNC_OFF,AQ_SYNC_MASTER,AQ_SYNC_SLAVE};
enum {AQ_CLOCK_OFF,AQ_CLOCK_PLAYING,AQ_CLOCK_ALWAYS};
enum {AQ_RADIO_OFF,AQ_RADIO_HOST,AQ_RADIO_DEVICE,AQ_RADIO_AIR};
typedef struct {
 uint8_t radio,sync,clock_out,source,tx_channel,rx_channel,input,peer;
 uint8_t usb_in,usb_out,name_variant,reserved;
 uint8_t peer_addr[6],peer_random,have_peer;char peer_name[21];
} aq_midi_settings;
typedef struct {
 aq_midi_settings settings;
 uint32_t source,last_in,last_clock,clock_window,clock_count;
 uint32_t received,rejected,switches,timeouts,last_sense;uint8_t active_sensing;
} aq_midi_core;
static uint32_t aq_midi_packet(unsigned s,unsigned a,unsigned b){
 unsigned cin=s>=0xf8?15:s==0xf2?3:s==0xf1||s==0xf3?2:s>>4;
 return cin|(s<<8)|(a<<16)|(b<<24);
}
static int aq_midi_valid(uint32_t p){
 unsigned s=(p>>8)&255u,a=(p>>16)&255u,b=p>>24,c=p&15u;
 if(s>=0xf8)return c==15&&(s==0xf8||s==0xfa||s==0xfb||s==0xfc||s==0xfe||s==0xff);
 if(s==0xf2)return c==3&&a<128&&b<128;
 if(s<0x80||s>=0xf0||c!=(s>>4)||a>=128)return 0;
 return ((s>>4)==12||(s>>4)==13)||b<128;
}
static void aq_midi_defaults(aq_midi_core *m){
 *m=(aq_midi_core){0};m->settings.sync=AQ_SYNC_MASTER;
 m->settings.clock_out=AQ_CLOCK_PLAYING;m->settings.usb_in=m->settings.usb_out=1;
}
static int aq_midi_accept(aq_midi_core *m,uint32_t p,unsigned source,uint32_t now){
 unsigned s=(p>>8)&255u,pref=m->settings.input;
 if(!aq_midi_valid(p)||!source||(source==1&&!m->settings.usb_in)||
    (pref==1&&source!=1)||(pref==2&&source==1)||
    (s<0xf0&&m->settings.rx_channel&&(s&15u)!=m->settings.rx_channel-1u)){
  m->rejected++;return 0;
 }
 if(m->source&&m->source!=source){m->rejected++;return 0;}
 m->source=source;m->last_in=now;m->received++;
 if(s==0xfe){m->active_sensing=1;m->last_sense=now;}
 if(s==0xf8){m->last_clock=now;if(!m->clock_count)m->clock_window=now;m->clock_count++;}
 return 1;
}
static unsigned aq_midi_step_frames(aq_midi_core *m,uint32_t now){
 /* 25 edges span 24 intervals, a quarter note. Phase uses each sixth edge. */
 if(m->clock_count<25)return 0;
 uint32_t ms=now-m->clock_window;m->clock_count=1;m->clock_window=now;
 return ms>=200&&ms<=2000?ms*441u/40u:0;
}
static void aq_midi_release_source(aq_midi_core *m){m->source=m->clock_count=0;m->switches++;}
#endif
