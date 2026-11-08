/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_AIR_SENSOR_H
#define AQUORA_AIR_SENSOR_H
#include <stdint.h>

/* 16 samples at the existing 40ms poll: bounded stationary calibration.
 * Source radio still supplies magnitude, not a distance or hand identity. */
typedef struct {
 float filtered,output;
 unsigned reference,raw,last_signal,calibrated,near_reference;
 unsigned noise,confidence,rejected,reacquired,window_count,window_pos;
 unsigned sample[16],recent[3],recent_pos,consistent,candidate,last_value;
 unsigned calibration_error,window_range;
} aq_air_sensor;
static inline unsigned aq_air_live(const aq_air_sensor *s,unsigned now){return s->raw&&s->confidence>=5&&now-s->last_signal<240u;}
static inline unsigned aq_air_span(const aq_air_sensor *s){return s->reference>s->near_reference?s->reference-s->near_reference:s->near_reference-s->reference;}
static inline unsigned aq_air_min_span(const aq_air_sensor *s){unsigned n=(s->noise?s->noise:3u)*4u+6u;return n<12u?12u:n;}
static inline unsigned aq_air_calibrate(aq_air_sensor *s,unsigned now){
 if(!aq_air_live(s,now)||s->window_count<16){s->calibration_error=1;return 0;}
 unsigned low=~0u,high=0,sum=0;
 for(unsigned i=0;i<16;i++){unsigned v=s->sample[i];sum+=v;if(v<low)low=v;if(v>high)high=v;}
 s->window_range=high-low;
 if(high-low>8u){s->calibration_error=2;return 0;}
 unsigned mean=(sum+8u)/16u;
 if(!s->calibrated){s->reference=mean;s->noise=(high-low+1u)/2u;if(!s->noise)s->noise=1;s->calibrated=1;s->window_count=0;}
 else if(s->calibrated==1){
  unsigned span=s->reference>mean?s->reference-mean:mean-s->reference;
  if(span<aq_air_min_span(s)){s->calibration_error=3;return 0;}
  s->near_reference=mean;s->calibrated=2;s->output=0;
 }else return 0;
 s->calibration_error=0;return 1;
}
static inline unsigned aq_air_update(aq_air_sensor *s,unsigned raw,unsigned packets,unsigned now){
 unsigned fresh=packets&&raw;
 if(now-s->last_signal>=240u){s->confidence=0;s->consistent=0;s->window_count=0;s->output=0;}
 if(fresh){
  unsigned lost=!s->confidence;
  s->raw=raw;s->last_signal=now;
  if(lost){for(unsigned i=0;i<3;i++)s->recent[i]=raw;s->filtered=raw;s->reacquired++;}
  s->recent[s->recent_pos++%3u]=raw;
  unsigned a=s->recent[0],b=s->recent[1],c=s->recent[2];
  unsigned median=a>b?(b>c?b:a>c?c:a):(a>c?a:b>c?c:b);
  int delta=(int)median-(int)(s->filtered+.5f);unsigned distance=delta<0?-delta:delta;
  unsigned threshold=(s->noise?s->noise:3u)*3u+3u;
  if(distance>threshold){
   unsigned difference=median>s->candidate?median-s->candidate:s->candidate-median;
   s->consistent=difference<=threshold?s->consistent+1u:1u;s->candidate=median;
   if(s->consistent<3u){s->rejected++;return s->last_value;}
  }else s->consistent=0;
  s->filtered+=((float)median-s->filtered)*.22f;
  s->sample[s->window_pos++%16u]=median;if(s->window_count<16u)s->window_count++;
  if(s->confidence<8u)s->confidence++;
 }
 unsigned target=0;
 if(aq_air_live(s,now)&&s->calibrated==2&&aq_air_span(s)>=aq_air_min_span(s)){
  int attenuation=(int)s->reference-(int)(s->filtered+.5f);
  if(s->near_reference>s->reference)attenuation=-attenuation;
  int dead=(int)(s->noise?s->noise:3u)*2+2;
  int span=(int)aq_air_span(s)-dead;
  int normalized=span>0?(attenuation-dead)*100/span:0;
  target=normalized<0?0u:normalized>100?100u:(unsigned)normalized;
 }
 int change=(int)target-(int)(s->output+.5f);
 if(change>3)s->output+=change>12?12:change;
 else if(change< -3)s->output+=change< -8?-8:change;
 if(!aq_air_live(s,now))s->output=0;
 s->last_value=(unsigned)(s->output+.5f);return s->last_value;
}
static inline const char *aq_air_state(const aq_air_sensor *s,unsigned enabled,unsigned radio_state,unsigned now){
 if(!enabled)return "RADIO OFF";
 if(radio_state==3)return "RF ERROR / RESTART AIR";
 if(radio_state!=2)return "RADIO STARTING";
 if(!aq_air_live(s,now))return s->raw?"RF SETTLING / FRESH PACKETS":"LISTENING / NO BROADCASTER";
 if(s->calibration_error==1)return "HOLD STILL / 16 SAMPLES";
 if(s->calibration_error==2)return "RF UNSTABLE / HOLD STILL";
 if(s->calibration_error==4)return "SOURCE CHANGED / RESET CAL";
 if(s->calibration_error==3)return "WEAK RANGE / RETRY NEAR";
 if(s->calibrated==2&&aq_air_span(s)<aq_air_min_span(s))return "WEAK CALIBRATION / RESET";
 return !s->calibrated?"RECEIVING / CALIBRATE FAR":s->calibrated==1?"FAR SET / HOLD NEAR":"ACTIVE / STABLE CONTROL";
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
