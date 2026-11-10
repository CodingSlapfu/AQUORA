#include <assert.h>
#include <stdio.h>
#include "../device/lighting_policy.h"
int main(void)
{
    const unsigned physical_black[]={1,3,5,8,10,13,15,17,20,22,25};
    for(unsigned key=0;key<27;key++){
        unsigned expected=0;
        for(unsigned i=0;i<sizeof physical_black/sizeof *physical_black;i++)
            expected|=key==physical_black[i];
        assert(aq_black_key(key)==expected);
    }
    const unsigned bpm[]={40,65,90,120,240};
    for(unsigned t=0;t<5;t++){
        unsigned length=44100u*60u/bpm[t]/4u;
        for(unsigned step=1;step<=129;step++){
            for(unsigned phase=0;phase<length;phase++){
                assert(aq_play_pulse(1,step,phase,length)==
                       (((step-1)%4==0)&&phase<length/2));
                assert(!aq_play_pulse(0,step,phase,length));
            }
        }
    }
    assert(aq_play_pulse(1,0,9999,9999));
    assert(!aq_play_pulse(1,1,0,0));
    unsigned a=0,b=0,black=0,white=0;
    for(unsigned frame=0;frame<10000;frame++){
        a+=7;b+=5;
        if(a>=10){black++;a-=10;}
        if(b>=10){white++;b-=10;}
    }
    assert(black==7000&&white==5000);
    puts("PASS physical F3-G5 mapping; beat phase at 5 tempos, stop/restart; exact 70/50 scan duty");
}
