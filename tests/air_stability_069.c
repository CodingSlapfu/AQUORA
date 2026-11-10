#include <assert.h>
#include <stdio.h>
#include "../device/air_sensor.h"
static aq_air_sensor s;static aq_air_gesture g;static unsigned now,events;
static unsigned feed(unsigned raw,unsigned packets){now+=40;unsigned v=aq_air_update(&s,raw,packets,now);events+=aq_air_gesture_update(&g,v,aq_air_live(&s,now)&&s.calibrated==2,packets,now,70);return v;}
int main(void){
 for(unsigned i=0;i<24;i++)feed(100+(i%3)-1,1);assert(aq_air_calibrate(&s,now));
 for(unsigned i=0;i<24;i++)feed(97,1);assert(!aq_air_calibrate(&s,now)&&s.calibrated==1);
 for(unsigned i=0;i<32;i++)feed(70+(i%3)-1,1);assert(aq_air_calibrate(&s,now)&&s.calibrated==2);
 for(unsigned i=0;i<60;i++)feed(100,1);events=0;
 unsigned max=0;
 for(unsigned i=0;i<45000;i++){unsigned v=feed(i%997==0?40:100+(i%5)-2,1);if(v>max)max=v;}
 assert(max<=3&&events==0);printf("IDLE 30min max=%u false_gestures=%u rejected=%u\n",max,events,s.rejected);
 for(unsigned i=0;i<8;i++)feed(0,0);assert(!aq_air_live(&s,now)&&s.last_value==0);
 for(unsigned i=0;i<4;i++){feed(70,1);assert(!aq_air_live(&s,now)&&s.last_value==0);}
 for(unsigned i=0;i<40;i++)feed(100,1);events=0;
 for(unsigned i=0;i<15;i++)feed(70,1);for(unsigned i=0;i<20;i++)feed(100,1);
 assert(events==1);unsigned before=events;
 for(unsigned i=0;i<60;i++)feed(70,1);for(unsigned i=0;i<50;i++)feed(100,1);assert(events==before);
 assert(s.calibrated==2);puts("PASS weak range rejected, 30min noisy idle/outliers, dropout/reacquisition, one deliberate gesture and held-hand suppression");
}
