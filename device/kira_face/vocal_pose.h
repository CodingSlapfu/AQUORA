/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_VOCAL_POSE_H
#define AQUORA_VOCAL_POSE_H
#include "../dsp/serenity.h"
#include "face.h"

static inline k9_face k9_recorded_vocal_pose(const sr_engine *e)
{
    k9_face pose={0};
    float peak=e->vocal_face_peak*e->master;
    if(!e->vocals_enabled||e->frequency_lock||e->nature_audition||
       e->tracks[SR_CHOIR].mute||(e->solo<SR_TRACKS&&e->solo!=SR_CHOIR)||
       (e->isolate!=SR_ISOLATE_MIX&&e->isolate!=SR_ISOLATE_TRACK1+SR_CHOIR))
        return pose;
    float lamp=e->vocal_direct_peak*e->master;
    if(lamp>.0002f){
        float strength=195.f+lamp*1200.f;
        pose.singing_light=(uint8_t)(strength<255.f?strength:255.f);
    }
    if(!e->vocal_face_mouth||peak<.0005f){
        if(e->running&&e->tick_length){
            /* One small nod per quarter note. Use the playhead and the
             * within-step clock so the motion follows tempo and transport. */
            unsigned step=(e->sequence_step+3u)&3u;
            unsigned phase=e->tick_phase*4u/e->tick_length;
            static const int8_t nod[]={0,1,2,2,2,1,1,0,0,-1,-1,-1,-1,0,0,0};
            unsigned beat=step*4u+(phase>3u?3u:phase);
            unsigned bar_step=(e->sequence_step+15u)&15u;
            unsigned accent=(bar_step&4u)==0u;
            pose.head_y=accent?nod[beat]:nod[beat]/2;
            pose.module=beat<4u?2u:1u;
            unsigned bar=(e->sequence_step+15u)>>4u;
            if(bar_step>=8u){
                int direction=(bar&1u)?1:-1;
                pose.head_x=bar_step>=10u?direction:0;
                pose.gaze=direction;
                pose.tilt=bar_step>=12u?direction:0;
            }
        }
        return pose;
    }
    float opening=peak*2048.f;
    pose.open=(uint16_t)(opening<58.f?opening:58.f);
    pose.mouth=e->vocal_face_mouth;
    /* Audible pitch motion adds expression while retaining the vowel shape. */
    int pitch_motion=e->vocal_face_pitch_ratio>1.06f?2:
                     e->vocal_face_pitch_ratio<.94f?-1:0;
    pose.jaw=(int)(pose.open/10u)+pitch_motion;
    if(pose.mouth==4u)pose.open=pose.jaw=0;
    pose.neck=pose.open/17u+(e->vocal_face_sustain&&pose.open>=16u);
    pose.brow=(e->vocal_face_midi>68?2:0)+pitch_motion;
    unsigned beat=e->tick_length?e->tick_phase*4u/e->tick_length:0u;
    pose.head_y=beat==0u?1:beat==3u?-1:0;
    pose.head_x=e->vocal_face_sustain?0:((e->sequence_step>>3u)&1u?1:-1);
    pose.tilt=e->vocal_face_sustain?(e->vocal_face_midi>68?1:-1):0;
    pose.lid=e->vocal_face_sustain&&e->vocal_face_midi<61?3u:0u;
    pose.module=e->tick_phase<e->tick_length/5u?2u:e->vocal_face_sustain?1u:0u;
    pose.gaze=e->vocal_face_sustain?0:(e->vocal_face_pitch_ratio>1.06f?1:
              e->vocal_face_pitch_ratio<.94f?-1:0);
    return pose;
}
#endif
