#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/air_sensor.h"
int main(void){
 aq_air_sensor s={0};
 assert(!strcmp(aq_air_state(&s,0,0,0),"RADIO OFF"));
 assert(strstr(aq_air_state(&s,1,1,0),"STARTING"));
 assert(strstr(aq_air_state(&s,1,3,0),"ERROR"));
 assert(strstr(aq_air_state(&s,1,2,0),"NO BROADCASTER"));
 assert(!aq_air_calibrate(&s,0));
 aq_air_update(&s,200,5,100);assert(strstr(aq_air_state(&s,1,2,100),"CALIBRATE FAR"));
 assert(aq_air_calibrate(&s,100)&&s.calibrated==1);
 assert(strstr(aq_air_state(&s,1,2,100),"CALIBRATE NEAR"));
 assert(!aq_air_calibrate(&s,100));
 for(unsigned t=140;t<=540;t+=40)aq_air_update(&s,180,8,t);
 assert(aq_air_calibrate(&s,540)&&s.calibrated==2);
 assert(aq_air_update(&s,180,8,580)>90);
 assert(strstr(aq_air_state(&s,1,2,580),"ACTIVE"));
 assert(!aq_air_update(&s,0,0,1500));
 assert(strstr(aq_air_state(&s,1,2,1500),"LOST"));
 assert(!aq_air_calibrate(&s,1500));
 for(unsigned t=1540;t<=1900;t+=40)aq_air_update(&s,200,8,t);
 assert(aq_air_update(&s,200,8,1940)==0);
 assert(!aq_air_calibrate(&s,1940)); /* Locked until explicit reset. */
 s=(aq_air_sensor){0};aq_air_update(&s,200,5,100);assert(aq_air_calibrate(&s,100));
 for(unsigned t=140;t<=540;t+=40)aq_air_update(&s,220,8,t);
 assert(aq_air_calibrate(&s,540)&&s.calibrated==2);
 assert(aq_air_update(&s,220,8,580)>90); /* Enhancement as well as shadowing. */
 puts("PASS truthful AIR radio/packet states, far/near calibration, measured range, bounded live output, signal loss and recovery");
}
