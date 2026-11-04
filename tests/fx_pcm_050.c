#include "serenity.h"
#include <stdio.h>
#include <time.h>

static sr_engine engine;
static int16_t pcm[512];

int main(void)
{
    for(unsigned mode=0;mode<4;mode++){
        sr_init(&engine,174);
        engine.drums_enabled=0;
        engine.bed_level=0;
        engine.reverb_amount=mode&1?.8f:0;
        engine.delay_amount=mode&2?.7f:0;
        sr_start(&engine);
        uint32_t hash=2166136261u;
        clock_t start=clock();
        for(unsigned block=0;block<3000;block++){
            if(block%50==0)
                for(unsigned i=0;i<16;i++)sr_note_on(&engine,i%7,45+i,90,3.f);
            sr_render(&engine,pcm,256);
            for(unsigned j=0;j<512;j++)hash=(hash^(uint16_t)pcm[j])*16777619u;
        }
        printf("%u %08x\n",mode,hash);
        fprintf(stderr,"mode %u: %.3f us/256 frames, including PCM checksum\n",mode,
            (double)(clock()-start)*1000000/CLOCKS_PER_SEC/3000);
    }
}
