#include <assert.h>
#include "../device/kira_face/vocal_pose.h"

static sr_engine engine;
static uint16_t silent[240*240], singing[240*240], next[240*240];

int main(void)
{
    engine.master=1.f;
    engine.vocals_enabled=1;
    engine.solo=SR_TRACKS;
    engine.vocal_face_peak=.1f;
    k9_face pose=k9_recorded_vocal_pose(&engine);
    assert(pose.singing_light==0);
    k9_face_render(silent,0,240,&pose);

    engine.vocal_direct_peak=.1f;
    pose=k9_recorded_vocal_pose(&engine);
    assert(pose.singing_light>0);
    k9_face_render(singing,0,240,&pose);
    pose.module=1;
    k9_face_render(next,0,240,&pose);
    unsigned light_pixels=0, pulsed_pixels=0;
    for(unsigned y=0;y<240;y++){
        for(unsigned x=0;x<240;x++){
            unsigned i=y*240+x;
            if(silent[i]==singing[i]&&singing[i]==next[i])continue;
            assert((x>=50&&x<=60&&y>=98&&y<=117)||
                   (x>=178&&x<=188&&y>=95&&y<=113)||
                   (x>=110&&x<=132&&y>=206&&y<=217));
            light_pixels+=silent[i]!=singing[i];
            pulsed_pixels+=singing[i]!=next[i];
        }
    }
    assert(light_pixels>100&&pulsed_pixels>20);
    engine.tracks[SR_CHOIR].mute=1;
    assert(k9_recorded_vocal_pose(&engine).singing_light==0);
}
