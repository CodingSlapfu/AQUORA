#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine e;

static void start(void)
{
    sr_init(&e, 727);
    sr_set_vocals(&e, 1, 4);
    sr_start(&e);
    e.external_sync = 1;
    e.tick_phase = 0;
    e.vocal_entry_pending = 0;
    for (unsigned i = 0; i < SR_TRACKS; ++i) e.role_target[i] = e.role_gain[i] = 1.f;
}

static void two_samples(void)
{
    start();
    sr_voice *first = recorded_note(&e, 60, 0, 96, 16.f);
    assert(first);
    assert(recorded_note(&e, 64, 1, 96, 16.f));
    release_voice(&e, first);
    unsigned offs = e.note_offs;
    assert(!recorded_note(&e, 65, 2, 0, 16.f));
    assert(first->active && first->sample_zone == 1 && e.note_offs == offs);
    assert(recorded_note(&e, 65, 2, 96, 16.f) == first);
    assert(recorded_note(&e, 67, 0, 96, 16.f));
    unsigned occupied = 0;
    for (unsigned i = 0; i < SR_VOICES; ++i)
        occupied += e.voices[i].active && e.voices[i].sample_zone;
    assert(occupied == 2);
    int16_t pcm[512];
    for (unsigned i = 0; i < 32; ++i) sr_render(&e, pcm, 256);
    assert(e.vocal_direct_peak > .0001f);
}

static void protected_pool(void)
{
    start();
    e.voice_limit = 6;
    for (unsigned i = 0; i < 6; ++i)
        assert(note_on(&e, i == 0 ? SR_BASS : SR_LEAD, 48 + i, 90, 16.f, 0));
    for (unsigned i = 1; i < 5; ++i) {
        e.voices[i].external = 1;
        e.voices[i].remain = 0xffffffffu;
    }
    e.frame_count += SR_RATE;
    assert(note_on(&e, SR_PIANO, 72, 90, 2.f, 0));
    int16_t pcm[512];
    for (unsigned i = 0; i < 32; ++i) sr_render(&e, pcm, 256);
    assert(e.tracks[SR_PIANO].meter > .0001f);
    assert(recorded_note(&e, 69, 0, 96, 3.f));
    for (unsigned i = 0; i < 32; ++i) sr_render(&e, pcm, 256);
    assert(e.vocal_direct_peak > .0001f);
    assert(e.voices[0].track == SR_BASS && e.voices[0].gate);
    for (unsigned i = 1; i < 5; ++i)
        assert(e.voices[i].external && e.voices[i].gate && e.voices[i].midi == 48 + i);
}

static void deadline_cycle(void)
{
    start();
    for (unsigned i = 0; i < SR_VOICES; ++i)
        assert(note_on(&e, i ? SR_LEAD : SR_BASS, 44 + i, 90, 16.f, 0));
    e.frame_count += SR_RATE;
    for (unsigned i = 0; i < 20; ++i) sr_deadline_observe(&e, 5900);
    assert(e.voice_limit == 6);
    assert(recorded_note(&e, 64, 0, 96, 2.f));
    int16_t pcm[512];
    for (unsigned i = 0; i < 32; ++i) sr_render(&e, pcm, 256);
    assert(e.vocal_direct_peak > .0001f);
    for (unsigned i = 0; i < 800; ++i) sr_deadline_observe(&e, 2800);
    assert(!e.voice_limit);
    assert(e.voices[0].gate && e.voices[0].track == SR_BASS);
    puts("PASS injected deadline pressure clamps to 6 and recovers to 16 without losing bass or blocking vocal PCM");
}

static void ensemble_endurance(void)
{
    static const unsigned caps[] = {6, 9};
    int16_t pcm[512];
    for (unsigned test = 0; test < sizeof caps / sizeof *caps; ++test) {
        for (unsigned mode = 0; mode < SR_MUSIC_MODES; ++mode) {
            sr_init(&e, 727u + mode);
            sr_set_music_mode(&e, mode);
            sr_set_vocals(&e, 1, 4);
            sr_start(&e);
            e.voice_limit = caps[test];
            for (unsigned cycle = 0; cycle < 2; ++cycle) {
                unsigned heard = 0, singer = 0, sections = 0;
                unsigned lead_requests = e.role_requested[SR_LEAD];
                unsigned lead_refusals = e.role_refused[SR_LEAD];
                unsigned bars = 32u * 2u * (2u + sr_scenes[e.scene].energy % 3u);
                unsigned end = e.form_loop + bars;
                while (e.form_loop < end) {
                    for (unsigned track = 0; track < SR_TRACKS; ++track)
                        e.tracks[track].meter = 0.f;
                    sr_render(&e, pcm, 256);
                    sections |= 1u << song_section(&e);
                    singer |= e.vocal_direct_peak > .0001f;
                    for (unsigned track = 0; track < SR_TRACKS; ++track)
                        if (e.tracks[track].meter > .0001f) heard |= 1u << track;
                }
                printf("cap %u mode %u window %u roles %02x sections %02x vocal %u lead requests %u refusals %u\n",
                       caps[test], mode, cycle + 1, heard, sections, singer,
                       e.role_requested[SR_LEAD] - lead_requests,
                       e.role_refused[SR_LEAD] - lead_refusals);
                fflush(stdout);
                assert(singer && (heard & 0xfeu) == 0xfeu);
            }
        }
    }
}

int main(void)
{
    two_samples();
    protected_pool();
    puts("PASS transactional sampled admission, direct vocal PCM, optional-role return at cap 6, held MIDI and bass");
    deadline_cycle();
    ensemble_endurance();
}
