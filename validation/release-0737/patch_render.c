#include <stdio.h>
#include "engine.c"

static sr_engine engine;
int main(void)
{
    puts("patch,midi,peak,rms,first_40ms_rms,tail_rms,crossings,clipped");
    for(unsigned patch=0;patch<SR_PATCHES;patch++){
        for(unsigned midi=48;midi<=84;midi+=18){
            sr_init(&engine,37u);
            engine.master=.8f;
            engine.external_sync=1;
            engine.isolate=SR_ISOLATE_TRACK1+SR_LEAD;
            engine.reverb_amount=engine.delay_amount=0.f;
            engine.tracks[SR_LEAD].patch=patch;
            sr_start(&engine);
            sr_note_on(&engine,SR_LEAD,midi,96,1.f);
            double energy=0., onset=0., tail=0.;
            unsigned peak=0,crossings=0,clipped=0;
            int prior=0;
            for(unsigned block=0;block<128;block++){
                int16_t pcm[512];
                sr_render(&engine,pcm,256);
                for(unsigned i=0;i<512;i+=2){
                    int level=pcm[i];
                    unsigned mag=(unsigned)(level<0?-level:level);
                    if(mag>peak)peak=mag;
                    clipped+=(level==32767||level==-32768);
                    crossings+=((prior<0)!=(level<0));
                    prior=level;
                    energy+=(double)level*level;
                    if(block<7)onset+=(double)level*level;
                    if(block>=96)tail+=(double)level*level;
                }
            }
            if(!peak||clipped)return 1;
            printf("%u,%u,%u,%.1f,%.1f,%.1f,%u,%u\n",patch,midi,peak,
                   sqrt(energy/32768.),sqrt(onset/1792.),sqrt(tail/8192.),crossings,clipped);
        }
    }
}
