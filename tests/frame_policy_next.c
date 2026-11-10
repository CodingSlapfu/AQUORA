#include <assert.h>
#include <stdio.h>
#include "../device/frame_policy.h"
int main(void)
{
    const unsigned pressure[]={0,4399,4400,4899,4900,5300,5400,5900};
    const unsigned auto_ms[]={50,50,75,75,100,100,150,150};
    for(unsigned i=0;i<8;i++){
        assert(aq_frame_interval(0,1,0,pressure[i],0)==50);
        assert(aq_frame_interval(1,1,0,pressure[i],0)==auto_ms[i]);
        for(unsigned mode=0;mode<2;mode++){
            assert(aq_frame_interval(mode,1,0,pressure[i],1)==150);
            assert(aq_frame_interval(mode,0,0,pressure[i],1)==50);
            assert(aq_frame_interval(mode,1,1,pressure[i],1)==50);
        }
    }
    assert(aq_frame_interval(0,1,0,5300,1)==150);
    assert(aq_frame_interval(0,1,0,5300,0)==50);
    puts("PASS fixed/AUTO thresholds, measured-XRUN override and recovery");
}
