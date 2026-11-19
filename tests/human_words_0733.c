#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"
#include "../device/kira_face/vocal_pose.h"

static sr_engine engine;
static unsigned pitch_near(float hz)
{
    return (unsigned)lroundf(69.f + 12.f * log2f(hz / 440.f));
}

int main(void)
{
    static const char *const words[] = {
        "GO","NO","WAY","BE","FEEL","COLORS","GREEN","BEAUTIFUL","I","GOING","WHEN","CALL","DO","HELLO","YES"
    };
    unsigned glow_frames = 0;
    for (unsigned w = 0; w < sizeof words / sizeof *words; w++) {
        unsigned attack, transition;
        unsigned zone = vocal_articulation(12, w + 5, &attack, &transition);
        assert(zone == VOCAL_GO_ZONE + w && !attack && !transition);
        unsigned index = zone - VOCAL_DAY_ZONE;
        assert(!vocal_words[index].loop);
        for (int transpose = -2; transpose <= 2; transpose += 2) {
            sr_init(&engine, 729);
            sr_start(&engine);
            sr_set_vocals(&engine, 1, 4);
            engine.external_sync = 1;
            engine.master = .8f;
            engine.isolate = SR_ISOLATE_TRACK1 + SR_CHOIR;
            engine.vocal_fx_manual = 7;
            engine.vocal_chorus = engine.vocal_reverb = engine.vocal_delay = 0;
            unsigned root = pitch_near(vocal_words[index].hz);
            sr_voice *v = recorded_note(&engine, (int)root + transpose, zone, 100, 4.f);
            assert(v && v->midi == root + transpose && v->sample_zone == zone + 1);
            unsigned peak = 0, heard = 0, endings = 0, blocks = 0;
            float phase = 0.f;
            while (v->active && blocks++ < 512) {
                int16_t pcm[256];
                sr_render(&engine, pcm, 128);
                assert(!v->active || v->phase >= phase);
                phase = v->phase;
                heard += engine.vocal_direct_peak > .001f;
                endings += phase > vocal_words[index].release && engine.vocal_direct_peak > .001f;
                glow_frames += k9_recorded_vocal_pose(&engine).singing_light != 0;
                for (unsigned i = 0; i < 256; i++) {
                    unsigned amplitude = pcm[i] < 0 ? -(int)pcm[i] : pcm[i];
                    if (amplitude > peak) peak = amplitude;
                }
            }
            assert(!v->active && heard > 40 && endings > 2 && peak > 100 && peak < 30000);
            printf("%s %+d semitones blocks=%u coda=%u peak=%u\n",
                   words[w], transpose, heard, endings, peak);
        }
    }
    unsigned heard_words = 0;
    for (unsigned mode = 0; mode < SR_MUSIC_MODES; mode++)
        for (unsigned scene = 0; scene < SR_SCENES; scene++) {
            sr_init(&engine, scene + 17 * mode);
            sr_set_scene(&engine, scene);
            sr_set_music_mode(&engine, mode);
            for (unsigned bar = 0; bar < 4; bar++)
                for (unsigned slot = 0; slot < 3; slot++) {
                    unsigned zone = auto_word_phrase(&engine, bar, 2, slot, SR_VOCAL_NORMAL);
                    if (zone >= VOCAL_GO_ZONE) heard_words |= 1u << (zone - VOCAL_GO_ZONE);
                }
        }
    assert(heard_words == (1u << (VOCAL_LAST_ZONE - VOCAL_GO_ZONE + 1u)) - 1u && glow_frames > 100);
    puts("PASS all added words in manual bank and AUTO choices, natural one-shot endings and direct-PCM glow");
}
