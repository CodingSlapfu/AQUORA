#include "kira_face/vocal_pose.h"
_Static_assert(AQUORA_FONT_FAMILY==0x41515335u,"AQUORA Sans atlas required");
static const char aquora_font_provenance[]="AQUORA Sans:" AQUORA_FONT_ATLAS_SHA256;
/* SPDX-License-Identifier: MPL-2.0 */
/* The bare-metal runtime deliberately has no general strcmp/strcpy ABI. */
static unsigned ui_len(const char *s){unsigned n=0;while(n<80u&&s[n])n++;return n;}
static int ui_equal(const char *a,const char *b){for(unsigned n=0;n<80u;n++){if(a[n]!=b[n])return 0;if(!a[n])return 1;}return 0;}
static void ui_copy(char *to,const char *from){unsigned n=0;while(n<79u&&from[n]){to[n]=from[n];n++;}to[n]=0;}
static const felucca_font_t *const fit_fonts[]={&FONT_L,&FONT_M,&FONT_B,&FONT_XS,&FONT_T};
static int fit_width(const felucca_font_t *f,const char *text){int width=0;for(;*text;text++)width+=f->adv[glyph(f,(uint8_t)*text)];return width;}
static const felucca_font_t *fit_font(const char *text,int width,int height){
 /* Main-loop only: bounded cache of actual glyph advances and font heights. */
 static struct {char text[80];int width,height;const felucca_font_t *font;} cache[32] PLAT_POOL;
 static unsigned next,initialized;
 if(!initialized){memset(cache,0,sizeof cache);initialized=1;}
 for(unsigned i=0;i<32;i++)if(cache[i].font&&cache[i].width==width&&cache[i].height==height&&ui_equal(cache[i].text,text))return cache[i].font;
 const felucca_font_t *result=fit_fonts[4];
 for(unsigned i=0;i<5;i++)if(fit_fonts[i]->h<=height&&fit_width(fit_fonts[i],text)<=width){result=fit_fonts[i];break;}
 if(ui_len(text)<sizeof cache[0].text){unsigned i=next++&31u;ui_copy(cache[i].text,text);cache[i].width=width;cache[i].height=height;cache[i].font=result;}
 return result;
}
static int text_room(int y){return y<8?24:47-y;}
#ifdef PLAT_HOST
static unsigned large_text_overflow,large_font_mask;
#endif
static int ui_text(int x,int y,const felucca_font_t *font,const char *text,uint16_t color){
 if(ui_font_size==2&&!intro_active){
  if(y>35)y=35;
  int room=text_room(y);if(room>font->h)room=font->h;
  if(ui_equal(text,"SEL MUTE / LFO SOLO"))text="SEL:M LFO:S";
  font=fit_font(text,236-x,room);
#ifdef PLAT_HOST
  if(fit_width(font,text)>236-x||font->h>47-y)large_text_overflow++;
  for(unsigned i=0;i<5;i++)if(font==fit_fonts[i])large_font_mask|=1u<<i;
#endif
 }
 return cv_text(x,y,font,text,color);
}
/* Graph lane labels cross DMA bands; keep their exact baseline and clip
 * glyphs on both bands rather than applying large-control row snapping. */
static int graph_text(int x,int y,const char *text,uint16_t color){return cv_text(x,y,&FONT_T,text,color);}
#define cv_text ui_text
static const felucca_font_t *ui_caption_font(void){return ui_font_size==0?&FONT_T:ui_font_size==2?(intro_active?&FONT_S:&FONT_T):&FONT_XS;}
static const felucca_font_t *ui_value_font(void){return ui_font_size==0?&FONT_XS:ui_font_size==2?&FONT_L:&FONT_B;}
#define FONT_XS (*ui_caption_font())
#define FONT_B (*ui_value_font())
static const char *pages[]={"PLAY","SOUND","EVOLVE","MIX","DRUMS","SETUP","GATE","VOCALS"};
static const char *beds[]={"Off","Wind","Water","Rain","Forest","Ocean","Birds","Leaves","Florida Night","Twilight Meadow","Crystal Creek","Mountain Rain","Woodland Brook","Distant Storm"};
static const char *groups[]={"Instrument","Nature","Frequency","Binaural","Percussion","Sacred Bass","Lead / Bass Motion","MIX SHORTCUT","Percussion HIT","Visualizer","Arrangement Banks","Source Isolate","Keyboard","Lock Textures","Nature Character","Chords / Strum","Filter / Slicer","Recorded Vocals"};
/* Static panel palettes. Canvas is independently exact black or white. */
static const struct {uint16_t canvas, panel, border, accent, text, dim;} skins[5][2]={
 {{RGB(0,0,0),RGB(0,0,0),RGB(37,86,90),RGB(119,255,225),RGB(238,248,243),RGB(153,196,194)},
  {RGB(0,0,0),RGB(0,0,0),RGB(108,154,161),RGB(10,106,99),RGB(20,38,47),RGB(70,91,97)}},
 {{RGB(0,0,0),RGB(0,0,0),RGB(144,63,170),RGB(255,94,240),RGB(236,251,252),RGB(158,199,205)},
  {RGB(0,0,0),RGB(0,0,0),RGB(132,71,153),RGB(133,21,144),RGB(14,53,56),RGB(66,99,102)}},
 {{RGB(0,0,0),RGB(0,0,0),RGB(26,89,229),RGB(63,168,255),RGB(243,247,255),RGB(162,178,220)},
  {RGB(0,0,0),RGB(0,0,0),RGB(98,126,203),RGB(10,91,177),RGB(24,35,73),RGB(88,98,137)}},
 {{RGB(0,0,0),RGB(0,0,0),RGB(157,88,48),RGB(255,186,85),RGB(255,244,219),RGB(225,182,145)},
  {RGB(0,0,0),RGB(0,0,0),RGB(201,130,91),RGB(169,70,40),RGB(69,38,29),RGB(116,73,53)}},
 {{RGB(0,0,0),RGB(0,0,0),RGB(73,84,188),RGB(140,158,255),RGB(237,238,255),RGB(162,172,216)},
  {RGB(0,0,0),RGB(0,0,0),RGB(110,105,194),RGB(73,60,171),RGB(32,29,74),RGB(92,87,138)}}
};
static uint16_t skin_text(void){return skins[ui_theme][ui_background].text;}
static uint16_t skin_dim(void){return skins[ui_theme][ui_background].dim;}
static uint16_t skin_accent(void){return skins[ui_theme][ui_background].accent;}
static void num(char *s,int v){char t[12];int n=0;unsigned u=v<0?-v:v;if(v<0)*s++='-';do{t[n++]=(char)('0'+u%10);u/=10;}while(u);while(n)*s++=t[--n];*s=0;}
static void limited_text(int x,int y,int width,const felucca_font_t *font,const char *s,uint16_t color);
static void label(int x,int y,const char *name,int value){
 char v[12];num(v,value);
 int width=x==86?70:x==87?72:x==166?66:x>=125?232-x:72;
 limited_text(x,y,width,&FONT_XS,name,skin_dim());
 limited_text(x,y+(ui_font_size==2?12:15),width,&FONT_B,v,skin_text());
}
static const char *role_names[SR_TRACKS]={
 "Lead","Bass","Pads","Harmony","Piano","Plucks","Vocals","Drums"
};
static const uint16_t colors[SR_TRACKS]={
  RGB(97,241,244),RGB(250,171,64),RGB(106,179,255),RGB(213,130,255),
  RGB(143,255,165),RGB(245,139,162),RGB(206,203,255),RGB(255,207,103)
 };
/* Clip to this DMA band BEFORE Bresenham walks pixels. The old cv_pset
 * clipping discarded pixels only after repeatedly walking the entire line. */
static void band_line(int x0,int y0,int x1,int y1,uint16_t color)
{
 if((y0<0&&y1<0)||(y0>=48&&y1>=48))return;
 if(y0>y1){int t=x0;x0=x1;x1=t;t=y0;y0=y1;y1=t;}
 if(y0<0){x0+=(x1-x0)*(-y0)/(y1-y0);y0=0;}
 if(y1>=48){x1=x0+(x1-x0)*(47-y0)/(y1-y0);y1=47;}
 cv_line(x0,y0,x1,y1,color);
}
/* The worker always computes a square 240x240 scene. An identical
 * scale on both axes maps that scene into the normal PLAY window. */
static int visual_x(int x){return fullscreen?x:78+x*84/240;}
static int visual_y(int y){return fullscreen?y:54+y*84/240;}
static void geometry(int offset)
{
 if(vis_input.water==2){
   uint16_t ink=ui_background?RGB(27,66,123):RGB(80,212,235);
   uint16_t grain=ui_background?RGB(107,55,17):RGB(255,218,139);
   for(unsigned i=0;i<vis_output.node_count;i++){
     const int16_t *p=vis_output.nodes[i];
     band_line(visual_x(p[0]),visual_y(p[1])-offset,visual_x(p[2]),visual_y(p[3])-offset,ink);
   }
   for(unsigned i=0;i<512;i++){
     int x=visual_x(vis_output.sand[i][0]),y=visual_y(vis_output.sand[i][1])-offset;
     if(y>=0&&y<48)cv_line(x,y,x+1,y,grain);
   }
   return;
 }
 if(vis_input.water==1){
   static const uint16_t water[16]={RGB(0,0,0),RGB(3,18,31),RGB(4,24,38),RGB(5,31,46),
     RGB(7,39,55),RGB(9,49,66),RGB(12,61,78),RGB(16,74,91),
     RGB(23,89,105),RGB(33,106,121),RGB(48,126,137),RGB(67,147,155),
     RGB(91,171,175),RGB(121,194,195),RGB(158,217,213),RGB(207,242,230)};
   for(unsigned row=0;row<30;row++){
     int start=visual_y((int)row*8)-offset;
     int end=visual_y((int)(row+1)*8)-offset;
     if(start<0)start=0;
     if(end>48)end=48;
     if(end<=start)continue;
     for(unsigned x=0;x<30;x++){
       unsigned light=vis_output.water_light[row][x];
       uint16_t color=ui_background?(light?RGB(255-light*5,255-light*2,255-light):RGB(255,255,255)):water[light];
       int left=visual_x((int)x*8);
       cv_rect(left,start,visual_x((int)(x+1u)*8)-left,end-start,color);
     }
   }
   unsigned shade=nodal_level>.65f?3:nodal_level>.3f?2:1;
   static const uint16_t ink[2][4]={{RGB(9,24,31),RGB(27,84,108),RGB(48,156,184),RGB(127,245,224)},
      {RGB(240,248,250),RGB(142,180,185),RGB(34,103,125),RGB(8,59,84)}};
   for(unsigned i=0;i<vis_output.node_count;i++){
     const int16_t *p=vis_output.nodes[i];int y0=visual_y(p[1])-offset,y1=visual_y(p[3])-offset;
     if((y0<0&&y1<0)||(y0>=48&&y1>=48))continue;
     band_line(visual_x(p[0]),y0,visual_x(p[2]),y1,ink[ui_background][shade]);
   }
   for(unsigned i=0;i<512;i++){
     int x=visual_x(vis_output.sand[i][0]),y=visual_y(vis_output.sand[i][1])-offset;
     if(y>=0&&y<48)cv_line(x,y,x+1,y,ui_background?RGB(117,79,25):i%3?RGB(229,196,126):RGB(255,235,187));
   }
   return;
 }
 for(int t=0;t<SR_TRACKS;t++){
   int quiet=engine.tracks[t].mute||(engine.solo<SR_TRACKS&&engine.solo!=t);
   uint16_t c=ui_background?(quiet?RGB(145,157,160):RGB((colors[t]>>11)*4,((colors[t]>>5)&63)*2,(colors[t]&31)*4)):quiet?RGB(25,45,49):colors[t];
   for(unsigned j=1;j<vis_output.count[t];j++){
     int y0=visual_y(vis_output.xy[t][j-1][1])-offset,y1=visual_y(vis_output.xy[t][j][1])-offset;
     if((y0<0&&y1<0)||(y0>=48&&y1>=48))continue;
     band_line(visual_x(vis_output.xy[t][j-1][0]),y0,visual_x(vis_output.xy[t][j][0]),y1,c);
   }
 }
}

static const char *onoff(int enabled){return enabled?"ON":"OFF";}
static const char *music_modes[]={"Standard","Cinematic","Neoclassical","Experimental Ambient","IDM"};
static const char *visual_name(void){static const char *n[]={"Original","Cymatics","Chladni"};return n[visual_mode];}
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
 if(ui_font_size==2||fit_width(font,s)>width){
   int height=text_room(y);
   if(height>font->h)height=font->h;
   font=fit_font(s,width,height);
 }
 char text[80];
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
static void edit_hint(int x,int y,unsigned current,unsigned total)
{
 char position[20]="ALGO ";
 num(position+5,current+1u);
 unsigned end=ui_len(position);
 position[end++]='/';
 num(position+end,total);
 limited_text(x,y,232-x,fit_font(position,232-x,16),position,skin_dim());
}
static void sidebar_text(int x,int y,int width,const char *s,uint16_t ink)
{
 unsigned start=0;
 for(unsigned row=0;row<3&&y+(int)row*12+FONT_T.h<=48&&s[start];row++){
   char line[32];
   unsigned count=0,space=0;
   int pixels=0;
   while(s[start+count]&&count+1<sizeof line){
     char c=s[start+count];
     pixels+=FONT_T.adv[glyph(&FONT_T,(uint8_t)c)];
     if(pixels>width)break;
     if(c==' ')space=count;
     line[count++]=c;
   }
   if(s[start+count]&&space)count=space;
   if(!count)break;
   line[count]=0;cv_text(x,y+(int)row*12,&FONT_T,line,ink);
   start+=count;
   while(s[start]==' ')start++;
 }
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
 case 1:*name="INTENTION";{
   static const char *short_name[]={"Miracles","Blessing","Energy Cleanse","Cell Renewal","Anxiety Release","Bliss","Positive Vibrations","Neuro-Freq Detox"};
   unsigned scene=shown_intention();
   return scene>=13&&scene-13<sizeof short_name/sizeof *short_name?short_name[scene-13]:sr_scenes[scene].name;
 }
 case 2:*name=selected==SR_DRUMS?"KIT":"PATCH";return patch_name();
 case 4:*name=engine.frequency_lock?"LOCK Hz":engine.frequency_auto?"AUTO Hz":"MANUAL Hz";return sr_frequencies[engine.frequency].name;
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
 case 18:*name="INTENSITY %";number=engine.energy*100;break;
 case 82:*name="EVOLUTION";return engine.variation==0?"Stable":engine.variation<.4f?"Subtle":engine.variation<.7f?"Flowing":"Adventurous";
 case 83:*name="PACE";{static const char *pace[]={"Intention","2 loops","4 loops","8 loops"};return pace[engine.evolve_pace];}
 case 84:*name="NEXT PHRASE";return engine.evolve_next?"Queued":"Turn right";
 case 19:*name="VARIATION %";number=engine.variation*100;break;
 case 20:*name="FREEZE";return onoff(engine.freeze);
 case 21:*name="NATURE BED";return beds[engine.bed_type];
 case 22:*name="FIXED TONE %";return tenth(buffer,(int)(engine.tone_level*1000.f+.5f));
 case 23:*name="BINAURAL %";number=engine.binaural_level*100;break;
 case 24:*name="PAN %";number=track->pan*100;break;
 case 25:*name=engine.motion_manual&4u?"LOCKED BPM":"SONG BPM";return tenth(buffer,(SR_RATE*150u+engine.tick_length/2u)/engine.tick_length);
 case 28:*name="SOLO";return engine.solo<SR_TRACKS?role_names[engine.solo]:"OFF";
 case 29:*name="FX SEND %";number=track->send*100;break;
 case 30:*name="SACRED LEVEL %";number=engine.sacred_level*100;break;
 case 31:*name="SACRED SOURCE";return engine.sacred_mode==0?"Off":engine.sacred_mode==1?"Musical":"Reference";
 case 32:*name="GLIDE MODE";return selected>SR_BASS?"Select Lead / Bass":motion->glide_mode==0?"Off":motion->glide_mode==1?"Legato":"Always";
 case 33:*name="GLIDE ms";if(selected>SR_BASS)return "-";number=motion->glide_ms;break;
 case 34:*name="VIBRATO Hz";if(selected>SR_BASS)return "-";return tenth(buffer,motion->vibrato_rate);
 case 35:*name="VIBRATO cents";if(selected>SR_BASS)return "-";number=motion->vibrato_depth;break;
 case 36:*name="HARMONIC %";number=engine.sacred_harmonics*100;break;
 case 37:*name="HIT %";number=engine.drum_hit;break;
 case 38:*name="VISUALIZER";return visual_name();
 case 39:*name="SONG SLOT";number=bank_slot+1;break;
 case 40:*name="ACTION";return bank_action?"SAVE":"LOAD";
 case 41:*name="OVERWRITE";return bank_confirm?"ARMED":"LOCKED";
 case 55:*name="EXTRA TEXTURES";return engine.lock_textures?"ON / not pure":"OFF / strict";
 case 58:*name="NEW SONG";return engine.running?"Stop first":song_arm==id?"SEL CONFIRM":"Turn to arm";
 case 59:*name="REROLL SOUNDS";return engine.running?"Stop first":song_arm==id?"SEL CONFIRM":"Turn to arm";
 case 60:*name="TRACK LOCK";return onoff(engine.sound_locks&(1u<<selected));
 case 64:{static const char *m[]={"Intention","Straight","Swing"};*name="GROOVE";return m[engine.groove_mode];}
 case 65:*name="STRENGTH %";number=engine.groove_strength;break;
 case 71:
   *name="QUICK FIND";
   if(quick_item<QUICK_CATEGORIES)return quick_names[quick_item];
   {
     const char *parameter;
     knob_value(parameter_routes[quick_item-QUICK_CATEGORIES].id,&parameter,buffer);
     return parameter;
   }
 case 72:*name="OPEN";return "Turn right";
 case 75:*name="BRIGHTNESS";number=engine.tracks[selected].tone;break;
 case 76:*name="ATTACK";number=engine.tracks[selected].attack;break;
 case 77:*name="RELEASE";number=engine.tracks[selected].release;break;
 case 73:*name="KEY SNAP %";number=engine.play_strength;break;
 case 74:*name="KEY FEEL";return engine.play_feel?"Intention":"Straight";
 case 63:{static const char *grid[]={"Live","1/16","1/32","Auto","1/8"};*name="PLAY GRID";return grid[engine.play_grid];}
 case 62:*name="PRESENCE %";number=engine.drum_presence;break;
 case 61:*name="NATURE / FX LOCK";return onoff(engine.sound_locks&(SR_SOUND_NATURE|SR_SOUND_FX));
 case 56:{static const char *m[]={"OFF","LOW","MID","HIGH"};*name="LIGHTS";return m[lighting];}
 case 141:{static const char *m[]={"OFF","C KEYS","WHITE KEYS","ALL KEYS"};*name="KEYS";return m[keys_baseline];}
 case 142:*name="NOTES";return onoff(notes_lights);
 case 143:*name="DANCE";return onoff(dance_lights);
 case 144:*name="ANSWER MY PLAYING";return onoff(engine.response_enabled);
 case 145:*name="PERFORMANCE";return fullscreen?"HOME TO EXIT":"RIGHT: ENTER";

 case 120:{static const char *v[]={"Auto","Together","Strum","Off"};*name="CHORDS";return v[engine.chord_mode];}
 case 121:*name="STRUM GAP ms";if(engine.chord_mode!=2)return "STRUM only";number=engine.chord_gap;break;
 case 122:{static const char *v[]={"Auto","Root","First","Second"};*name="INVERSION";return v[engine.chord_inversion];}
 case 123:{static const char *v[]={"Down / Auto","Down","Up","Alternate"};*name="DIRECTION";return v[engine.chord_direction];}
 case 129:{static const char *v[]={"Intention / Auto","Experimental IDM","Ambient Broken"};*name=engine.rhythm_pending?"NEXT RHYTHM":"RHYTHM STYLE";return v[engine.rhythm_pending?engine.rhythm_pending-1u:engine.rhythm_style];}
 case 130:{static const char *v[]={"Auto","Gentle","Experimental","Off"};*name="TRANSITIONS";return v[engine.fill_character];}
 case 137:*name="BROWSE PLAN";num(buffer,arrange_browse);return buffer;
 case 138:*name="QUEUE NEXT";return sr_arrange_name(&engine,1);
 case 139:*name="LENGTH LOOPS";num(buffer,engine.arrange_length?engine.arrange_length:2u+sr_scenes[engine.scene].energy%3u);return buffer;
 case 140:*name="QUEUE / AUTO";return engine.arrange_next?"Left queue / Right auto":"AUTO";
 case 132:{static const char *m[]={"Off","Play Alone","Alone + Along"};*name="LEARN CAPTURE";return m[engine.learn_mode];}
 case 133:{static const char *m[]={"Original","Subtle","Balanced","Strong"};*name="LEARN STRENGTH";return m[engine.learn_strength];}
 case 134:*name="APPLY TO SONG";return engine.learn_pending?"Queued / next loop":engine.capture.count>=3?"Right: Apply":"Play 3+ notes";
 case 135:*name="DISCARD / CLEAR";return "Left / Right";
 case 97:*name="LEAD MOTION";return engine.motion_manual&1u?"MANUAL":"AUTO";
 case 98:*name="BASS MOTION";return engine.motion_manual&2u?"MANUAL":"AUTO";
 case 91:{static const char *fonts[]={"Small","Medium","Large"};*name="FONT SIZE";return fonts[ui_font_size];}

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
 case 99:*name="OPEN BUS HOME";return "Turn right";
 case 100:*name="OPEN CHANNEL";return "Turn / FX";
 case 101:{static const char *m[]={"Off","Master","USB Slave"};*name="MIDI SYNC";return m[midi.settings.sync];}
 case 102:{static const char *m[]={"Off","Playing","Always"};*name="CLOCK OUT";return m[midi.settings.clock_out];}
 case 103:*name="MIDI ROLE";return role_names[midi.settings.source];
 case 104:*name="TX CHANNEL";number=midi.settings.tx_channel+1;break;
 case 105:*name="RX CHANNEL";if(!midi.settings.rx_channel)return "Omni";number=midi.settings.rx_channel;break;
 case 106:*name="USB MIDI IN";return onoff(midi.settings.usb_in);
 case 107:*name="USB MIDI OUT";return onoff(midi.settings.usb_out);
 case 108:*name="MIDI PANIC";return midi_panic_pending?"Flushing":"Turn right";
 case 110:*name="MIDI CONTROLS";return onoff(midi.settings.controls_out);
 case 152:*name="BRIGHTNESS";if(!engine.filter_cutoff)return "Patch";number=engine.filter_cutoff;break;
 case 153:*name="FILTER EMPHASIS";number=engine.filter_resonance;break;
 case 230:*name="RESET FX";return "AUTO >";
 case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229:{
   static const char *const labels[]={"DECAY","DAMP","WIDTH","TIME","FEEDBACK","TONE",
     "RATE","PHASER","FLANGER","VIB RATE","VIB DEPTH","VIB ONSET","GLIDE",
     "PRESENCE","COMP","MAKEUP","SHIMMER","FLG RATE","FLG FB","CHR DEPTH"};
   unsigned param=id-210u;
   *name=labels[param];
   if(!(engine.vocal_param_lock&(1u<<param)))return "Auto";
   number=engine.vocal_param[param];break;
 }
 case 188:*name="BUILD";return sr_arrange_name(&engine,1);
 case 189:*name="VOCAL HOOK";return engine.vocals_enabled?"ON":"OFF";
 case 190:{
   static const char *const modes[]={"AUTO","PLAY ALONG","AUTO + PLAY"};
   *name="VOCAL MODE";return modes[engine.vocal_play];
 }
 case 187:{
   static const char *const styles[]={"AUTO","SMOOTH PULSE","RHYTHMIC GATE","GROOVE CHOP",
                                     "IDM GLITCH","TRIPLET SYNC","STUTTER FILL","CINEMATIC"};
   *name="STYLE";return styles[engine.slicer_style];
 }
 case 154:{
   static const char *names[]={"OFF","LEAD","BASS","LEAD+BASS","DRUMS","LEAD+DRUM",
     "BASS+DRUM","BAND","VOCALS","VOX+LEAD","VOX+BASS","VOX+L+B",
     "VOX+DRUM","VOX+L+D","VOX+B+D","ALL"};
   *name="GATE TRACKS";return names[engine.slicer_mask];
 }
 case 149:case 150:case 151:{
   static const char *parts[]={"BD KICK","SD SNARE","HH HAT"};
   unsigned part=id-149u;
   *name=parts[part];return engine.drum_mutes&(1u<<part)?"Mute":"On";
 }
 case 169:*name="DECAY %";number=engine.fx_decay;break;
 case 170:*name="FEEDBACK %";number=engine.fx_feedback;break;
 case 171:*name="DAMPING %";number=engine.fx_damping;break;
 case 172:*name="WIDTH %";number=engine.fx_width;break;
 case 173:*name="CHORUS %";number=engine.fx_chorus;break;
 case 174:*name="AUTOPAN %";number=engine.fx_autopan;break;
 case 175:*name="WET FILTER %";number=engine.fx_filter;break;
 case 176:*name="BITCRUSH %";number=engine.fx_crush;break;
 case 177:*name="FLANGER %";number=engine.fx_flanger;break;
 case 178:*name="TREMOLO %";number=engine.fx_tremolo;break;
 case 179:*name="SOFT DRIVE %";number=engine.fx_drive;break;
 case 180:*name="STUTTER %";number=engine.fx_stutter;break;
 case 181:*name="SHIMMER %";number=engine.fx_shimmer;break;
 case 182:*name="FREEZE %";number=engine.fx_granular;break;
 case 183:*name="DUCKING %";number=engine.fx_duck;break;
 case 168:{
   static const char *times[]={"NORMAL","HALF","DOUBLE"};
   *name="TIME";return times[engine.drum_time];
 }
 case 163:*name="CHORUS %";if(!(engine.vocal_fx_manual&1u))return "Auto";number=engine.vocal_chorus;break;
 case 164:*name="REVERB %";if(!(engine.vocal_fx_manual&2u))return "Auto";number=engine.vocal_reverb;break;
 case 166:*name="TUNE %";number=(engine.vocal_fx_manual&8u)?engine.vocal_tune:100;break;
 case 165:*name="ECHO %";if(!(engine.vocal_fx_manual&4u))return "Auto";number=engine.vocal_delay;break;
 case 185:*name="VOCAL LEVEL";if(!engine.vocal_level_manual)return "Auto";number=engine.vocal_level;break;
 case 186:*name="VOICE PATCH";return engine.vocal_patch?sr_vocal_patch_names[engine.vocal_patch-1u]:"Auto";
 case 232:{
   static const char *const flows[]={"Auto","Half","Normal","Double"};
   *name="VOCAL FLOW";return flows[engine.vocal_flow];
 }
 case 231:{
   static const char *const voices[]={"AUTO","AH","OOH","OH","EE","EH","UH","HUM",
     "DO-RE-MI","LA-LA","NA-NA","SCAT","WORDS","OH-AH HOOK","OH-AH","OO-EE","AY-OH","EH-OH","MM-OOH"};
   *name="VOICE";return voices[engine.vocal_articulation];
 }
 case 155:*name="RECORDED VOCALS";return engine.vocals_enabled?"On":"Off";
 case 156:{static const char *names[]={"Sparse","Gentle","Present","Featured"};*name="VOCAL PRESENCE";return names[engine.vocal_presence?engine.vocal_presence-1u:1u];}
 case 157:*name="PART MUTE";return engine.drum_mutes&(1u<<drum_part)?"Mute":"On";
 case 158:*name="PART SOLO";return engine.drum_solo==drum_part?"Solo":"Off";
 case 160:*name="TEMPO MODE";return engine.motion_manual&4u?"Locked":"Auto";
 case 161:*name="INTENSITY MODE";return engine.expression_manual&SR_MANUAL_ENERGY?"Locked":"Auto";
 case 159:*name="PART LEVEL";num(buffer,engine.drum_levels[drum_part]);return buffer;
 case 146:*name="AUDIO OUTPUT";return engine.speaker_enabled?"FM-1 Speaker":"Headphones";
 case 147:{static const char *names[]={"Lagoon","Orchid","Electric Blue","Amber","Indigo"};*name="THEME";return names[ui_theme];}
 case 148:*name="BACKGROUND";return ui_background?"White":"Black";
 case 109:{static const char *m[]={"Off","Quiet","Normal"};*name="STARTUP SOUND";return m[startup_chime];}

 case 52:*name="RESET MIX";return mix_reset?"SEL RESET":"Turn to arm";
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
 const felucca_font_t *caption=ui_font_size==2?&FONT_T:&FONT_XS;
 int label_x=cv_text(x,0,caption,tag,skin_accent());
 limited_text(label_x,0,108-(label_x-x),caption,name,skin_dim());
 int focus=knob_until&&knob_focus==knob;
 if(ui_font_size==2){
   const felucca_font_t *font=fit_font(shown,106,34);
   if(font->h<17){
     char first[80],second[80];unsigned split=0,length=(unsigned)ui_len(shown);
     for(unsigned j=1;j<length;j++)if(shown[j]==' '){
       unsigned before=j>length/2?j-length/2:length/2-j;
       unsigned best=split>length/2?split-length/2:length/2-split;
       if(!split||before<best)split=j;
     }
     if(split&&length<sizeof first){
       memcpy(first,shown,split);first[split]=0;ui_copy(second,shown+split+1);
       const felucca_font_t *a=fit_font(first,106,17),*b=fit_font(second,106,17);
       if(fit_width(a,first)<=106&&fit_width(b,second)<=106){
         limited_text(x,12,106,a,first,focus?skin_accent():skin_text());
         limited_text(x,30,106,b,second,focus?skin_accent():skin_text());return;
       }
     }
   }
   limited_text(x,12,106,font,shown,focus?skin_accent():skin_text());return;
 }
 const felucca_font_t *font=ui_font_size==0?&FONT_S:&FONT_B;
 if(text_pixels(font,shown)>108)font=&FONT_B;
 if(text_pixels(font,shown)>108)font=&FONT_XS;
 limited_text(x,14,108,font,shown,focus?skin_accent():skin_text());
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
   if(band==3){label(8,0,"PRESETS ACK us",intention_latency_us);label(127,0,"PLAN BUILD us",intention_prepare_us);}
   if(band==4){
     int voices=0;for(int i=0;i<SR_VOICES;i++)voices+=!!engine.voices[i].active;
     label(8,0,"ACTIVE VOICES",voices);label(127,0,"INPUT DROPS",dropped);
     timing snapshot;unsigned flags=plat_lock();snapshot=control_queue_timing[!!engine.running];plat_unlock(flags);
     timing_summary q=summarize(&snapshot);
     if(ui_font_size!=2){char v[12];cv_text(8,31,&FONT_XS,"ACK-DSP ms P95",skin_dim());num(v,q.p95/100u);cv_text(106,31,&FONT_XS,v,skin_text());cv_text(132,31,&FONT_XS,"P99",skin_dim());num(v,q.p99/100u);cv_text(166,31,&FONT_XS,v,skin_text());}
   }
   return;
 }
 if(band==0){
   cv_text(8,1,&FONT_B,"PERFORMANCE " APP_VERSION,skin_text());
   cv_text(8,25,&FONT_XS,"VISUAL TARGET: 20 FPS",skin_accent());
 }else if(band==1){
   label(8,0,"PLAY BLITS FPS x10",shown_fps10);
   label(127,0,"MISSED / 2s",shown_missed);
   if(ui_font_size!=2)cv_text(8,31,&FONT_XS,"DRAW us",skin_text());
   char draw[12];num(draw,frame_us);if(ui_font_size!=2)cv_text(65,31,&FONT_XS,draw,skin_text());
   if(ui_font_size!=2){cv_text(125,31,&FONT_XS,"LCD us",skin_text());num(draw,lcd_submit_us);cv_text(170,31,&FONT_XS,draw,skin_text());}

 }else if(band==2){
   label(8,0,"AUDIO MEAN us",audio_stats.mean);
   label(127,0,"FX MEAN us",fx_stats.mean);
   if(ui_font_size!=2)cv_text(8,32,&FONT_XS,"GLO: CALLBACK AND VOICE STATS",skin_text());
 }else if(band==3){
   label(8,0,"DMA XRUN DELTA",xrun_delta);
   label(127,0,"AUDIO+FX % BUDGET",(audio_stats.mean+fx_stats.mean)*100/5805);
   char v[12];num(v,plat_xruns());if(ui_font_size!=2)cv_text(66,15,&FONT_B,v,skin_text());
 }else if(band==4){
   if(ui_font_size==2){label(8,0,"GEOMETRY JOBS",worker_completed);label(127,0,"FALLBACK",worker_fallbacks);}
   else{
   cv_text(8,0,&FONT_XS,plat_cpu2_on()?"CORE 1 DISPLAY / CORE 0 AUDIO":"CORE 0 / SINGLE-CORE FALLBACK",skin_text());
   label(8,12,"GEOMETRY us",vis_output.busy_us);
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
   limited_text(x+2,2,width-3,&FONT_XS,names[channel],ui_background?skin_text():color);
   float magnitude=bus?engine.bus_meter[id]:track->meter;
   int meter=(int)(fm_sqrtf(fm_sqrtf(fm_clampf(magnitude,0,1)))*26.f);
   if(meter)cv_rect(x+3,45-meter,width-7,meter+1,ui_background?skin_accent():color);
   if(!bus){
     int fader=45-(int)(track->volume*26.f);
     cv_line(x+11,19,x+11,45,RGB(75,90,112));
     cv_line(x+10,fader,x+width-4,fader,color);
     if(engine.solo==id)cv_text(x+width-10,19,&FONT_XS,"S",skin_text());
   }
   x+=width;
 }
}
enum { WATER_TOP=75, WATER_BOTTOM=188 };
static struct { uint8_t height, peak, splash, events; } water[SR_TRACKS];
static unsigned water_phase;
static int intention_match=-1;

static int water_wave(unsigned phase)
{
 phase&=31u;
 return phase<16u?(int)phase-8:23-(int)phase;
}

static uint16_t water_tint(uint16_t color,unsigned scale)
{
 return (uint16_t)((((color>>11)*scale>>4)<<11)|
     (((((color>>5)&63u)*scale)>>4)<<5)|((color&31u)*scale>>4));
}

/* Pitch-class agreement with the selected anchor's octave, fifth and
 * major third families. Exact/octave alignment has full credit.
 * Weights approximate audible amplitude; percussion and noise are excluded. */
static float match_pitch(float pitch,float anchor)
{
 if(!(pitch>=10.f&&pitch<=24000.f))return 0.f;
 while(pitch<anchor)pitch*=2.f;
 while(pitch>=2.f*anchor)pitch*=.5f;
 float octave=fm_minf(pitch-anchor,2.f*anchor-pitch);
 float fifth=fm_fabsf(pitch-1.5f*anchor);
 float third=fm_fabsf(pitch-1.25f*anchor);
 float inverse=1.f/anchor;
 return fm_maxf(fm_clampf(1.f-32.f*octave*inverse,0,1),
     fm_maxf(.8f*fm_clampf(1.f-32.f*fifth*inverse,0,1),
           .6f*fm_clampf(1.f-32.f*third*inverse,0,1)));
}

static void mixer_snapshot(void)
{
 struct {float pitch,weight;} notes[SR_VOICES+2];
 unsigned total=0,flags=plat_lock();
 unsigned scene=engine.scene;
 for(unsigned i=0;i<SR_TRACKS;i++){
   const sr_track *track=&engine.tracks[i];
   unsigned events=(uint8_t)track->events;
   if(events!=water[i].events)water[i].splash=i==SR_DRUMS?36:i==SR_DRONE?5:17;
   else if(water[i].splash)water[i].splash--;
   water[i].events=events;
 }
 float meter[SR_TRACKS];
 for(unsigned i=0;i<SR_TRACKS;i++)meter[i]=engine.tracks[i].meter;
 for(unsigned i=0;i<SR_VOICES;i++){
   const sr_voice *voice=&engine.voices[i];
   if(!voice->active||voice->track==SR_DRUMS)continue;
   const sr_track *track=&engine.tracks[voice->track];
   if(track->mute||(engine.solo<SR_TRACKS&&engine.solo!=voice->track)||
       (engine.isolate!=SR_ISOLATE_MIX&&engine.isolate!=SR_ISOLATE_TRACK1+voice->track))continue;
   float weight=voice->env*track->volume*engine.gain_trim[voice->track]*
       (voice->sample_zone?.27f:sr_patches[voice->patch].gain);
   if(voice->track==SR_DRONE)weight*=1.f-.24f*engine.vocal_priority;
   if(weight>.0002f){
     notes[total].pitch=engine.frequency_lock?sr_frequencies[engine.frequency].hz:
         voice->sample_zone?voice->freq:voice->increment*SR_RATE;
     notes[total++].weight=weight;
   }
 }
 if(engine.bus_meter[SR_BUS_REFERENCE]>.0002f){
   notes[total].pitch=engine.reference_hz_now;
   notes[total++].weight=engine.bus_meter[SR_BUS_REFERENCE];
 }
 if(engine.bus_meter[SR_BUS_SACRED]>.0002f){
   notes[total].pitch=engine.sacred_hz_now;
   notes[total++].weight=engine.bus_meter[SR_BUS_SACRED];
 }
 plat_unlock(flags);
 float anchor=sr_frequencies[sr_scenes[scene].frequency].hz;
 float mass=0.f,aligned=0.f;
 for(unsigned i=0;i<total;i++){
   if(notes[i].pitch<=0.f)continue;
   mass+=notes[i].weight;
   aligned+=notes[i].weight*match_pitch(notes[i].pitch,anchor);
 }
 static unsigned previous_scene=SR_SCENES;
 if(mass<=.0002f)intention_match=-1;
 else{
   int score=(int)(100.f*aligned/mass+.5f);
   if(intention_match<0||previous_scene!=scene)intention_match=score;
   else if(score!=intention_match){
     int delta=score-intention_match;
     intention_match+=delta>0?(delta+3)/4:(delta-3)/4;
   }
 }
 previous_scene=scene;
 water_phase=plat_ms()/50u;
 for(unsigned i=0;i<SR_TRACKS;i++){
   int target=(int)(fm_sqrtf(fm_sqrtf(fm_clampf(meter[i],0,1)))*105.f);
   int last=water[i].height;
   int rise=i==SR_DRONE?3:1,fall=i==SR_DRONE?6:4;
   water[i].height=target>last?last+(target-last+rise-1)/rise:last-(last-target+fall-1)/fall;
   water[i].peak=target>=water[i].peak?target:water[i].peak-1;
 }
}

static void mixer_console(int band)
{
 static const char *names[]={"LEAD","BASS","PIANO","PLUCK","PADS","VOCALS","HARM","DRUMS"};
 static const uint16_t neon[]={RGB(255,45,142),RGB(23,201,255),RGB(255,207,46),RGB(145,255,44),
   RGB(38,249,214),RGB(178,96,255),RGB(255,74,212),RGB(255,135,29)};
 int offset=band*48;
 for(unsigned column=0;column<SR_TRACKS;column++){
   unsigned t=mix_order[column];
   const sr_track *track=&engine.tracks[t];
   int x=3+(int)column*29;
   int enabled=!track->mute&&(engine.solo==SR_TRACKS||engine.solo==t);
   uint16_t ink=enabled?neon[column]:RGB(124,135,160);
   uint16_t dim=water_tint(ink,5);
   int chosen=mix_focus==t;
   if(band==1){
     const char *name=names[column];
     char letter[2]={0,0};
     int tx=x+(28-fit_width(&FONT_T,name)+(int)ui_len(name))/2;
     while(*name){
       letter[0]=*name++;
       tx=graph_text(tx,0,letter,ink)-1;
     }
     for(unsigned k=0;k<5;k++)
       cv_rect(x+1+(int)k*5,16,3,5,water[t].peak>(int)k*21?ink:dim);
   }
   int top=WATER_TOP-offset,bottom=WATER_BOTTOM-offset;
   if(top<0)top=0;
   if(bottom>47)bottom=47;
   if(top<=bottom){
     cv_rect(x,top,27,bottom-top+1,RGB(5,12,26));
     cv_line(x,top,x,bottom,chosen?ink:dim);
     cv_line(x+26,top,x+26,bottom,chosen?ink:dim);
     int level=WATER_BOTTOM-(int)water[t].height-offset;
     int amp=1+water[t].splash/7;
     if(water[t].height){
       int fill=level+amp;
       if(fill<top)fill=top;
       if(fill<=bottom){
         cv_rect(x+2,fill,23,bottom-fill+1,water_tint(ink,5));
         cv_rect(x+5,fill,3,bottom-fill+1,water_tint(ink,8));
         cv_rect(x+19,fill,3,bottom-fill+1,water_tint(ink,7));
       }
       for(int j=2;j<=24&&level+amp>=top&&level-amp<=bottom;j++){
         int y=level+water_wave(water_phase+column*7u+(unsigned)j*2u)*amp/8;
         if(y<top)y=top;
         if(y>bottom)continue;
         int end=fill<bottom?fill:bottom;
         if(y<end)cv_line(x+j,y,x+j,end,dim);
         cv_rect(x+j,y,1,1,ink);
       }
       if(fill<=bottom){
         cv_line(x+3,fill,x+3,bottom,dim);
         cv_line(x+23,fill,x+23,bottom,dim);
       }
       for(unsigned k=0;k<4;k++){
         unsigned rise=(water_phase*(1u+k%2u)+column*19u+k*17u)%
             (water[t].height+1u+water[t].splash/3u);
         int y=WATER_BOTTOM-offset-(int)rise;
         if(y<top||y>bottom)continue;
         int bubble=x+4+(int)((k*7u+column*5u+(water_phase>>3))%18u);
         cv_rect(bubble,y,k==0?2:1,1,k==0?RGB(220,249,255):ink);
       }
     }
     if(WATER_TOP>=offset){
       cv_line(x+4,top,x+22,top,dim);
       cv_line(x+2,top+2,x+24,top+2,water_tint(ink,8));
     }
     if(WATER_BOTTOM<offset+48)cv_line(x+2,bottom,x+24,bottom,ink);
   }
   if(band==4){
     char value[12];
     num(value,(int)(track->volume*100.f+.5f));
     unsigned end=ui_len(value);value[end]='%';value[end+1]=0;
     graph_text(x,1,value,ink);
     if(track->mute||engine.solo==t)graph_text(x+8,13,track->mute?"M":"S",ink);
     if(chosen)cv_line(x+1,27,x+24,27,ink);
   }
 }
 if(band==4){
   if(!mix_tab){
     for(unsigned knob=0;knob<4;knob++){
       char text[16];const char *name="TRACK";
       const char *value=knob?knob_value(control_for(knob),&name,text):role_names[mix_focus<SR_TRACKS?mix_focus:selected];
       limited_text(4+59*knob,25,56,&FONT_T,name,skin_dim());
       limited_text(4+59*knob,37,56,&FONT_T,value,skin_text());
     }
   }else graph_text(7,33,"VOL %   EDIT + ALGO: CHANNEL",RGB(173,206,224));
 }
}
static void mixer_focus_strip(int band)
{
 unsigned t=selected;
 const sr_track *tr=&engine.tracks[t];
 static const char *big[]={"LEAD","BASS","PADS","HARMONY","PIANO","PLUCKS","VOCALS","DRUMS"};
 if(band==1){
   limited_text(9,1,200,ui_font_size==2?&FONT_M:&FONT_B,big[t],skin_accent());
   cv_text(8,31,&FONT_XS,tr->mute?"MUTE":engine.solo==t?"SOLO":"CHANNEL STRIP",skin_text());
   cv_text(143,31,&FONT_XS,"SEL MUTE / LFO SOLO",skin_dim());
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
   static const char *names[]={"K1 LVL","K2 PAN","K3 SEND","K4 SOLO"};
   const char *values[]={a,b,c,engine.solo==t?"ON":"OFF"};
   for(unsigned k=0;k<4;k++){
     int x=7+k*58;
     limited_text(x,1,55,&FONT_XS,names[k],skin_dim());
     limited_text(x,20,55,&FONT_B,values[k],skin_text());
   }
 }
}
static void mixer_band(int band)
{
 if(band==1&&(mix_tab==1||mix_tab==2)){
   static const char *const sections[]={"MASTER FX","SPACE FX","MOTION FX","TEXTURE FX","AMBIENT FX"};
   static const char *const next[]={"FX: SPACE FX","FX: MOTION FX","FX: TEXTURE FX","FX: AMBIENT FX","FX: MASTER FX"};
   limited_text(8,1,224,&FONT_B,mix_tab==1?sections[mix_fx_detail]:"RESET MIX",skin_accent());
   limited_text(8,27,224,&FONT_XS,mix_tab==1?next[mix_fx_detail]:mix_reset?"MIX RESET ARMED":"TURN K1 TO ARM",skin_text());return;
 }
 if(mix_tab==0||mix_tab==3){mixer_overview(band);return;}
 if(mix_tab==4){
   static const char *names[]={"Nature","Sacred","Ref","Binaural","FX"};
   int offset=(band-1)*48;
   for(unsigned b=0;b<SR_BUS_COUNT;b++){
     int x=4+(int)b*47;
     uint16_t color=colors[b];
     if(band==1){
       if(b==mix_bus)cv_line(x,0,x+43,0,skin_text());
       limited_text(x+1,2,43,&FONT_XS,names[b],ui_background?skin_text():color);
     }
     float mag=fm_clampf(engine.bus_meter[b],0,1);
     int meter=(int)(fm_sqrtf(fm_sqrtf(mag))*58.f);
     cv_line(x+5,29-offset,x+5,87-offset,RGB(75,90,112));
     if(meter)cv_rect(x+8,87-meter-offset,32,meter+1,ui_background?skin_accent():color);
   }
   return;
 }
 if(band==2&&(mix_tab==1||mix_tab==2)){
   label(8,2,"PEAK %",(int)(engine.master_peak*100.f));
   label(87,2,"LIMIT %",(int)(engine.limiter*100.f));
   label(166,2,"GAIN %",(int)(engine.gain_trim[selected]*100.f));

   return;
 }
 unsigned ids[SR_TRACKS], count=0;
 for(unsigned t=0;t<SR_TRACKS;t++){
   const sr_track *track=&engine.tracks[t];
   if(mix_tab!=3||(!track->mute&&track->volume>0&&(t!=SR_DRUMS||engine.drums_enabled)))
     ids[count++]=t;
 }
 if(!count){
   if(band==1)cv_text(8,4,&FONT_XS,"NO ACTIVE TRACKS / ALGO FOR ALL",skin_text());
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
     limited_text(x+2,2,width-3,&FONT_XS,count>4?short_names[t]:role_names[t],ui_background?skin_text():color);
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
/* Per-frame snapshots keep strip rendering on one musical timebase. */
static __typeof__(engine.timeline) graph_notes;
static sr_engine graph_snapshot PLAT_POOL;
static sr_event graph_future[384] PLAT_POOL;
/* Fingerprint 8-row regions. A rendered stripe is submitted as a contiguous
 * range; only the canvas buffers hold pixels. The signature combines a
 * rotating sum and an independent multiplicative sequence. */
static uint32_t panel_signature[30][2];
static unsigned panel_cached_bands;
static unsigned submit_panel_band(unsigned band,int full_motion)
{
    enum { width=240, rows=48, stripe=8, stripes=rows/stripe };
    unsigned first=stripes,last=0,mask=1u<<band;
    if(full_motion){
        panel_cached_bands&=~mask;
        first=0;last=stripes;
    }else{
        for(unsigned i=0;i<stripes;i++){
            const uint16_t *pixels=cv_px+i*stripe*width;
            uint32_t a=2166136261u,b=0x94d049bbu;
            for(unsigned j=0;j<stripe*width;j+=2){
                uint32_t pair=pixels[j]|((uint32_t)pixels[j+1]<<16);
                a=(a^pair)*16777619u;
                b=((b<<5)|(b>>27))+pair;
            }
            uint32_t *previous=panel_signature[band*stripes+i];
            if(!(panel_cached_bands&mask)||a!=previous[0]||b!=previous[1]){
                if(first==stripes)first=i;
                last=i+1u;
                previous[0]=a;previous[1]=b;
            }
        }
        panel_cached_bands|=mask;
    }
    if(first>=last)return 0;
    lcd_blit(0,band*rows+first*stripe,width,(last-first)*stripe,cv_px+first*stripe*width);
#if APP_CANVAS_DOUBLE_BUFFER
    cv_last_sent=cv_px;
#endif
    return 1;
}
static unsigned graph_future_count;
static uint32_t drum_forecast[16],drum_view_history[16];
static unsigned drum_view_page,drum_view_pages;
static unsigned drum_view_step,drum_view_mutes,drum_view_solo;
static uint32_t graph_frame,graph_tick,graph_boundary,graph_phase;
static float graph_scale;
static const char *graph_now,*graph_next;
static void graph_block(unsigned band,int x,int width,int y,int height,uint16_t ink){
 if(y<48){height-=48-y;y=48;}if(y+height>208)height=208-y;
 int top=y-(int)band*48;
 if(x<35){width-=35-x;x=35;}if(x+width>233)width=233-x;
 if(top<0){height+=top;top=0;}if(top+height>48)height=48-top;
 if(width>0&&height>0)cv_rect(x,top,width,height,ink);
}
static void visual_title(int x,int y,int width,int height,const char *text)
{
 const felucca_font_t *font=fit_font(text,width,height);
 const uint16_t shade=ui_background?RGB(255,255,255):RGB(0,0,0);
 const uint16_t ink=ui_background?RGB(8,21,32):RGB(239,247,255);
 cv_text(x+1,y+1,font,text,shade);
 cv_text(x,y,font,text,ink);
}
static void fullscreen_labels(void)
{
 unsigned scene=shown_intention();
 unsigned mode=engine.pending_mode?engine.pending_mode-1u:engine.music_mode;
 visual_title(8,3,224,23,sr_scenes[scene].name);
 visual_title(8,26,154,17,music_modes[mode]);
 visual_title(164,28,68,13,engine.pending_mode?"NEXT BAR":"EDIT+ALGO");
}
static void intention_mode_header(void){
 unsigned scene=shown_intention();
 unsigned mode=engine.pending_mode?engine.pending_mode-1u:engine.music_mode;
 int width=133;
 limited_text(8,0,width,fit_font(sr_scenes[scene].name,width,25),
              sr_scenes[scene].name,skin_text());
 limited_text(8,26,31,&FONT_T,"MODE",skin_dim());
 limited_text(42,25,131,fit_font(music_modes[mode],131,18),
              music_modes[mode],skin_accent());
 limited_text(150,8,65,&FONT_T,engine.pending_mode?"NEXT BAR":"EDIT+ALGO",skin_text());
}
static void evolve_graph(unsigned band){
 unsigned offset=band*48;
 for(unsigned t=0;t<SR_TRACKS;t++){
  int y=49+t*13;
  if(y+12>(int)offset&&y<(int)offset+48){
   char n[3];num(n,t+1);graph_text(6,y-offset,n,t==selected?skin_accent():skin_dim());
   cv_line(35,y+9-offset,232,y+9-offset,skins[ui_theme][ui_background].border);
  }
 }
 /* History uses actual note onset/duration/pitch. Future blocks are only
  * already committed scheduled notes; no decorative or guessed events. */
 unsigned sprites=0;
 for(unsigned i=0;i<96u+graph_future_count;i++){
  unsigned role,note,velocity;uint32_t frame,duration;
  if(i<96){role=graph_notes[i].role;note=graph_notes[i].note;velocity=graph_notes[i].velocity;frame=graph_notes[i].frame;duration=graph_notes[i].duration;}
  else {unsigned j=i-96;role=graph_future[j].role;note=graph_future[j].note;velocity=graph_future[j].velocity;frame=graph_future[j].frame;duration=graph_future[j].duration;}
  if(!duration||role>=SR_TRACKS)continue;
  if(i>=96u&&(int32_t)(frame-graph_frame)<0)continue;
  int32_t age=(int32_t)(graph_frame-frame);
  int x=78-(int)((float)age*graph_scale);
  int width=(int)((float)duration*graph_scale);if(width<5)width=5;
  if(x>232||x+width<35)continue;
  uint16_t ink=ui_background?skin_accent():colors[role];
  int glint=age>=0&&age<(int32_t)(SR_RATE/7u);
  /* Full true duration is a thin sustain rail. Separate substantial attack
   * blocks keep successive onsets legible even when releases overlap. */
  graph_block(band,x,width,53+role*13,2,ink);
  graph_block(band,x,6,50+role*13,9,ink);
  graph_block(band,x+width-1,1,52+role*13,4,ink);
  if(glint){
   unsigned core=7u-(unsigned)age/(SR_RATE/49u);if(core>7u)core=7u;
   graph_block(band,x,core,50+role*13,3,skin_text());
   graph_block(band,x+1,2,48+role*13,2,skin_accent());
  }
  if(i<96u&&sprites<64u&&age>=0){
   int32_t release=age-(int32_t)duration;
   unsigned burst=age<(int32_t)(SR_RATE/3u)|| (release>=0&&release<(int32_t)(SR_RATE/3u));
   unsigned sustain=age<(int32_t)duration;
   if(burst||sustain){
     unsigned life=release>=0?(unsigned)release:(unsigned)age;
     unsigned phase=life/(SR_RATE/20u),count=burst?3u+velocity/32u:2u;
     for(unsigned q=0;q<count&&sprites<64u;q++,sprites++){
       unsigned hash=(frame/64u+role*131u+note*17u+q*71u);
       int dx=(int)(hash%13u)-6,dy=(int)((hash/13u)%11u)-5;
       int center=sustain?(burst?78:x+(int)((hash+graph_frame/(SR_RATE/12u))%(unsigned)width)):x+width;
       if(burst){dx=dx*(int)(phase+1u)/3;dy=dy*(int)(phase+1u)/3;}
       else {dx=(int)((hash+graph_frame/(SR_RATE/12u))%9u)-4;}
       graph_block(band,center+dx,burst?2:1,53+role*13+dy,2,phase<2u?skin_text():ink);
     }
   }
  }
  if(role==selected){
   int pitch=role==SR_DRUMS?note-32:note-36;if(pitch<0)pitch=0;if(pitch>60)pitch=60;
   int y=201-pitch/2;
   graph_block(band,x,width,y+2,2,ink);
   graph_block(band,x,6,y,6,ink);
   if(glint)graph_block(band,x,5,y,3,skin_text());
  }
 }
 if(band==3){limited_text(6,13,100,&FONT_T,role_names[selected],skin_accent());
  limited_text(110,13,122,&FONT_T,"SEL: DETAIL",skin_dim());}
 int boundary=78+(int)(graph_boundary*24u/5u)-(int)((float)graph_phase*graph_scale);
 int bottom=band==4?15:47;
 if(boundary>78&&boundary<233)for(int y=0;y<bottom-2;y+=6)cv_line(boundary,y,boundary,y+2,skin_dim());
 cv_line(78,0,78,bottom,skin_accent());
 if(band==4){
  cv_rect(0,16,240,32,skins[ui_theme][ui_background].canvas);
  static const char *captions[]={"SEL ROLE","K2 HOLD","K3 BPM","K4 LOOP"};
  for(unsigned knob=0;knob<4;knob++){
   char value[16];const char *name;
   const char *shown=knob?knob_value(control_for(knob),&name,value):role_names[selected];
   int x=8+(int)knob*57;
   limited_text(x,17,52,&FONT_T,captions[knob],skin_dim());
   limited_text(x,30,52,&FONT_T,shown,knob_until&&knob_focus==knob?skin_accent():skin_text());
  }
 }
}
static unsigned display_strip,display_animate,display_first,display_inputs,display_selections,display_revision;
static unsigned display_fullscreen,display_visual_mode,display_debug;

static int display_frame_invalid(void)
{
 return aq_frame_invalid(display_fullscreen,fullscreen,display_visual_mode,visual_mode,
                         display_debug,debug,input_revision-display_inputs,
                         select_revision-display_selections);
}
#ifndef PLAT_HOST
static unsigned panel_transfer_started;
#endif
static int draw(int animate)
{
#ifndef PLAT_HOST
 if(app_lcd_transfer_pending()&&plat_ticks24()-panel_transfer_started<240000u)return 0;
#endif
 if(display_strip&&display_frame_invalid()){
    display_strip=0;
    dirty=1;
 }
 unsigned revision=display_strip?display_revision:state_revision;
 if(display_strip)animate=display_animate;

 static unsigned prior_page=~0u;
 if(page!=prior_page){prior_page=page;page_animation_ms=plat_ms();}
 unsigned entry_age=plat_ms()-page_animation_ms;
 int entry_x=entry_age<220u?(int)((220u-entry_age)*(220u-entry_age)/1600u):0;
 if(entry_age<220u)dirty=1;
 static unsigned forecast_valid, forecast_step, forecast_revision, forecast_clock, forecast_sync, forecast_page, forecast_drum;
 unsigned score_visible=page==PAGE_EVOLVE&&song_tab==0&&!debug&&!intro_active&&!fullscreen;
 unsigned drums_visible=page==PAGE_DRUMS&&!debug&&!intro_active&&!fullscreen;
 if(!score_visible&&!drums_visible)forecast_valid=0;
 if(!display_strip&&(page==PAGE_EVOLVE||page==PAGE_PLAY||fullscreen||drums_visible)&&!debug&&!intro_active){
  unsigned flags=plat_lock();
  unsigned refresh=0;
  unsigned drum_position=engine.drum_playhead&15u;
  if(drums_visible){
   drum_view_step=drum_position;
   drum_view_page=engine.drum_playhead/16u;
   drum_view_pages=engine.loop_length/16u;
   drum_view_mutes=engine.drum_mutes|(engine.drum_group_solo?(~engine.drum_group_solo&65535u):0u);
   drum_view_solo=engine.drum_solo;
   memcpy(drum_view_history,engine.drum_steps+(engine.drum_playhead&~15u),sizeof drum_view_history);
  }
  if(score_visible||drums_visible){
   if(score_visible)memcpy(graph_notes,engine.timeline,sizeof graph_notes);
   refresh=!forecast_valid||forecast_page!=page||forecast_step!=engine.sequence_step||
     (drums_visible&&forecast_drum!=engine.drum_playhead)||
     forecast_revision!=revision||forecast_clock!=engine.tick_length||
     forecast_sync!=engine.external_sync;
   if(refresh){
    sr_score_snapshot(&graph_snapshot,&engine);
    forecast_step=engine.sequence_step;forecast_page=page;forecast_drum=engine.drum_playhead;
    forecast_revision=revision;
    forecast_clock=engine.tick_length;
    forecast_sync=engine.external_sync;
   }
  }
  graph_frame=engine.frame_count;graph_tick=engine.tick_length?engine.tick_length:1;graph_phase=engine.tick_phase;
  graph_now=sr_section_name(&engine,0);graph_next=sr_section_name(&engine,1);
  graph_boundary=sr_section_remaining(&engine)*16u-(engine.loop_pos&15u);
  plat_unlock(flags);
  graph_scale=4.8f/(float)graph_tick;
  if(refresh){
   unsigned input_before=input_revision;
   app_input_only();
   if(input_before!=input_revision){forecast_valid=0;dirty=1;return 0;}
   if(drums_visible){
    memset(drum_forecast,0,sizeof drum_forecast);
    unsigned remaining=15u-drum_position;
    unsigned ticks=graph_snapshot.drum_time==1u?remaining*2u-(remaining&&!(graph_snapshot.sequence_step&1u)):
                   graph_snapshot.drum_time==2u?remaining/2u:remaining;
    /* Include a pending DOUBLE step before the next song tick. */
    if(graph_snapshot.drum_time==2u&&remaining&&!(drum_position&1u))ticks++;
    if(sr_score_preview(&graph_snapshot,graph_future,384,ticks))
      memcpy(drum_forecast,graph_snapshot.drum_steps+drum_view_page*16u,sizeof drum_forecast);
   }else graph_future_count=sr_score_preview(&graph_snapshot,graph_future,384,34);
   forecast_valid=1;
  }
 }
 if(!display_strip){
   display_fullscreen=fullscreen;
   display_visual_mode=visual_mode;
   display_debug=debug;
   display_inputs=input_revision;
   display_selections=select_revision;
   display_first=!intro_active&&labelled_input!=input_revision;
   display_revision=revision;
   display_animate=animate;
 }
 static const uint8_t control_first[5]={3,4,0,1,2}, fullscreen_first[5]={4,0,1,2,3};
 int water_visible=page==PAGE_MIX&&(mix_tab==0||mix_tab==3)&&!debug&&!intro_active&&!fullscreen;
 if(water_visible&&!display_strip&&(animate||dirty))mixer_snapshot();
 /* Bind one audio observation to every portrait band in this frame. */
 static k9_face vocal_pose;
 static unsigned expressive_lid;
 static int head_lift;
 if(page==PAGE_VOCALS&&!vocal_tab&&!display_strip&&(animate||dirty)){
   unsigned flags=plat_lock();
   vocal_pose=k9_recorded_vocal_pose(&engine);
   plat_unlock(flags);
   unsigned now=plat_ms();
   unsigned blink=now%3959u;
   vocal_pose.blink=blink<75u?255:blink<130u?130:0;
   unsigned lid_target=!!vocal_pose.lid;
   int lift_target=lid_target?(vocal_pose.brow>0?-5:4):0;
   if(head_lift<lift_target)head_lift++;
   if(head_lift>lift_target)head_lift--;
   vocal_pose.head_y=(int8_t)(head_lift/3);
   vocal_pose.tilt=(int8_t)(head_lift/3);
   vocal_pose.lid=0;
   if(lid_target)expressive_lid=expressive_lid<211u?expressive_lid+44u:255u;
   else expressive_lid=expressive_lid>31u?expressive_lid-32u:0;
   if(expressive_lid>vocal_pose.blink)vocal_pose.blink=(uint8_t)expressive_lid;
   if(vocal_shutdown&&!requested_play){
     unsigned elapsed=now-vocal_shutdown_at;
     vocal_pose.blink=elapsed>=720u?255u:(uint8_t)(elapsed*255u/720u);
     vocal_pose.singing_light=0;
     vocal_pose.module=0;
   }
   if(vocal_pose.singing_light)vocal_pose.module=(now/90u)&3u;
 }else if(page!=PAGE_VOCALS||vocal_tab){
   expressive_lid=0;head_lift=0;
 }
 for(;display_strip<5;display_strip++){
   int band=display_first?(fullscreen?fullscreen_first:control_first)[display_strip]:display_strip;
   /* Input dispatch must not wait for a complete animated LCD frame. Poll
    * only between bands, never while the canvas/snapshot is being drawn.
    * Navigation can change every page-local selector: abandon this frame
    * rather than mixing snapshots or clearing the new dirty notification. */
   app_input_only();
   if(display_frame_invalid()){display_strip=0;dirty=1;return 0;}
   /* Only explicit fullscreen is immersive: both PLAY visualizers always keep
    * the lower physical K1-K4 control panels visible in normal mode. */
   int visual=page==PAGE_VOCALS||page==PAGE_GATE||intro_active||fullscreen||band==1||band==2||water_visible||(drums_visible&&band>0)||(page==PAGE_EVOLVE&&song_tab==0&&band>=3)||(page==PAGE_MIX&&band>=3&&!debug&&(mix_tab==0||mix_tab==3||mix_tab==5));
   unsigned band_input=input_revision,band_revision=state_revision;
   if(!(battery_dirty&&(band==0||(band==2&&page==PAGE_SETUP&&setup_tab==2)))&&(debug ? !dirty : visual ? !animate&&!dirty : !dirty))continue;
   /* Intro-only vibrant flat color fills the unused screen area. RGB565
    * palette is chosen once, not per pixel; normal UI still follows user
    * Black/White preference on entering PLAY. */
   const int artwork=intro_active||fullscreen;
   const uint16_t bg=RGB(0,0,0);
   cv_begin(240,48,bg);
   if(!intro_active&&!fullscreen&&band==0&&plat_ms()-last_control_ms<250u){
     unsigned w=24u+(250u-(plat_ms()-last_control_ms))*180u/250u;
     cv_rect(8,45,w,2,skin_accent());
   }
   /* One surface per logical region; no boxes around 48px transfer bands.
    * The PLAY preview spans two bands with no nested frame. */
   if(!artwork&&!debug&&!(page==PAGE_VOCALS&&!vocal_tab)){
     const uint16_t panel=skins[ui_theme][ui_background].panel;
     const uint16_t edge=skins[ui_theme][ui_background].border;
     if(band==0){cv_line(8,47,231,47,edge);}
     else if(band>=3&&page!=PAGE_MIX&&page!=PAGE_DRUMS&&!(page==PAGE_EVOLVE&&song_tab==0)){
       cv_rect(6,2,110,42,panel);cv_rect(124,2,110,42,panel);
       if(ui_theme==2||ui_theme==4){
         cv_line(6,2,115,2,edge);cv_line(124,2,233,2,edge);
       }else if(ui_theme==3){
         cv_line(6,43,115,43,edge);cv_line(124,43,233,43,edge);
       }
     }
   }
   if(intro_active){
     /* Independent AQUORA intro: flowing sound-pulse typography, never the
      * selected musical visualizer. Uses a precomputed sine LUT and up to 42
      * short LCD strokes per frame; no worker job or audio ISR computation. */
     unsigned elapsed=plat_ms()-intro_start;
     unsigned phase=(elapsed/35u)&127u;
     if(band==1||band==2||band==3){
       static const uint16_t spiral_ink[6]={RGB(0,0,0),RGB(0,0,0),RGB(255,235,55),RGB(158,41,255),RGB(0,199,180),RGB(240,52,181)};
       unsigned reveal=elapsed<2000u?elapsed*48u/2000u:48u;
       int px=120,py=116-(int)(band*48u);
       for(unsigned j=1;j<=reveal;j++){
         unsigned angle=(j*7u+phase)&127u;float radius=j*1.65f;
         int x=120+(int)(radius*vis_sine[angle]);
         int y=116+(int)(radius*vis_sine[(angle+32u)&127u])-(int)(band*48u);
         band_line(px+2,py+2,x+2,y+2,RGB(5,24,39));
         band_line(px,py,x,y,spiral_ink[(j/8u+elapsed/300u)%6]);
         band_line(px+1,py,x+1,y,spiral_ink[(j/8u+elapsed/300u)%6]);px=x;py=y;
       }
     }
     if(band==2){
       /* The same generated AQUORA Sans glyphs as the firmware, revealed
        * after the spiral forms. Preserve full-word placement as it unfolds. */
       unsigned letters=elapsed<2400u?0u:1u+(elapsed-2400u)/260u;
       if(letters>6u)letters=6u;
       char title[7]="AQUORA";title[letters]=0;
       int x=(240-fit_width(&FONT_L,"AQUORA"))/2,y=(240-FONT_L.h)/2-96;
       /* Draw only glyph pixels, never an opaque box or plaque.
        * Each 48px band is cleared and the spiral is redrawn before the word. */
       if(letters)cv_text(x,y,&FONT_L,title,RGB(119,255,225));
     }
     if(band==4){
       if(elapsed>=4100u)limited_text(8,1,224,&FONT_T,APP_VERSION,skin_text());
       /* Progress fills gently across the intro; never touches audio logic. */
       unsigned progress=elapsed>=8000u?176u:(elapsed*176u)/8000u;
       cv_line(32,6,208,6,RGB(22,108,120));
       if(progress)cv_line(32,6,32+(int)progress,6,RGB(119,255,225));
       cv_text(44,23,&FONT_XS,"ANY CONTROL TO SKIP",RGB(119,255,225));
     }
   }else if(debug)performance(band);
   else if(fullscreen){
     geometry(band*48);
     if(band==0)fullscreen_labels();
     else if(band==4&&knob_until){
       char value[16];const char *name;
       const char *current=knob_value(control_for(knob_focus),&name,value);
       visual_title(8,29,224,17,current);
     }else if(band==4&&fullscreen_performance){
       static const char *keys[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
       char bpm[12];
       num(bpm,(SR_RATE*15u+graph_tick/2u)/graph_tick);
       visual_title(8,9,176,14,graph_now);
       visual_title(188,9,44,14,keys[sr_tonic(&engine)%12]);
       visual_title(8,27,176,14,graph_next);
       visual_title(188,27,44,14,bpm);
     }
   }
   else if(page==PAGE_GATE){
     static const uint16_t prism[][3]={
       {RGB(48,218,237),RGB(51,122,249),RGB(136,246,215)},
       {RGB(255,169,54),RGB(255,84,111),RGB(255,227,132)},
       {RGB(133,244,152),RGB(23,193,162),RGB(202,245,106)},
       {RGB(227,120,255),RGB(107,112,249),RGB(255,142,201)}
     };
     unsigned pattern=sr_slicer_pattern(&engine);
     unsigned position=engine.sequence_step?(engine.sequence_step-1u)&15u:0u;
     unsigned open=(pattern>>position)&1u;
     unsigned enabled=engine.running&&engine.slicer_mask;
     const uint16_t *palette=prism[engine.scene%4u];
     if(band==0){
       char value[16];const char *name;
       limited_text(8,0,150,&FONT_B,"GATE / SLICER",skin_text());
       graph_text(172,4,!engine.slicer_mask?"BYPASS":!engine.running?"STOPPED":open?"OPEN":"GATED",palette[0]);
       limited_text(8,26,224,&FONT_XS,knob_value(187,&name,value),skin_dim());
     }else if(band>=1&&band<=3){
       int offset=(band-1)*48;
       unsigned selected_gain=engine.slicer_mask?__builtin_ctz(engine.slicer_mask):0;
       int energy=(int)fm_clampf(engine.tracks[SR_LEAD].meter*90.f+engine.drum_meter*21.f,0,19);
       int aperture=(int)(engine.slicer_gain[selected_gain]*17.f);
       for(unsigned i=0;i<16;i++){
         int x=30+(int)(i%8u)*29,y=35+(int)(i/8u)*65-offset;
         int lit=(pattern>>i)&1u;
         int active=engine.running&&i==position;
         int link_y=y+(i&1u?6:-6);
         int half=lit?18:9;
         if(active&&enabled&&open)half+= (energy+aperture)/3;
         if(link_y+half+4<0||link_y-half>=48)continue;
         if(i%8u!=7u)cv_line(x,link_y,x+29,y-(i&1u?6:-6),skin_dim());
         uint16_t accent=lit?palette[i%3u]:skin_dim();
         uint16_t body=water_tint(accent,lit?7:4);
         for(int j=-half;j<=half;j+=2){
           int width=11-((j<0?-j:j)*10/half);
           cv_line(x-width,link_y+j,x,link_y+j,body);
           cv_line(x,link_y+j,x+width,link_y+j,accent);
         }
         cv_line(x,link_y-half,x,link_y+half,active?skin_text():accent);
         if(active){
           cv_line(x-10,link_y+half+4,x+10,link_y+half+4,palette[0]);
           if(enabled&&open)cv_line(x-12,link_y,x+12,link_y,skin_text());
         }
       }
     }else if(band==4){
       for(unsigned k=0;k<4;k++){
         char value[16];const char *name;
         const char *current=knob_value(control_for(k),&name,value);
         limited_text(5+59*k,0,57,&FONT_T,name,skin_dim());
         limited_text(5+59*k,14,57,&FONT_T,current,skin_text());
       }
       limited_text(6,31,112,&FONT_T,sr_section_name(&engine,0),skin_accent());
       limited_text(123,31,110,&FONT_T,sr_section_name(&engine,1),skin_dim());
     }
   }
   else if(page==PAGE_VOCALS){
     const sr_track *track=&engine.tracks[SR_CHOIR];
     if(!vocal_tab)k9_face_render_canvas(cv_px,48u*band,48,&vocal_pose);
     if(band==0){
       static const char *const title[]={"KIRA-9 VOCALS","VOCAL FX 1/6 SPACE",
         "VOCAL FX 2/6 ECHO","VOCAL FX 3/6 MODULATION","VOCAL FX 4/6 EXPRESSION",
         "VOCAL FX 5/6 TONE + TUNING","VOCAL FX 6/6 SPATIAL","VOCAL FLOW / PLAY ALONG"};
       if(!vocal_tab){
         const felucca_font_t *font=fit_font(title[0],222,24);
         limited_text(9,2,225,font,title[0],RGB(0,0,0));
         limited_text(8,1,225,font,title[0],RGB(242,207,255));
       }else limited_text(8,1,226,&FONT_XS,title[vocal_tab],RGB(228,182,255));
       if(vocal_tab)limited_text(8,24,224,&FONT_XS,"ALGORITHM: TAB   FX: AUDITION",skin_text());
     }
     if(vocal_tab==7u&&band==3){
       static const char *const status[]={"READY","OFF","STOPPED","FREQUENCY LOCK",
         "LEVEL ZERO","MUTED","OTHER TRACK SOLO","OTHER SOURCE ISOLATED","PLAY ALONG ONLY"};
       limited_text(8,2,224,&FONT_XS,status[sr_vocal_status(&engine)],skin_text());
       char value[16];
       num(value,engine.vocal_started);
       graph_text(8,25,"VOCAL STARTS",skin_dim());
       graph_text(100,25,value,skin_accent());
       num(value,engine.vocal_rejected);
       graph_text(145,25,"DENIED",skin_dim());
       graph_text(200,25,value,skin_accent());
     }
     if(vocal_tab&&band==2){
       for(unsigned k=0;k<4;k++){
         char buf[16];const char *name;
         const char *value=knob_value(control_for(k),&name,buf);
         limited_text(6+59*k,2,57,&FONT_T,name,skin_dim());
         limited_text(6+59*k,24,57,&FONT_XS,value,skin_text());
       }
     }
     if(band==4){
       if(!vocal_tab)for(unsigned k=0;k<4;k++){
         static const char *const names[]={"VOCALS","PRESENCE","LEVEL","PATCH"};
         char buf[16];const char *name;
         const char *value=knob_value(control_for(k),&name,buf);
         if(k==0&&track->mute)value="Muted";
         int x=6+59*k;
         const char *label=k<3u||!engine.vocal_patch?value:"Manual";
         limited_text(x+1,2,56,&FONT_T,names[k],RGB(0,0,0));
         limited_text(x,1,56,&FONT_T,names[k],RGB(235,209,249));
         limited_text(x+1,15,56,&FONT_XS,label,RGB(0,0,0));
         limited_text(x,14,56,&FONT_XS,label,RGB(255,242,255));
       }
       if(vocal_tab==7u){
         char value[16];
         graph_text(8,0,"SAMPLE",skin_dim());
         num(value,engine.vocal_sample_denied);
         graph_text(60,0,value,skin_accent());
         graph_text(122,0,"VOICE",skin_dim());
         num(value,engine.vocal_budget_denied);
         graph_text(175,0,value,skin_accent());
         graph_text(8,15,"VOICES",skin_dim());
         num(value,engine.voice_limit?engine.voice_limit:SR_VOICES);
         graph_text(60,15,value,skin_accent());
         graph_text(122,15,"PCM AGE MS",skin_dim());
         if(engine.vocal_audio_frames)num(value,(engine.frame_count-engine.vocal_last_pcm_frame)/(SR_RATE/1000u));
         else ui_copy(value,"--");
         graph_text(200,15,value,skin_accent());
       }
       if(!vocal_tab&&engine.vocal_patch){
         const char *patch=sr_vocal_patch_names[engine.vocal_patch-1u];
         limited_text(7,31,226,fit_font(patch,226,17),patch,RGB(228,182,255));
       }else limited_text(7,31,226,&FONT_T,vocal_tab?"ALGO: TAB   SEL: MUTE":"FX: AUDITION  ALGO: FX  SEL: MUTE",RGB(228,182,255));
     }
   }
   else if(water_visible){
     if(band) mixer_console(band);
     else{
       unsigned role=mix_focus<SR_TRACKS?mix_focus:selected;
       limited_text(7,0,137,&FONT_B,"WATER MIXER",RGB(46,226,255));
       limited_text(147,0,88,&FONT_XS,role_names[role],skin_text());
       graph_text(7,23,engine.tracks[role].mute?"MUTED":engine.solo==role?"SOLO":
                     engine.solo<SR_TRACKS?"OTHER SOLO":"IN MIX",skin_accent());
       limited_text(77,23,158,&FONT_T,"SEL MUTE / LFO SOLO / EDIT CLEAR",skin_text());
     }
   }else if(visual&&page==PAGE_MIX&&mix_tab==5){
     mixer_focus_strip(band);
   }else if(visual&&page==PAGE_DRUMS){
     static const char *lanes[]={"Kick","Snare","Cl Hat","Kick 2","Clap","Op Hat","Ped Hat","Rim","Snare 2","Low Tom","Hi Tom","Crash","Ride","Shaker","Conga","Bell"};
     unsigned first=!drum_tab?(drum_part&8u):0;
     unsigned end=!drum_tab?first+8u:SR_DRUM_PARTS;
     if(!drum_tab&&band==3){
       graph_text(6,0,first?"LANES 9-16":"LANES 1-8",skin_accent());
       graph_text(92,0,engine.tracks[SR_DRUMS].mute?"MIXER MUTED":
          engine.solo<SR_TRACKS&&engine.solo!=SR_DRUMS?"OTHER TRACK SOLO":
          engine.drum_group_solo?"GROUP SOLO":engine.drum_solo<SR_DRUM_PARTS?"PART SOLO":"LIVE MIX",skin_text());
       graph_text(6,15,"SEL MUTE / LFO SOLO / FX AUDITION",skin_text());
       graph_text(6,31,"EDIT: GROUP / FX CLEAR",skin_dim());
       char page_number[]="BAR 1/1";
       page_number[4]=(char)('1'+drum_view_page);
       page_number[6]=(char)('0'+drum_view_pages);
       graph_text(167,31,page_number,skin_accent());
     }
     for(unsigned lane=first;lane<end;lane++){
       unsigned row_band=!drum_tab?1u+(lane-first)/4u:1u+lane/5u;
       int y=!drum_tab?(int)(lane%4u)*12:(int)(lane%5u)*9;
       if(row_band!=(unsigned)band)continue;
       uint16_t ink=(drum_view_mutes&(1u<<lane))||(drum_view_solo<SR_DRUM_PARTS&&drum_view_solo!=lane)
         ?skin_dim():colors[lane%3u==0?SR_BASS:lane%3u==1?SR_DRUMS:SR_GUITAR];
       if(lane==drum_part&&(!drum_tab||drum_tab>=5))cv_rect(3,y,2,9,skin_accent());
       graph_text(8,y,lanes[lane],ink);
       for(unsigned step=0;step<16;step++){
         unsigned events=step>drum_view_step?drum_forecast[step]:drum_view_history[step];
         unsigned hit=events&(1u<<lane);
         int x=69+(int)step*10;
         if(step==drum_view_step)cv_rect(x-1,y,10,9,skins[ui_theme][ui_background].border);
         cv_rect(x,y+3,7,hit?(!drum_tab?6:4):1,hit?ink:skins[ui_theme][ui_background].border);
         if(!drum_tab&&hit&&step==drum_view_step&&engine.running)
           cv_line(x+1,y+2,x+5,y+2,skin_text());
         if(hit&&step>drum_view_step)cv_rect(x+1,y+4,5,2,bg);
         if(hit&&(events&(65536u<<lane)))cv_rect(x+2,y+3,2,1,skin_text());
       }
     }
     if(band==4){
       for(unsigned k=0;k<4;k++){
         char number[16];const char *name;
         const char *value=knob_value(control_for(k),&name,number);
         int x=4+(int)k*59;
         if(k==0&&(!drum_tab||drum_tab>=5)){name="K1 PART";value=sr_drum_parts[drum_part];}
         limited_text(x,12,56,&FONT_T,name,skin_dim());
         limited_text(x,25,56,&FONT_XS,value,skin_text());
       }
     }
   }else if(visual&&page==PAGE_SOUND&&sound_picker){
     if(band==1){
       cv_text(8,0,&FONT_B,"SOUND / SECTIONS",skin_text());
       cv_text(8,28,&FONT_XS,"SEL: TRACK   ALGO: SECTION",skin_text());
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
     if(band==1){cv_text(8,0,&FONT_B,role_names[selected],skin_accent());limited_text(105,0,125,&FONT_XS,(engine.running||engine.preview_frames)?"LIVE SCOPE":"PATCH MODEL",skin_text());}
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
         cv_text(8,24,&FONT_XS,"LIVE RESPONSE",skin_text());
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
       for(int x=8;x<=end;x+=6){
         int half=3+(x-8)*10/222;
         cv_line(x,15-half,x,15+half,water_tint(colors[selected],7+(unsigned)x%6u));
         cv_rect(x,14,3,3,colors[selected]);
       }
       label(8,ui_font_size==2?0:24,"POST MIX PEAK %",(int)(level*100.f));
     }
   }else if(visual&&page==PAGE_SETUP){
     if(band==1&&setup_tab==2){limited_text(8,1,224,&FONT_B,"AQUORA SANS / ABOUT",skin_text());limited_text(8,28,224,&FONT_T,aquora_font_provenance,skin_dim());}
     else if(band==1)cv_text(8,3,&FONT_B,setup_tab>=4?"STARTUP SOUND":setup_tab==3?"QUICK FIND":setup_tab==2?"AQUORA SANS / ABOUT":setup_tab?"USER SONGS":display_subtab?"DISPLAY / APPEARANCE":"LIGHTS / DISPLAY",skin_text());
     if(band==2){
       if(setup_tab>=4){
         limited_text(8,0,224,&FONT_XS,"K1: STARTUP   K2: AUDIO OUTPUT",skin_text());
         limited_text(8,32,224,&FONT_T,engine.speaker_enabled?"SPEAKER MODE PAUSES BINAURAL":preferences_dirty?"STOP TO SAVE PREFERENCES":"STEREO HEADPHONE OUTPUT",skin_dim());
       }else if(setup_tab==2){
         cv_text(8,1,&FONT_B,"AQUORA " APP_VERSION,skin_text());
         limited_text(8,26,224,&FONT_XS,save_info==0?"EDIT + ALGO: SAVE SUMMARY":save_info==1?"EDIT + ALGO: FLASH TIMES":"EDIT + ALGO: COUNTERS",skin_text());
       }else if(setup_tab==1){
         unsigned slot=bank_slot+1u;
         char number[]="00", bpm[8];
         number[0]=(char)('0'+slot/10u);number[1]=(char)('0'+slot%10u);
         int x=cv_text(8,0,&FONT_XS,number,skin_accent());
         const char *status=bank_status[bank_slot]==1?" SAVED":bank_status[bank_slot]==2?" INVALID":" EMPTY";
         cv_text(x,0,&FONT_XS,status,skin_text());
         if(bank_status[bank_slot]==1){
           const char *scene=sr_scenes[bank_summary[bank_slot].scene].name;
           limited_text(110,0,122,&FONT_XS,scene,skin_accent());
           const char *mode=music_modes[bank_summary[bank_slot].mode];
           limited_text(8,24,168,&FONT_XS,mode,skin_text());
           num(bpm,bank_summary[bank_slot].bpm);
           x=cv_text(180,24,&FONT_XS,bpm,skin_accent());
           cv_text(x,24,&FONT_T," BPM",skin_text());
         }
       }else cv_text(8,10,&FONT_XS,setup_tab==3?"K1: DESTINATION   K2: OPEN":setup_tab==0?(display_subtab?"SEL: LIGHTS":"SEL: APPEARANCE"):"SEL: LOAD / SAVE: STORE",skin_text());
     }
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==0){
     evolve_graph(band);
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==6){
     if(band==1||band==2){
       for(unsigned row=0;row<3;row++){
        unsigned ahead=(band-1u)*3u+row+arrange_browse;
        char text[12];num(text,ahead);
        int y=row*16;cv_text(6,y,&FONT_T,ahead==0?"NOW":ahead==1?"NEXT":text,ahead==1?skin_accent():skin_dim());
        limited_text(38,y,150,&FONT_T,sr_arrange_name(&engine,ahead),skin_text());
        unsigned bars=ahead==0?sr_section_remaining(&engine):ahead==1&&engine.arrange_next?engine.arrange_length*(engine.loop_length/16u):2u*(2u+sr_scenes[engine.scene].energy%3u);
        num(text,bars);cv_text(195,y,&FONT_T,text,skin_accent());cv_text(218,y,&FONT_T,"b",skin_dim());
       }
     }
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==7){
     if(band==1)limited_text(8,0,224,&FONT_B,"MUSICAL RESPONSE",skin_text());
     else if(band==2)limited_text(8,0,224,&FONT_XS,"PLAY A PHRASE, THEN REST",skin_accent());
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==5){
     if(band==1)limited_text(8,2,224,&FONT_B,"LEARN FROM PLAYING",skin_accent());
     if(band==2){label(8,0,"CAPTURED",engine.capture.count);label(127,0,"APPLIED",engine.learned.count);}
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==1){
     if(band==1){limited_text(8,1,224,&FONT_B,"NEW SONG / REROLL",skin_accent());
       limited_text(8,27,224,&FONT_XS,engine.running?"STOP TO GENERATE":song_arm==58?"NEW SONG ARMED":song_arm==59?"REROLL ARMED":"TURN K1 / K2 TO ARM",skin_text());}
     if(band==2){limited_text(8,1,224,&FONT_XS,"SEL: CONFIRM / SAVE: BANK",skin_text());limited_text(8,27,224,&FONT_XS,"REROLL KEEPS MOTIF / LOCKS",skin_dim());}
   }else if(visual&&page==PAGE_EVOLVE&&song_tab==4){
     if(band==1){limited_text(8,1,224,&FONT_B,music_modes[engine.music_mode],skin_accent());
       limited_text(8,27,224,&FONT_I,engine.music_mode==0?"Ambient balance":engine.music_mode==1?"Spacious expression":engine.music_mode==2?"Chamber movement":"Evolving pulse / textures",skin_text());}
     if(band==2){
       limited_text(8,0,224,&FONT_XS,"INTENTION SELECTS STYLE",skin_text());
       limited_text(8,24,224,&FONT_XS,"MANUAL OVERRIDES AUTO DEFAULTS",skin_dim());
     }
   }else if(visual&&page==PAGE_EVOLVE){
     evolve_graph(band);
   }else if(visual&&page==3&&band<3)mixer_band(band);
   else if(visual&&page==PAGE_PLAY){
     geometry(band*48);
     if(band==1){
       limited_text(8,2,66,&FONT_T,"NOW",skin_accent());
       const char *now=graph_now?graph_now:"Planning";
       sidebar_text(8,15,67,now,skin_text());
       limited_text(170,2,62,&FONT_T,"NEXT",skin_dim());
       const char *next=graph_next?graph_next:"Planning";
       sidebar_text(170,15,62,next,skin_text());
     }
     if(band==2){
       limited_text(8,8,68,&FONT_T,role_names[selected],skin_accent());
       sidebar_text(170,8,62,patch_name(),skin_text());
     }
   }else if(band==0&&page==PAGE_PLAY){
     intention_mode_header();
   }else if(band==0&&page==PAGE_EVOLVE){
     cv_text(6+entry_x,0,ui_font_size==2?&FONT_M:&FONT_B,"EVOLVE",skin_accent());
     edit_hint(155,2,song_tab,8);
     int x=cv_text(6,21,&FONT_T,"NOW ",skin_accent());
     limited_text(x,21,210-x,&FONT_XS,graph_now?graph_now:"Planning",skin_text());
     x=cv_text(6,35,&FONT_T,"NEXT ",skin_dim());
     limited_text(x,35,224-x,&FONT_XS,graph_next?graph_next:"Planning",skin_text());
   }else if(band==0){
     cv_text(8+entry_x,0,ui_font_size==2?&FONT_M:&FONT_B,pages[page],skin_accent());
     if(page==PAGE_DRUMS){
       static const char *tabs[]={"PATTERN","SOUND / MIX","MUTE / SOLO","RHYTHM / FILLS","PART MUTES","16-PART / FX AUDITION","PATTERN / TIME"};
       limited_text(104,2,110,&FONT_XS,tabs[drum_tab],colors[SR_DRUMS]);
     }else if(page==PAGE_SOUND){
       limited_text(104,2,128,fit_font(groups[group],128,20),groups[group],skin_text());
     }else if(page!=0&&ui_font_size!=2)limited_text(104,2,110,&FONT_XS,page==PAGE_SETUP?(setup_tab>=4?"STARTUP SOUND":setup_tab==3?"QUICK FIND":setup_tab==2?"ABOUT":setup_tab?"USER SONGS":(display_subtab?"APPEARANCE":"LIGHTS / DISPLAY")):page==2?(song_tab==6?"ARRANGE / EDIT":song_tab==5?"LEARN / EDIT":song_tab==4?"AUTO STYLE / MOTION":song_tab==3?"DEVELOPMENT / EDIT":song_tab==2?"GROOVE / EDIT":song_tab?"NEW SONG / EDIT":"LOOP / EDIT"):page==3?(mix_tab==0?"ALL CHANNELS":mix_tab==1?"MASTER FX":mix_tab==2?"RESET MIX":mix_tab==3?"ACTIVE CHANNELS":mix_tab==5?"CHANNEL STRIP":"BUS SHORTCUTS"):visual_name(),skin_text());
     char track[12];num(track,selected+1);
     if(page==PAGE_SOUND||page==PAGE_PLAY){
     cv_text(8,25,&FONT_XS,"SEL",skin_dim());
     cv_text(30,25,&FONT_XS,track,skin_text());
     limited_text(44,25,63,&FONT_XS,role_names[selected],skin_accent());
     unsigned section=0;
     for(unsigned i=0;i<sizeof(sound_groups);i++)if(sound_groups[i]==group)section=i;
     edit_hint(141,25,section,sizeof(sound_groups));
     }
     if(0){
       cv_text(101,25,&FONT_XS,"PRE",skin_dim());
       limited_text(127,25,105,&FONT_XS,patch_name(),skin_text());
     }
     if(preferences_saved_ms&&plat_ms()-preferences_saved_ms<1800u&&page==PAGE_SETUP)
       limited_text(173,25,59,&FONT_T,"SAVED",skin_accent());
     if(page!=0&&page!=PAGE_SOUND){
       unsigned section=page==PAGE_MIX?mix_tab:page==PAGE_DRUMS?drum_tab:page==PAGE_SETUP?setup_tab:vocal_tab;
       unsigned total=page==PAGE_MIX?5:page==PAGE_DRUMS?7:page==PAGE_SETUP?5:2;
       if(page==PAGE_MIX&&mix_tab==5){
         limited_text(8,25,224,&FONT_T,"ALGO: MIX   SEL MUTE / LFO SOLO",skin_accent());
       }else{
       char pos[12];num(pos,section+1);int x=cv_text(8,25,&FONT_XS,"ALGO ",skin_accent());
       x=cv_text(x,25,&FONT_XS,pos,skin_text());x=cv_text(x,25,&FONT_XS,"/",skin_dim());num(pos,total);x=cv_text(x,25,&FONT_XS,pos,skin_text());
       limited_text(x+5,25,163-x,&FONT_XS,"NEXT PAGE",skin_text());
       }
     }

   }else if(page==PAGE_SETUP&&setup_tab==2){
     if(save_info==0){
       if(band==3){label(8,0,"SNAPSHOT ms",quick_snapshot_ms>=quick_request_ms?quick_snapshot_ms-quick_request_ms:0);label(127,0,"FLASH ms",quick_flash_ms);}
       if(band==4){label(8,0,"TOTAL ms",quick_total_ms);
         limited_text(127,0,105,&FONT_XS,"SAVE STATE",skin_dim());
         const char *state=quick_timeout?"TIMEOUT":quick_started?"WRITING":quick_pending?"QUEUED":quick_last_result<0?"FAILED":quick_has_save?"VERIFIED":"IDLE";
         limited_text(127,18,105,&FONT_B,state,skin_text());
       }
     }else if(save_info==1){
       if(band==3){label(8,0,"ERASE us",quick_progress[4]);label(127,0,"PAYLOAD us",quick_progress[5]);}
       if(band==4){label(8,0,"HEADER us",quick_progress[6]);label(127,0,"VERIFY us",quick_progress[7]);}
     }else{
       if(band==3){label(8,0,"REQUESTS",quick_requests);label(127,0,"RETRIES",quick_retries);}
       if(band==4){label(8,0,"WORKER BUSY",quick_progress[2]);label(127,0,"SUSPENDS",quick_progress[8]);}
     }
   }else if(page==PAGE_EVOLVE&&song_tab==0&&band==4){
     /* Enlarging the graph must not hide the tempo/loop controls. */
     static const char *captions[]={"SEL ROLE","K2 FREEZE","K3 BPM","K4 LOOP"};
     for(unsigned knob=0;knob<4;knob++){
       char value[16];const char *name;
       const char *shown=knob?knob_value(control_for(knob),&name,value):role_names[selected];
       int x=8+(int)knob*57;
       limited_text(x,0,52,&FONT_T,captions[knob],skin_dim());
       const felucca_font_t *font=fit_font(shown,52,24);
       limited_text(x,15,52,font,shown,knob_until&&knob_focus==knob?skin_accent():skin_text());
       if(knob<3)cv_line(x+54,0,x+54,43,skins[ui_theme][ui_background].border);
     }
   }else{
     unsigned start=band==3?0:2;
     if(page==PAGE_DRUMS&&drum_tab==5&&start==0){
       cv_text(8,0,&FONT_T,"K1 DRUM PART",skin_dim());
       limited_text(8,15,108,&FONT_B,sr_drum_parts[drum_part],skin_accent());
     }else knob_cell(start,8);
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
         cv_text(8,33,&FONT_XS,selected>SR_BASS?"EDIT + ALGO: LEAD OR BASS":
            (engine.motion_manual&(1u<<selected)?"MANUAL / EVOLVE: MOTION AUTO":"AUTO: INTENTION x MUSIC MODE"),skin_text());
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
         limited_text(62,33,170,&FONT_XS,(bank_message==BANK_STOP&&engine.external_audio)?"RELEASE MIDI NOTES":messages[bank_message],skin_text());
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
     const char *message=quick_timeout?"Save Failed / Retry ":quick_started||quick_pending?"Saving Bank ":bank_message==BANK_ERROR?"SAVE FAILED - RETRY ":"Saved to Bank ";
     int x=cv_text(8,21,&FONT_XS,message,skin_accent());
     cv_text(x,21,&FONT_XS,bank,skin_text());
   }
   if(band==0&&!intro_active&&!fullscreen&&page==PAGE_PLAY){
     unsigned slot=bank_slot+1u;
     char tag[]="B00*";
     tag[1]=(char)('0'+slot/10u);tag[2]=(char)('0'+slot%10u);
     if(quick_has_save&&quick_saved_revision==state_revision&&bank_slot==quick_slot)tag[3]=0;
     cv_text(205,25,&FONT_XS,tag,skin_dim());
   }
   if(band==0&&!intro_active&&!fullscreen)battery_glyph();
   unsigned lcd_start=plat_ticks24();
   if(submit_panel_band(band,animate&&(intro_active||
      (page==PAGE_PLAY&&band>0&&(fullscreen||band<3))))){
     AQ_VIS_COUNT(ui_blits);
#ifndef PLAT_HOST
     panel_transfer_started=plat_ticks24();
#endif
   }
   if(!debug)lcd_submit_us+=(plat_ticks24()-lcd_start)/24u;
   /* Acknowledge the submitted control band, rather than charging later
    * optional graphics to label feedback. Physical DMA completion is a
    * separate device measurement, not represented by this counter. */
   unsigned feedback_band=knob_until?(fullscreen||knob_focus>=2?4u:3u):0u;
   if((unsigned)band==feedback_band&&band_revision==state_revision&&band_input==input_revision&&qread==qwrite&&labelled_input!=band_input){
     measure(&control_label_timing[!!engine.running],(plat_ms()-input_changed_ms)*100u);
     labelled_input=band_input;
   }
#ifndef PLAT_HOST
   if(display_strip<4){display_strip++;return 0;}
#endif
 }
 display_strip=0;
 dirty=input_revision!=display_inputs;
 battery_dirty=0;
 displayed_revision=revision;
 return 1;
}

#undef cv_text
