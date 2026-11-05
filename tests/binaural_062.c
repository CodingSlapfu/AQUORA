/* AQUORA v0.6.2: headphone stereo offset, fade, isolation, and tuning. */
#define _POSIX_C_SOURCE 200809L
#include "serenity.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static sr_engine e;
static int16_t pcm[512];
static const double pi2=6.28318530717958647692;
static void isolated(float carrier, float difference)
{
    sr_init(&e,731);
    for(unsigned role=0;role<SR_TRACKS;role++)e.tracks[role].mute=1;
    e.isolate=SR_ISOLATE_BINAURAL;
    e.bed_type=0;e.bed_level=0;e.tone_level=0;
    e.reverb_amount=e.delay_amount=0;e.drums_enabled=0;
    e.binaural_level=.65f;
    sr_start(&e);
    sr_set_binaural_carrier(&e,carrier);
    sr_set_binaural_rate(&e,difference);
    sr_set_binaural(&e,1);
}
static void render_seconds(unsigned seconds)
{
    unsigned frames=SR_RATE*seconds;
    while(frames){unsigned n=frames>256?256:frames;sr_render(&e,pcm,n);frames-=n;}
}
static void stereo_spectrum(float carrier,float beat)
{
    isolated(carrier,beat);
    render_seconds(2);
    double real[2][2]={{0}},imag[2][2]={{0}};
    unsigned frames=0;
    for(unsigned b=0;b<173;b++){
        unsigned n=256;
        if(frames+n>SR_RATE)n=SR_RATE-frames;
        sr_render(&e,pcm,n);
        for(unsigned i=0;i<n;i++){
            for(int tone=0;tone<2;tone++){
                double hz=carrier+(tone?+.5:-.5)*beat;
                double theta=pi2*hz*(frames+i)/SR_RATE;
                double co=cos(theta),si=sin(theta);
                for(int channel=0;channel<2;channel++){
                    real[channel][tone]+=pcm[2*i+channel]*co;
                    imag[channel][tone]+=pcm[2*i+channel]*si;
                }
            }
        }
        frames+=n;
        if(frames==SR_RATE)break;
    }
    /* Append remaining samples when 173*256=44288 (last is a short chunk). */
    assert(frames==SR_RATE);
    for(int ch=0;ch<2;ch++){
      double own=hypot(real[ch][ch],imag[ch][ch]);
      double cross=hypot(real[ch][1-ch],imag[ch][1-ch]);
      assert(own>1e6 && cross<own*.014);
      printf("%g/%g Hz channel %d rejection %.1f dB\n",carrier,beat,ch,
             20*log10(own/fmax(cross,1.0)));
    }
}
static void toggle_and_safety(void)
{
    isolated(220.f,6.f);
    for(unsigned block=0;block<100;block++)sr_render(&e,pcm,256);
    sr_set_binaural(&e,0);
    render_seconds(1);
    for(unsigned i=0;i<512;i++)assert(abs(pcm[i])<10);
    assert(e.binaural_env<.001f);
    sr_set_binaural(&e,1);
    sr_set_binaural_carrier(&e,1000.f);
    sr_set_binaural_rate(&e,40.f);
    unsigned max_jump=0;
    int previous=0;
    for(unsigned block=0;block<360;block++){
        sr_render(&e,pcm,256);
        for(unsigned i=0;i<256;i++){
            unsigned jump=abs(pcm[2*i]-previous);
            if(jump>max_jump)max_jump=jump;
            previous=pcm[2*i];
        }
    }
    /* A 1020-Hz sine at this level easily stays below this threshold;
       a discontinuous phase reset or dry/wet blast would violate it. */
    assert(max_jump<5000);
    sr_set_binaural(&e,0);
    render_seconds(1);
    for(unsigned i=0;i<512;i++)assert(abs(pcm[i])<10);
    e.frequency_lock=1;
    e.binaural_enabled=1;
    render_seconds(1);
    for(unsigned i=0;i<512;i++)assert(abs(pcm[i])<10);
    printf("PASS OFF/ON/OFF fades, 1000/40 sweep, frequency lock, peak step %u\n",max_jump);
}
int main(void)
{
    stereo_spectrum(220.f,6.f);
    stereo_spectrum(432.f,8.f);
    stereo_spectrum(980.f,40.f);
    toggle_and_safety();
    puts("PASS binaural stereo and routing");
}
