# AQUORA v0.7.10 development acceptance

**Release state: held.** Official v0.7.9 is the rollback and current public firmware. This branch is an engineering checkpoint. The firmware under validation is for the original Puya FM-1. Start from the verified v0.7.9 tag, commit `e41d1b08a9f87f95b01ca3db9822e5feca5fd07b`.

| Requirement | Verified work | Remaining acceptance |
|---|---|---|
| #83 P0 visualizer crackle and responsiveness | One 20 FPS target. Pressure and XRUN backoff is internal. Cymatics shares the original separable axis terms across finite differences and light-map cells. The exact v0.7.9 field and 512 grains are retained. `tests/cymatics_parity_0710.py` confirmed identical particle coordinates, light cells and glints over 1,200 controlled host frames. The host pressure/fallback and latency fixtures exercise all visuals. | Original FM-1 audio P95/P99, XRUN/scan/LCD timing and listening under dense playback. A SLOOP-style disjoint voice render is still open. AQUORA's current worker timeout is 8 ms, beyond the approximately 5.805 ms DMA half-buffer window. Core 0 still owns audio and core 1 handles visual geometry. A deadline-bounded audio worker and differential LCD transfer remain open. |
| #83 and #6 FM instrument engine | All IDs 0–62 now use the same two-operator phase-modulation engine. IDs 0–23 have individual FM operators and the other 39 have velocity-dependent index scaling. Original patch names, IDs, family assignments and envelope parameters are retained. All 63 preview paths pass the focused DSP test. See the [patch audit](FM_PATCH_AUDIT.md). | Patch-by-patch rendered note and register assessment, audibility and realism against v0.7.9, and worst-case native polyphony. Complete fine-tuning of the existing 39 needs that comparison. |
| #83, #72, #77 and #66 long-form composition | Section identities now influence the shared harmonic center, cadences into the next section, and featured role dynamics. Preserved returning motifs, existing mode profiles, 32-section route and deterministic score preview. The score traversal covers 22 intentions, all four modes and three seeds. | Arrangement timing still advances in phrase-length units. Fully decouple song form from changes to 16/32/64 and validate uninterrupted 10/20/30-minute performances with level-matched parts and owner listening. Cinematic film-score character and lead/pluck/STRUM fidelity still need audible comparison. |
| #83 and #73 Fortune | Added append-only intention ID 21, artistic reference 726 Hz, separate melodic, groove, motion, nature, drum and ensemble choices. Original 21 identifiers remain at their previous positions. Host selected Fortune through the global PRESETS encoder. | Listen to the new profile in each mode and verify the physical device's reference output. |
| #83 drum visualization and part controls; #18/#65/#68 percussion | Real per-step BD/SD/HH history, accented hits and scheduled fill hits, playhead, direct SELECT tab browsing, part-mute encoders, short click-free gains and legacy-compatible bank mute bits. Existing whole-track mute/solo controls remain. DSP test verifies muted output and legacy-compatible recall. | The graph now marks predicted hits through the current bar using the score preview. The expanded fixture matched 5,179 rendered events across 88 intention/mode pairs, including drum velocity. External sync has no speculative hits. Verify all kits, track settings and fill transitions on hardware. |
| #83 optional built-in speaker output | Physical controls map to SETUP's K2 output setting. Default is stereo headphones; FM-1 Speaker folds to mono, reduces sub-bass and adds mild presence before the limiter. Switching has a short gain ramp. Binaural output fades out and resumes when switching back. Host verifies setting persistence and signal separation. | Measure the FM-1 speaker's response, level and distortion and listen for transitions, punch and masking. |
| #83 and #75 240×240 readability and navigation | Restored five selectable palettes, Black/White background, moved Theme to Appearance K1, preserved text-size controls, added drum tab browsing and a visible hint. Host verifies all three text sizes and Fortune, footer and display dimensions. | Complete the all-theme clip/contrast and animation pass, including physical LCD and newcomer navigation checks. Discoverability still needs a page-by-page review. |
| #72, #77, #48, #40, #69, #78 approved baseline music | Existing global PRESETS, kick-locked bass, ARRANGE queue, patch locks, AUTO gain/level, Nature and Fixed Tone caps, Mellower Standard/Cinematic tempo and pad/piano/choir accompaniment carry forward. Score, render isolation, transition, arranger and bank tests exercise these paths. | Level-matched stems, old/new realism and feel. Additional FX orchestration including cutoff/resonance and three-role gate/slicer remains open. Verify the full Nature library and restored-preset audible differences on hardware. |
| #82, #79, #75, #67 and #71 preservation | Classic UI and animated transparent spiral host test, 240×240 visuals, audio-reactive LED and EVOLVE fixtures. The worker still offers single-core fallback. | Physical LCD and LED/scan checks, and exact fullscreen/aspect and text review for the updated visuals. |
| #58, #60, #61, #54, #59 and #70 approved baseline interaction | Existing learn/motif response, randomized startup, passive AIR, role selection and musical detail behavior retained. Existing host intelligence test checked motif capture and response. | Hardware AIR regression, musical response and live input feel. |
| #62, performance USB/BLE MIDI | Existing owner deferrals remain recorded in the inherited matrix. | Outside this release's authorized scope. |
| #76 and native release gate | Pinned toolchain and device SDK. Dual-core native packages matched byte-for-byte on repeated builds; single-core build and updater simulation verified separately. | Physical flash/rollback and vendor M-UPGRADE test where applicable. |
| #81, #74, #63 existing Site and Web Installer | Prior v0.7.9 Site publication is recorded in the repository's lease history. | Publish the complete v0.7.10 firmware only once all software gates are met. The matching WASM build and staged installer passed validation for the checkpoint below. Deploy only to the existing Sites project after the release clears those gates. |

## Recovery

Use `tools/prepare_next_source.sh` to recover the pinned vendor source and overlay this branch. Build with the pinned JieLi toolchain and SHA from `.github/workflows/aquora-079.yml`, passing `--release 0.7.10`. For a single-core control build, set `FM1B_CPU2=0`. Run the updater test from the recovered source so its vendor installer fixtures are available. Run `tests/audio_0710.c`, `tests/frame_policy_next.c`, `tests/controls_0710.txt`, the existing composition/bank tests, and the host LCD and performance suites before publishing.

The host LCD, classic UI, worker fallback, pressure, input latency, LED, DSP, arrangement and bank suites passed. The Quick Find directory resolves all registered routes. Its legacy generic edit fixture still fails on the same 10 controls as v0.7.9: STRUM gap, AIR sensitivity/invert/music request, learn apply/clear, keys/notes/dance lighting, and fullscreen performance. The fixture sends engine actions directly, while some controls require a page-local input path or a specific operating mode. The new physical-control fixture passed independently for Fortune, themes, speaker preference persistence and three drum mutes.

The previous integrated native checkpoint is retained in the same directory with the checksums below. Its source is commit `116e489eccc920a3a75b7179c001face09d4d01a`. Later host-test-only changes do not enter the native callback.

| Artifact | SHA-256 |
|---|---|
| Native checkpoint `FM-1_73000710` | `f668de135f8d30f3287eb929c47fdc1735fe4e2538501106ac0c11828ab65f07` |
| Single-core build `FM-1_73000710` | `b06b2509b6594f1227c2f30d3b383c88f412d425139756cf705e041221642b15` |
| Official v0.7.9 rollback `FM-1_73000709` | `0b98bb4b7df0f18cd2079b301509b18f789dc13e4b7a3748b2bf26b0a7ec0d32` |

## Full-fidelity Cymatics and drum preview checkpoint

Source commit `3d3516ffe02c22ef7391636fbedaf0cf75afec04`. Dual-core build reproduced byte-for-byte. Both hardware variants passed the installer simulation from v0.7.9. The matching WASM module passed WebAssembly validation. The staged installer accepted the exact package and rejected a one-byte change with the same product identity.

| Checkpoint artifact | SHA-256 |
|---|---|
| Dual-core native | `bbc75980653c3b6c583e1fafa07a8e1f6c74be4481e220ceea97f80f1bf3c931` |
| Single-core native | `f932d74dd5fceb3ead9055a13c96a0c0b7b8d6f1537cc7ace9f13c86c2eabbed` |
| WASM | `b848dd987bc341a12b75fcb492c905c90f6d294e4762a8abd6538b87c9716a9b` |

The Cymatics optimization preserves the original sample positions and lighting calculations. It reduces the number of sine interpolations inside field evaluation from 17,300 to 9,664 per frame by sharing unchanged axis terms. The full-resolution drawing inputs matched v0.7.9 in the host fixture. Physical frame timings and clips are still required.

## Physical FM-1 acceptance

Use the original supported Puya FM-1. Record the candidate identity and SHA-256, back up the four banks, and keep the verified v0.7.9 rollback available. Use the same output level, seed, intention, mode and control sequence when comparing versions.

1. Flash the final candidate, verify boot and recall each bank. Check that patch IDs, locks and drum state survive reboot. Confirm rollback accepts the signed-off v0.7.9 firmware and that the original banks still load.
2. Run a dense arrangement on each visualizer in normal and fullscreen modes. Repeat with Cymatics for at least ten minutes while turning PRESETS, SELECT and the four parameter knobs. Record callback P95/P99, XRUN delta, draw timing and input response. Record the same test on v0.7.9. Flag any audible crackle, stalled animation, missed control or persistent FPS drop.
3. Record matching Cymatics clips in quiet and dense passages. Compare particle count and motion, highlights, intensity, color, reactivity and visible detail. Compare both normal and fullscreen framing.
4. Listen on headphones and the built-in speaker. Toggle the speaker profile during playback and check for clicks, lost bass/kick definition or harshness. Check stereo binaural output and its return when leaving speaker mode.
5. Play multiple long scores in each mode and change 16/32/64 during playback. Listen for retained musical identity, recognizable section transitions and returning motifs. Audition Fortune in all four modes and sample the original leads, plucks, STRUM, piano, pads and choirs against v0.7.9.
6. Compare the drum playhead and hollow predicted steps with actual BD/SD/HH hits and fills. Mute each part, change kit, save and reload. Check all themes and all text sizes for clipped labels, and exercise AIR, LEDs, ARRANGE, EVOLVE and supported MIDI.

Keep release publication held until outstanding software items and required physical gates have explicit evidence.
