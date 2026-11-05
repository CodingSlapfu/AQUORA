#ifndef SERENITY_H
#define SERENITY_H
#include <stdint.h>
#include <stddef.h>
#define SR_RATE 44100
#define SR_HIT_MAX 100u
#define SR_DENSITY_MAX 8u
#define SR_SOUND_NATURE (1u<<8)
#define SR_SOUND_FX (1u<<9)
#define SR_SOUND_LOCK_MASK 1023u
#define SR_TRACKS 8
#define SR_VOICES 16
#define SR_PATCHES 36
#define SR_SCENES 13
#define SR_BEDS 12
#define SR_FREQUENCIES 13
#define SR_DRUM_KITS 19
#define SR_BEAT_PRESETS 8
#ifdef __cplusplus
extern "C" {
#endif

enum {SR_BUS_NATURE, SR_BUS_SACRED, SR_BUS_REFERENCE, SR_BUS_BINAURAL, SR_BUS_FX, SR_BUS_COUNT};
enum {SR_STANDARD, SR_CINEMATIC, SR_NEOCLASSICAL, SR_MUSIC_MODES};
typedef enum {SR_LEAD, SR_BASS, SR_DRONE, SR_HARMONY, SR_PIANO, SR_GUITAR, SR_CHOIR, SR_DRUMS} sr_role;
enum {SR_ISOLATE_MIX, SR_ISOLATE_TRACK1, SR_ISOLATE_FIXED=SR_TRACKS+1,
      SR_ISOLATE_SACRED, SR_ISOLATE_BINAURAL, SR_ISOLATE_NATURE, SR_ISOLATE_FX};
typedef enum {SR_SINE, SR_TRI, SR_HOLLOW, SR_REED, SR_PLUCK, SR_CHOIR_WAVE} sr_wave;
typedef struct { const char *name; sr_wave wave; float attack, release, brightness, gain; } sr_patch;
typedef struct { const char *name; uint16_t bpm; uint8_t scale, energy, ambiance, tonic, frequency; } sr_scene;
typedef struct { const char *name; float hz; } sr_frequency;
typedef struct { const char *name; float kick, hand, shaker, root, skin, decay, brush; } sr_drum_kit;
typedef struct { uint8_t patch, mute; int8_t tone, attack, release; float volume, pan, meter, send; uint32_t events; } sr_track;
typedef struct { uint8_t active, track, gate, midi, key; float tail, pitch, increment, freq, phase, phase2, env, peak, filter, attack_coef, release_coef; uint32_t remain, born; } sr_voice;
typedef struct { uint16_t glide_ms; uint8_t glide_mode, vibrato_depth, vibrato_rate; } sr_motion;
typedef struct {
  sr_track tracks[SR_TRACKS]; sr_voice voices[SR_VOICES];
  uint32_t rng, frame_count, tick_phase, tick_length, sequence_step, phrase;
  uint8_t scene, frequency, tuning_432, running, bed_type, isolate, frequency_auto, frequency_lock, pending_lock, lock_textures;
  uint8_t drums_enabled, drum_density, drum_kit, binaural_enabled;
  float master, reverb_amount, delay_amount, bed_level, current_energy;
  float wind_lp, rain_lp, limiter, dc_x, dc_y, low_bass;
  float drum_level, drum_meter, kick_phase, kick_env, hand_phase, hand_env;
  float shaker_env, shaker_lp, hand_noise_lp;
  float binaural_carrier, binaural_rate, binaural_level, binaural_env;
  float binaural_phase_l, binaural_phase_r;
  float lock_phase;
  float bass_phase,tone_phase,transport_env,tone_level,carrier_now,beat_now;
  float energy,variation,bed_phase,bird_phase,bird_env;
  uint8_t freeze, perfect_key, solo, sacred_mode, restart, fx_mask, drum_hit;
  uint32_t song_seed, visit_nonce;
  uint8_t music_mode, motion_manual, recent_patches[SR_MUSIC_MODES][SR_SCENES][SR_TRACKS];
  uint16_t sound_locks;
  uint8_t drum_presence, play_grid, groove_mode, groove_strength;
  uint8_t air_track, air_fx, air_depth, evolve_pace, evolve_next;
  uint8_t scope_track, scope_write;
#ifdef PLAT_HOST
  uint32_t scope_samples;
#endif
  int16_t scope[128];
  float air_target, air_wet;
  float bus_meter[SR_BUS_COUNT];
  uint32_t play_queued, play_released;
  uint8_t play_track[27], play_strength, play_feel;
  uint32_t play_due[27];
  uint32_t nature_audition, bird_wait, nature_rng, preview_frames;
  uint8_t preview_role;
  uint8_t nature_active, nature_from, nature_style, nature_level_manual, nature_activity, nature_distance, bird_phrase, bird_note;
  float nature_blend, bird_pitch, bird_pan;
  float nature_mono, nature_side;
  uint16_t bird_left;
  float nature_peak;
  uint32_t note_ons, note_offs, watchdogs, drum_rng, optional_hits;
  float sacred_hz_now;
  uint16_t stop_frames;
  float sacred_level, sacred_gain, sacred_harmonics, pitch_last[2], vibrato_phase[2];
  sr_motion motion[2];
  uint8_t auto_gain, auto_level;
  float input_peak[SR_TRACKS], gain_trim[SR_TRACKS], master_peak;
  uint32_t (*clock_ticks)(void);
  uint32_t fx_ticks;
  int16_t reverb[4][1789]; int16_t delay[16384];
  uint16_t rev_pos[4], delay_pos, rev_history, delay_history;
  float reverb_wet, delay_wet;
  uint8_t loop_length, loop_pos, pending_length, keyboard_mode, tonic_override, pending_key, pending_physical_key;
  int8_t melody[16], motif_home[16];
  struct { uint16_t phase; uint8_t midi, velocity; } ornaments[4];
} sr_engine;

extern const sr_patch sr_patches[SR_PATCHES];
extern const sr_scene sr_scenes[SR_SCENES];
extern const sr_frequency sr_frequencies[SR_FREQUENCIES];
extern const sr_drum_kit sr_drum_kits[SR_DRUM_KITS];
extern const float sr_binaural_presets[SR_BEAT_PRESETS];
void sr_init(sr_engine *e, uint32_t seed);
void sr_generate_song(sr_engine *e,uint32_t seed,int sounds_only);
void sr_select_nature(sr_engine *e, uint32_t seed);
void sr_set_scene(sr_engine *e, uint8_t scene);
void sr_set_music_mode(sr_engine *e,uint8_t mode);
unsigned sr_legacy_motion_manual(const sr_motion *motion,unsigned scene);
void sr_motion_auto(sr_engine *e,unsigned role,int automatic);
void sr_set_frequency_lock(sr_engine *e, uint8_t enabled);
void sr_set_frequency_auto(sr_engine *e, uint8_t enabled);
void sr_set_frequency(sr_engine *e, uint8_t frequency);
void sr_set_perfect_key(sr_engine *e, uint8_t enabled);
void sr_set_tuning_432(sr_engine *e, uint8_t enabled);
void sr_set_drums(sr_engine *e, uint8_t enabled);
float sr_patch_preview(const sr_track *track,float phase);
float sr_patch_envelope(const sr_track *track,float seconds,int releasing);
void sr_set_drum_level(sr_engine *e, float level);
void sr_set_drum_density(sr_engine *e, uint8_t density);
void sr_set_drum_kit(sr_engine *e, uint8_t kit);
void sr_set_binaural(sr_engine *e, uint8_t enabled);
void sr_set_binaural_rate(sr_engine *e, float difference_hz);
void sr_set_binaural_carrier(sr_engine *e, float carrier_hz);
void sr_preview_sound(sr_engine *e, unsigned role);
void sr_set_track_patch(sr_engine *e, uint8_t track, uint8_t patch);
void sr_set_loop_length(sr_engine *e, uint8_t length);
uint8_t sr_tonic(const sr_engine *e);
void sr_start(sr_engine *e);
void sr_stop(sr_engine *e);
void sr_note_on(sr_engine *e, uint8_t track, uint8_t midi, uint8_t velocity, float beats);
uint8_t sr_key_note(const sr_engine *e, unsigned key);
void sr_key_down(sr_engine *e, uint8_t track, unsigned key);
void sr_key_up(sr_engine *e, unsigned key);
void sr_render(sr_engine *e, int16_t *interleaved_stereo, size_t frames);
float sr_note_frequency(const sr_engine *e, int midi);
float sr_sacred_bass_frequency(const sr_engine *e);
#ifdef __cplusplus
}
#endif
#endif
