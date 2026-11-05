#define _POSIX_C_SOURCE 200809L
#include "serenity.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static sr_engine e;
static int16_t pcm[512];
static unsigned durations[2000];
static unsigned long long ns(void){struct timespec t;clock_gettime(CLOCK_THREAD_CPUTIME_ID,&t);return (unsigned long long)t.tv_sec*1000000000ull+t.tv_nsec;}
static int cmp(const void*a,const void*b){unsigned x=*(const unsigned*)a,y=*(const unsigned*)b;return(x>y)-(x<y);}
int main(void){for(unsigned mode=0;mode<3;mode++){sr_init(&e,456);sr_start(&e);e.drum_density=8;e.drums_enabled=1;e.bed_type=4;e.bed_level=.7f;e.reverb_amount=.7f;e.delay_amount=.5f;e.binaural_enabled=mode!=0;for(unsigned b=0;b<400;b++)sr_render(&e,pcm,256);if(mode==2){e.binaural_enabled=0;for(unsigned b=0;b<400;b++)sr_render(&e,pcm,256);}/* Reset seed and musical time identically while retaining binaural settled state. */float env=e.binaural_env,carrier=e.carrier_now,beat=e.beat_now;sr_init(&e,456);sr_start(&e);e.drum_density=8;e.drums_enabled=1;e.bed_type=4;e.bed_level=.7f;e.reverb_amount=.7f;e.delay_amount=.5f;e.binaural_enabled=mode==1;e.binaural_env=env;e.carrier_now=carrier;e.beat_now=beat;unsigned long long total=0;unsigned voices=0;for(unsigned b=0;b<2000;b++){if(!(b%16))for(unsigned t=0;t<7;t++)sr_note_on(&e,t,44+t*3,96,1.5f);unsigned long long t=ns();sr_render(&e,pcm,256);durations[b]=(unsigned)(ns()-t);total+=durations[b];unsigned active=0;for(unsigned v=0;v<SR_VOICES;v++)active+=e.voices[v].active!=0;if(active>voices)voices=active;}qsort(durations,2000,sizeof(unsigned),cmp);printf("%s mean %.2f p95 %.2f p99 %.2f max %.2f us voices %u env %.8f\n",mode==0?"OFF":mode==1?"ON":"OFF-after-ON",total/2000000.,durations[1899]/1000.,durations[1979]/1000.,durations[1999]/1000.,voices,e.binaural_env);}}
