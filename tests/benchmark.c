#define _POSIX_C_SOURCE 200809L
#include "serenity.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCKS 3500
static sr_engine engine;
static int16_t pcm[512];
static unsigned times[BLOCKS];

static unsigned long long ns(void)
{
    struct timespec t;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,&t);
    return (unsigned long long)t.tv_sec*1000000000ull+t.tv_nsec;
}

static int compare(const void *a,const void *b)
{
    unsigned x=*(const unsigned *)a,y=*(const unsigned *)b;
    return (x>y)-(x<y);
}

int main(void)
{
    const char *names[]={"quiet","pitched dry","all + wet"};
    for(unsigned profile=0;profile<3;profile++){
        sr_init(&engine,456);
        engine.bed_level=profile==2?.9f:0;
        engine.reverb_amount=profile==2?.8f:0;
        engine.delay_amount=profile==2?.7f:0;
        engine.drums_enabled=profile==2;
        engine.drum_density=3;
        engine.binaural_enabled=profile==2;
        unsigned long long elapsed=0;
        double power=0;
        unsigned peak=0;
        if(profile)sr_start(&engine);
        for(unsigned block=0;block<BLOCKS;block++){
            if(profile&&block%16==0)
                for(unsigned track=0;track<7;track++)
                    sr_note_on(&engine,track,44+track*3,96,1.5f);
            unsigned long long begin=ns();
            sr_render(&engine,pcm,256);
            times[block]=(unsigned)(ns()-begin);
            elapsed+=times[block];
            for(unsigned i=0;i<512;i++){
                unsigned magnitude=(unsigned)abs(pcm[i]);
                if(magnitude>peak)peak=magnitude;
                power+=(double)pcm[i]*pcm[i];
            }
        }
        qsort(times,BLOCKS,sizeof(*times),compare);
        printf("%s: mean %.1f us p95 %.1f us peak %.1f us, PCM peak %u RMS %.0f\n",
            names[profile],elapsed/(1000.0*BLOCKS),times[BLOCKS*95/100-1]/1000.0,
            times[BLOCKS-1]/1000.0,peak,sqrt(power/(512.0*BLOCKS)));
    }
}
