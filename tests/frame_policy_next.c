#include <assert.h>
#include <stdio.h>
#include "../device/frame_policy.h"
int main(void)
{
    const unsigned pressure[]={0,4399,4400,4899,4900,5300,5400,5900};
    const unsigned target_ms[]={50,50,75,75,100,100,150,150};
    for(unsigned i=0;i<8;i++){
        assert(aq_frame_interval(1,0,pressure[i],0)==target_ms[i]);
        assert(aq_frame_interval(1,0,pressure[i],1)==150);
        assert(aq_frame_interval(0,0,pressure[i],1)==50);
        assert(aq_frame_interval(1,1,pressure[i],1)==50);
    }
    assert(aq_frame_interval(1,0,0,0)==50);
    for(unsigned visual=0;visual<3;visual++){
        for(unsigned edge=1;edge<=600;edge++)
            assert(!aq_frame_invalid(1,1,visual,visual,0,0,edge,0));
        assert(aq_frame_invalid(1,1,visual,(visual+1)%3,0,0,1,0));
        assert(aq_frame_invalid(1,0,visual,visual,0,0,1,0));
        assert(aq_frame_invalid(1,1,visual,visual,0,1,1,0));
    }
    assert(aq_frame_invalid(0,0,0,0,0,0,1,0));
    assert(!aq_frame_invalid(0,0,0,0,0,0,1,1));
    puts("PASS 20 FPS target, audio pressure and XRUN recovery");
}
