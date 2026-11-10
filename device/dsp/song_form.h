/* SPDX-License-Identifier: MPL-2.0
 * Continuous mode-specific journeys. Functions differ in orchestration,
 * harmony and percussion grammar; the macro form never stops transport. */
typedef struct {const char *name;uint8_t family,roles,harmony,pulse;} sr_section_spec;
static const sr_section_spec sr_sections[32]={
 {"Opening",0,0x46,0,0},{"Emerging Pulse",1,0xa6,0,1},
 {"Theme A",1,0xb3,0,1},{"Answer",1,0xba,2,1},
 {"Counterpoint",2,0xbb,4,2},{"Harmonic Departure",2,0x6e,5,1},
 {"Textural Expansion",2,0x66,2,0},{"Broken Groove",2,0xba,4,2},
 {"Suspension",3,0x46,3,0},{"Bridge",1,0xb6,5,1},
 {"Spacious Breakdown",3,0x4a,2,0},{"Rebuild",1,0xae,4,1},
 {"Bloom",2,0xf7,5,2},{"Theme B",1,0xeb,3,1},
 {"Reprise",4,0xb3,0,1},{"Resolution",4,0x56,0,0},
 {"Bass Drift",1,0xa2,0,1},{"Piano Dialogue",1,0x92,2,1},
 {"Harp Interlock",2,0xba,4,2},{"Choir Horizon",3,0x46,5,0},
 {"Percussion Rise",2,0xa6,0,2},{"Held Breath",3,0x4a,3,0},
 {"Harmonic Lift",2,0xf6,4,1},{"Afterglow",4,0x56,2,0},
 {"Displaced Pulse",1,0xaa,0,2},{"Glass Response",1,0x9a,5,1},
 {"Second Crest",2,0xfb,2,2},{"Tonal Sanctuary",3,0x66,0,0},
 {"Motif Homecoming",4,0xb3,0,1},{"Quiet Interplay",1,0x9e,3,1},
 {"Rhythmic Weave",2,0xba,4,2},{"Open Continuation",4,0x96,0,1}
};
/* Original bounded lookahead planner: all decisions are made before playback
 * of a 32-section arc. No audio random stream or per-sample search is used.
 * Each arc resolves and returns to the motif, then starts another journey.
 * Planning inputs are persisted song identity/home motif and intention/mode;
 * transient learning/energy edits cannot change a restored committed route. */
void sr_plan_sections(sr_engine *e){
 unsigned hold=2u+sr_scenes[e->scene].energy%3u;
 unsigned cycle=e->form_loop/(hold*32u);
 uint32_t key=song_draw(e->song_seed,cycle+701u+e->scene*7u+e->music_mode*149u);
 if(e->form_key==key&&e->form_route[32]==28u)return;
 e->form_key=key;
 unsigned used[32]={0};
 e->form_route[0]=cycle?28u:0u;used[e->form_route[0]]=1;
 for(unsigned slot=1;slot<32;slot++){
  unsigned prev=e->form_route[slot-1u],family=sr_sections[prev].family;
  unsigned phase=slot%8u,energy=sr_scenes[e->scene].energy;
  unsigned target=phase<3u?1u:phase<6u?2u:phase==6u?3u:4u;
  if(energy==1u&&target==2u&&e->music_mode!=SR_EXPERIMENTAL)target=1u;
  unsigned weights[32],total=0;
  for(unsigned id=1;id<32;id++){
   const sr_section_spec *a=&sr_sections[id];
   unsigned w=a->family==target?20u:3u;
   /* A reduction needs a prepared re-entry; a crest needs release. */
   if(family==3u)w=a->family==1u||a->family==4u?24u:1u;
   if(family==2u&&phase>=5u)w=a->family==3u||a->family==4u?24u:w;
   unsigned delta=(a->harmony+7u-sr_sections[prev].harmony)%7u;
   if(delta==0u||delta==3u||delta==4u)w*=2u;
   if(e->music_mode==SR_NEOCLASSICAL&&(a->roles&(1u<<SR_PIANO)))w*=2u;
   if(e->music_mode==SR_EXPERIMENTAL&&a->pulse==2u)w*=2u;
   if(energy>=3u&&a->pulse==2u)w*=2u;
   /* Retain a useful common instrumental voice over the harmonic move. */
   if((a->roles&sr_sections[prev].roles)&(1u<<SR_PIANO))w+=3u;
   if(e->motif_home[0]>=0&&a->harmony==(unsigned)e->motif_home[0]%7u)w+=3u;
   /* Prepared rise promises a crest, held breath promises a theme. These
    * are musical contracts, not weighted chances to abandon the build. */
   if(prev==20u&&id!=12u&&id!=26u)w=0;
   if(prev==21u&&id!=28u&&id!=2u&&id!=12u)w=0;
   if(family==3u&&a->family==3u)w=0;
   /* Score the cadence of this song's actual stored theme. Its final
    * voiced scale degree biases the destination without re-rolling notes. */
   int cadence=0;
   for(unsigned n=0;n<16;n++)if(e->motif_home[n]>=0)cadence=e->motif_home[n];
   unsigned distance=(a->harmony+7u-(unsigned)cadence)%7u;
   if(distance==0u||distance==3u||distance==4u)w*=2u;
   unsigned penalty=1u+used[id]*3u;
   w=(w+penalty-1u)/penalty;
   if(id==prev||(slot>1u&&id==e->form_route[slot-2u]))w=0;
   if(slot==30u&&(id==15u||id==20u||id==21u))w=0;
   if(slot==31u)w=id==15u?1u:0u;
   weights[id]=w;total+=w;
  }
  unsigned draw=song_draw(key,slot)%total,id=1;
  for(;id<31;id++){if(draw<weights[id])break;draw-=weights[id];}
  e->form_route[slot]=id;used[id]++;
 }
 e->form_route[32]=28u; /* committed next arc starts with homecoming */
}
static unsigned form_index(const sr_engine *e,unsigned ahead){
 unsigned hold=2u+sr_scenes[e->scene].energy%3u;
 unsigned slot=(e->form_loop/hold)%32u;
 return e->form_route[slot+!!ahead];
}
