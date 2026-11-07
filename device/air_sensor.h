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
#endif
