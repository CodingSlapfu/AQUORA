/* SPDX-License-Identifier: MPL-2.0
 * USB adapter over the reusable MIDI core. BLE is intentionally not exposed. */
static aq_midi_core midi;
static unsigned startup_chime=2;
static uint8_t midi_tx_notes[128],midi_rx_keys[16][128],midi_sustain[16];
static int16_t midi_bends[16];
static unsigned midi_tx_running,midi_tx_tick,midi_idle_clock,midi_panic_pending,midi_panic_channel;
static unsigned midi_engine_lost;
static unsigned midi_tx_count,midi_clock_sent,midi_drops,midi_usb_was,midi_status_ms;
static volatile unsigned midi_rx_overrun;
#ifndef PLAT_HOST
extern int plat_aquora_usb_ready(void);
#endif
#ifdef PLAT_HOST
static unsigned midi_test_online=1;
#endif
static int midi_usb_ready(void){
#ifdef PLAT_HOST
 return midi_test_online;
#else
 return plat_aquora_usb_ready();
#endif
}
static int midi_emit(unsigned status,unsigned a,unsigned b){
 if(!midi_usb_ready()||!plat_midi_out_room(TO_USB)){midi_drops++;return 0;}
 plat_midi_send(aq_midi_packet(status,a,b),TO_USB);midi_tx_count++;
 if(status==0xf8)midi_clock_sent++;
 return 1;
}
static void midi_panic(void){
 if(!midi_panic_pending)midi_panic_channel=midi.settings.tx_channel;
 midi_panic_pending=1;
 for(unsigned note=0;note<128;note++)if(midi_tx_notes[note]){
  if(!midi_emit(0x80u|midi_panic_channel,note,0))return;
  midi_tx_notes[note]=0;
 }
 if(midi_emit(0xb0u|midi_panic_channel,123,0))midi_panic_pending=0;
}
static void midi_note_event(unsigned role,unsigned note,unsigned velocity){
 if(role!=midi.settings.source||note>127||!midi.settings.usb_out)return;
 if(midi_panic_pending)return;
 if(velocity){
  if(!engine.running)return;
  if(!midi_tx_notes[note]&&!midi_emit(0x90u|midi.settings.tx_channel,note,velocity)){
   midi_panic();return;
  }
  if(midi_tx_notes[note]<SR_VOICES)midi_tx_notes[note]++;
 }else if(midi_tx_notes[note]){
  if(--midi_tx_notes[note]==0&&!midi_emit(0x80u|midi.settings.tx_channel,note,0)){
   midi_tx_notes[note]=1;midi_panic();
  }
 }
}
static void midi_receive_audio(uint32_t packet){
 unsigned s=(packet>>8)&255u,a=(packet>>16)&127u,b=packet>>24;
 if(s==0xff){sr_midi_panic(&engine);memset(midi_sustain,0,sizeof midi_sustain);memset(midi_rx_keys,0,sizeof midi_rx_keys);return;}
 if(s>=0xf8||s==0xf2){
  if(midi.settings.sync!=AQ_SYNC_SLAVE)return;
  engine.external_sync=1;
  if(s==0xfa){requested_play=1;sr_start(&engine);engine.external_clock_count=255;}
  else if(s==0xfb){requested_play=1;engine.running=1;engine.restart=0;engine.external_clock_count=255;}
  else if(s==0xfc){requested_play=0;sr_stop(&engine);sr_midi_panic(&engine);memset(midi_sustain,0,sizeof midi_sustain);memset(midi_rx_keys,0,sizeof midi_rx_keys);}
  else if(s==0xf2){
   unsigned pos=a|((b&127u)<<7);engine.sequence_step=pos;
   engine.loop_pos=pos%engine.loop_length;engine.tick_phase=engine.tick_length;
   engine.external_clock_count=255;
  }else if(s==0xf8){
   unsigned length=packet>>16;if(length)engine.tick_length=length;
   if(engine.running){
    if(engine.external_clock_count==255)engine.external_clock_count=0;
    else if(++engine.external_clock_count>=6){engine.external_clock_count=0;engine.tick_phase=engine.tick_length;}
   }
  }
  return;
 }
 unsigned type=s>>4,channel=s&15u,role=midi.settings.source;
 if(type==8||(type==9&&!b)){
  midi_rx_keys[channel][a]=0;
  if(!midi_sustain[channel])sr_midi_note(&engine,role,channel,a,0);
 }else if(type==9){midi_rx_keys[channel][a]=1;sr_midi_note(&engine,role,channel,a,b);if(midi_bends[channel])sr_midi_bend(&engine,channel,midi_bends[channel]);}
 else if(type==14){midi_bends[channel]=(int)(a|(b<<7))-8192;sr_midi_bend(&engine,channel,midi_bends[channel]);}
 else if(type==12)sr_set_scene(&engine,a%SR_SCENES);
 else if(type==11){
  if(a==64){
   midi_sustain[channel]=b>=64;
   if(!midi_sustain[channel])for(unsigned n=0;n<128;n++)if(!midi_rx_keys[channel][n])sr_midi_note(&engine,role,channel,n,0);
  }else if(a==120||a==123){
   for(unsigned n=0;n<128;n++)sr_midi_note(&engine,role,channel,n,0);
   midi_sustain[channel]=0;memset(midi_rx_keys[channel],0,sizeof midi_rx_keys[channel]);
  }
  else if(a==7||a==11)engine.tracks[role].volume=b*(1.f/127.f);
  else if(a==1&&role<=SR_BASS)engine.motion[role].vibrato_depth=b*50u/127u;
 }
}
static unsigned midi_position(void){
 return engine.sequence_step&&engine.tick_length?(engine.sequence_step-1u)*6u+engine.tick_phase*6u/engine.tick_length:0;
}
static void midi_audio_before(void){
 engine.midi_enabled=midi.settings.usb_out&&midi_usb_ready();
 if(midi_panic_pending)midi_panic();
 if(engine.running!=midi_tx_running){
  if(midi.settings.sync==AQ_SYNC_MASTER&&midi.settings.usb_out){
   if(engine.running&&engine.sequence_step){
    unsigned pos=(engine.sequence_step-1u)&16383u;
    midi_emit(0xf2,pos&127u,pos>>7);midi_emit(0xfb,0,0);
   }else midi_emit(engine.running?0xfa:0xfc,0,0);
  }
  if(!engine.running){midi_panic();midi_idle_clock=engine.tick_length?engine.tick_phase*6u%engine.tick_length:0;}
  else midi_idle_clock=0;
  midi_tx_running=engine.running;midi_tx_tick=midi_position();
  if(engine.running&&midi.settings.sync==AQ_SYNC_MASTER&&midi.settings.usb_out&&midi.settings.clock_out)
   midi_emit(0xf8,0,0);
 }
}
static void midi_audio_after(unsigned frames){
 if(engine.midi_event_lost!=midi_engine_lost){
  midi_engine_lost=engine.midi_event_lost;engine.midi_read=engine.midi_write;midi_panic();
 }
 uint32_t event;
 for(unsigned n=0;n<64&&sr_midi_event_pop(&engine,&event);n++)
  midi_note_event(event>>16,(event>>8)&127u,event&127u);
 if(midi.settings.sync!=AQ_SYNC_MASTER||!midi.settings.usb_out||!midi.settings.clock_out)return;
 if(engine.running&&engine.sequence_step&&engine.tick_length){
  unsigned position=midi_position();
  if(position<midi_tx_tick)midi_tx_tick=position;
  unsigned n=0;
  while(midi_tx_tick<position&&n++<24){midi_tx_tick++;midi_emit(0xf8,0,0);}
 }else if(midi.settings.clock_out==AQ_CLOCK_ALWAYS&&engine.tick_length){
  midi_idle_clock+=frames*6u;
  while(midi_idle_clock>=engine.tick_length){midi_idle_clock-=engine.tick_length;midi_emit(0xf8,0,0);}
 }
}
static void midi_poll(void){
 unsigned now=plat_ms(),online=midi_usb_ready();
 if(plat_usb_lost()||online!=midi_usb_was){
  send(200,(int)aq_midi_packet(0xff,0,0));
  aq_midi_release_source(&midi);midi_usb_was=online;
  /* The next audio callback flushes the previous channel before new notes. */
  send(201,0);
 }
 if(midi.source&&midi.settings.sync==AQ_SYNC_SLAVE&&requested_play&&now-midi.last_clock>2000u){
  send(200,(int)aq_midi_packet(0xfc,0,0));send(200,(int)aq_midi_packet(0xff,0,0));
  midi.timeouts++;aq_midi_release_source(&midi);
 }
 if(midi.active_sensing&&now-midi.last_sense>350u){
  send(200,(int)aq_midi_packet(0xff,0,0));midi.active_sensing=0;midi.timeouts++;
 }
 uint32_t packet;int port;
 for(unsigned n=0;n<32&&plat_midi_in(&packet,&port);n++){
  /* No BLE/TRS performance interface in this release; AIR owns the radio. */
  if(!online||port!=PORT_USB||!aq_midi_accept(&midi,packet,1,now))continue;
  unsigned status=(packet>>8)&255u;
  if(intro_active&&(status==0xfa||status==0xfb||((status>>4)==9&&(packet>>24)))){
   intro_active=0;dirty=1;last_control_ms=now;
  }
  if(status==0xf8){unsigned length=aq_midi_step_frames(&midi,now);packet=(packet&65535u)|(length<<16);}
  if(((qwrite+1u)&63u)==qread){midi_rx_overrun=1;midi_drops++;break;}
  send(200,(int)packet);
 }
 if(page==PAGE_SETUP&&setup_tab>=4&&now-midi_status_ms>=200u){dirty=1;midi_status_ms=now;}
}
typedef struct {uint32_t version;aq_midi_settings midi;uint8_t startup;} aq_usb_preferences;
static void midi_load(void){
 aq_midi_defaults(&midi);midi.settings.input=1;startup_chime=2;
 aq_usb_preferences p={0};
 if(plat_store_load(OBJ_MIDI,&p,sizeof p)==sizeof p&&p.version==0x41514d01u){
  if(p.midi.sync<=2&&p.midi.clock_out<=2&&p.midi.source<SR_DRUMS&&
    p.midi.tx_channel<16&&p.midi.rx_channel<=16&&p.midi.usb_in<=1&&p.midi.usb_out<=1){
   midi.settings=p.midi;midi.settings.radio=0;midi.settings.input=1;
  }
  if(p.startup<=2)startup_chime=p.startup;
 }
 memset(midi_tx_notes,0,sizeof midi_tx_notes);memset(midi_rx_keys,0,sizeof midi_rx_keys);
 midi_tx_running=midi_tx_tick=midi_idle_clock=midi_panic_pending=0;
 memset(midi_sustain,0,sizeof midi_sustain);memset(midi_bends,0,sizeof midi_bends);
 midi_usb_was=midi_usb_ready();midi_tx_count=midi_clock_sent=midi_drops=0;
 engine.midi_enabled=1;midi_engine_lost=0;engine.external_sync=midi.settings.sync==AQ_SYNC_SLAVE;
}
static int midi_save(void){
 aq_usb_preferences p={0};p.version=0x41514d01u;p.midi=midi.settings;p.midi.radio=0;p.startup=(uint8_t)startup_chime;
 return plat_store_save(OBJ_MIDI,&p,sizeof p);
}
