/* SPDX-License-Identifier: MPL-2.0 */
static const felucca_font_t *ui_caption_font(void){return ui_font_size==0?&FONT_T:ui_font_size==2?&FONT_S:&FONT_XS;}
static const felucca_font_t *ui_value_font(void){return ui_font_size==0?&FONT_XS:ui_font_size==2?&FONT_M:&FONT_B;}
#define FONT_XS (*ui_caption_font())
#define FONT_B (*ui_value_font())
static const char *pages[]={"PLAY","SOUND","EVOLVE","MIX","DRUMS","SETUP","AIR"};
static const char *beds[]={"Off","Wind","Water","Rain","Forest","Ocean","Birds","Leaves","Florida Night","Twilight Meadow","Crystal Creek","Mountain Rain"};
static const char *groups[]={"Instrument","Nature","Frequency","Binaural","Percussion","Sacred Bass","Lead / Bass Motion","MIX SHORTCUT","Percussion HIT","Visualizer","Arrangement Banks","Source Isolate","Keyboard","Lock Textures","Nature Character"};
/* Static panel palettes. Canvas is independently exact black or white. */
static const struct {uint16_t canvas, panel, border, accent, text, dim;} skins[5][2]={
 {{RGB(5,17,22),RGB(10,32,38),RGB(37,86,90),RGB(119,255,225),RGB(238,248,243),RGB(153,196,194)},
  {RGB(246,250,251),RGB(229,239,241),RGB(108,154,161),RGB(10,106,99),RGB(20,38,47),RGB(70,91,97)}},
 {{RGB(7,22,29),RGB(18,47,55),RGB(67,142,148),RGB(90,245,209),RGB(236,251,252),RGB(158,199,205)},
  {RGB(246,252,250),RGB(226,247,242),RGB(66,146,136),RGB(12,121,121),RGB(14,53,56),RGB(66,99,102)}},
 {{RGB(12,15,38),RGB(26,31,65),RGB(62,95,189),RGB(58,247,247),RGB(243,247,255),RGB(162,178,220)},
  {RGB(250,251,255),RGB(230,238,252),RGB(98,126,203),RGB(10,91,177),RGB(24,35,73),RGB(88,98,137)}},
 {{RGB(32,20,17),RGB(66,39,29),RGB(157,88,48),RGB(255,186,85),RGB(255,244,219),RGB(225,182,145)},
  {RGB(255,249,240),RGB(252,231,212),RGB(201,130,91),RGB(169,70,40),RGB(69,38,29),RGB(116,73,53)}},
 {{RGB(10,10,32),RGB(21,23,62),RGB(73,84,188),RGB(140,158,255),RGB(237,238,255),RGB(162,172,216)},
  {RGB(247,247,255),RGB(229,230,251),RGB(110,105,194),RGB(73,60,171),RGB(32,29,74),RGB(92,87,138)}}
};
static uint16_t skin_text(void){return skins[ui_theme][ui_background].text;}
static uint16_t skin_dim(void){return skins[ui_theme][ui_background].dim;}
static uint16_t skin_accent(void){return skins[ui_theme][ui_background].accent;}
static void num(char *s,int v){char t[12];int n=0;unsigned u=v<0?-v:v;if(v<0)*s++='-';do{t[n++]=(char)('0'+u%10);u/=10;}while(u);while(n)*s++=t[--n];*s=0;}
static void limited_text(int x,int y,int width,const felucca_font_t *font,const char *s,uint16_t color);
static void label(int x,int y,const char *name,int value){
 char v[12];num(v,value);if(ui_font_size==2&&y>6)y=6;
 int width=x==86?70:x==87?88:x>=125?232-x:110;
 limited_text(x,y,width,&FONT_XS,name,skin_dim());
 limited_text(x,y+(ui_font_size==2?18:15),width,&FONT_B,v,skin_text());
}
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
   if(vis_input.water&&t>0)
     for(unsigned j=t&1u;j<32;j+=4){
       int y0=vis_output.xy[t-1][j][1]-offset,y1=vis_output.xy[t][j][1]-offset;
       if((y0<0&&y1<0)||(y0>=48&&y1>=48))continue;
       cv_line(vis_output.xy[t-1][j][0],y0,vis_output.xy[t][j][0],y1,RGB(29,74,94));
     }
 }
}
static const char *onoff(int enabled){return enabled?"ON":"OFF";}
static const char *visual_name(void){return visual_mode?"Polar Bloom":"Original";}
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
 case 82:*name="EVOLUTION";return engine.variation==0?"Stable":engine.variation<.4f?"Subtle":engine.variation<.7f?"Flowing":"Adventurous";
 case 83:*name="PACE";{static const char *pace[]={"Intention","2 loops","4 loops","8 loops"};return pace[engine.evolve_pace];}
 case 84:*name="NEXT PHRASE";return engine.evolve_next?"Queued":"Turn right";
 case 19:*name="VARIATION %";number=engine.variation*100;break;
 case 20:*name="FREEZE";return onoff(engine.freeze);
 case 21:*name="NATURE BED";return beds[engine.bed_type];
 case 22:*name="FIXED TONE %";return tenth(buffer,(int)(engine.tone_level*1000.f+.5f));
 case 23:*name="BINAURAL %";number=engine.binaural_level*100;break;
 case 24:*name="PAN %";number=track->pan*100;break;
 case 25:*name="TEMPO BPM";return tenth(buffer,(SR_RATE*150u+engine.tick_length/2u)/engine.tick_length);
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
 case 58:*name="NEW SONG";return engine.running?"Stop first":song_arm==id?"SAVE to confirm":"Turn to arm";
 case 59:*name="REROLL SOUNDS";return engine.running?"Stop first":song_arm==id?"SAVE to confirm":"Turn to arm";
 case 60:*name="TRACK LOCK";return onoff(engine.sound_locks&(1u<<selected));
 case 64:{static const char *m[]={"Intention","Straight","Swing"};*name="GROOVE";return m[engine.groove_mode];}
 case 65:*name="STRENGTH %";number=engine.groove_strength;break;
 case 66:*name="AIR RADIO";
   if(!FM1B_BLE_CENTRAL)return "Unavailable";
   if(!air_on)return "Off";
   if(aq_air_live(&air_sensor,plat_ms()))return "Receiving";
   return air_sensor.calibrated?"Signal lost":"Listening / no broadcaster";
 case 67:*name="AIR TARGET";return engine.air_track<SR_TRACKS?role_names[engine.air_track]:"All tracks";
 case 68:*name="CALIBRATE";return !air_sensor.calibrated?"Turn right":
   aq_air_live(&air_sensor,plat_ms())?"Calibrated / live":"Calibrated / waiting";
 case 71:
   *name="QUICK FIND";
   if(quick_item<QUICK_CATEGORIES)return quick_names[quick_item];
   {
     const char *parameter;
     knob_value(parameter_routes[quick_item-QUICK_CATEGORIES].id,&parameter,buffer);
     return parameter;
   }
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
 case 90:{static const char *themes[]={"AQUORA Legacy","Aurora","Neon Lagoon","Solar Bloom","Midnight Prism"};*name="PANEL SKIN";return themes[ui_theme];}
 case 93:{static const char *effects[]={"Reverb Bloom","Echo Throw","Bloom + Echo"};*name="AIR FX";return effects[engine.air_fx];}
 case 94:*name="AIR DEPTH %";number=engine.air_depth;break;
 case 95:*name="MANUAL AIR %";number=engine.air_target*100;break;
 case 91:{static const char *fonts[]={"Small","Medium","Large"};*name="FONT SIZE";return fonts[ui_font_size];}
 case 92:*name="BACKGROUND";return ui_background?"White":"Black (Stock)";
 case 57:*name="VISUAL FPS";return fps_auto?"Auto / 20":"20 target";
 case 78:*name="NATURE MODE";return !engine.bed_type?"Off":(engine.sound_locks&SR_SOUND_NATURE)?"Manual":"Auto";
 case 80:*name="LED TEST";return onoff(led_test);
 case 81:if(led_step==NKEYS+NB){*name="LED SLOT";return "PLAY green";}*name=led_step<NKEYS?"KEY SLOT":"BUTTON SLOT";number=led_step<NKEYS?led_step+1:led_step-NKEYS+1;break;
 case 85:*name="BIRD ACTIVITY";number=engine.nature_activity;break;
 case 86:*name="BIRD DISTANCE";number=engine.nature_distance;break;
 case 79:*name="NATURE STYLE";{static const char *styles[]={"Ambient","Organic","Rhythmic","Ethereal"};return styles[engine.nature_style];}
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
 const felucca_font_t *caption=ui_font_size==2?&FONT_S:&FONT_XS;
 int label_x=cv_text(x,0,caption,tag,skin_accent());
 limited_text(label_x,0,108-(label_x-x),caption,name,skin_dim());
 int focus=knob_until&&knob_focus==knob;
 const felucca_font_t *font=ui_font_size==2?&FONT_M:ui_font_size==0?&FONT_S:&FONT_B;
 if(ui_font_size==2){
    for(const char *q=shown;*q;q++)if(*q>='a'&&*q<='z'){font=&FONT_B;break;}
 }
 if(text_pixels(font,shown)>108)font=&FONT_B;
 if(text_pixels(font,shown)>108)font=&FONT_XS;
 limited_text(x,ui_font_size==2?18:14,108,font,shown,focus?skin_accent():skin_text());
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
   cv_text(8,1,&FONT_B,"VOICE INSPECTOR",skin_text());
   cv_text(8,25,&FONT_XS,"K1 SELECT   SEQ ALL NOTES OFF",skin_text());
 }else if(band==1){
   label(8,0,"VOICE SLOT",voice_slot+1);
   cv_text(126,0,&FONT_XS,"ROLE / ORIGIN",skin_dim());
   cv_text(126,15,&FONT_B,voice.active?role_names[voice.track]:"Idle",skin_text());
   const char *origin=voice.key?"Physical key":"Generator";
   if(voice.active&&engine.tracks[voice.track].mute)origin=voice.key?"Key / muted":"Gen / muted";
   if(ui_font_size!=2)cv_text(126,31,&FONT_XS,voice.active?origin:"-",skin_text());
 }else if(band==2){
   label(8,0,"MIDI / AGE ms",voice.active?voice.midi:0);
   label(126,0,"REMAIN ms",voice.active?voice.remain/(SR_RATE/1000):0);
   char value[16];
   num(value,voice.active?age/(SR_RATE/1000):0);if(ui_font_size!=2)cv_text(50,15,&FONT_B,value,skin_text());
   if(ui_font_size!=2)cv_text(126,31,&FONT_XS,voice.active?(voice.gate?"Gate on":"Release"):"Idle",skin_text());
 }else if(band==3){
   label(8,0,"NOTE ON / OFF",notes_on);
   char value[16];num(value,notes_off);if(ui_font_size!=2)cv_text(62,15,&FONT_B,value,skin_text());
   label(126,0,"GUARDS",watchdogs);
   num(value,voice.active?voice.env*100:0);if(ui_font_size!=2){cv_text(8,31,&FONT_XS,"ENVELOPE %",skin_text());
   cv_text(86,31,&FONT_XS,value,skin_text());}
 }else{
   label(8,0,"SACRED ACTIVE Hz",engine.sacred_gain>.0001f?engine.sacred_hz_now:0);
   label(126,0,"FIXED ACTIVE Hz",engine.running&&engine.solo==SR_TRACKS&&engine.tone_level>0?sr_frequencies[engine.frequency].hz:0);
   if(ui_font_size!=2)cv_text(8,32,&FONT_XS,engine.binaural_enabled?"BINAURAL ON":"BINAURAL OFF",skin_text());
   if(ui_font_size!=2)cv_text(126,32,&FONT_XS,engine.solo<SR_TRACKS?role_names[engine.solo]:"SOLO OFF",skin_text());
 }
}
static void performance(int band)
{
 if(debug==3){voice_diagnostics(band);return;}
 if(debug==2){
   if(band==0){
     cv_text(8,1,&FONT_B,"CALLBACK / CORE",skin_text());
     cv_text(8,25,&FONT_XS,"256 FRAMES / 5805 us / GLO VOICES",skin_text());
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
   cv_text(8,1,&FONT_B,"PERFORMANCE " APP_VERSION,skin_text());
   cv_text(8,25,&FONT_XS,fps_auto?"K1 VISUAL FPS: AUTO (20 TARGET)":"K1 VISUAL FPS: 20 TARGET",skin_accent());
 }else if(band==1){
   label(8,0,"PLAY BLITS FPS x10",shown_fps10);
   label(127,0,"MISSED / 2s",shown_missed);
   if(ui_font_size!=2)cv_text(8,31,&FONT_XS,"DRAW us",skin_text());
   char draw[12];num(draw,frame_us);if(ui_font_size!=2)cv_text(65,31,&FONT_XS,draw,skin_text());

 }else if(band==2){
   label(8,0,"AUDIO MEAN us",audio_stats.mean);
   label(127,0,"FX MEAN us",fx_stats.mean);
   if(ui_font_size!=2)cv_text(8,32,&FONT_XS,"GLO: CALLBACK AND VOICE STATS",skin_text());
 }else if(band==3){
   label(8,0,"DMA XRUN DELTA",xrun_delta);
   label(127,0,"AUDIO+FX % BUDGET",(audio_stats.mean+fx_stats.mean)*100/5805);
   char v[12];num(v,plat_xruns());if(ui_font_size!=2)cv_text(66,15,&FONT_B,v,skin_text());
 }else if(band==4){
   if(ui_font_size==2){label(8,0,"WORK us",vis_output.busy_us);label(127,0,"FALLBACK",worker_fallbacks);}
   else{
   cv_text(8,0,&FONT_XS,plat_cpu2_on()?"CORE 1 GEOMETRY / CORE 0 AUDIO":"CORE 0 / SINGLE-CORE FALLBACK",skin_text());
   label(8,12,"WORK / WAIT us",vis_output.busy_us);
   char v[12];num(v,worker_wait_us);cv_text(66,27,&FONT_B,v,skin_text());
   label(127,12,"FALLBACK / BAD",worker_fallbacks);
   num(v,plat_cpu2_bad());cv_text(185,27,&FONT_B,v,skin_text());
   }
 }
}
static void mixer_overview(int band)
{
 static const char *names[]={"LD","BS","PD","HM","PN","PL","CH","DR","Nature","Sacred","Ref","Binaural","FX"};
 unsigned first=band==1?0:SR_TRACKS, end=band==1?SR_TRACKS:SR_TRACKS+SR_BUS_COUNT;
 unsigned count=0;
 for(unsigned channel=first;channel<end;channel++)count+=mix_visible(channel);
 if(!count){
   cv_text(8,12,&FONT_XS,band==1?"NO ACTIVE TRACKS":"NO ACTIVE BUSES",skin_text());
   return;
 }
 int width=232/(int)count, x=4;
 for(unsigned channel=first;channel<end;channel++){
   if(!mix_visible(channel))continue;
   unsigned bus=channel>=SR_TRACKS;
   unsigned id=bus?channel-SR_TRACKS:channel;
   const sr_track *track=&engine.tracks[id];
   uint16_t color=!bus&&track->mute?RGB(75,90,112):colors[id];
   if(channel==mix_focus)cv_line(x,0,x+width-3,0,skin_text());
   limited_text(x+2,2,width-3,&FONT_XS,names[channel],color);
   float magnitude=bus?engine.bus_meter[id]:track->meter;
   int meter=(int)(fm_sqrtf(fm_sqrtf(fm_clampf(magnitude,0,1)))*26.f);
   for(int w=0;w<5;w++)cv_line(x+3+w,45-meter,x+3+w,45,color);
   if(!bus){
     int fader=45-(int)(track->volume*26.f);
     cv_line(x+11,19,x+11,45,RGB(75,90,112));
     cv_line(x+10,fader,x+width-4,fader,color);
     if(engine.solo==id)cv_text(x+width-10,19,&FONT_XS,"S",skin_text());
   }
   x+=width;
 }
}
/* 0.6.2 professional instrument console: all eight role levels and meter
 * states are drawn only on the actually visible MIX page. No extra DSP or
 * FFT: data comes from the engine's existing post-mix track meters. */
static void mixer_console(int band)
{
 static const char *abbr[]={"LD","BS","PD","HM","PN","PL","CH","DR"};
 const int offset=band*48;
 for(unsigned t=0;t<SR_TRACKS;t++){
   const sr_track *tr=&engine.tracks[t];
   int x=5+(int)t*29;
   int chosen=mix_focus==t;
   uint16_t c=tr->mute?skin_dim():colors[t];
   if(ui_background&&tr->mute)c=RGB(97,100,116);
   if(band==1){
     limited_text(x,1,26,&FONT_XS,abbr[t],chosen?skin_accent():skin_text());
     if(chosen)cv_line(x,18,x+22,18,skin_accent());
     if(tr->mute)cv_text(x,29,&FONT_XS,"M",skin_dim());
     if(engine.solo==t)cv_text(x+13,29,&FONT_XS,"S",skin_accent());
   }
   const int top=99, bottom=182;
   int y0=top-offset, y1=bottom-offset;
   if(y0<0)y0=0;
   if(y1>47)y1=47;
   if(y1>=y0){
     cv_rect(x+3,y0,18,y1-y0+1,skins[ui_theme][ui_background].panel);
     cv_line(x+3,y0,x+3,y1,skins[ui_theme][ui_background].border);
     cv_line(x+21,y0,x+21,y1,skins[ui_theme][ui_background].border);
     if(top>=offset&&top<offset+48)cv_line(x+3,top-offset,x+21,top-offset,skin_dim());
     if(bottom>=offset&&bottom<offset+48)cv_line(x+3,bottom-offset,x+21,bottom-offset,skin_dim());
     float mag=fm_clampf(tr->meter,0,1);
     int meter=(int)(fm_sqrtf(fm_sqrtf(mag))*78.f);
     int hi=bottom-meter;
     if(hi<offset)hi=offset;
     int lo=bottom;if(lo>offset+47)lo=offset+47;
     if(meter&&lo>=hi)cv_rect(x+5,hi-offset,7,lo-hi+1,c);
     int fy=bottom-(int)(tr->volume*77.f);
     if(fy>=offset&&fy<offset+48)cv_line(x+12,fy-offset,x+21,fy-offset,chosen?skin_accent():skin_text());
   }
 }
 if(band==4){
   unsigned t=mix_focus<SR_TRACKS?mix_focus:selected;
   const sr_track *tr=&engine.tracks[t];
   char v[12];
   limited_text(7,1,99,&FONT_B,role_names[t],skin_accent());
   cv_text(114,1,&FONT_XS,tr->mute?"MUTED":engine.solo==t?"SOLO":"ACTIVE",skin_text());
   num(v,(int)(tr->meter*100.f));cv_text(193,1,&FONT_XS,v,skin_text());
   limited_text(8,26,224,&FONT_XS,"FX: STRIP / SELECT: CH / LFO: SOLO",skin_dim());
 }
}
static void mixer_focus_strip(int band)
{
 unsigned t=selected;
 const sr_track *tr=&engine.tracks[t];
 static const char *big[]={"LEAD","BASS","PADS","HARMONY","PIANO","PLUCKS","CHOIR","DRUMS"};
 if(band==1){
   limited_text(9,1,200,ui_font_size==2?&FONT_M:&FONT_B,big[t],skin_accent());
   cv_text(8,31,&FONT_XS,tr->mute?"MUTE":engine.solo==t?"SOLO":"CHANNEL STRIP",skin_text());
   cv_text(143,31,&FONT_XS,"FX: OVERVIEW",skin_dim());
 }else if(band==2){
   float mag=fm_clampf(tr->meter,0,1);
   int fader=(int)(tr->volume*185.f);
   int peak=(int)(fm_sqrtf(fm_sqrtf(mag))*185.f);
   cv_rect(10,11,198,12,skins[ui_theme][ui_background].border);
   if(fader)cv_rect(11,12,fader,10,skin_accent());
   cv_line(11+peak,10,11+peak,26,skin_text());
   char level_text[12],peak_text[12];
   num(level_text,(int)(tr->volume*100.f));num(peak_text,(int)(mag*100.f));
   cv_text(9,29,&FONT_XS,"LEVEL %",skin_dim());
   cv_text(75,27,&FONT_B,level_text,skin_text());
   cv_text(136,29,&FONT_XS,"PEAK %",skin_dim());
   cv_text(197,27,&FONT_B,peak_text,skin_text());
 }else if(band==3){
   cv_text(8,0,&FONT_XS,"PAN LEFT           CENTER           RIGHT",skin_dim());
   cv_line(11,22,230,22,skins[ui_theme][ui_background].border);
   cv_line(120,18,120,27,skin_dim());
   int x=120+(int)(tr->pan*108.f);
   cv_rect(x-2,16,5,13,skin_accent());
   cv_text(8,34,&FONT_XS,"FX SEND",skin_dim());
   cv_line(93,39,228,39,skins[ui_theme][ui_background].border);
   cv_line(93,39,93+(int)(tr->send*135.f),39,skin_accent());
 }else if(band==4){
   char a[14],b[14],c[14];num(a,(int)(tr->volume*100.f));
   num(b,(int)(tr->pan*100.f));num(c,(int)(tr->send*100.f));
   static const char *names[]={"K1 LVL","K2 PAN","K3 SEND","K4 MUTE"};
   const char *values[]={a,b,c,tr->mute?"ON":"OFF"};
   for(unsigned k=0;k<4;k++){
     int x=7+k*58;
     limited_text(x,1,55,&FONT_XS,names[k],skin_dim());
     limited_text(x,20,55,&FONT_B,values[k],skin_text());
   }
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
       if(b==mix_bus)cv_line(x,0,x+43,0,skin_text());
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
   if(ui_font_size!=2)cv_text(8,30,&FONT_XS,"EDIT: TAB   LFO: SOLO   SEL: MUTE",skin_text());
   return;
 }
 unsigned ids[SR_TRACKS], count=0;
 for(unsigned t=0;t<SR_TRACKS;t++){
   const sr_track *track=&engine.tracks[t];
   if(mix_tab!=3||(!track->mute&&track->volume>0&&(t!=SR_DRUMS||engine.drums_enabled)))
     ids[count++]=t;
 }
 if(!count){
   if(band==1)cv_text(8,4,&FONT_XS,"NO ACTIVE TRACKS / EDIT FOR ALL",skin_text());
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
     if(t==selected)cv_line(x,0,x+width-3,0,skin_text());
     limited_text(x+2,2,width-3,&FONT_XS,count>4?short_names[t]:role_names[t],color);
     if(engine.solo==t)cv_text(x+2,19,&FONT_XS,"S",skin_text());
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
static void battery_glyph(void)
{
 if(battery_level<0||battery_level>4||!battery_fade)return;
 unsigned r=battery_level==0?246u:battery_level==4?106u:144u;
 unsigned g=battery_level==0?98u:battery_level==4?211u:218u;
 unsigned b=battery_level==0?89u:190u;
 unsigned fade=battery_fade;
 uint16_t ink=RGB(r*fade/4u,g*fade/4u,b*fade/4u);
 cv_line(218,5,233,5,ink);
 cv_line(218,15,233,15,ink);
 cv_line(218,5,218,15,ink);
 cv_line(233,5,233,15,ink);
 cv_line(234,9,235,9,ink);
 cv_line(234,11,235,11,ink);
 if(battery_level==4){
   cv_line(227,7,223,11,ink);
   cv_line(223,11,228,11,ink);
   cv_line(228,11,225,14,ink);
 }else for(int bar=0;bar<battery_level;bar++){
   int x=220+4*bar;
   for(int y=8;y<=12;y++)cv_line(x,y,x+2,y,ink);
 }
}
static void draw(int animate)
{
 unsigned revision=state_revision;
 for(int band=0;band<5;band++){
   /* Only explicit fullscreen is immersive: both PLAY visualizers always keep
    * the lower physical K1-K4 control panels visible in normal mode. */
   int visual=intro_active||fullscreen||band==1||band==2||(page==PAGE_MIX&&band>=3&&!debug);
   if(!(battery_dirty&&(band==0||(band==2&&page==PAGE_SETUP&&setup_tab==2)))&&(debug ? !dirty : visual ? !animate&&!dirty : !dirty))continue;
   const int artwork=intro_active||fullscreen;
   const uint16_t bg=ui_background?RGB(255,255,255):RGB(0,0,0);
   cv_begin(240,48,bg);
   /* One surface per logical region; no boxes around 48px transfer bands.
    * The PLAY preview spans two bands with no nested frame. */
   if(!artwork&&!debug){
     const uint16_t panel=skins[ui_theme][ui_background].panel;
     const uint16_t edge=skins[ui_theme][ui_background].border;
     if(band==0){cv_line(8,47,231,47,edge);}
     else if(band>=3&&page!=PAGE_MIX){
       cv_rect(6,2,110,42,panel);cv_rect(124,2,110,42,panel);
       if(ui_theme==2||ui_theme==4){
         cv_line(6,2,115,2,edge);cv_line(124,2,233,2,edge);
       }else if(ui_theme==3){
         cv_line(6,43,115,43,edge);cv_line(124,43,233,43,edge);
       }
     }
   }
   if(intro_active){
     /* New AQUORA water/crystal ribbon: an independent original startup
      * choreography transitioning over the stored visualizer. Fixed 32-line
      * budget, theme-aware, with no hidden work once introduction exits. */
     unsigned phase=((plat_ms()-intro_start)/42u)&127u;
     unsigned frames=(plat_ms()-intro_start)/90u;
     int old_x=120,old_y=120;
     for(unsigned j=0;j<=32;j++){
       unsigned angle=(j*5u+phase)&127u;
       int radius=(int)(3u+j*3u)+(int)(frames<25u?frames/4u:6u);
       int x=120+(int)(radius*vis_sine[(angle+32u)&127u]);
       int y=120+(int)(radius*vis_sine[angle]);
       if(j){
         int y0=old_y-band*48,y1=y-band*48;
         if(!((y0<0&&y1<0)||(y0>=48&&y1>=48)))
           cv_line(old_x,y0,x,y1,skin_accent());
       }
       old_x=x;old_y=y;
     }
     if(band==2){
       cv_text(68,1,&FONT_B,"A Q U O R A",skin_text());
       cv_text(96,28,&FONT_XS,APP_VERSION,skin_text());
     }
     if(band==4)cv_text(44,23,&FONT_XS,"ANY CONTROL TO SKIP",skin_text());
   }else if(debug)performance(band);
   else if(fullscreen)geometry(band*48);
   else if(visual&&page==PAGE_MIX&&(mix_tab==0||mix_tab==3)){
     mixer_console(band);
   }else if(visual&&page==PAGE_MIX&&mix_tab==5){
     mixer_focus_strip(band);
   }else if(visual&&page==PAGE_AIR){
     unsigned now=plat_ms();
     unsigned live=air_on&&aq_air_live(&air_sensor,now);
     if(band==1){
       limited_text(8,0,226,&FONT_B,air_tab?"AIR / FX MAPPING":"AIR / LIVE SIGNAL",skin_text());
       const char *state=!air_on?"RADIO OFF":live?(air_sensor.calibrated?"SIGNAL LIVE / CALIBRATED":"RECEIVING / CALIBRATE FAR"):
                    air_sensor.calibrated?"CALIBRATED / SIGNAL LOST":"NO BROADCASTER";
       limited_text(8,24,226,&FONT_XS,state,live?skin_accent():skin_dim());
     }else if(band==2){
       unsigned value=(unsigned)(engine.air_wet*100.f+.5f);
       if(value>100)value=100;
       char line[72],numtext[12];
       unsigned pos=0;
       const char *tag="RAW ";while(*tag)line[pos++]=*tag++;
       num(numtext,air_sensor.raw);for(const char*q=numtext;*q;q++)line[pos++]=*q;
       tag="  SMOOTH ";while(*tag)line[pos++]=*tag++;
       num(numtext,(int)(air_sensor.filtered+.5f));for(const char*q=numtext;*q;q++)line[pos++]=*q;
       tag="  PKT/s ";while(*tag)line[pos++]=*tag++;
       num(numtext,air_packet_rate);for(const char*q=numtext;*q;q++)line[pos++]=*q;
       line[pos]=0;limited_text(8,0,224,&FONT_XS,line,skin_dim());
       cv_text(8,20,&FONT_XS,"OUT %",skin_dim());num(numtext,value);
       cv_text(52,20,&FONT_XS,numtext,skin_accent());
       static const char *effects[]={"REVERB","ECHO","BLOOM + ECHO"};
       limited_text(86,20,146,&FONT_XS,effects[engine.air_fx],skin_text());
       cv_line(7,43,230,43,skin_dim());
       if(value)cv_rect(8,39,(int)(value*222u/100u),4,skin_accent());
     }else{
       label(8,0,"RAW RSSI",air_sensor.raw);
       label(87,0,"SMOOTH RSSI",(int)(air_sensor.filtered+.5f));
       label(183,0,"LAST PKTS",air_packets);
       limited_text(8,33,226,&FONT_XS,air_tab?"K3: MANUAL AIR / EDIT: SENSOR":"EDIT: FX + DEPTH / K3: FAR",skin_text());
     }
   }else if(visual&&page==PAGE_DRUMS){
     if(band==1){
       static const char *tabs[]={"PATTERN","SOUND / MIX","MUTE / SOLO"};
       cv_text(8,0,&FONT_B,tabs[drum_tab],colors[SR_DRUMS]);
       limited_text(8,25,225,&FONT_XS,sr_drum_kits[engine.drum_kit].name,skin_text());
     }else{
       label(8,0,"DENSITY",engine.drum_density);
       label(91,0,"HIT %",engine.drum_hit);
       label(163,0,"LEVEL %",engine.drum_level*100);
       if(ui_font_size!=2)cv_text(8,33,&FONT_XS,"EDIT: TAB   PRESET: KIT",skin_text());
     }
   }else if(visual&&page==PAGE_SOUND&&sound_picker){
     if(band==1){
       cv_text(8,0,&FONT_B,"SOUND / EDIT SECTION",skin_text());
       cv_text(8,28,&FONT_XS,"SELECT: ROLE   EDIT: SECTION",skin_text());
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
     if(band==1)cv_text(8,0,&FONT_XS,(engine.running||engine.preview_frames)?"LIVE ROLE / MODEL":"PATCH MODEL",skin_text());
     else cv_text(8,0,&FONT_XS,"ATTACK / RELEASE",skin_text());
     int16_t captured[128];
     if(band==1&&(engine.running||engine.preview_frames)){
       unsigned flags=plat_lock();
       unsigned start=engine.scope_write;
       for(unsigned j=0;j<128;j++)captured[j]=engine.scope[(start+j)&127u];
       plat_unlock(flags);
     }
     for(unsigned j=1;j<=128;j++){
       int y0=band==1?29+shape[j-1]:88-envelope[j-1];
       int y1=band==1?29+shape[j]:88-envelope[j];
       if(band==1||j!=41)
         cv_line(7+(j-1)*225/128,y0-offset,7+j*225/128,y1-offset,
                 band==1&&(engine.running||engine.preview_frames)?RGB(50,86,91):colors[selected]);
       if(band==1&&(engine.running||engine.preview_frames)&&j<128){
         int live0=(int)fm_clampf(captured[j-1]*(1.f/320.f),-17,17);
         int live1=(int)fm_clampf(captured[j]*(1.f/320.f),-17,17);
         cv_line(7+(j-1)*225/128,29-live0,7+j*225/128,29-live1,colors[selected]);
       }
     }
   }else if(visual&&page==PAGE_SOUND){
     if(band==1){
       if(group==1||group==14){
         limited_text(8,0,220,&FONT_B,beds[engine.bed_type],skin_text());
         cv_text(8,24,&FONT_XS,!engine.bed_type?"OFF":engine.sound_locks&SR_SOUND_NATURE?"MANUAL":"AUTO / INTENTION + STYLE",skin_text());
       }else{
         limited_text(8,0,220,&FONT_B,groups[group],skin_text());
         cv_text(8,24,&FONT_XS,"LEVELS FROM AUDIO ENGINE",skin_text());
       }
     }else if(group==6&&selected<=SR_BASS){
       unsigned flags=plat_lock();
       float pitch=0,target=0;
       for(unsigned i=0;i<SR_VOICES;i++){
         const sr_voice *voice=&engine.voices[i];
         if(voice->active&&voice->gate&&voice->track==selected){
           pitch=voice->increment*SR_RATE;
           target=voice->freq;
           break;
         }
       }
       plat_unlock(flags);
       label(8,1,"OUTPUT Hz",(int)(pitch+.5f));
       label(125,1,"TARGET Hz",(int)(target+.5f));
       unsigned length=(unsigned)fm_clampf(engine.motion[selected].glide_ms*.25f,0,200);
       cv_line(8,37,8+length,37,colors[selected]);
       cv_line(8+length,34,8+length,41,colors[selected]);
     }else if(group==3){
       float difference=engine.beat_now*.5f;
       label(8,1,"LEFT CARRIER Hz",(int)(engine.carrier_now-difference+.5f));
       label(125,1,"RIGHT CARRIER Hz",(int)(engine.carrier_now+difference+.5f));
       int end=8+(int)fm_clampf(engine.bus_meter[SR_BUS_BINAURAL]*2200.f,0,222);
       for(int y=34;y<39;y++)cv_line(8,y,end,y,colors[selected]);
     }else{
       unsigned bus=group==2?SR_BUS_REFERENCE:group==5?SR_BUS_SACRED:SR_BUS_NATURE;
       float level=group==1||group==14||group==2||group==5?
          engine.bus_meter[bus]:engine.tracks[selected].meter;
       int end=8+(int)fm_clampf(level*1200.f,0,222);
       for(int y=8;y<19;y++)cv_line(8,y,end,y,colors[selected]);
       label(8,ui_font_size==2?0:24,"POST MIX PEAK %",(int)(level*100.f));
     }
   }else if(visual&&page==PAGE_SETUP){
     if(band==1)cv_text(8,3,&FONT_B,setup_tab==3?"QUICK FIND":setup_tab==2?"ABOUT":setup_tab?"ARRANGEMENT BANKS":display_subtab?"DISPLAY / APPEARANCE":"LIGHTS / DISPLAY",skin_text());
     if(band==2){
       if(setup_tab==2){
         cv_text(8,1,&FONT_B,"AQUORA " APP_VERSION,skin_text());
         static const char *power[]={"BATTERY LOW","BATTERY 1 / 3","BATTERY 2 / 3","BATTERY FULL","USB POWER"};
         cv_text(8,26,&FONT_XS,battery_level>=0&&battery_level<=4?power[battery_level]:"BATTERY UNREAD",skin_text());
       }else if(setup_tab==1&&ui_font_size==2){
         static const char *messages[]={"SEL: LOAD / SAVE: STORE","STOP FIRST","EMPTY BANK","K3: ARM OVERWRITE","SAVED","LOADED","BANK ERROR","SAVING..."};
         limited_text(8,10,224,&FONT_XS,messages[bank_message],skin_accent());
       }else cv_text(8,10,&FONT_XS,setup_tab==3?"K1: DESTINATION   K2: OPEN":setup_tab==0?(display_subtab?"TURN SELECT: LIGHTS":"TURN SELECT: APPEARANCE"):"SEL: LOAD / SAVE: STORE",skin_text());
     }
   }else if(visual&&page==PAGE_EVOLVE){
     if(band==1){
       label(8,0,"STEP",engine.loop_pos+1);
       label(86,0,engine.freeze?"FROZEN":engine.evolve_next?"NEXT QUEUED":"PHRASE",engine.phrase);
       static const char *sections[]={"Opening","Build","Resolve","Rest","Return"};
       cv_text(162,0,&FONT_XS,"SECTION",skin_dim());
       limited_text(162,15,72,&FONT_B,sections[engine.phrase/4u%5u],skin_text());
     }else{
       for(unsigned t=0;t<SR_TRACKS;t++){
         unsigned y=t*5u+3u;
         for(unsigned j=0;j<64;j++){
           unsigned index=(evolve_step+1u+j)&63u;
           if(evolve_history[index]&(1u<<t)){
             unsigned x=9+j*222u/64u;
             cv_line(x,y,x+2,y,colors[t]);
           }
         }
       }
       cv_line(232,2,232,42,RGB(87,135,140));
     }
   }else if(visual&&page==3)mixer_band(band);
   else if(visual)geometry(band*48);
   else if(band==0){
     cv_text(8,0,ui_font_size==2?&FONT_M:&FONT_B,pages[page],skin_accent());
     if(page==PAGE_DRUMS&&ui_font_size!=2){
       char count[12];num(count,engine.tracks[SR_DRUMS].events%10000u);
       int x=cv_text(104,2,&FONT_XS,"PULSE ",skin_text());
       cv_text(x,2,&FONT_XS,count,skin_accent());
       char meter[12];num(meter,engine.tracks[SR_DRUMS].meter*100);
       cv_text(181,2,&FONT_XS,meter,skin_text());cv_text(205,2,&FONT_XS,"%",skin_text());
     }else if(page!=0&&ui_font_size!=2)limited_text(104,2,110,&FONT_XS,page==PAGE_SOUND?groups[group]:page==PAGE_SETUP?(setup_tab==3?"QUICK FIND":setup_tab==2?"ABOUT":setup_tab?"ARRANGEMENT BANKS":(display_subtab?"APPEARANCE":"LIGHTS / DISPLAY")):page==2?(song_tab==3?"DEVELOPMENT / EDIT":song_tab==2?"GROOVE / EDIT":song_tab?"NEW SONG / EDIT":"LOOP / EDIT"):page==3?(mix_tab==0?"ALL CHANNELS":mix_tab==1?"MASTER FX":mix_tab==2?"RESET MIX":mix_tab==3?"ACTIVE CHANNELS":mix_tab==5?"CHANNEL STRIP":"BUS SHORTCUTS"):visual_name(),skin_text());
     char track[12];num(track,selected+1);
     if(page!=0){
     cv_text(8,25,&FONT_XS,"SEL",skin_dim());
     cv_text(30,25,&FONT_XS,track,skin_text());
     limited_text(44,25,55,&FONT_XS,role_names[selected],skin_text());
     }
     if(page!=0&&page!=PAGE_AIR){
       cv_text(101,25,&FONT_XS,"PRE",skin_dim());
       limited_text(127,25,105,&FONT_XS,patch_name(),skin_text());
     }
     if(page==0){
       cv_text(8,25,&FONT_XS,engine.keyboard_mode?"EDIT: Key Change":"EDIT: Play Along",skin_accent());
       const char *mode=engine.frequency_lock?"LOCK":engine.tone_level>0?"REF ON":"REF OFF";
       int x=cv_text(107,2,&FONT_XS,mode,skin_accent());
       cv_text(x+4,2,&FONT_XS,sr_frequencies[engine.frequency].name,skin_text());
     }
   }else{
     unsigned start=band==3?0:2;
     knob_cell(start,8);
     knob_cell(start+1,126);
     if(band==4&&ui_font_size!=2){
       if(page==PAGE_DRUMS){
         int x=cv_text(8,33,&FONT_XS,drum_status(),skin_accent());
         char hit[12];num(hit,engine.drum_hit);
         x=cv_text(x+8,33,&FONT_XS,"HIT %",skin_text());
         cv_text(x+5,33,&FONT_XS,hit,skin_text());
       }else if(page==1&&(group==1||group==14)){
         const char *hint=engine.nature_audition?"AUDITION / HOME TO EXIT":
          engine.frequency_lock?"NATURE OFF IN LOCK":
          engine.solo<SR_TRACKS?"NATURE MUTED BY SOLO":"NATURE PEAK %";
         int x=cv_text(8,33,&FONT_XS,hint,skin_text());
         char value[12];num(value,(int)(engine.nature_peak*100.f));
         cv_text(x+6,33,&FONT_XS,value,skin_accent());
       }else if(page==1&&group==6){
         cv_text(8,33,&FONT_XS,selected>SR_BASS?"TURN SELECT TO LEAD OR BASS":
            "LEGATO: OVERLAP NOTES  /  ALWAYS: SLIDE",skin_text());
       }else if(page==2||(page==1&&group==GROUP_KEYS)){
         static const char *notes[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
         unsigned length=engine.loop_length,pos=engine.loop_pos;
         char step[12];
         num(step,engine.running?(pos?pos:length):0);
         int x=cv_text(8,33,&FONT_XS,"STEP ",skin_text());
         x=cv_text(x,33,&FONT_XS,step,skin_text());
         x=cv_text(x,33,&FONT_XS," / ",skin_text());
         num(step,length);cv_text(x,33,&FONT_XS,step,skin_text());
         x=cv_text(126,33,&FONT_XS,"KEY ",skin_text());
         x=cv_text(x,33,&FONT_XS,notes[sr_tonic(&engine)%12],RGB(97,241,244));
         unsigned next=engine.pending_key;
         if(next){x=cv_text(x+3,33,&FONT_XS," > ",skin_text());cv_text(x,33,&FONT_XS,notes[(next-1)%12],RGB(250,171,64));}
       }else if(page==PAGE_SETUP&&setup_tab==1){
         static const char *messages[]={"SEL LOAD / SAVE STORE","STOP FIRST","EMPTY BANK",
           "K3 ARM OVERWRITE","ARRANGEMENT SAVED","ARRANGEMENT LOADED","BANK READ / WRITE FAILED","SAVING..."};
         cv_text(8,33,&FONT_XS,bank_status[bank_slot]==1?"SAVED":bank_status[bank_slot]==2?"INVALID":"EMPTY",skin_accent());
         limited_text(62,33,170,&FONT_XS,messages[bank_message],skin_text());
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
           int x=cv_text(8,33,&FONT_XS,tag,skin_accent());
           x=cv_text(x,33,&FONT_XS,name,skin_text());
           limited_text(x+7,33,225-x,&FONT_B,value,skin_text());
           status=0;
         }
         if(engine.frequency_lock&&status)status=engine.lock_textures?"UNISON / TEXTURES ON":"UNISON / TEXTURES OFF";
         if(engine.isolate&&status)status="SOURCE ISOLATION ON";
         if(status)cv_text(8,33,&FONT_XS,status,skin_accent());
       }
     }
   }
   if(band==4&&!intro_active&&(quick_started||quick_pending||
      (quick_finished_ms&&plat_ms()-quick_finished_ms<3000u))){
     cv_rect(4,17,232,29,ui_background?RGB(255,255,255):RGB(0,0,0));
     char bank[12];num(bank,quick_slot+1);
     const char *message=quick_started||quick_pending?"Saving Bank ":bank_message==BANK_ERROR?"SAVE FAILED - RETRY ":"Saved to Bank ";
     int x=cv_text(8,21,&FONT_XS,message,skin_accent());
     cv_text(x,21,&FONT_XS,bank,skin_text());
   }
   if(band==0&&!intro_active&&!fullscreen&&page==PAGE_PLAY){
     char tag[]="B1";tag[1]=(char)('1'+bank_slot);
     cv_text(205,25,&FONT_XS,tag,skin_dim());
   }
   if(band==0&&!intro_active)battery_glyph();
   cv_blit(0,band*48);AQ_VIS_COUNT(ui_blits);
 }
 dirty=0;
 battery_dirty=0;
 displayed_revision=revision;
}
