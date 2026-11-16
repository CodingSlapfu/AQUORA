/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_VOCAL_POSE_H
#define AQUORA_VOCAL_POSE_H
#include "../dsp/serenity.h"
#include "face.h"

static inline k9_face k9_recorded_vocal_pose(const sr_engine *e)
{
    k9_face pose={0};
    float peak=e->vocal_face_peak*e->master;
    if(!e->vocals_enabled||!e->vocal_face_zone||e->frequency_lock||
       e->nature_audition||e->tracks[SR_CHOIR].mute||peak<.0005f||
       (e->solo<SR_TRACKS&&e->solo!=SR_CHOIR)||
       (e->isolate!=SR_ISOLATE_MIX&&e->isolate!=SR_ISOLATE_TRACK1+SR_CHOIR))
        return pose;
    float opening=peak*2048.f;
    pose.open=(uint16_t)(opening<58.f?opening:58.f);
    pose.mouth=1+(e->vocal_face_zone-1u)%3u;
    pose.jaw=pose.open/10u;
    pose.neck=pose.open/15u;
    pose.brow=e->vocal_face_midi>68?2:0;
    return pose;
}
#endif
