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
        float strength=36.f+lamp*1550.f;
        pose.singing_light=(uint8_t)(strength<255.f?strength:255.f);
    }
    if(!e->vocal_face_mouth||peak<.0005f)return pose;
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
    /* Close the eyes on sustained highlights. The frame compositor eases
     * into and out of the captured lid layers, leaving head position stable. */
    unsigned step=(e->sequence_step+15u)&15u;
    if(e->vocal_face_sustain&&step>=4u&&step<=14u&&
       (e->current_energy>.52f||e->music_mode==SR_CINEMATIC||e->vocal_face_midi>=68u)){
        pose.lid=3u;
        pose.brow=1;
    }
    return pose;
}
#endif
