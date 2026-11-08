/* SPDX-License-Identifier: MPL-2.0
 * Physical-key motif capture. Fixed velocity: FM-1 digital keys do not
 * provide velocity. Timing is quantized to the current sixteenth grid. */
void sr_learn_event(sr_engine *e,unsigned key,unsigned down,uint32_t ms){
 if(!e->learn_mode||key>=27||(e->running&&e->learn_mode!=2))return;
 if(down){
  if(e->capture.count>=16)return;
  if(!e->capture.count)e->capture_ms=ms;
  unsigned n=e->capture.count++,unit=e->tick_length*1000u/SR_RATE;
  if(!unit)unit=1;
  unsigned step=(ms-e->capture_ms+unit/2u)/unit;
  if(step>63u){e->capture.count--;return;}
  e->capture.note[n]=sr_key_note(e,key);e->capture.step[n]=step;e->capture.length[n]=1;
  e->capture_raw[n]=key;e->capture_down[n]=ms;
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
