#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/dsp/engine.c"
static sr_engine e,unchanged;
static sr_event plan[768],heard[768];
static int16_t pcm[512];
int main(void){
 unsigned cases=0,events=0,max=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned scene=0;scene<21;scene++){
  sr_init(&e,1771+scene);sr_set_scene(&e,scene);sr_set_music_mode(&e,mode);sr_generate_song(&e,1251+scene,0);sr_start(&e);e.timeline_enabled=1;
  unchanged=e;unsigned count=sr_score_preview(&e,plan,768,34);assert(!memcmp(&e,&unchanged,sizeof e));assert(count&&count<768);if(count>max)max=count;
  unsigned total=0;
  while(e.sequence_step<34){unsigned before=e.timeline_write;sr_render(&e,pcm,1);total=preview_collect(&e,heard,768,total,before);}
  unsigned expected=0;
  for(unsigned i=0;i<count;i++)if(plan[i].frame<e.frame_count){
   unsigned match=0;for(unsigned j=0;j<total;j++)if(heard[j].frame==plan[i].frame&&heard[j].role==plan[i].role&&heard[j].note==plan[i].note&&heard[j].duration==plan[i].duration){match=1;break;}
   if(!match){fprintf(stderr,"missing m%u s%u role%u note%u frame%u len%u count%u heard%u\n",mode,scene,plan[i].role,plan[i].note,plan[i].frame,plan[i].duration,count,total);return 1;}expected++;
  }
  assert(expected==total);events+=total;cases++;
 }
 printf("PASS %u real score/audio comparisons, %u exact timestamp/pitch/duration matches, maximum %u future events; source unchanged\n",cases,events,max);
}
