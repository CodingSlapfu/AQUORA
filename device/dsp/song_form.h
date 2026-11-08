/* SPDX-License-Identifier: MPL-2.0
 * Original bounded multi-loop arranger. Each section has an orchestration,
 * harmonic path and rhythmic character, not merely a display label. */
typedef struct {const char *name;uint8_t family,roles,harmony,pulse;} sr_section_spec;
static const sr_section_spec sr_sections[16]={
 {"Opening",0,0x46,0,0},{"Emerging Pulse",1,0xa6,0,1},
 {"Theme A",1,0xb3,0,1},{"Answer",1,0xba,2,1},
 {"Counterpoint",2,0xbb,4,2},{"Harmonic Departure",2,0x6e,5,1},
 {"Textural Expansion",2,0x66,2,0},{"Broken Groove",2,0xba,4,2},
 {"Suspension",3,0x46,3,0},{"Bridge",1,0xb6,5,1},
 {"Spacious Breakdown",3,0x4a,2,0},{"Rebuild",1,0xae,4,1},
 {"Bloom",2,0xf7,5,2},{"Theme B",1,0xeb,3,1},
 {"Reprise",4,0xb3,0,1},{"Resolution",4,0x56,0,0}
};
static unsigned form_index(const sr_engine *e,unsigned ahead){
 static const uint8_t forms[SR_MUSIC_MODES][16]={
  {0,1,2,3,4,9,2,5,6,11,12,13,7,10,14,15},
  {0,6,1,2,3,5,9,12,8,11,13,4,12,10,14,15},
  {0,2,3,4,9,2,5,13,8,11,4,12,3,10,14,15},
  {0,6,1,2,7,3,5,8,9,13,4,10,11,12,14,15}
 };
 unsigned hold=2u+sr_scenes[e->scene].energy%3u;
 unsigned cycle=e->form_loop/(hold*16u);
 unsigned slot=(e->form_loop/hold+ahead)%16u;
 /* Later cycles revisit home while varying the middle route deterministically. */
 if(cycle&&slot>3u&&slot<13u)slot=4u+(slot-4u+cycle*3u)%9u;
 return forms[e->music_mode][slot];
}
