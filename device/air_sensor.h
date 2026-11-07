/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_AIR_SENSOR_H
#define AQUORA_AIR_SENSOR_H
#include <stdint.h>

typedef struct {
    float filtered;
    unsigned reference, raw, last_signal, calibrated, near_reference;
} aq_air_sensor;

static inline unsigned aq_air_calibrate(aq_air_sensor *sensor, unsigned now)
{
    if(!sensor->filtered||now-sensor->last_signal>=800u)return 0;
    unsigned measured=(unsigned)(sensor->filtered+.5f);
    if(sensor->calibrated==1){
        unsigned change=sensor->reference>measured?sensor->reference-measured:measured-sensor->reference;
        if(change<3u)return 0;
        sensor->near_reference=measured;sensor->calibrated=2;
    }else if(sensor->calibrated>=2)return 0;
    else {sensor->reference=measured;sensor->near_reference=0;sensor->calibrated=1;}
    return 1;
}

/* A stored calibration does not imply live radio packets or proximity. */
static inline unsigned aq_air_live(const aq_air_sensor *s, unsigned now)
{
    return !!s->raw && !!s->filtered && now-s->last_signal<800u;
}
static inline unsigned aq_air_update(aq_air_sensor *sensor, unsigned raw, unsigned packets, unsigned now)
{
    if(packets&&raw){
        sensor->raw=raw;
        if(!sensor->filtered)sensor->filtered=(float)raw;
        sensor->filtered+=((float)raw-sensor->filtered)*.36f;
        sensor->last_signal=now;
    }
    if(now-sensor->last_signal>=800u){
        sensor->filtered=0;
        sensor->raw=0;
    }
    if(!sensor->calibrated||!aq_air_live(sensor,now))return 0;
    int attenuation=(int)sensor->reference-(int)(sensor->filtered+.5f);
    int span=(int)sensor->reference-(int)sensor->near_reference;
    if(sensor->calibrated==2&&span<0){span=-span;attenuation=-attenuation;}
    int target=sensor->calibrated==2&&span>2?(attenuation-2)*100/(span-2):(attenuation-2)*9;
    return target>100?100u:target>0?(unsigned)target:0;
}
/* BLE radio state is read-only here: AIR alone owns its receive-only scan. */
static inline const char *aq_air_state(const aq_air_sensor *s,unsigned enabled,
                                      unsigned radio_state,unsigned now)
{
    if(!enabled)return "RADIO OFF";
    if(radio_state==3)return "RF ERROR / RESTART AIR";
    if(radio_state!=2)return "RADIO STARTING";
    if(!aq_air_live(s,now))return s->calibrated?"SIGNAL LOST":"LISTENING / NO BROADCASTER";
    return !s->calibrated?"RECEIVING / CALIBRATE FAR":s->calibrated==1?
        "FAR SET / CALIBRATE NEAR":"ACTIVE / FAR + NEAR SET";
}
/* A one-dimensional near/far pulse, never a left/right or distance claim.
 * Require several fresh packets, deliberate residence and a full release.
 * Loss, jitter and sustained holds cannot repeatedly retrigger. */
typedef struct {unsigned stage,packets,began,cooldown;} aq_air_gesture;
static inline unsigned aq_air_gesture_update(aq_air_gesture *g,unsigned value,unsigned valid,unsigned packets,unsigned now,unsigned sensitivity)
{
 if(!valid){g->stage=g->packets=0;return 0;}
 unsigned high=100u-sensitivity/2u,low=high/3u;
 if((int32_t)(now-g->cooldown)<0)return 0;
 if(g->stage==0){if(value<=low){g->stage=1;g->packets=0;}return 0;}
 if(g->stage==1){if(value>=high){g->stage=2;g->began=now;g->packets=packets;}return 0;}
 g->packets+=packets;
 if(now-g->began>1500u){g->stage=0;return 0;}
 if(value<=low){
  unsigned trigger=g->packets>=3u&&now-g->began>=80u;
  g->stage=0;if(trigger)g->cooldown=now+1600u;
  return trigger;
 }
 return 0;
}
#endif
