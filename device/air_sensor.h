/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_AIR_SENSOR_H
#define AQUORA_AIR_SENSOR_H
#include <stdint.h>

typedef struct {
    float filtered;
    unsigned reference, raw, last_signal, calibrated;
} aq_air_sensor;

static inline unsigned aq_air_calibrate(aq_air_sensor *sensor, unsigned now)
{
    if(!sensor->filtered||now-sensor->last_signal>=800u)return 0;
    sensor->reference=(unsigned)(sensor->filtered+.5f);
    sensor->calibrated=1;
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
        sensor->filtered+=((float)raw-sensor->filtered)*.24f;
        sensor->last_signal=now;
    }
    if(now-sensor->last_signal>=800u){
        sensor->filtered=0;
        sensor->raw=0;
    }
    if(!sensor->calibrated||!aq_air_live(sensor,now))return 0;
    int target=((int)sensor->reference-(int)(sensor->filtered+.5f)-2)*9;
    return target>100?100u:target>0?(unsigned)target:0;
}
#endif
