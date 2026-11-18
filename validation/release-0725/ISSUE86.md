# Issue #86 implementation ledger

Resumed branch aquora-v0.7.22-corrective-development at 64e62948c0145072243bdcebdadb318e51a8972b. Issue #87 and #86 were read before changes. v0.7.24 publication was already complete and its public firmware/WASM hashes match; release-0724/PUBLICATION.md records the receipt. Its immutable binaries remain unchanged.

## Gaps corrected for v0.7.25 testing

- Primary identity now combines the existing intention melodic/breathing/harmonic profile with the song seed and mode. It stays deterministic within a song. Section delivery develops it: sparse intro/bridge humming, original verse identity (including available words), Standard OH/AH chorus, Cinematic sustained vowels, IDM rhythmic vocables, and recalled outro/reprise hooks.
- Sparse intro/bridge phrases respect the user's presence cap and leave whole bars for breath. Intention breathing profile, mode, actual kick and current musical energy affect note duration/accent. Existing chord support, bass-linked motif, snare/hat avoidance, melodic hook memory and response arrangement remain in use.
- All 20 existing AUTO DSP targets now depend on intention/mode/section/energy as appropriate, rather than mostly fixed defaults. Main chorus/room/echo also follow these profiles. Existing tempo-synchronized delay divisions remain; no new DSP engine or per-sample searches were introduced.
- AUTO tuning now adapts to mode/intention/section. Manual TUNE, including explicit zero, wins; UI displays Auto until overridden. Nonzero legacy values remain explicit. Existing AUTO reset clears the override.
- Manual articulation now wins over recalled vowels/onsets. Hook memory also retains its diphthong transition; all scene/song/bank/manual-articulation resets clear that derived memory.
- K1 enable, K2 presence, K3 real gain, K4 FX send remain user-owned. Director does not write these controls or enable vocals. Main FX and every individual parameter retain final manual precedence.

## Preserved, not reimplemented

v0.7.23/24 scheduler reentry, voice-tail reclamation, invalid-phase recovery, live-keyboard routing, OFF behavior, Gate/Slicer, animations, instrument composition, bank formats, licensed word samples and all rollback installers. No new recorded words or intention-name recordings were created in this change.

Recorded DAY/YOU/LOVE/ME remain the existing documented human sources. BABY remains speech-derived, not a natural sung take. UH, syllable consonants and AY remain documented approximations. The director only uses available skills; intelligibility, beauty and stylistic appropriateness require owner listening.

## Verification and acceptance

New test covers 110 intention/mode direct-vocal PCM cases, 550 section profiles, variation in all 20 AUTO targets, parameter/main-effect locks, manual tune zero, manual articulation over hook recall, OFF and zero-presence guards. Updated the preexisting articulation fixture to exercise words in verses and hook memory in choruses, matching intentional section development.

Changed-code regressions, native reproducibility, WASM, updater identity/corruption guards and performance tests all passed; their evidence is recorded alongside this ledger. The featured-voice fixture now explicitly tests the low-presence instrumental break, while high presence permits sparse humming. Native compile/link checks pass: image 537912 bytes; data+BSS 82540/98304; pool 318692/344064. These are static resource checks, not hardware CPU measurements.

Physical FM-1 acceptance remains open: prolonged knob/intention/mode changes without silence or reboot, prompt entry, actual CPU/XRUN and input latency under all visualizers, speaker/headphone level balance, manual override listening, hook/word intelligibility, natural singing/phrasing, and aesthetic approval. Puya/new-flash write acceptance remains hardware-dependent. No new official stable release is authorized.
