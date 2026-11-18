#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine live, forecast;

static void render_until(uint32_t frame)
{
    while(live.frame_count<frame){
        int16_t pcm[256];
        unsigned frames=frame-live.frame_count;
        sr_render(&live,pcm,frames>128u?128u:frames);
    }
}

int main(void)
{
    for(unsigned length=16;length<=64;length*=2){
        for(unsigned time=0;time<3;time++){
            sr_init(&live,734u);
            sr_set_loop_length(&live,length);
            live.drum_time=time;
            live.drum_hit=100;
            live.timeline_enabled=1;
            live.isolate=SR_ISOLATE_TRACK1+SR_DRUMS;
            sr_start(&live);
            const unsigned interval=time==0?live.tick_length:
                time==1?live.tick_length*2u:live.tick_length/2u;
            for(unsigned step=0;step<length+2u;step++){
                unsigned song_tick=time==2?step/2u:time==1?step*2u:step;
                uint32_t frame=song_tick*live.tick_length+(time==2&&(step&1u)?interval:0u);
                render_until(frame+1u);
                assert(live.drum_playhead==(step&(length-1u)));
                if(!(step&15u))assert(live.drum_steps[live.drum_playhead]&1u);
                if(step==1u){
                    sr_score_snapshot(&forecast,&live);
                    sr_event future[128];
                    unsigned count=sr_score_preview(&forecast,future,128u,4);
                    assert(count>0);
                    unsigned forecast_position=forecast.drum_playhead;
                    unsigned beat=forecast.drum_steps[forecast_position];
                    render_until(forecast.frame_count+1u);
                    assert(live.drum_playhead==forecast_position);
                    assert(live.drum_steps[forecast_position]==beat);
                    step=time==2?8u:time==1?3u:5u;
                }
            }
            printf("%u steps, time %u, one pattern and wrap verified\n",length,time);
        }
    }
}
