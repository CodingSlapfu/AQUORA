#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"
static sr_engine e,unchanged,scratch;
static sr_event plan[768],heard[768];
static int16_t pcm[512];
int main(void){
 /* A burst spanning the ring end must preserve all 96 most recent events. */
 for(unsigned i=0;i<320;i++) e.timeline[timeline_slot(&e)].frame=i;
 for(unsigned i=0;i<96;i++) assert(e.timeline[(e.timeline_write+i)%96].frame==224+i);
 unsigned cases=0,events=0,max=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++){
  sr_init(&e,1771+scene);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,1251+scene,0);sr_start(&e);e.timeline_enabled=1;
  unchanged=e;sr_score_snapshot(&scratch,&e);unsigned count=sr_score_preview(&scratch,plan,768,34);assert(!memcmp(&e,&unchanged,sizeof e));assert(count&&count<768);if(count>max)max=count;
  unsigned total=0;
  while(e.sequence_step<34){unsigned before=e.timeline_write;sr_render(&e,pcm,64);total=preview_collect(&e,heard,768,total,before);}
  unsigned expected=0;
  for(unsigned i=0;i<count;i++)if(plan[i].frame<e.frame_count){
   unsigned match=0;for(unsigned j=0;j<total;j++)if(heard[j].frame==plan[i].frame&&heard[j].role==plan[i].role&&heard[j].note==plan[i].note&&heard[j].duration==plan[i].duration){match=1;break;}
   if(!match){fprintf(stderr,"missing m%u s%u role%u note%u frame%u len%u count%u heard%u\n",mode,scene,plan[i].role,plan[i].note,plan[i].frame,plan[i].duration,count,total);return 1;}expected++;
  }
  if(expected!=total){fprintf(stderr,"count m%u s%u expected%u total%u frames%u\n",mode,scene,expected,total,e.frame_count);for(unsigned j=0;j<total;j++){unsigned match=0;for(unsigned i=0;i<count;i++)if(heard[j].frame==plan[i].frame&&heard[j].role==plan[i].role&&heard[j].note==plan[i].note)match=1;if(!match)fprintf(stderr,"extra role%u note%u frame%u\n",heard[j].role,heard[j].note,heard[j].frame);}return 1;}events+=total;cases++;
 }
 printf("PASS %u real score/audio comparisons, %u exact timestamp/pitch/duration matches, maximum %u future events; source unchanged\n",cases,events,max);
}
