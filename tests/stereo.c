#include "serenity.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static sr_engine engine;

int main(void)
{
    sr_init(&engine, 731);
    for (int track = 0; track < SR_TRACKS; ++track)
        engine.tracks[track].mute = 1;
    engine.bed_level = engine.tone_level = 0;
    engine.reverb_amount = engine.delay_amount = 0;
    engine.drums_enabled = 0;
    sr_start(&engine);
    sr_set_binaural(&engine, 1);
    int16_t sample[2];
    for (int i = 0; i < SR_RATE; ++i)
        sr_render(&engine, sample, 1);
    double real[2][2] = {{0}}, imag[2][2] = {{0}};
    for (int i = 0; i < SR_RATE; ++i) {
        sr_render(&engine, sample, 1);
        for (int tone = 0; tone < 2; ++tone) {
            double phase = 6.283185307179586 * (217 + 6 * tone) * i / SR_RATE;
            for (int channel = 0; channel < 2; ++channel) {
                real[channel][tone] += sample[channel] * cos(phase);
                imag[channel][tone] += sample[channel] * sin(phase);
            }
        }
    }
    for (int channel = 0; channel < 2; ++channel) {
        double own = hypot(real[channel][channel], imag[channel][channel]);
        double other = hypot(real[channel][1-channel], imag[channel][1-channel]);
        assert(own > 100 * SR_RATE && other < own / 1000);
        printf("Channel %d: %d Hz, opposite-tone rejection %.1f dB\n",
               channel + 1, 217 + 6 * channel, 20 * log10(own / fmax(other, 1)));
    }
}
