/* SPDX-License-Identifier: MPL-2.0
 * Physical-key motif capture. Fixed velocity: FM-1 digital keys do not
 * provide velocity. Timing is quantized to the current sixteenth grid. */
void sr_learn_event(sr_engine *e,unsigned key,unsigned down,uint32_t ms){
 if((!e->learn_mode&&!e->response_enabled)||key>=27||(e->running&&e->learn_mode!=2&&!e->response_enabled))return;
 if(down){
  if(e->response_enabled&&e->capture.count&&ms-e->capture_ms>e->tick_length*16000u/SR_RATE)e->capture.count=0;
  if(e->capture.count>=16)return;
  if(!e->capture.count)e->capture_ms=ms;
  unsigned n=e->capture.count++,unit=e->tick_length*1000u/SR_RATE;
  if(!unit)unit=1;
  unsigned step=(ms-e->capture_ms+unit/2u)/unit;
  if(step>63u){e->capture.count--;return;}
  e->capture.note[n]=sr_key_note(e,key);e->capture.step[n]=step;e->capture.length[n]=1;
  e->capture_raw[n]=key;e->capture_down[n]=ms;
  if(e->response_enabled){e->response_pending=1;e->response_wait=0;e->response_index=0;}
 }else for(unsigned n=0;n<e->capture.count;n++)if(e->capture_raw[n]==key&&e->capture_down[n]!=0xffffffffu){
  unsigned unit=e->tick_length*1000u/SR_RATE;if(!unit)unit=1;
  unsigned length=(ms-e->capture_down[n]+unit/2u)/unit;
  e->capture.length[n]=length<1u?1u:length>63u?63u:length;
  e->capture_down[n]=0xffffffffu;
 }
}
void sr_learn_action(sr_engine *e,unsigned action){
 if(action==0){e->capture.count=0;e->learn_pending=0;return;} /* Discard */
 if(action==2){e->learned.count=e->capture.count=e->learn_pending=0;return;} /* Clear */
 if(e->capture.count<3)return;
 if(e->running){e->learn_pending=1;return;}
 e->learned=e->capture;e->capture.count=0;e->learn_pending=0;
}
static unsigned learned_active(const sr_engine *e){return e->learned.count>=3&&e->learn_strength&&(e->learn_strength==3||e->phrase%4u<e->learn_strength);}
static int learned_index(const sr_engine *e,unsigned step){
 if(!learned_active(e))return -1;
 unsigned span=16u,last=e->learned.count-1u;
 while(span<=e->learned.step[last]&&span<64u)span*=2u;
 unsigned position=e->running&&e->sequence_step?(e->sequence_step-1u-e->learn_origin)%span:step%span;
 for(unsigned n=0;n<e->learned.count;n++)if(e->learned.step[n]==position)return (int)n;
 return -1;
}
static int learned_degree(const sr_engine *e,unsigned step,int original){
 if(!learned_active(e))return original;
 int n=learned_index(e,step);if(n<0)return -1;
 const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
 int pc=((int)e->learned.note[n]-sr_tonic(e)+120)%12,best=0,distance=99;
 for(unsigned d=0;d<7;d++){int delta=pc-scale[d];if(delta<0)delta=-delta;if(delta>6)delta=12-delta;if(delta<distance){best=d;distance=delta;}}
 return e->phrase%4u==2u?(best+2)%7:best;
}

/* Score recognizable closure, stepwise contour and a repeat-with-variation
 * grammar. Fixed four-slot memory; this is a musical heuristic, not taste AI. */
static void remember_motif(sr_engine *e){
 unsigned score=0,notes=0;int previous=-1;
 for(unsigned n=0;n<16;n++)if(e->melody[n]>=0){
   int d=e->melody[n];notes++;
   if(previous>=0){int delta=d-previous;if(delta<0)delta=-delta;if(delta<=2)score+=2;}
   previous=d;if(n>=8&&d==e->melody[n-8])score++;
 }
 if(previous==0||previous==2||previous==4)score+=4;
 if(notes<3||score<6)return;
 for(unsigned i=0;i<e->motif_count;i++){unsigned same=1;for(unsigned n=0;n<16;n++)if(e->motif_memory[i][n]!=e->melody[n])same=0;if(same)return;}
 unsigned slot=e->motif_count<4?e->motif_count++:e->motif_cursor++%4;
 memcpy(e->motif_memory[slot],e->melody,16);e->motif_scores[slot]=score;
}
static void response_tick(sr_engine *e){
 if(!e->response_enabled||!e->response_pending||e->freeze||!e->running)return;
 for(unsigned n=0;n<e->capture.count;n++)if(e->capture_down[n]!=0xffffffffu)return;
 if(e->capture.count<3)return;
 if(!e->tracks[SR_HARMONY].mute&&e->tracks[SR_HARMONY].volume>0)e->role_target[SR_HARMONY]=.65f;
 if(e->response_wait<4){e->response_wait++;return;}
 if(!e->response_index){e->response=e->capture;}
 unsigned n=e->response_index,base=e->response.step[0];
 unsigned offset=e->response_wait++-4u;
 if(n<e->response.count&&offset>=e->response.step[n]-base){
   const int8_t *scale=scale_notes[sr_scenes[e->scene].scale];
   int pc=((int)e->response.note[n]-sr_tonic(e)+120)%12,best=0,distance=99;
   for(unsigned d=0;d<7;d++){int delta=pc-scale[d];if(delta<0)delta=-delta;if(delta<distance){best=d;distance=delta;}}
   /* Answer with a third below; last note resolves to the current chord. */
   unsigned degree=n+1u==e->response.count?phrase_chord(e,e->loop_pos/16u)%7u:(best+5u)%7u;
   sr_note_on(e,SR_HARMONY,sr_tonic(e)+24+scale[degree],60,e->response.length[n]*.25f);
   e->response_index++;
   if(e->response_index>=e->response.count){
     e->response_pending=0;
     if(e->learn_mode==2&&e->learn_strength){e->learned=e->response;e->learn_origin=e->sequence_step;}
     e->capture.count=0;
   }
 }
}
