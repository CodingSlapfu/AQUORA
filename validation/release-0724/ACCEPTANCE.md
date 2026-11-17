# AQUORA v0.7.24 testing acceptance

Source implementation commit b69d77b909ed76d78ee751601e70a1abf7cf5fa8.
Native updater identity FM-1_73000724. Original Puya 0x856014 FM-1 hardware.

| Owner requirement | Implementation and evidence | Remaining acceptance |
| --- | --- | --- |
| Vocal shutdown | Removed Freeze suppression, reclaimed released vocal slots, allowed AUTO+PLAY ALONG to make room when two external notes occupy the pool, recovered invalid playback state, and rearmed entry on eligibility/control/bank transitions. Added status and direct-audio diagnostics. 1200 PCM-verified setting-change windows passed. | Confirm that the owner's physical shutdown symptom is resolved during prolonged FM-1 playback. |
| Expressive singing | Existing vibrato, glide, sustained vowels, bass/drum-aware phrases and accompaniment responses retained. Added section-aware delivery and stable articulation. | Physical listening and phrasing judgment. |
| Coherent vocal identity | Six deterministic primary characters per song, with purposeful section changes. Remembered hooks retain vowel and onset character. | Listener preference across new songs. |
| Non-lyrical vocabulary | Distinct recorded AH/OOH/OH/EE/EH and genuine recorded humming. Explicit Voice selector exposes UH, DO-RE-MI, LA, NA, SCAT, OH-AH hooks and five vowel transitions. All 18 options produced distinct audible PCM. | UH, syllable consonants and AY transition are approximations. Articulation and lip shapes need physical listening/display review. |
| Recorded words | DAY/YOU retained. Added sung LOVE/ME and speech-derived pitched BABY. Three lexical phrase templates follow composed pitches and note duration. Credits and edit hashes included. | BABY naturalness, all word intelligibility, a larger sung vocabulary and recorded intention names. |
| Vocal memory | Stable melodic themes plus stored hook vowels/onsets, restored for musical reprises and fullscreen recall. Distinct seed-based characters across new songs. | Subjective hook quality. |
| Vocal FX | Higher automatic room/echo/chorus, bounded combined feedback, longer room tails, tempo-divided delay, shimmer and existing modulation/EQ/compression/manual controls. 20-parameter PCM and impulse/override checks passed. | FM-1 timbre, intelligibility, output balance and CPU deadlines. |
| Animation | Original correctly colored art retained. Phoneme/morph-aware mouth shapes, closed-mouth HUM, groove nods/tilts, expressive eyelids/gaze and three brighter module indicators. Renderer remains bounded and visible-page-only. | Actual LCD appearance, timing and smoothness. |
| Gate/Slicer | Dedicated page 6, removed from SOUND navigation. Existing targets and K4 style preserved. Pattern, playhead, gate/bypass status and current/next section display. | Hardware feel and musical gating acceptance. |
| Fullscreen controls | Existing section, build, intensity and vocal-hook controls retained. All three visualizers exercised with full vocal FX across worker/fallback/pressure fixtures. | Physical controls, callback deadlines and display timing. |
| Composition and production | Instrumental FM choir restored independently of KIRA-9. Choir responses follow shared harmony. Added restrained role-aware warmth/filter motion and IDM transition hat ratchets. Successful lead/pluck/STRUM, pads, piano, bass groove, arrangements and motif systems retained. Full composition and isolated-stem regressions passed. | Aesthetic quality, danceability and mix balance require listening. |
| Banks and rollbacks | Bank schema retained. Four V10 round trips, V9 migration and corrupt-payload rejection exercised. Existing stable v0.7.14 and testing v0.7.22 routes retained. | Owner bank/flash/rollback test on hardware. |

## Native build

Two builds with the retained pinned JieLi toolchain produced byte-identical
611496-byte firmware packages. SHA-256

4eb10ac41fac595ef1eff5b065c674076ab677b8e65cf43954cb146fc7d8c5e6

Application image 535860 bytes. Data plus BSS 82540 / 98304 bytes.
Pool 318684 / 344064 bytes. The v0.7.23 forced single-core build compiled before the final keyboard-routing guard.

Matching WebAssembly SHA-256

39651691086cd38c030c9bbf1be68eef07ccc573d41e7f3be3eefc9e3bcebf18

The v0.7.23 WebAssembly passed boot/render and live browser startup. The matching v0.7.24 build includes the final keyboard-routing guard.
Host load timings are recorded in load.log. Native timing instrumentation
is retained for FM-1 evaluation. Host timings do not establish hardware CPU
headroom, delivered LCD frame rate or absence of physical XRUNs.

## Final keyboard correction

v0.7.24 adds one routing guard after the comprehensive v0.7.23 checks.
Manual keyboard playing on the instrumental choir now selects FM voices when
vocals are OFF or in AUTO, and sampled voices when vocal Play Along is active.
All six OFF/ON × AUTO/PLAY ALONG/AUTO+PLAY ALONG routes and key release passed.
Native reproducibility, version identity, matching WASM and installer integrity
were checked for v0.7.24. The extensive unchanged-engine evidence below is
retained in validation/release-0723.

## Test scope

- More than one simulated hour across 1200 setting-change windows. Each window required direct-vocal activity and audible isolated output PCM.
- Invalid sample phase recovery and the two-held-external-note allocation case.
- Eighteen distinct audible articulation outputs, six stable song characters and articulation-aware hook recall.
- Twenty vocal parameter PCM responses, room/echo/chorus impulses, manual room override and a processed minimum FX send.
- Twenty-four host maximum-vocal-FX cases across three visualizers, four worker states and two injected callback-pressure fixtures.
- Full composition traversal across 5 modes, 22 intentions and 3 seeds, plus isolated PCM stems for all 8 roles in all 5 modes. The legacy test's printed 264/32 totals predate the fifth mode and expanded scene table. Its current loops exercise 330 scores and 40 stems.
- ARRANGE queue/boundary/duration checks, bank migration/round trips, sixteen manual drum lanes, mixed 16-voice load, no full-scale clipping in tested mixes, and portrait palette/phoneme checks.
- Browser updater simulations from v0.7.14, v0.7.22 and same-identity v0.7.24.

This is a testing firmware candidate. Stable release approval follows the
owner's physical FM-1 evaluation.
