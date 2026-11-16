#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static sr_event events[512];
static int16_t pcm[512];

static uint32_t mix_hash(uint32_t hash, uint32_t value)
{
    return (hash ^ value) * 16777619u;
}

static void check_score(unsigned mode, unsigned scene, unsigned seed)
{
    sr_init(&engine, 41);
    sr_set_music_mode(&engine, mode);
    sr_set_scene(&engine, scene);
    sr_generate_song(&engine, seed, 0);
    sr_set_loop_length(&engine, 32);
    sr_start(&engine);
    unsigned notes[SR_TRACKS] = {0};
    unsigned sections = 0;
    uint32_t lead = 2166136261u, pluck = lead, bass = lead;
    const unsigned loops = 32u * (2u + sr_scenes[scene].energy % 3u);
    for (unsigned loop = 0; loop < loops; loop++) {
        sections |= 1u << song_section(&engine);
        unsigned count = sr_score_preview(&engine, events, 512, 32);
        assert(count > 0 && count < 512);
        for (unsigned i = 0; i < count; i++) {
            const sr_event event = events[i];
            assert(event.role < SR_TRACKS && event.note < 128 && event.velocity > 0);
            assert(event.duration > 0);
            notes[event.role]++;
            if (event.role == SR_DRONE)
                assert(event.note >= 55);
            uint32_t *hash = event.role == SR_LEAD ? &lead :
                             event.role == SR_GUITAR ? &pluck :
                             event.role == SR_BASS ? &bass : NULL;
            if (hash) {
                *hash = mix_hash(*hash, event.note | (event.velocity << 8u));
                *hash = mix_hash(*hash, event.frame);
                *hash = mix_hash(*hash, event.duration);
            }
        }
    }
    assert((sections & 17u) == 17u && (sections & 14u));
    assert(form_index(&engine, 0) == 15u);
    for (unsigned role = 0; role < SR_TRACKS; role++)
        assert(notes[role] > 0);
    assert(engine.scheduled_overflows == 0);
    printf("%u %u %u %08x %08x %08x %u %u %u\n", mode, scene, seed,
           lead, pluck, bass, notes[SR_DRONE], notes[SR_PIANO], notes[SR_CHOIR]);
}

static void check_isolation(unsigned mode, unsigned role)
{
    sr_init(&engine, 2476);
    sr_set_music_mode(&engine, mode);
    sr_generate_song(&engine, 987, 0);
    sr_set_loop_length(&engine, 32);
    sr_start(&engine);
    engine.form_route[0] = 12u; /* exercise all voices in a Bloom passage */
    plan_roles(&engine);
    engine.isolate = SR_ISOLATE_TRACK1 + role;
    engine.tick_length = 5512; /* fastest supported native sixteenth-note clock */
    unsigned peak = 0;
    for (unsigned block = 0; block < 6200; block++) {
        sr_render(&engine, pcm, 256);
        for (unsigned i = 0; i < 512; i++) {
            unsigned amplitude = abs(pcm[i]);
            if (amplitude > peak) peak = amplitude;
            assert(amplitude < 32767);
        }
    }
    if (peak <= 20 || !engine.tracks[role].events || engine.scheduled_overflows)
        fprintf(stderr, "Stem mode=%u role=%u peak=%u events=%u queue overflows=%u\n",
                mode, role, peak, engine.tracks[role].events, engine.scheduled_overflows);
    assert(peak > 20 && engine.tracks[role].events > 0 && !engine.scheduled_overflows);
}

int main(void)
{
    for (unsigned mode = 0; mode < SR_MUSIC_MODES; mode++) {
        for (unsigned scene = 0; scene < SR_SCENES; scene++)
            for (unsigned seed = 1; seed <= 3; seed++)
                check_score(mode, scene, 2387u * seed);
        for (unsigned role = 0; role < SR_TRACKS; role++)
            check_isolation(mode, role);
    }
    sr_init(&engine, 14);
    engine.expression_manual = SR_MANUAL_ENERGY | SR_MANUAL_VARIATION;
    engine.energy = .73f;
    engine.variation = 0.f;
    sr_generate_song(&engine, 876, 0);
    sr_set_scene(&engine, 8);
    sr_set_music_mode(&engine, SR_CINEMATIC);
    assert(engine.energy == .73f && engine.variation == 0.f);
    fprintf(stderr, "PASS 264 complete score traversals, 32 isolated PCM cases and manual expression overrides\n");
}
