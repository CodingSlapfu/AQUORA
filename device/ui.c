/* SPDX-License-Identifier: MPL-2.0 */
static const char *pages[]={"PLAY","SOUND","EVOLVE","MIX"};
static const char *beds[]={"Off","Wind","Water","Rain","Forest","Ocean","Birds","Leaves"};
static const char *groups[]={"Instrument","Nature","Frequency","Binaural","Percussion","Sacred Bass","Lead / Bass Motion","Track Routing"};
static void num(char *s,int v){char t[12];int n=0;unsigned u=v<0?-v:v;if(v<0)*s++='-';do{t[n++]=(char)('0'+u%10);u/=10;}while(u);while(n)*s++=t[--n];*s=0;}
static void label(int x,int y,const char *name,int value){char v[12];num(v,value);cv_text(x,y,&FONT_XS,name,RGB(126,166,164));cv_text(x,y+15,&FONT_B,v,RGB(218,240,220));}
static const char *role_names[SR_TRACKS]={
 "Lead","Bass","Pads","Harmony","Piano","Plucks","Choir","Drums"
};
static void geometry(int offset)
{
 static const uint16_t colors[SR_TRACKS]={
  RGB(97,241,244),RGB(250,171,64),RGB(106,179,255),RGB(213,130,255),
  RGB(143,255,165),RGB(245,139,162),RGB(206,203,255),RGB(255,207,103)
 };
 for(int t=0;t<SR_TRACKS;t++){
   int quiet=engine.tracks[t].mute||(engine.solo<SR_TRACKS&&engine.solo!=t);
   uint16_t c=quiet?RGB(25,45,49):colors[t];
   for(int j=1;j<=32;j++){
     int y0=vis_output.xy[t][j-1][1]-offset,y1=vis_output.xy[t][j][1]-offset;
     if((y0<0&&y1<0)||(y0>=48&&y1>=48))continue;
     cv_line(vis_output.xy[t][j-1][0],y0,vis_output.xy[t][j][0],y1,c);
   }
 }
}
static void performance(int band)
{
 if(debug==2){
   if(band==0){
     cv_text(8,1,&FONT_B,"CALLBACK / CORE",C_WHITE);
     cv_text(8,25,&FONT_XS,"256 FRAMES / 5805 us / GLO CLOSE",C_WHITE);
   }
   if(band==1){label(8,0,"CALLBACK MEAN us",callback_stats.mean);label(127,0,"CALLBACK PEAK us",callback_stats.peak);}
   if(band==2){label(8,0,"CALLBACK P95 us",callback_stats.p95);label(127,0,"CALLBACK P99 us",callback_stats.p99);}
   if(band==3){label(8,0,"GEOMETRY+LCD us",graphics_us);label(127,0,"WORKER REASON",plat_cpu2_gave_up());}
   if(band==4){
     int voices=0;for(int i=0;i<SR_VOICES;i++)voices+=!!engine.voices[i].active;
     label(8,0,"ACTIVE VOICES",voices);label(127,0,"INPUT DROPS",dropped);
   }
   return;
 }
 if(band==0){
   cv_text(8,1,&FONT_B,"PERFORMANCE " APP_VERSION,C_WHITE);
   cv_text(8,25,&FONT_XS,engine.perfect_key?"LFO: Perfect Key ON":"LFO: Perfect Key OFF",RGB(147,221,207));
 }else if(band==1){
   label(8,0,"Audio Processing us",audio_stats.mean);
   label(127,0,"FX Processing us",fx_stats.mean);
 }else if(band==2){
   label(8,0,"Audio P95 / PEAK",audio_stats.p95);
   label(127,0,"FX P95 / PEAK",fx_stats.p95);
   char v[12];
   num(v,audio_stats.peak);cv_text(66,15,&FONT_B,v,C_WHITE);
   num(v,fx_stats.peak);cv_text(185,15,&FONT_B,v,C_WHITE);
 }else if(band==3){
   label(8,0,"CPU / DMA XRUN",plat_cpu_pct());
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
static void draw(void){unsigned revision=state_revision;int first=dirty?0:1,last=dirty?5:3;if(debug){first=0;last=5;}
 for(int band=first;band<last;band++){cv_begin(240,48,RGB(5,17,22));
 if(debug)performance(band);
 else{
 if(band==0){
   cv_text(8,1,&FONT_B,pages[page],RGB(174,236,208));
   cv_text(83,2,&FONT_XS,page==1?groups[group]:sr_scenes[engine.scene].name,C_WHITE);
   cv_text(8,24,&FONT_XS,role_names[selected],RGB(174,236,208));
   cv_text(60,24,&FONT_XS,selected==SR_DRUMS?sr_drum_kits[engine.drum_kit].name:sr_patches[engine.tracks[selected].patch].name,
      engine.tracks[selected].mute?RGB(90,110,110):C_WHITE);
   cv_text(188,24,&FONT_XS,engine.running?"PLAY":"STOP",C_WHITE);
 }
 if(band==1||band==2)geometry(band*48);
 if(band==3){
 if(page==0){label(8,0,"K1 SCENE",engine.scene+1);label(126,0,"K2 ENERGY %",(int)(engine.energy*100));}
 if(page==1){switch(group){case 0:label(8,0,selected==SR_DRUMS?"K1 KIT":"K1 PATCH",selected==SR_DRUMS?engine.drum_kit+1:engine.tracks[selected].patch+1);label(126,0,"K2 LEVEL %",engine.tracks[selected].volume*100);break;case 1:cv_text(8,0,&FONT_XS,"K1 NATURE",C_WHITE);cv_text(8,16,&FONT_B,beds[engine.bed_type],C_WHITE);label(126,0,"K2 LEVEL %",engine.bed_level*100);break;case 2:label(8,0,"K1 TONE Hz",sr_frequencies[engine.frequency].hz);label(126,0,"K2 LEVEL %",engine.tone_level*100);break;case 3:label(8,0,"K1 CARRIER Hz",engine.binaural_carrier);label(126,0,"K2 BEAT x10",engine.binaural_rate*10);break;case 4:cv_text(8,0,&FONT_XS,"K1 KIT",C_WHITE);cv_text(8,15,&FONT_B,sr_drum_kits[engine.drum_kit].name,C_WHITE);label(126,0,"K2 LEVEL %",engine.drum_level*100);break;
 case 5:label(8,0,"K1 SACRED LEVEL %",engine.sacred_level*100);cv_text(126,0,&FONT_XS,"K2 SACRED SOURCE",C_WHITE);cv_text(126,15,&FONT_B,engine.sacred_mode==0?"Off":engine.sacred_mode==1?"Musical":"Reference",C_WHITE);break;
 case 6:if(selected<=SR_BASS){
   static const char *modes[]={"Off","Legato","Always"};
   cv_text(8,0,&FONT_XS,"K1 GLIDE MODE",C_WHITE);cv_text(8,15,&FONT_B,modes[engine.motion[selected].glide_mode],C_WHITE);
   label(126,0,"K2 GLIDE ms",engine.motion[selected].glide_ms);
 }else cv_text(8,12,&FONT_B,"SELECT LEAD OR BASS",C_WHITE);break;
 case 7:label(8,0,"K1 LEVEL %",engine.tracks[selected].volume*100);label(126,0,"K2 PAN %",engine.tracks[selected].pan*100);break;}}
 if(page==2){label(8,0,"K1 VARIATION",engine.variation*100);label(126,0,"K2 FREEZE",engine.freeze);}
 if(page==3){label(8,0,"K1 LEVEL %",engine.tracks[selected].volume*100);label(126,0,"K2 PAN %",engine.tracks[selected].pan*100);}}
 if(band==4 && page==1 && group==6 && selected<=SR_BASS){
 label(8,0,"K3 VIBRATO x0.1Hz",engine.motion[selected].vibrato_rate);
 label(126,0,"K4 DEPTH CENTS",engine.motion[selected].vibrato_depth);
 cv_text(8,32,&FONT_XS,saved_status==2?"SAVE FAILED":saved_status==1?"SETTINGS SAVED":"STOP THEN SAVE TO KEEP SETTINGS",C_WHITE);
 }else if(band==4){const char *hint=page==0?"K3 TONE  K4 NATURE":page==2?"K3 TEMPO  K4 ENERGY":page==3?"K3 REVERB  K4 DELAY":group==0?"K3 PAN  K4 FX SEND":group==1?"K3 REVERB  K4 DELAY":group==2?"K3 432/440  K4 ENERGY":group==3?"K3 LEVEL  K4 ON/OFF":group==4?"K3 DENSITY  K4 ON/OFF":group==5?"K3 REFERENCE  K4 TONE":group==7?"K3 FX SEND  K4 SOLO":"SELECT LEAD OR BASS";cv_text(8,0,&FONT_XS,hint,C_WHITE);cv_text(8,15,&FONT_XS,"ALGO PAGE  SELECT TRACK",RGB(126,166,164));cv_text(8,30,&FONT_XS,engine.binaural_enabled?"HEADPHONES / STEREO":"EDIT GROUP  ENV 432/440",RGB(174,236,208));}}
 cv_blit(0,band*48);}dirty=0;displayed_revision=revision;}
