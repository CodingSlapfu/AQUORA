#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static int16_t pcm[512];

int main(void)
{
    puts("id,name,midi,peak,rms,first_quarter_rms,zero_crossings");
    for(unsigned patch=0;patch<SR_PATCHES;patch++){
        for(unsigned note=45;note<=81;note+=18){
            sr_init(&engine,710);
            engine.tracks[SR_LEAD].patch=patch;
            engine.preview_role=SR_LEAD;
            engine.preview_frames=SR_RATE*2u;
            assert(note_on(&engine,SR_LEAD,note,104,2.f));
            unsigned peak=0,crossings=0;
            int prior=0;
            double energy=0.,attack=0.;
            for(unsigned block=0;block<256;block++){
                sr_render(&engine,pcm,256);
                for(unsigned i=0;i<512;i+=2){
                    int sample=pcm[i];
                    unsigned amplitude=sample<0?-sample:sample;
                    if(amplitude>peak)peak=amplitude;
                    if((sample>0&&prior<0)||(sample<0&&prior>0))crossings++;
                    if(sample)prior=sample;
                    double square=(double)sample*sample;
                    energy+=square;
                    if(block<64)attack+=square;
                }
            }
            assert(peak>10&&peak<32767);
            printf("%u,%s,%u,%u,%.3f,%.3f,%u\n",patch,sr_patches[patch].name,
                note,peak,sqrt(energy/65536.),sqrt(attack/16384.),crossings);
        }
    }
}
