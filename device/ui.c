/* SPDX-License-Identifier: MPL-2.0 */
static const char *pages[]={"PLAY","SOUND","EVOLVE","MIX","DRUMS","SETUP","AIR"};
static const char *beds[]={"Off","Wind","Water","Rain","Forest","Ocean","Birds","Leaves"};
static const char *groups[]={"Instrument","Nature","Frequency","Binaural","Percussion","Sacred Bass","Lead / Bass Motion","Track Routing","Percussion HIT","Visualizer","Arrangement Banks","Source Isolate","Keyboard","Lock Textures"};
static void num(char *s,int v){char t[12];int n=0;unsigned u=v<0?-v:v;if(v<0)*s++='-';do{t[n++]=(char)('0'+u%10);u/=10;}while(u);while(n)*s++=t[--n];*s=0;}
static void label(int x,int y,const char *name,int value){char v[12];num(v,value);cv_text(x,y,&FONT_XS,name,RGB(126,166,164));cv_text(x,y+15,&FONT_B,v,RGB(218,240,220));}
static const char *role_names[SR_TRACKS]={
 "Lead","Bass","Pads","Harmony","Piano","Plucks","Choir","Drums"
};
static const uint16_t colors[SR_TRACKS]={
  RGB(97,241,244),RGB(250,171,64),RGB(106,179,255),RGB(213,130,255),
  RGB(143,255,165),RGB(245,139,162),RGB(206,203,255),RGB(255,207,103)
 };
static void geometry(int offset)
{
 for(int t=0;t<SR_TRACKS;t++){
   int quiet=engine.tracks[t].mute||(engine.solo<SR_TRACKS&&engine.solo!=t);
   uint16_t c=quiet?RGB(25,45,49):colors[t];
   if(vis_input.water){
     static const uint16_t crystal_colors[8]={
       RGB(119,255,243),RGB(63,199,255),RGB(110,175,255),RGB(179,111,255),
       RGB(66,245,184),RGB(224,142,255),RGB(62,132,234),RGB(42,116,187)
     };
     c=crystal_colors[(t+(t<6?vis_input.scene%2:0))&7];
     if(quiet&&t<6)c=RGB(42,84,108);
   }
   for(unsigned j=1;j<vis_output.count[t];j++){
     int y0=vis_output.xy[t][j-1][1]-offset,y1=vis_output.xy[t][j][1]-offset;
     if((y0<0&&y1<0)||(y0>=48&&y1>=48))continue;
     cv_line(vis_output.xy[t][j-1][0],y0,vis_output.xy[t][j][0],y1,c);
   }
 }
}
static const char *onoff(int enabled){return enabled?"ON":"OFF";}
static const char *visual_name(void){return visual_mode?"Resonance":"Original";}
static const char *patch_name(void)
{
 return selected==SR_DRUMS?sr_drum_kits[engine.drum_kit].name:
      sr_patches[engine.tracks[selected].patch].name;
}
static const char *drum_status(void)
{
 if(!engine.drums_enabled)return "DRUMS OFF";
 if(engine.tracks[SR_DRUMS].mute)return "TRACK 8 MUTED";
 if(engine.solo<SR_TRACKS&&engine.solo!=SR_DRUMS)return "OTHER TRACK SOLO";
 if(!engine.drum_density)return "DENSITY ZERO";
 if(engine.drum_level<=0||engine.tracks[SR_DRUMS].volume<=0)return "DRUM LEVEL ZERO";
 return engine.solo==SR_DRUMS?"DRUMS SOLO":"DRUMS READY";
}
static void limited_text(int x,int y,int width,const felucca_font_t *font,const char *s,uint16_t color)
{
 char text[36];
 unsigned length=0;
 int used=0;
 while(s[length]&&length+1<sizeof(text)){
   int advance=font->adv[glyph(font,(uint8_t)s[length])];
   if(used+advance>width)break;
   text[length]=s[length];
   used+=advance;length++;
 }
 text[length]=0;
 cv_text(x,y,font,text,color);
}
static const char *tenth(char *buffer,int value)
{
 num(buffer,value/10);
 unsigned n=0;while(buffer[n])n++;
 buffer[n++]='.';buffer[n++]=(char)('0'+value%10);buffer[n]=0;
 return buffer;
}
static const char *knob_value(unsigned id,const char **name,char *buffer)
{
 sr_track *track=&engine.tracks[page==PAGE_DRUMS?SR_DRUMS:selected];
 sr_motion *motion=&engine.motion[selected<=SR_BASS?selected:0];
 int number=0;
 switch(id){
 case 1:*name="SCENE";return sr_scenes[engine.scene].name;
 case 2:*name=selected==SR_DRUMS?"KIT":"PATCH";return patch_name();
 case 4:*name="FIXED Hz";return sr_frequencies[engine.frequency].name;
 case 5:*name="NATURE %";number=engine.bed_level*100;break;
 case 6:*name="BINAURAL";return onoff(engine.binaural_enabled);
 case 7:*name="BEAT Hz";return tenth(buffer,engine.binaural_rate*10+.5f);
 case 8:*name="CARRIER Hz";number=engine.binaural_carrier;break;
 case 9:*name="DRUMS";return onoff(engine.drums_enabled);
 case 10:*name="DENSITY";number=engine.drum_density;break;
 case 11:*name="REVERB %";number=engine.reverb_amount*100;break;
 case 12:*name="DELAY %";number=engine.delay_amount*100;break;
 case 13:*name="TRACK %";number=track->volume*100;break;
 case 14:*name="A4 TUNING Hz";return engine.tuning_432?"432":"440";
 case 16:*name="DRUM KIT";return sr_drum_kits[engine.drum_kit].name;
 case 17:*name="DRUM LEVEL %";number=engine.drum_level*100;break;
 case 18:*name="ENERGY %";number=engine.energy*100;break;
 case 19:*name="VARIATION %";number=engine.variation*100;break;
 case 20:*name="FREEZE";return onoff(engine.freeze);
 case 21:*name="NATURE BED";return beds[engine.bed_type];
 case 22:*name="FIXED TONE %";number=engine.tone_level*100;break;
 case 23:*name="BINAURAL %";number=engine.binaural_level*100;break;
 case 24:*name="PAN %";number=track->pan*100;break;
 case 25:*name="TEMPO BPM";number=(SR_RATE*15u+engine.tick_length/2)/engine.tick_length;break;
 case 28:*name="SOLO";return engine.solo<SR_TRACKS?role_names[engine.solo]:"OFF";
 case 29:*name="FX SEND %";number=track->send*100;break;
 case 30:*name="SACRED LEVEL %";number=engine.sacred_level*100;break;
 case 31:*name="SACRED SOURCE";return engine.sacred_mode==0?"Off":engine.sacred_mode==1?"Musical":"Reference";
 case 32:*name="GLIDE MODE";return selected>SR_BASS?"Use SELECT knob":motion->glide_mode==0?"Off":motion->glide_mode==1?"Legato":"Always";
 case 33:*name="GLIDE ms";if(selected>SR_BASS)return "-";number=motion->glide_ms;break;
 case 34:*name="VIBRATO Hz";if(selected>SR_BASS)return "-";return tenth(buffer,motion->vibrato_rate);
 case 35:*name="VIBRATO cents";if(selected>SR_BASS)return "-";number=motion->vibrato_depth;break;
 case 36:*name="HARMONIC %";number=engine.sacred_harmonics*100;break;
 case 37:*name="HIT %";number=engine.drum_hit;break;
 case 38:*name="VISUALIZER";return visual_name();
 case 39:*name="BANK";number=bank_slot+1;break;
 case 40:*name="ACTION";return bank_action?"SAVE":"LOAD";
 case 41:*name="OVERWRITE";return bank_confirm?"ARMED":"LOCKED";
 case 55:*name="EXTRA TEXTURES";return engine.lock_textures?"ON / not pure":"OFF / strict";
 case 58:*name="NEW SONG";return engine.running?"Stop first":"Turn right";
 case 59:*name="REROLL SOUNDS";return engine.running?"Stop first":"Turn right";
 case 60:*name="TRACK LOCK";return onoff(engine.sound_locks&(1u<<selected));
 case 64:{static const char *m[]={"Intention","Straight","Swing"};*name="GROOVE";return m[engine.groove_mode];}
 case 65:*name="STRENGTH %";number=engine.groove_strength;break;
 case 66:*name="AIR RADIO";return FM1B_BLE_CENTRAL?(air_on?"Scanning":"Off"):"Unavailable";
 case 67:*name="AIR TARGET";return engine.air_track<SR_TRACKS?role_names[engine.air_track]:"All tracks";
 case 68:*name="CALIBRATE";return air_calibrated?"Ready":"Turn right";
 case 71:*name="QUICK FIND";return quick_names[quick_item];
 case 72:*name="OPEN";return "Turn right";
 case 69:*name="AIR HOLD";return onoff(air_hold);
 case 75:*name="BRIGHTNESS";number=engine.tracks[selected].tone;break;
 case 76:*name="ATTACK";number=engine.tracks[selected].attack;break;
 case 77:*name="RELEASE";number=engine.tracks[selected].release;break;
 case 73:*name="KEY SNAP %";number=engine.play_strength;break;
 case 74:*name="KEY FEEL";return engine.play_feel?"Intention":"Straight";
 case 63:{static const char *grid[]={"Live","1/16","1/32","Auto","1/8"};*name="PLAY GRID";return grid[engine.play_grid];}
 case 62:*name="PRESENCE %";number=engine.drum_presence;break;
 case 61:*name="NATURE / FX LOCK";return onoff(engine.sound_locks&(SR_SOUND_NATURE|SR_SOUND_FX));
 case 56:{static const char *modes[]={"On","Keys","Off","Dance"};*name="LIGHTS";return modes[lighting];}
 case 57:*name="VISUAL FPS";return fps_auto?"Auto / 20":"20 target";
 case 54:*name="AUDITION";return engine.nature_audition?"Playing":"Turn right";
 case 49:*name="MUTE";return onoff(track->mute);
 case 50:*name="AUTO GAIN";return onoff(engine.auto_gain);
 case 51:*name="AUTO LEVEL";return onoff(engine.auto_level);
 case 52:*name="RESET MIX";return mix_reset?"Save to reset":"Turn to arm";
 case 48:*name="PITCH MODE";return engine.pending_lock?"Frequency Lock":"Reference Layer";
 case 47:*name="KEYBOARD";return engine.keyboard_mode?"Change Key":"Play Along";
 case 46:*name=engine.pending_length?"NEXT LOOP STEPS":"LOOP STEPS";number=engine.pending_length?engine.pending_length:engine.loop_length;break;
 case 45:*name="HZ SOURCE";return engine.frequency_auto?"Auto / intention":"Manual";
 case 43:
   *name="ISOLATE";
   if(engine.isolate==SR_ISOLATE_MIX)return "Full mix";
   if(engine.isolate<SR_ISOLATE_FIXED)return role_names[engine.isolate-SR_ISOLATE_TRACK1];
   {
     static const char *sources[]={"Fixed tone","Sacred Bass","Binaural","Nature","Wet FX"};
     return sources[engine.isolate-SR_ISOLATE_FIXED];
   }
 case 42:*name="VOICE";number=voice_slot+1;break;
 default:*name="";return "-";
 }
 num(buffer,number);
 return buffer;
}
static int text_pixels(const felucca_font_t *font,const char *text)
{
 int width=0;
 for(;*text;text++)width+=font->adv[glyph(font,(uint8_t)*text)];
 return width;
}
static void knob_cell(unsigned knob,int x)
{
 char value[16],tag[]="K1 ";
 const char *name;
 const char *shown=knob_value(control_for(knob),&name,value);
 tag[1]=(char)('1'+knob);
 int label_x=cv_text(x,0,&FONT_XS,tag,RGB(143,213,201));
 limited_text(label_x,0,108-(label_x-x),&FONT_XS,name,RGB(126,166,164));
 int focus=knob_until&&knob_focus==knob;
 const felucca_font_t *font=focus&&knob<2&&text_pixels(&FONT_M,shown)<=108?&FONT_M:&FONT_B;
 if(text_pixels(font,shown)>108)font=&FONT_XS;
 limited_text(x,14,108,font,shown,focus?RGB(255,229,156):RGB(218,240,220));
}
static void voice_diagnostics(int band)
{
 sr_voice voice;
 unsigned irq=plat_lock();
 voice=engine.voices[voice_slot];
 unsigned age=engine.frame_count-voice.born;
 unsigned notes_on=engine.note_ons, notes_off=engine.note_offs, watchdogs=engine.watchdogs;
 plat_unlock(irq);
 if(band==0){
   cv_text(8,1,&FONT_B,"VOICE INSPECTOR",C_WHITE);
   cv_text(8,25,&FONT_XS,"K1 SELECT   SEQ ALL NOTES OFF",C_WHITE);
 }else if(band==1){
   label(8,0,"VOICE SLOT",voice_slot+1);
   cv_text(126,0,&FONT_XS,"ROLE / ORIGIN",RGB(126,166,164));
   cv_text(126,15,&FONT_B,voice.active?role_names[voice.track]:"Idle",C_WHITE);
   const char *origin=voice.key?"Physical key":"Generator";
   if(voice.active&&engine.tracks[voice.track].mute)origin=voice.key?"Key / muted":"Gen / muted";
   cv_text(126,31,&FONT_XS,voice.active?origin:"-",C_WHITE);
 }else if(band==2){
   label(8,0,"MIDI / AGE ms",voice.active?voice.midi:0);
   label(126,0,"REMAIN ms",voice.active?voice.remain/(SR_RATE/1000):0);
   char value[16];
   num(value,voice.active?age/(SR_RATE/1000):0);cv_text(50,15,&FONT_B,value,C_WHITE);
   cv_text(126,31,&FONT_XS,voice.active?(voice.gate?"Gate on":"Release"):"Idle",C_WHITE);
 }else if(band==3){
   label(8,0,"NOTE ON / OFF",notes_on);
   char value[16];num(value,notes_off);cv_text(62,15,&FONT_B,value,C_WHITE);
   label(126,0,"GUARDS",watchdogs);
   num(value,voice.active?voice.env*100:0);cv_text(8,31,&FONT_XS,"ENVELOPE %",C_WHITE);
   cv_text(86,31,&FONT_XS,value,C_WHITE);
 }else{
   label(8,0,"SACRED ACTIVE Hz",engine.sacred_gain>.0001f?engine.sacred_hz_now:0);
   label(126,0,"FIXED ACTIVE Hz",engine.running&&engine.solo==SR_TRACKS&&engine.tone_level>0?sr_frequencies[engine.frequency].hz:0);
   cv_text(8,32,&FONT_XS,engine.binaural_enabled?"BINAURAL ON":"BINAURAL OFF",C_WHITE);
   cv_text(126,32,&FONT_XS,engine.solo<SR_TRACKS?role_names[engine.solo]:"SOLO OFF",C_WHITE);
 }
}
static void performance(int band)
{
 if(debug==3){voice_diagnostics(band);return;}
 if(debug==2){
   if(band==0){
     cv_text(8,1,&FONT_B,"CALLBACK / CORE",C_WHITE);
     cv_text(8,25,&FONT_XS,"256 FRAMES / 5805 us / GLO VOICES",C_WHITE);
   }
   if(band==1){label(8,0,"CALLBACK MEAN us",callback_stats.mean);label(127,0,"CALLBACK PEAK us",callback_stats.peak);}
   if(band==2){label(8,0,"CALLBACK P95 us",callback_stats.p95);label(127,0,"CALLBACK P99 us",callback_stats.p99);}
   if(band==3){label(8,0,"GEOMETRY us",graphics_us);label(127,0,"UI DRAW us",frame_us);}
   if(band==4){
     int voices=0;for(int i=0;i<SR_VOICES;i++)voices+=!!engine.voices[i].active;
     label(8,0,"ACTIVE VOICES",voices);label(127,0,"INPUT DROPS",dropped);
   }
   return;
 }
 if(band==0){
   cv_text(8,1,&FONT_B,"PERFORMANCE " APP_VERSION,C_WHITE);
   cv_text(8,25,&FONT_XS,fps_auto?"K1 VISUAL FPS: AUTO (20 TARGET)":"K1 VISUAL FPS: 20 TARGET",RGB(147,221,207));
 }else if(band==1){
   label(8,0,"PLAY BLITS FPS x10",shown_fps10);
   label(127,0,"MISSED / 2s",shown_missed);
   cv_text(8,31,&FONT_XS,"DRAW us",C_WHITE);
   char draw[12];num(draw,frame_us);cv_text(65,31,&FONT_XS,draw,C_WHITE);

 }else if(band==2){
   label(8,0,"AUDIO MEAN us",audio_stats.mean);
   label(127,0,"FX MEAN us",fx_stats.mean);
   cv_text(8,32,&FONT_XS,"GLO: CALLBACK AND VOICE STATS",C_WHITE);
 }else if(band==3){
   label(8,0,"DMA XRUN DELTA",xrun_delta);
   label(127,0,"AUDIO+FX % BUDGET",(audio_stats.mean+fx_stats.mean)*100/5805);
   char v[12];num(v,plat_xruns());cv_text(66,15,&FONT_B,v,C_WHITE);
 }else if(band==4){
   cv_text(8,0,&FONT_XS,plat_cpu2_on()?"CORE 1 GEOMETRY / CORE 0 AUDIO":"CORE 0 / SINGLE-CORE FALLBACK",C_WHITE);
   label(8,12,"WORK / WAIT us",vis_output.busy_us);
   char v[12];num(v,worker_wait_us);cv_text(66,27,&FONT_B,v,C_WHITE);
   label(127,12,"FALLBACK / BAD",worker_fallbacks);
   num(v,plat_cpu2_bad());cv_text(185,27,&FONT_B,v,C_WHITE);
 }
}
static void mixer_overview(int band)
{
 static const char *names[]={"LD","BS","PD","HM","PN","PL","CH","DR","Nature","Sacred","Ref","Binaural","FX"};
 unsigned first=band==1?0:SR_TRACKS, end=band==1?SR_TRACKS:SR_TRACKS+SR_BUS_COUNT;
 unsigned count=0;
 for(unsigned channel=first;channel<end;channel++)count+=mix_visible(channel);
 if(!count){
   cv_text(8,12,&FONT_XS,band==1?"NO ACTIVE TRACKS":"NO ACTIVE BUSES",C_WHITE);
   return;
 }
 int width=232/(int)count, x=4;
 for(unsigned channel=first;channel<end;channel++){
   if(!mix_visible(channel))continue;
   unsigned bus=channel>=SR_TRACKS;
   unsigned id=bus?channel-SR_TRACKS:channel;
   const sr_track *track=&engine.tracks[id];
   uint16_t color=!bus&&track->mute?RGB(75,90,112):colors[id];
   if(channel==mix_focus)cv_line(x,0,x+width-3,0,C_WHITE);
   limited_text(x+2,2,width-3,&FONT_XS,names[channel],color);
   float magnitude=bus?engine.bus_meter[id]:track->meter;
   int meter=(int)(fm_sqrtf(fm_sqrtf(fm_clampf(magnitude,0,1)))*26.f);
   for(int w=0;w<5;w++)cv_line(x+3+w,45-meter,x+3+w,45,color);
   if(!bus){
     int fader=45-(int)(track->volume*26.f);
     cv_line(x+11,19,x+11,45,RGB(75,90,112));
     cv_line(x+10,fader,x+width-4,fader,color);
     if(engine.solo==id)cv_text(x+width-10,19,&FONT_XS,"S",C_WHITE);
   }
   x+=width;
 }
}
static void mixer_band(int band)
{
 if(mix_tab==0||mix_tab==3){mixer_overview(band);return;}
 if(mix_tab==4){
   static const char *names[]={"Nature","Sacred","Ref","Binaural","FX"};
   int offset=(band-1)*48;
   for(unsigned b=0;b<SR_BUS_COUNT;b++){
     int x=4+(int)b*47;
     uint16_t color=colors[b];
     if(band==1){
       if(b==mix_bus)cv_line(x,0,x+43,0,C_WHITE);
       limited_text(x+1,2,43,&FONT_XS,names[b],color);
     }
     float mag=fm_clampf(engine.bus_meter[b],0,1);
     int meter=(int)(fm_sqrtf(fm_sqrtf(mag))*58.f);
     cv_line(x+5,29-offset,x+5,87-offset,RGB(75,90,112));
     if(meter)for(int w=0;w<6;w++)cv_line(x+8+w,87-meter-offset,x+8+w,87-offset,color);
   }
   return;
 }
 if(band==2&&(mix_tab==1||mix_tab==2)){
   label(8,2,"MASTER PEAK %",(int)(engine.master_peak*100.f));
   label(93,2,"LIMITER %",(int)(engine.limiter*100.f));
   label(174,2,"GAIN %",(int)(engine.gain_trim[selected]*100.f));
   cv_text(8,30,&FONT_XS,"EDIT: TAB   LFO: SOLO   SEL: MUTE",C_WHITE);
   return;
 }
 unsigned ids[SR_TRACKS], count=0;
 for(unsigned t=0;t<SR_TRACKS;t++){
   const sr_track *track=&engine.tracks[t];
   if(mix_tab!=3||(!track->mute&&track->volume>0&&(t!=SR_DRUMS||engine.drums_enabled)))
     ids[count++]=t;
 }
 if(!count){
   if(band==1)cv_text(8,4,&FONT_XS,"NO ACTIVE TRACKS / EDIT FOR ALL",C_WHITE);
   return;
 }
 static const char *short_names[]={"LD","BS","PD","HM","PN","PL","CH","DR"};
 int offset=(band-1)*48, width=232/(int)count;
 for(unsigned i=0;i<count;i++){
   unsigned t=ids[i];
   int x=4+(int)i*width;
   const sr_track *track=&engine.tracks[t];
   uint16_t color=track->mute?RGB(75,90,112):colors[t];
   if(band==1){
     if(t==selected)cv_line(x,0,x+width-3,0,C_WHITE);
     limited_text(x+2,2,width-3,&FONT_XS,count>4?short_names[t]:role_names[t],color);
     if(engine.solo==t)cv_text(x+2,19,&FONT_XS,"S",C_WHITE);
   }
   float mag=fm_clampf(track->meter,0,1);
   int meter=(int)(fm_sqrtf(fm_sqrtf(mag))*58.f);
   int fader=87-(int)(track->volume*58.f);
   cv_line(x+5,29-offset,x+5,87-offset,RGB(75,90,112));
   if(fader>=offset&&fader<offset+48)
     for(int w=-1;w<=1;w++)cv_line(x+11,fader+w-offset,x+width-4,fader+w-offset,color);
   if(meter)for(int w=0;w<5;w++)cv_line(x+3+w,87-meter-offset,x+3+w,87-offset,color);
 }
}
static void draw(int animate)
{
 unsigned revision=state_revision;
 for(int band=0;band<5;band++){
   int immersion=page==0&&visual_mode&&!debug;
   int visual=fullscreen||band==1||band==2||(immersion&&band>=3);
   if(debug ? !dirty : visual ? !animate&&!dirty : !dirty)continue;
   cv_begin(240,48,RGB(5,17,22));
   if(debug)performance(band);
   else if(fullscreen)geometry(band*48);
   else if(immersion&&band==4&&knob_until){
     for(unsigned k=0;k<4;k++){
       char value[16],key[]="K1";
       const char *name;
       const char *result=knob_value(control_for(k),&name,value);
       key[1]=(char)('1'+k);
       int x=4+(int)k*59;
       uint16_t color=k==knob_focus?RGB(116,255,223):C_WHITE;
       cv_text(x,0,&FONT_XS,key,color);
       limited_text(x,15,55,&FONT_XS,name,color);
       limited_text(x,29,55,&FONT_XS,result,C_WHITE);
     }
   }else if(visual&&page==PAGE_AIR){
     if(band==1){
       cv_text(8,0,&FONT_B,"EXPERIMENTAL AIR",C_WHITE);
       cv_text(8,23,&FONT_XS,"REQUIRES BLE MIDI ADVERTISER",C_WHITE);
     }else{
       label(8,0,"SIGNAL dBm",air_rssi);
       label(120,0,"MODULATION %",engine.air_wet*100.f);
     }
   }else if(visual&&page==PAGE_DRUMS){
     if(band==1){
       static const char *tabs[]={"PATTERN","SOUND / MIX","MUTE / SOLO"};
       cv_text(8,0,&FONT_B,tabs[drum_tab],colors[SR_DRUMS]);
       limited_text(8,25,225,&FONT_XS,sr_drum_kits[engine.drum_kit].name,C_WHITE);
     }else{
       label(8,0,"DENSITY",engine.drum_density);
       label(91,0,"HIT %",engine.drum_hit);
       label(163,0,"LEVEL %",engine.drum_level*100);
       cv_text(8,33,&FONT_XS,"EDIT: TAB   PRESET: KIT",C_WHITE);
     }
   }else if(visual&&page==PAGE_SOUND&&sound_picker){
     if(band==1){
       cv_text(8,0,&FONT_B,"SOUND CATEGORIES",C_WHITE);
       cv_text(8,28,&FONT_XS,"SELECT: CHOOSE   EDIT: OPEN",C_WHITE);
     }else limited_text(8,8,225,&FONT_B,groups[group],colors[SR_LEAD]);
   }else if(visual&&page==PAGE_SOUND&&group==0&&selected<SR_DRUMS){
     static unsigned last_patch=SR_PATCHES;
     static int last_tone,last_attack,last_release;
     static int8_t shape[129],envelope[129];
     const sr_track *track=&engine.tracks[selected];
     if(track->patch!=last_patch||track->tone!=last_tone||
        track->attack!=last_attack||track->release!=last_release){
       for(unsigned j=0;j<=128;j++){
         shape[j]=(int8_t)(sr_patch_preview(track,j*(1.f/128.f))*18.f);
         envelope[j]=(int8_t)(sr_patch_envelope(track,j<=40?j*.015f:(j-41)*.04f,j>40)*30.f);
       }
       last_patch=track->patch;last_tone=track->tone;
       last_attack=track->attack;last_release=track->release;
     }
     int offset=(band-1)*48;
     if(band==1)cv_text(8,0,&FONT_XS,"OSCILLATOR",C_WHITE);
     else cv_text(8,0,&FONT_XS,"ATTACK / RELEASE",C_WHITE);
     for(unsigned j=1;j<=128;j++){
       int y0=band==1?29+shape[j-1]:88-envelope[j-1];
       int y1=band==1?29+shape[j]:88-envelope[j];
       if(band==1||j!=41)
         cv_line(7+(j-1)*225/128,y0-offset,7+j*225/128,y1-offset,colors[selected]);
     }
   }else if(visual&&page==PAGE_SETUP){
     if(band==1)cv_text(8,3,&FONT_B,setup_tab==3?"QUICK FIND":setup_tab==2?"ABOUT":setup_tab?"ARRANGEMENT BANKS":"LIGHTS / DISPLAY",C_WHITE);
     if(band==2){
       if(setup_tab==2){
         cv_text(8,1,&FONT_B,"AQUORA " APP_VERSION,C_WHITE);
         cv_text(8,26,&FONT_XS,"M-VAVE FM-1 / FM-1_73",C_WHITE);
       }else cv_text(8,10,&FONT_XS,setup_tab==3?"K1: DESTINATION   K2: OPEN":"EDIT: TAB   GLO: PERFORMANCE",C_WHITE);
     }
   }else if(visual&&page==3)mixer_band(band);
   else if(visual)geometry(band*48);
   else if(band==0){
     cv_text(8,0,&FONT_XS,"ALGO",RGB(126,166,164));
     cv_text(46,0,&FONT_B,pages[page],RGB(174,236,208));
     if(page==PAGE_DRUMS){
       char count[12];num(count,engine.tracks[SR_DRUMS].events%10000u);
       int x=cv_text(104,2,&FONT_XS,"PULSE ",C_WHITE);
       cv_text(x,2,&FONT_XS,count,RGB(147,221,207));
       char meter[12];num(meter,engine.tracks[SR_DRUMS].meter*100);
       cv_text(187,2,&FONT_XS,meter,C_WHITE);cv_text(212,2,&FONT_XS,"%",C_WHITE);
     }else if(page!=0)limited_text(104,2,128,&FONT_XS,page==PAGE_SOUND?groups[group]:page==PAGE_SETUP?(setup_tab==3?"QUICK FIND":setup_tab==2?"ABOUT":setup_tab?"ARRANGEMENT BANKS":"LIGHTS / DISPLAY"):page==2?(song_tab==2?"GROOVE / EDIT":song_tab?"NEW SONG / EDIT":"LOOP / EDIT"):page==3?(mix_tab==0?"ALL CHANNELS":mix_tab==1?"MASTER FX":mix_tab==2?"RESET MIX":mix_tab==3?"ACTIVE CHANNELS":"SOURCE BUSES"):visual_name(),C_WHITE);
     char track[12];num(track,selected+1);
     if(page!=0){
     cv_text(8,25,&FONT_XS,"SEL",RGB(126,166,164));
     cv_text(30,25,&FONT_XS,track,C_WHITE);
     limited_text(44,25,55,&FONT_XS,role_names[selected],C_WHITE);
     }
     cv_text(101,25,&FONT_XS,"PRE",RGB(126,166,164));
     limited_text(127,25,105,&FONT_XS,page==0?sr_scenes[engine.scene].name:patch_name(),C_WHITE);
     if(page==0){
       cv_text(8,25,&FONT_XS,engine.keyboard_mode?"EDIT: Key Change":"EDIT: Play Along",RGB(119,255,243));
       const char *mode=engine.frequency_lock?"LOCK":engine.tone_level>0?"REF ON":"REF OFF";
       int x=cv_text(107,2,&FONT_XS,mode,RGB(119,255,243));
       cv_text(x+4,2,&FONT_XS,sr_frequencies[engine.frequency].name,C_WHITE);
     }
   }else{
     unsigned start=band==3?0:2;
     knob_cell(start,8);
     knob_cell(start+1,126);
     if(band==4){
       if(page==PAGE_DRUMS){
         int x=cv_text(8,33,&FONT_XS,drum_status(),RGB(147,221,207));
         char hit[12];num(hit,engine.drum_hit);
         x=cv_text(x+8,33,&FONT_XS,"HIT %",C_WHITE);
         cv_text(x+5,33,&FONT_XS,hit,C_WHITE);
       }else if(page==1&&group==1){
         const char *hint=engine.nature_audition?"AUDITION / HOME TO EXIT":
          engine.frequency_lock?"NATURE OFF IN LOCK":
          engine.solo<SR_TRACKS?"NATURE MUTED BY SOLO":"NATURE PEAK %";
         int x=cv_text(8,33,&FONT_XS,hint,C_WHITE);
         char value[12];num(value,(int)(engine.nature_peak*100.f));
         cv_text(x+6,33,&FONT_XS,value,RGB(119,255,243));
       }else if(page==1&&group==6){
         cv_text(8,33,&FONT_XS,selected>SR_BASS?"TURN SELECT TO LEAD OR BASS":
            "LEGATO: OVERLAP NOTES  /  ALWAYS: SLIDE",C_WHITE);
       }else if(page==2||(page==1&&group==GROUP_KEYS)){
         static const char *notes[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
         unsigned length=engine.loop_length,pos=engine.loop_pos;
         char step[12];
         num(step,engine.running?(pos?pos:length):0);
         int x=cv_text(8,33,&FONT_XS,"STEP ",C_WHITE);
         x=cv_text(x,33,&FONT_XS,step,C_WHITE);
         x=cv_text(x,33,&FONT_XS," / ",C_WHITE);
         num(step,length);cv_text(x,33,&FONT_XS,step,C_WHITE);
         x=cv_text(126,33,&FONT_XS,"KEY ",C_WHITE);
         x=cv_text(x,33,&FONT_XS,notes[sr_tonic(&engine)%12],RGB(97,241,244));
         unsigned next=engine.pending_key;
         if(next){x=cv_text(x+3,33,&FONT_XS," > ",C_WHITE);cv_text(x,33,&FONT_XS,notes[(next-1)%12],RGB(250,171,64));}
       }else if(page==PAGE_SETUP&&setup_tab==1){
         static const char *messages[]={"SAVE RUNS ACTION","STOP FIRST","EMPTY BANK",
           "K3 ARM OVERWRITE","ARRANGEMENT SAVED","ARRANGEMENT LOADED","BANK READ / WRITE FAILED"};
         cv_text(8,33,&FONT_XS,bank_status[bank_slot]==1?"SAVED":bank_status[bank_slot]==2?"INVALID":"EMPTY",RGB(147,221,207));
         limited_text(62,33,170,&FONT_XS,messages[bank_message],C_WHITE);
       }else{
         const char *status=engine.running?"PLAYING":"STOPPED";
         if(page==1&&group==6)status=saved_status==1?"MOTION SAVED":saved_status==2?"SAVE FAILED":"STOP THEN SAVE MOTION";
         else if(engine.tracks[selected].mute)status="TRACK MUTED";
         else if(engine.solo<SR_TRACKS)status="SOLO ACTIVE";
         else if(knob_until){
           char buffer[16],tag[]="K1 ";
           const char *name;
           const char *value=knob_value(control_for(knob_focus),&name,buffer);
           tag[1]=(char)('1'+knob_focus);
           int x=cv_text(8,33,&FONT_XS,tag,RGB(147,221,207));
           x=cv_text(x,33,&FONT_XS,name,C_WHITE);
           limited_text(x+7,33,225-x,&FONT_B,value,C_WHITE);
           status=0;
         }
         if(engine.frequency_lock&&status)status=engine.lock_textures?"UNISON / TEXTURES ON":"UNISON / TEXTURES OFF";
         if(engine.isolate&&status)status="SOURCE ISOLATION ON";
         if(status)cv_text(8,33,&FONT_XS,status,RGB(147,221,207));
       }
     }
   }
   cv_blit(0,band*48);
 }
 dirty=0;
 displayed_revision=revision;
}
