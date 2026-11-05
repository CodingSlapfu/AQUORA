/* SPDX-License-Identifier: MPL-2.0 */
#include "../device/air_sensor.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    aq_air_sensor sensor={0};
    assert(!aq_air_live(&sensor,0));
    assert(!aq_air_calibrate(&sensor,0));
    assert(!aq_air_update(&sensor,200,0,80));
    assert(!aq_air_live(&sensor,80));
    assert(!aq_air_update(&sensor,200,5,100));
    assert(aq_air_live(&sensor,100));
    assert(aq_air_calibrate(&sensor,100));
    assert(sensor.calibrated);
    for(unsigned now=180;now<700;now+=80){
        aq_air_update(&sensor,180,7,now);
        assert(aq_air_live(&sensor,now));
    }
    assert(!aq_air_live(&sensor,1500));
    assert(!aq_air_update(&sensor,0,0,1500));
    assert(sensor.calibrated); /* Calibration remains stored, LIVE does not. */
    assert(!aq_air_calibrate(&sensor,1500));
    assert(!aq_air_live(&sensor,1500));
    assert(!aq_air_update(&sensor,200,5,1560));
    assert(aq_air_live(&sensor,1560));
    puts("PASS calibrated/lost/recovered/live AIR states");
    return 0;
}
