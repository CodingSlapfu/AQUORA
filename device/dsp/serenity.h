#ifndef SERENITY_H
#define SERENITY_H
#include <stdint.h>
#include <stddef.h>
#define SR_RATE 44100
#define SR_FIXED_TONE_MAX .020f
#define SR_NATURE_MAX .20f

static inline __attribute__((always_inline)) float sr_limit_level(float level, float maximum)
{
  return level > 0.f ? (level < maximum ? level : maximum) : 0.f;
}
#define SR_HIT_MAX 100u
#define SR_DENSITY_MAX 8u
#define SR_SOUND_NATURE (1u<<8)
#define SR_SOUND_FX (1u<<9)
#define SR_SOUND_LOCK_MASK 1023u
#define SR_TRACKS 8
#define SR_VOICES 16
#define SR_PATCHES 63
#define SR_SCENES 22
#define SR_BEDS 14
#define SR_FREQUENCIES 22
#define SR_DRUM_KITS 19
enum { SR_DRUM_PARTS=16 };
#define SR_BEAT_PRESETS 8
#ifdef __cplusplus
extern "C" {
#endif

enum {SR_BUS_NATURE, SR_BUS_SACRED, SR_BUS_REFERENCE, SR_BUS_BINAURAL, SR_BUS_FX, SR_BUS_COUNT};
enum {SR_STANDARD, SR_CINEMATIC, SR_NEOCLASSICAL, SR_EXPERIMENTAL, SR_IDM, SR_MUSIC_MODES};
enum {SR_MANUAL_ENERGY=1u, SR_MANUAL_VARIATION=2u};
typedef enum {SR_LEAD, SR_BASS, SR_DRONE, SR_HARMONY, SR_PIANO, SR_GUITAR, SR_CHOIR, SR_DRUMS} sr_role;
enum {SR_ISOLATE_MIX, SR_ISOLATE_TRACK1, SR_ISOLATE_FIXED=SR_TRACKS+1,
      SR_ISOLATE_SACRED, SR_ISOLATE_BINAURAL, SR_ISOLATE_NATURE, SR_ISOLATE_FX};
typedef enum {SR_SINE, SR_TRI, SR_HOLLOW, SR_REED, SR_PLUCK, SR_CHOIR_WAVE} sr_wave;
typedef struct { const char *name; sr_wave wave; float attack, release, brightness, gain; } sr_patch;
enum { VF_DECAY,VF_DAMPING,VF_WIDTH,VF_TIME,VF_FEEDBACK,VF_TONE,
       VF_RATE,VF_PHASER,VF_FLANGER,VF_VIB_RATE,VF_VIB_DEPTH,VF_VIB_WAIT,
       VF_GLIDE,VF_PRESENCE,VF_COMP,VF_MAKEUP,VF_SHIMMER,VF_FLANGE_RATE,VF_FLANGE_FEEDBACK,VF_CHORUS_DEPTH,VF_COUNT };
enum { SR_VOCAL_PATCHES=10 };
enum { SR_VOCAL_AUTO,SR_VOCAL_HALF,SR_VOCAL_NORMAL,SR_VOCAL_DOUBLE };
void sr_startup_vocal(unsigned frame,float *left,float *right);
extern const char *const sr_vocal_patch_names[SR_VOCAL_PATCHES];
typedef struct { const char *name; uint16_t bpm; uint8_t scale, energy, ambiance, tonic, frequency; } sr_scene;
typedef struct { const char *name; float hz; } sr_frequency;
typedef struct { const char *name; float kick, hand, shaker, root, skin, decay, brush; } sr_drum_kit;
typedef struct { uint8_t patch, mute; int8_t tone, attack, release; float volume, pan, meter, send; uint32_t events; uint8_t last_note; } sr_track;
typedef struct { uint8_t active, track, gate, midi, key, external, channel, patch, sample_zone, vocal_consonant, vocal_vowel_to; float tail, pitch, increment, freq, phase, phase2, env, peak, filter, filter_tail, attack_coef, release_coef; uint32_t remain, born; float vocal_turn_first, vocal_turn_second, vocal_turn_rate, vocal_morph_rate, vocal_other_phase; } sr_voice;
typedef struct { uint16_t glide_ms; uint8_t glide_mode, vibrato_depth, vibrato_rate; } sr_motion;
typedef struct {uint32_t frame,duration;uint8_t role,note,velocity,reserved;} sr_event;
typedef struct {uint8_t count;uint8_t note[16],step[16],length[16];} sr_learn_phrase;
typedef struct {
  sr_learn_phrase learned,capture;
  uint32_t capture_ms,capture_down[16],learn_origin;
  uint8_t capture_raw[16],learn_mode,learn_strength,learn_pending;
  uint8_t response_enabled,response_pending,response_wait,response_index;
  sr_learn_phrase response;
  int8_t motif_memory[4][16];
  uint8_t motif_scores[4],motif_count,motif_cursor;
  uint8_t chord_mode, chord_gap, chord_inversion, chord_direction, rhythm_style, fill_character;
  uint8_t air_mode, air_request, air_result, chord_alternate, rhythm_pending;
  int8_t chord_previous[3];
  uint32_t scheduled_overflows, air_actions;
  struct { uint32_t due; float beats; uint8_t role,note,velocity,active; } scheduled[12];
  sr_track tracks[SR_TRACKS]; sr_voice voices[SR_VOICES];
  uint32_t rng, frame_count, tick_phase, tick_length, sequence_step, phrase;
  uint32_t midi_events[64],midi_event_lost;
  uint8_t midi_read,midi_write,midi_enabled;
  uint8_t midi_suppress,external_sync,external_clock_count,external_audio;
  uint8_t scene, frequency, tuning_432, running, bed_type, isolate, frequency_auto, frequency_lock, pending_lock, lock_textures;
  uint8_t drums_enabled, drum_density, drum_kit, binaural_enabled;
  float master, reverb_amount, delay_amount, bed_level, current_energy;
  uint8_t fx_decay, fx_feedback, fx_damping, fx_width;
  uint8_t fx_chorus, fx_autopan, fx_filter, fx_crush;
  uint8_t fx_flanger, fx_tremolo, fx_drive, fx_stutter;
  uint8_t fx_shimmer, fx_granular, fx_duck;
  float fx_shimmer_now, fx_granular_now, fx_duck_now, fx_duck_env, fx_duck_gain;
  uint16_t fx_shimmer_phase, fx_grain_written, fx_grain_phase;
  int16_t fx_grain[1024];
  float fx_mod_phase, fx_chorus_now, fx_pan_now, fx_filter_now, fx_crush_now;
  float fx_flanger_now, fx_tremolo_now, fx_drive_now, fx_stutter_now;
  float fx_filter_low, fx_crush_hold;
  uint16_t fx_repeat_start, fx_repeat_pos, fx_repeat_length, fx_repeat_left;
  float fx_decay_now, fx_feedback_now, fx_damping_now, fx_width_now, fx_delay_low;
  float wind_lp, rain_lp, limiter, dc_x, dc_y, low_bass;
  float drum_level, drum_meter, kick_phase, kick_env, kick_previous, kick_attack, hand_phase, hand_env;
  float shaker_env, shaker_lp, hand_noise_lp;
  float drum_part_gain[3], slicer_gain[4];
  uint8_t filter_cutoff, filter_resonance, slicer_mask, slicer_style;
  uint16_t drum_group_solo,drum_mutes, drum_active;
  uint32_t drum_steps[64], drum_phase[13];
  float drum_amp[13], drum_aux_from, drum_aux_to;
  uint8_t drum_levels[16], drum_solo, drum_playhead, drum_subsample, drum_time, drum_mid_pending;
  float binaural_carrier, binaural_rate, binaural_level, binaural_env;
  float binaural_phase_l, binaural_phase_r;
  float speaker_low, speaker_body, speaker_mix;
  uint8_t speaker_enabled,vocals_enabled,vocal_presence,vocal_play,vocal_articulation;
  uint8_t vocal_hook[3],vocal_hook_vowel[3],vocal_hook_attack[3],vocal_hook_transition[3],vocal_reprise,vocal_auto_ready;
  uint8_t last_auto_bpm[SR_MUSIC_MODES][SR_SCENES];
  uint32_t vocal_started,vocal_rejected,vocal_reclaimed,vocal_recovered,vocal_audio_frames,vocal_last_pcm_frame;
  uint32_t vocal_sample_denied,vocal_budget_denied;
  float vocal_direct_peak;
  uint8_t vocal_chorus,vocal_reverb,vocal_delay,vocal_tune,vocal_fx_manual;
  uint8_t vocal_level,vocal_fx_send,vocal_entry_pending,vocal_call;
  uint8_t vocal_phrase_note;
  uint32_t vocal_phrase_frame;
  uint8_t track_send_manual,vocal_level_manual,vocal_patch,vocal_flow;
  float vocal_auto_gain;
  float track_send_now[SR_TRACKS],track_echo_now[SR_TRACKS];
  float vocal_level_smoothed,vocal_face_peak,vocal_face_pitch_ratio;
  uint8_t vocal_face_mouth,vocal_face_midi,vocal_face_sustain;
  uint16_t vocal_fx_pos,vocal_fx_history;
  float vocal_fx_left,vocal_fx_right,vocal_fx_input;
  float vocal_room_now,vocal_echo_now,vocal_chorus_now,vocal_send_now;
  uint32_t vocal_fx_tail;
  uint32_t vocal_param_lock;
  uint8_t vocal_param[VF_COUNT];
  float vocal_param_now[VF_COUNT];
  float vocal_fx_tone,vocal_phaser_in,vocal_phaser_out,vocal_fx_phase,vocal_flange_phase,vocal_pitch_last;
  float lock_phase;
  float bass_phase,tone_phase,transport_env,tone_level,carrier_now,beat_now;
  float energy,variation,bed_phase,bird_phase,bird_env;
  uint8_t freeze, perfect_key, solo, sacred_mode, restart, fx_mask, drum_hit;
  uint32_t song_seed, visit_nonce, form_loop;
  uint16_t last_theme[SR_MUSIC_MODES][SR_SCENES];
  uint32_t form_key; uint8_t form_route[33];
  uint8_t arrange_next,arrange_length,arrange_active,arrange_elapsed;
  uint8_t timeline_enabled,timeline_write;
  sr_event timeline[96];
  uint8_t pending_scene, pending_mode, profile_applying, expression_manual;
  uint8_t music_mode, motion_manual, tone_level_manual, recent_patches[SR_MUSIC_MODES][SR_SCENES][SR_TRACKS];
  uint16_t sound_locks;
  uint8_t drum_presence, play_grid, groove_mode, groove_strength;
  uint8_t air_track, air_fx, air_depth, evolve_pace, evolve_next;
  uint8_t scope_track, scope_write;
#ifdef PLAT_HOST
  uint32_t scope_samples;
#endif
  int16_t scope[128];
  float air_target, air_wet, drum_air_lp;
  float bus_meter[SR_BUS_COUNT];
  uint32_t play_queued, play_released;
  uint8_t play_track[27], play_strength, play_feel;
  uint32_t play_due[27];
  uint32_t nature_audition, bird_wait, nature_rng, preview_frames;
  uint8_t preview_role;
  uint8_t nature_active, nature_from, nature_style, nature_level_manual, nature_activity, nature_distance, bird_phrase, bird_note;
  float nature_blend, bird_pitch, bird_pan;
  float nature_mono, nature_side, nature_gain_now;
  uint16_t bird_left;
  float nature_peak;
  uint32_t note_ons, note_offs, watchdogs, drum_rng, optional_hits;
  float sacred_hz_now, reference_hz_now, reference_gain_now, vocal_priority;
  uint16_t stop_frames;
  float sacred_level, sacred_gain, sacred_harmonics, pitch_last[2], vibrato_phase[2];
  sr_motion motion[2];
  uint8_t auto_gain, auto_level;
  float input_peak[SR_TRACKS], gain_trim[SR_TRACKS], master_peak;
  float role_gain[SR_TRACKS], role_target[SR_TRACKS];
  uint32_t role_requested[SR_DRUMS],role_refused[SR_DRUMS],role_evicted[SR_DRUMS];
  uint32_t (*clock_ticks)(void);
  uint32_t fx_ticks;
  uint8_t voice_limit,deadline_high,deadline_low;
#if defined(__PI32V2__)
  /* The hardware has one audio engine; score previews have no FX storage. */
  int16_t (*reverb)[1789];
  int16_t *delay, *vocal_fx_buffer;
#else
  int16_t reverb[4][1789]; int16_t delay[16384];
  int16_t vocal_fx_buffer[8192];
#endif
  uint16_t rev_pos[4], delay_pos, rev_history, delay_history;
  float reverb_wet, delay_wet;
  uint8_t loop_length, loop_pos, pending_length, keyboard_mode, tonic_override, pending_key, pending_physical_key;
  int8_t melody[16], motif_home[16];
  struct { uint16_t phase; uint8_t midi, velocity; float beats; } ornaments[4];
} sr_engine;

/* finish returns 1 for completed output or retires the outstanding job
 * before returning 0. available excludes other mailbox users. */
typedef struct {
  int (*available)(void);
  int (*start)(void (*job)(uint32_t),uint32_t);
  int (*finish)(void);
} sr_fm_worker;
void sr_set_fm_worker(const sr_fm_worker *worker);
void sr_deadline_observe(sr_engine *e,unsigned callback_us);

extern const sr_patch sr_patches[SR_PATCHES];
extern const sr_scene sr_scenes[SR_SCENES];
extern const sr_frequency sr_frequencies[SR_FREQUENCIES];
extern const sr_drum_kit sr_drum_kits[SR_DRUM_KITS];
extern const char *const sr_drum_parts[SR_DRUM_PARTS];
extern const uint8_t sr_drum_notes[SR_DRUM_PARTS];
extern const float sr_binaural_presets[SR_BEAT_PRESETS];
/* The caller takes a coherent snapshot while holding the audio lock.
 * Expansion mutates only that scratch engine and must run outside the lock. */
void sr_score_snapshot(sr_engine *scratch,const sr_engine *engine);
unsigned sr_score_preview(sr_engine *scratch,sr_event *events,unsigned capacity,unsigned ticks);
void sr_plan_sections(sr_engine *e);
unsigned sr_section_remaining(const sr_engine *e);
unsigned sr_arrange_section(const sr_engine *e,unsigned ahead);
const char *sr_arrange_name(const sr_engine *e,unsigned ahead);
void sr_arrange_queue(sr_engine *e,unsigned section,unsigned loops);
void sr_arrange_auto(sr_engine *e);
const char *sr_section_name(const sr_engine *e,unsigned next);
void sr_learn_event(sr_engine *e,unsigned key,unsigned down,uint32_t ms);
void sr_learn_action(sr_engine *e,unsigned action);
void sr_init(sr_engine *e, uint32_t seed);
void sr_generate_song(sr_engine *e,uint32_t seed,int sounds_only);
void sr_select_nature(sr_engine *e, uint32_t seed);
unsigned sr_intention_style(unsigned scene,uint32_t seed);
void sr_set_scene(sr_engine *e, uint8_t scene);
unsigned sr_slicer_pattern(const sr_engine *e);
void sr_set_auto_tempo(sr_engine *e, int automatic);
void sr_set_rhythm_style(sr_engine *e, unsigned style);
void sr_set_music_mode(sr_engine *e,uint8_t mode);
unsigned sr_legacy_motion_manual(const sr_motion *motion,unsigned scene);
void sr_motion_auto(sr_engine *e,unsigned role,int automatic);
void sr_set_frequency_lock(sr_engine *e, uint8_t enabled);
void sr_set_frequency_auto(sr_engine *e, uint8_t enabled);
void sr_set_frequency(sr_engine *e, uint8_t frequency);
void sr_set_perfect_key(sr_engine *e, uint8_t enabled);
void sr_set_tuning_432(sr_engine *e, uint8_t enabled);
void sr_set_drums(sr_engine *e, uint8_t enabled);
enum {SR_VOCAL_READY,SR_VOCAL_OFF,SR_VOCAL_STOPPED,SR_VOCAL_LOCKED,
      SR_VOCAL_LEVEL,SR_VOCAL_MUTED,SR_VOCAL_SOLO,SR_VOCAL_ISOLATED,SR_VOCAL_MANUAL};
unsigned sr_vocal_status(const sr_engine *e);
int sr_vocal_sample_valid(unsigned id);
void sr_set_vocals(sr_engine *e,unsigned enabled,unsigned presence);
float sr_patch_preview(const sr_track *track,float phase);
float sr_patch_envelope(const sr_track *track,float seconds,int releasing);
void sr_set_drum_level(sr_engine *e, float level);
void sr_set_drum_density(sr_engine *e, uint8_t density);
void sr_set_drum_kit(sr_engine *e, uint8_t kit);
void sr_set_binaural(sr_engine *e, uint8_t enabled);
void sr_set_binaural_rate(sr_engine *e, float difference_hz);
void sr_set_binaural_carrier(sr_engine *e, float carrier_hz);
void sr_audition_drum(sr_engine *e,unsigned lane);
void sr_audition_vocal(sr_engine *e);
void sr_preview_sound(sr_engine *e, unsigned role);
void sr_set_track_patch(sr_engine *e, uint8_t track, uint8_t patch);
void sr_set_loop_length(sr_engine *e, uint8_t length);
uint8_t sr_tonic(const sr_engine *e);
int sr_midi_event_pop(sr_engine *e,uint32_t *event);
void sr_midi_note(sr_engine *e,unsigned role,unsigned channel,unsigned note,unsigned velocity);
void sr_midi_panic(sr_engine *e);
void sr_midi_bend(sr_engine *e,unsigned channel,int value);
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
