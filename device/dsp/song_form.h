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
static unsigned form_index(const sr_engine *e,unsigned ahead){
 static const uint8_t forms[SR_MUSIC_MODES][32]={
  {0,1,2,3,17,4,9,18,5,6,20,11,12,23,16,24,13,25,7,22,26,21,10,27,29,30,14,19,28,15,31,8},
  {0,6,19,1,2,3,17,5,9,20,11,22,12,23,16,8,21,13,25,4,18,26,27,10,29,24,14,28,15,30,31,7},
  {0,17,2,3,4,25,9,18,5,13,29,22,12,23,16,8,21,11,1,6,26,20,7,10,27,24,14,28,19,15,30,31},
  {0,6,1,24,7,18,3,5,8,9,25,13,4,20,30,10,27,11,22,12,26,21,16,29,23,17,14,19,28,15,31,2}
 };
 unsigned hold=2u+sr_scenes[e->scene].energy%3u;
 unsigned cycle=e->form_loop/(hold*32u);
 unsigned slot=(e->form_loop/hold+ahead)%32u;
 if(cycle&&slot>3u&&slot<28u)slot=4u+(slot-4u+cycle*5u)%24u;
 return forms[e->music_mode][slot];
}
