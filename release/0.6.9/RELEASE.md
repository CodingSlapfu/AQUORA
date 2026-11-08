# AQUORA v0.6.9 — original FM-1 hardware only

AQUORA v0.6.9 supports the original FM-1 hardware with Puya P25Q80 flash (JEDEC 0x856014). Newer FM-1 units with UC25HQ80 flash (0xB36014) are NOT supported by this release. Do not install on an unverified newer unit.

The installer does not reliably identify the flash chip. Ask your seller/manufacturer to confirm it, or ask for help at https://github.com/CodingSlapfu/AQUORA/issues/62 before installing. Saving, updating and recovery on unsupported units may fail or be unsafe.

#62 is owner-deferred to a future release; it is not fixed. All accepted firmware bytes and original-hardware safety checks remain unchanged. Software acceptance through #61 passed; physical hardware and artistic acceptance remain owner-unverified. Performance MIDI is deferred. v0.6.7/v0.6.8 rollback is retained.

Includes Learn from Playing, Experimental Ambient and musical evolution, AIR stability corrections, randomized startup arrangement, track selection and control responsiveness improvements.

## Full usage guide

# AQUORA 0.6.9 guide
After the intro finishes or is skipped, AQUORA starts one fresh, coherent randomized song: one of21 intentions, one of4 Music Modes, and a compatible sound/drum palette. The physical master knob controls output; the existing smooth master/transport ramp protects startup. PLAY stops it. It will not resume automatically after you stop. This startup behavior is always ON in production. It does not overwrite saved banks.

PLAY SELECT now shows the same role and patch/kit used by SOUND and MIX. SELECT-only turns keep the current animation frame intact. Intentions and Music Mode commit at the next quarter-note beat, with latest-choice replacement and a pending label; the maximum internal-clock wait is one beat at the currently active tempo, plus the next audio block. Key-change performance and loop length remain explicitly loop-bound. Manually changing levels/FX stays immediate; sound shape/voicing affects new notes with existing smoothing and tails.

EVOLVE → EDIT through to LEARN. K1 enables Play Alone or Alone+Along capture; OFF is the default. While stopped, play up to16 original physical-key events over at most64 sixteenth steps. Recorded pitches follow Perfect Key; digital keys supply fixed velocity, not pressure/velocity sensing. Durations and intervals use the current tempo grid. At least3 events are needed. K3 applies the phrase immediately when stopped or at the next loop when playing; Freeze protects a queued phrase. K2 chooses Original/Subtle/Balanced/Strong influence. K4 left discards the current unapproved capture; right clears learned influence. SAVE stores an approved motif in the current bank; explicit bank recall restores it. New Song clears it; Reroll Sounds preserves it. Long captured phrases continue across successive16-step loops. The motif is adapted to the current intention scale, with original answers/returns and duration-aware playback. No cloud upload or neural-model training occurs.

Experimental Ambient adds interlocking3/5-step melodic/percussive figures and distinct sound pools. Sixteen functional song sections include openings, thematic question/answer, counterpoint, departure, textures, broken groove, suspension, bridge, breakdown, rebuild, bloom, secondary theme, reprise and resolution. Their instrument masks, harmonic paths and pulse differ; the macro form spans multiple loops and later cycles vary the middle route while retaining home returns. Freeze holds evolution. Manual mutes, solo and drum OFF remain authoritative.

LIGHTS ON uses physical black/white idle scan duties70/50%, with held/pending keys full-bright. KEYS gives physical and generated-note feedback, with short visibility holds. DANCE extends role/drum feedback and music-driven button accents. OFF retains the tempo PLAY/status signal. Generated-note holds and pending-key feedback are separate from the audio generator; no RECORD action is triggered. Physical brightness and mapping still require owner retest.

AIR follows one advertising source, rather than averaging unrelated broadcasters. Enable AIR, keep hands away and hold still for at least1 second, then calibrate FAR; hold your intended near position for at least1 second and calibrate NEAR. Weak/overlapping or unstable windows are rejected. Median/outlier filtering, a measured noise floor, hysteresis, bounded rise/release and fresh-packet reacquisition protect control and gesture paths. A changed broadcaster suspends old calibration and requests an intentional reset; stored calibration is not silently overwritten. Diagnostics separate raw RSSI, filtered RSSI, stable control, applied DSP, packet freshness, confidence and rejected samples. RF attenuation is not a calibrated distance measurement. Real multipath/hand behavior remains owner-unverified.

19 kits now give quiet generated kicks a proper onset: the previous attack threshold excluded most generated hits. Full-mix contribution and ceiling checks accompany level-matched original listening comparisons. Actual aesthetic preference is for the owner to judge.

Banks use schemaV6. V1–V5 read/migrate; older firmware safely rejects new V6 records. Preserved0.6.7/0.6.8 firmware and installers remain rollback options. USB/BLE performance MIDI remain deferred; AIR is retained. KIRA-9 is unchanged.
