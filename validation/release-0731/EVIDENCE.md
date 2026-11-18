# AQUORA v0.7.31 development checkpoint

Starting point: dc188d72094161bc633d5292eef9a1aa977b0f3a. Stable and hosted testing releases are unchanged. Development package only.

## Implemented

- Ten new recorded human female one-shots: GO, NO, WAY, BE, FEEL, COLORS, GREEN, BEAUTIFUL, I and GOING. The five existing sung words keep their IDs and occupy the first five WORDS keys. No new word is persisted in hook memory. No new word loops its consonants. Clip pitch starts near its measured source register and the new word sample follows its natural recorded pitch movement without synthetic vibrato or glide on top.
- Manual WORDS selects the expanded bank. Featured AUTO phrases choose occasional lyrical motifs from the musical context and use shorter material when a section is busy. The same voice allocator, callback budget, saved formats and purple-lamp renderer are retained.
- Cinematic and Neoclassical AUTO recover baseline NORMAL phrasing. Explicit HALF and DOUBLE remain available, with automatic variation in other modes. Baseline production offsets and send scaling return for the AUTO voice; explicit patches retain their offsets.
- Original performer names, licenses, source and encoded/decoded hashes and per-clip edits are in device/docs/VOCAL_CREDITS.md and device/dsp/human_words.json.

## Evidence

| Artifact | SHA-256 |
|---|---|
| AQUORA-0.7.31-development.fwsc | 7743e31d149c63999f7835a7436431ae0b7ac530c97e65654f6a93267c41e24c |
| app.wasm | ec0961a09f8e631e8a838ace7d1127182d8af70dadb4910164bae4a4b44ef2ea |

Native application uses 581,500 of 581,564 bytes. Data/BSS 81,756 of 98,304. Pool 325,960 of 344,064. Identity-specific updater, resume/mismatch, corrupted package, and WASM framebuffer/audio smoke passed. Existing five word PCM checks and explicit flow variants passed. The new host fixture verified all ten at the source register and +/-2 semitones, non-clipping, endings, manual selection, availability through AUTO and direct-PCM glow. Portrait render confirmed 1,105 changed pixels with direct vocal PCM and mute gating at 240x240. No physical timing or listening result is claimed.

Matched 25.08-second dry and full-mix renders for Cinematic and Neoclassical matched the audition trial's measured onsets, peaks and direct-block counts. Neoclassical dry peak and active blocks changed from 5,591 / 1,735 to 3,620 / 1,341 (v0.7.27 reference 3,913 / 1,341); Cinematic changed from 5,709 / 2,315 to 3,597 / 2,004 (reference 3,597 / 2,066). Both sets retained accepted onset counts with no denials. These metrics verify the production and phrasing changes, not a listening verdict.

## Audio gate

The audition pack and two additional source leads were screened using original-phrase transcription, forced alignment, pitch and envelope checks, alternate edits, isolated-word ASR, and repeated-word ASR on actual decoded firmware assets. FAR, AWAY, FROM, HOME, OTHER and layered LONG were rejected from this iteration. Alternate BEAUTIFUL and new BE use the same ScarlettWitch89 repertoire as GO and NO; GOING was newly cut from 417410. Objective codec SNR is 23.43–24.86 dB; normal-scale PCM has clean span and endings at all three tested pitches. No human perceptual listening was performed in this environment. The stronger repeat transcription recognizes BEAUTIFUL, GO, GOING and NO consistently, with inflection or phoneme ambiguity on COLORS, FEEL, I and WAY and differing recognition for GREEN and BE. GREEN was recognized by the other isolated model, and BE's source word was supported by source alignment. Independent evidence is still insufficient to close the ten-word clarity and musical-quality gate; therefore the development build was backed up without testing deployment. No owner sample-approval gate is implied.
