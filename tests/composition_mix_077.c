#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../device/dsp/engine.c"

static sr_engine score, repeat, level_on, level_off;
static int16_t on_pcm[512], off_pcm[512];

static void set_intention(sr_engine *engine, unsigned intention)
{
    const uint32_t old_seed = engine->song_seed;
    sr_set_scene(engine, intention);
    assert(engine->pending_scene == intention + 1);
    for (unsigned i = 0; engine->pending_scene && i < 4; i++)
        trigger(engine);
    assert(!engine->pending_scene);
    assert(engine->scene == intention && engine->song_seed != old_seed);
    assert(engine->form_loop == 0 && engine->loop_pos == 1 && engine->phrase == 0);
}

static void test_visits(void)
{
    for (unsigned mode = 0; mode < SR_MUSIC_MODES; mode++) {
        for (unsigned scene = 0; scene < SR_SCENES; scene++) {
            sr_init(&score, 774);
            sr_set_music_mode(&score, mode);
            sr_set_scene(&score, scene);
            sr_generate_song(&score, 1777, 0);
            unsigned bass_patch = score.tracks[SR_BASS].patch;
            score.sound_locks |= 1u << SR_BASS;
            int8_t a[16], b[16];
            memcpy(a, score.motif_home, sizeof a);
            uint32_t tempo = score.tick_length, seed = score.song_seed;
            sr_generate_song(&score, 44, 1);
            assert(score.tick_length == tempo && score.song_seed == seed);
            assert(!memcmp(a, score.motif_home, sizeof a));
            sr_start(&score);
            repeat = score;
            set_intention(&score, (scene + 1) % SR_SCENES);
            set_intention(&score, scene);
            assert(memcmp(a, score.motif_home, sizeof a));
            memcpy(b, score.motif_home, sizeof b);
            set_intention(&score, scene);
            assert(memcmp(b, score.motif_home, sizeof b));
            set_intention(&repeat, (scene + 1) % SR_SCENES);
            set_intention(&repeat, scene);
            set_intention(&repeat, scene);
            assert(repeat.song_seed == score.song_seed);
            assert(!memcmp(repeat.motif_home, score.motif_home, sizeof a));
            assert(!memcmp(repeat.form_route, score.form_route, sizeof score.form_route));
            assert(score.tracks[SR_BASS].patch == bass_patch);
            assert(score.tone_level <= SR_FIXED_TONE_MAX && score.bed_level <= SR_NATURE_MAX);
        }
    }
}

static void test_mix(void)
{
    unsigned peak = 0, difference = 0;
    for (unsigned mode = 0; mode < SR_MUSIC_MODES; mode++) {
        for (unsigned scene = 0; scene < SR_SCENES; scene++) {
            sr_init(&level_on, 871);
            sr_set_music_mode(&level_on, mode);
            sr_set_scene(&level_on, scene);
            sr_set_loop_length(&level_on, 32);
            level_on.drum_density = SR_DENSITY_MAX;
            level_on.binaural_enabled = 1;
            level_on.bed_level = SR_NATURE_MAX;
            level_on.reverb_amount = .75f;
            level_on.delay_amount = .60f;
            sr_start(&level_on);
            level_on.tick_length = 1200; /* accelerate score coverage on host */
            level_off = level_on;
            level_off.auto_level = 0;
            for (unsigned block = 0; block < 600; block++) {
                sr_render(&level_on, on_pcm, 256);
                sr_render(&level_off, off_pcm, 256);
                assert(level_on.sequence_step == level_off.sequence_step);
                for (unsigned role = 0; role < SR_TRACKS; role++) {
                    assert(level_on.tracks[role].events == level_off.tracks[role].events);
                    assert(level_on.tracks[role].last_note == level_off.tracks[role].last_note);
                    assert(level_on.role_target[role] == level_off.role_target[role]);
                    if (block > 100) {
                        float reference = role == SR_DRUMS ? .27f : sr_patches[level_on.tracks[role].patch].gain;
                        assert(isfinite(level_on.gain_trim[role]) && reference > 0.f);
                        assert(fabsf(level_off.gain_trim[role] - 1.f) < .0001f);
                    }
                }
                for (unsigned i = 0; i < 512; i++) {
                    unsigned amplitude = abs(on_pcm[i]);
                    if (amplitude > peak) peak = amplitude;
                    assert(amplitude < 32767 && abs(off_pcm[i]) < 32767);
                }
                difference |= !!memcmp(on_pcm, off_pcm, sizeof on_pcm);
            }
            float before[SR_TRACKS];
            memcpy(before, level_on.gain_trim, sizeof before);
            for (unsigned block = 0; block < 64; block++) {
                sr_render(&level_on, on_pcm, 256);
                for (unsigned role = 0; role < SR_TRACKS; role++)
                    assert(fabsf(before[role] - level_on.gain_trim[role]) < .0001f);
            }
        }
    }
    assert(difference && peak > 100);
    printf("PASS 84 mix pairs with identical note scheduling and stable trims; peak PCM %u\n", peak);
}

int main(void)
{
    test_visits();
    puts("PASS 84 intentions/modes, A-B-A and same-intention reselect, repeat determinism, locks, caps and timbre reroll");
    test_mix();
}
