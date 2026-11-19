# AQUORA v0.7.38 TESTING

Firmware identity `FM-1_73000738`. Qualified installer target is the original Puya 0x856014 FM-1. Keep a pre-update copy of your banks for rollback.

Hold **EDIT** and turn **ALGO** on HOME to select Standard, Cinematic, Neoclassical, Experimental Ambient, or IDM. The current mode and gesture are shown on HOME. Changes preview immediately and apply at the next bar while playing. PRESETS selects intentions independently, and New Song, Reroll, bank recall and power cycles retain applicable mode selections.

KIRA now keeps glide, vibrato and pitch turns while her recorded words are tuned. The corrected pitch map uses reduced gain to keep more of the source delivery. A duplicate finite word can continue on a sung vowel while the preceding word finishes. The unlooped new words retain their consonants and codas. The portrait keeps its seven direct-voice-reactive lights and adds eased one-pixel lateral movement and gaze during expressive phrases.

**11 new recorded words** are available in AUTO and in **VOCALS → VOCAL FLOW / PLAY ALONG → K2 VOICE → WORDS 2** for manual singing. In manual Play Along, the first 11 keys play PEACE, FAITH, HOPE, LIGHT, JOY, DIVINE, CONSOLE, UNDERSTAND, RECEIVE, LIFE and WELCOME in that order. The prior WORDS setting and physical key order remain available. The new words are genuine female spoken performances from shadoWisp and deleted_user_1390811, licensed CC BY 3.0. See `VOCAL_CREDITS.md` and the per-word manifest. Lossless palette encoding of the portrait freed 40,014 bytes for expansion. The 9 kHz packed additions consume 29,135 bytes before index metadata. Banks saved in WORDS 2 use the new articulation value, which v0.7.37 cannot read.

Piano, bass, pad, string, guitar, harp and mallet FM operator profiles were revised. The ensemble planner now accounts for the selected lead and piano when choosing complementary plucks. Intention revisits compare melodic intervals and onset spacing, including uniform variations of the same contour. The patch audit renders 63 patches at three registers, and A–B–A motif checks cover 22 intentions and five modes.

Native memory use is 566,344 / 581,564 flash bytes, 81,788 / 98,304 static RAM bytes, and 262,472 / 344,064 DSP pool bytes. The matching WASM passed 512,000 audio frames and changing 240×240 display output. The HOME regression covers the five modes, settings, next-bar apply, PRESETS, fullscreen and saved-bank migration. The new recordings were exercised from the manual keyboard and the AUTO phrase director through final PCM playback. The HELLO pitch trace and TUNE regression are included.

Physical musical quality, intelligibility in full arrangements, actual FM-1 callback timing and long-session operation still need owner assessment. DSP pitch correction remains playback-rate-based. A real-time formant-preserving retuner, full acoustic differentiation of every instrument pair, and a broad melody/rhythm/harmony/arrangement novelty audit remain open. See `ISSUE_AUDIT.md`.

Stable rollback is v0.7.14. Earlier testing installers v0.7.36 and v0.7.37 remain available.

The matched 15 full-mix renders counted 42 recorded-word starts in v0.7.36,
29 in v0.7.37 and 40 in v0.7.38. No digital clipping was observed in those runs.
Metrics also include the dry renders, vocal starts, large leaps and active
blocks. Isolated measurements changed for 24 patch IDs and produced no
clipping across all 189 note renders. These are engineering checks. The
versioned listening page includes the current dry voice, full mix and word
audition.

In a 1,320-pair New Song check, no adjacent pair matched across contour and
onsets, rhythm mask, the first eight chord degrees and the 32-section route.
Contour and onset signatures repeated in four pairs, rhythm masks in 239,
and eight-chord signatures in 96. This check measures structural variation.
The updater port simulation passed 0.7.36 → 0.7.38, 0.7.37 → 0.7.38,
same-version reflash, and 0.7.38 → 0.7.37 rollback, including rejection of a
corrupted binary.
