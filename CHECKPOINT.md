# AQUORA 0.3.0 software checkpoint

Matching release archives and instructions are under release/0.3.0/. Read NEXT_BUILD_ACTIVE.md for the remaining FM-1 validation. The 0.2.0 and 0.1.0 archives remain rollback options. Check publication status in release/0.3.0/PUBLICATION.md.

Use nonblocking flock on /workspace/.fm1-sound-healing-build.lock before editing, compiling or publishing. Reconcile remote main and local changes before editing. Restore the checksummed source archive to recover pinned vendor and SDK files. Preserve updater family FM-1_73.

## 0.4.0 development checkpoint

Resume from this development commit under the exclusive lock. The installer still serves the published 0.3.0. The 0.4.0 partial candidate built with JieLi 20260730.1 and passed the targeted transport/HIT ASan and UBSan run. Logs and candidate hash are in evidence/0.4.0/. Recover the toolchain from the verified EIN1-JieLi-20260730.1.tar.xz backup (same pinned SHA in FAST_RESUME.md), extracting with tar --no-same-owner. Recover SDK and vendor from the 0.3.0 source archive as before.

HIT now supports 100% across the DSP, controls and bank restore. At 100% the host test hit all 7000 eligible steps. Other probability, kit, and full acceptance remain in the active queue.

The isolated fixed-reference oscillator reproduces the STOP/PLAY and scene-change signature in PCM when explicitly enabled. There were 528 rising crossings before the scene change, after the scene change and after restart. At zero level the isolated output was silent. The unit's active tone source is still unknown. Next implement independent source isolation controls and PCM checks for Sacred Bass Reference, binaural, generated tracks, and FX. Do not close the reported P0 from this reproduction alone. Continue with all outstanding requirements before publishing.

The display scheduler now enforces a 50 ms minimum between display updates while retaining slower geometry updates under load. Host Percussion, Arrangement and Knob Matrix scripts passed. The updated partial candidate is backed up as evidence/0.4.0/candidate.part-*; concatenate in numeric order and verify candidate-sha256.txt. It is a development artifact, not a release. Physical frame cadence and full release acceptance remain open.

### Source isolation and intention-frequency checkpoint

SOUND > Source Isolate provides dry track and independent Fixed, Sacred Bass, Binaural, Nature and Wet FX monitoring. HOME restores the mix. Isolation is transient. Thirteen scene mappings use Auto by default, reference level stays off until raised, and Manual overrides survive scene changes. Legacy banks retain their stored Hz in Manual. The physical source of the recurring tone is still unknown, and Frequency Lock and the remainder of NEXT_BUILD_ACTIVE.md are still open.

The new native candidate is stored as isolation.part-* in evidence/0.4.0/. Concatenate in numeric order and verify isolation-sha256.txt. Transport/scene PCM, sanitizer, knob matrix and arrangement tests passed. The live installer still serves 0.3.0. Next implement Frequency Lock with measured exact-pitch isolation, add an on-device isolation workflow test and continue the active queue before publication.

### Sequence and keyboard development checkpoint

EVOLVE K4 selects 16/32/64 steps; longer loops derive response notes from the stored motif. SOUND > Keyboard selects Play Along or Change Key. Pending tonic and length apply before step zero. Bank v2 appends loop/key state and reads the previous CRC and payload layout as v1. The native build and targeted transport, arrangement and knob tests passed. More tests are needed for multi-bar percussion variation, legacy migration fixtures, all banks in both keyboard modes, and full acceptance.

The partial native candidate is in evidence/0.4.0/loop.part-* with loop-sha256.txt. Preserve earlier candidates and 0.3.0 production. Next implement Frequency Lock and its PCM isolation acceptance, then complete mixer/auto modes, Water Cymatics, bass/nature/glide and all remaining queue items. No 0.4.0 publication yet. Physical source of the reported tone remains unconfirmed.

### 0.4.0 integrated development candidate

This candidate integrates strict Frequency Lock and optional non-pure textures, graphical MIX with both autos, immersive Water Cymatics, stronger Bass, Nature audition and glide guidance, in addition to the earlier HIT, source isolation, scenes, loops and keyboard work. The live installer remains 0.3.0. Use the files in evidence/0.4.0/integrated/ to recover the exact tested firmware, browser emulator, installer and pinned source. Both archives are split into numbered parts; concatenate each kind and validate SHA256SUMS.txt.

The existing host acceptance suite passed through native and forced single-core builds. The emulator and installer protocol checks passed separately after setting EM_CONFIG to a copy of the 3.1.64 emsdk configuration in the emsdk root with NODE_JS pointing to the workspace Node runtime. Use TAR_OPTIONS=--no-same-owner for emsdk installation in the container. Firmware family remains FM-1_73.

Next, complete the release-specific acceptance still in NEXT_BUILD_ACTIVE.md. Start with a legacy bank fixture from the archived 0.3.0 format, all four extended bank round-trips, and scripted source isolation/lock/mixer workflows. Then complete longer-loop rhythmic evolution, the remaining interaction/performance acceptance, and release publication. The exact cause of the intermittent FM-1 tone is unconfirmed; all thirteen Auto reference frequencies passed PCM measurement, and the deliberate Manual reference stays fixed. The candidate is staged only.

### 0.4.0 final software acceptance

Native and forced core-0 builds, sanitizer/PCM, legacy fixture migration, all four bank round trips, eight mixer strips, source routing and Frequency Lock, percussion loop evolution, independent Lead/Bass vibrato, graphics fallback, browser and installer protocol passed. Exact artifact hashes and recovery instructions are in release/0.4.0. Use the final release package in place of the integrated development candidate. The exact source of the intermittent hardware tone is still unconfirmed. Follow release/0.4.0/README.md for the FM-1 checklist and publication provenance.

### 0.5.0 development checkpoint

Native candidate (FM-1_73000500) in evidence/0.5.0/candidate.part-*; concatenate in order and verify FIRMWARE-SHA256.txt. Reuse the 0.4.0 source archive for the SDK and the pinned JieLi 20260730.1 archive from FAST_RESUME.md. The live installer still serves 0.4.0.

Implemented Density 0–8 with legacy scheduling tiers retained, 14% drum synthesis gain, direct PLAY EDIT key-mode toggle, four lighting modes, physical pending-key latch, Performance K1 20/Auto selector with software blit cadence, missed slots and XRUN delta, four-channel bank mixer meters, and an actual-Hz perspective wave surface. Density/key-latch host regression and native compilation passed. Software blit measurements do not establish delivered LCD FPS. These changes are partial and the active queue remains authoritative.

Next start with audio/DMA profiling and FX hot-path A/B, then complete the dedicated page redesign and drum consolidation. Verify graphics and control behavior, migration and extended audio tests, build the matching emulator and complete release publication only after acceptance. The reported 89 XRUN and 90% budget have not been reproduced or explained in software. Real 20 FPS and DMA behavior still require FM-1 measurement.

### 0.5.0 song generation checkpoint

EVOLVE > EDIT exposes stopped-only New Song and Reroll Sounds. Per-role and per-intention weighted pools, component locks, startup timer seeding, and stopped intention changes are implemented. V2 bank records store sound locks in the CRC-covered reserved word; legacy records default unlocked. Seeded tests cover 128 palette draws and eight different 10-second PCM renders per intention. Reroll preserves motif and musical RNG; locked Lead, drum kit, Nature and FX stay put. Exact current native candidate and evidence are under evidence/0.5.0/song/. The live installer remains 0.4.0.

Host dry/wet A/B showed overlapping timing ranges and does not explain the FM-1 DMA underruns. The SDK boot/config mismatch warnings also occur in the released 0.4.0 log; the two files match its archived inputs byte-for-byte. Keep the pinned inputs. Next implement and test FX optimizations with PCM A/B, finish dedicated navigation and drum consolidation, extend bank/lock and on-device variation acceptance, rebuild the browser target and complete publication. Physical 20 delivered FPS and DMA behavior remain open.

### 0.5.0 software release

Native, forced single-core, matching WASM, installer protocol, bank migration, control workflows, rendered PCM and worker fallback passed. Recover matching firmware/browser/source archives from release/0.5.0 and validate checksums. Both visualizers still require physical LCD and DMA timing. The FX control caching preserved tested pitched PCM signatures; host timing did not establish a speedup.

### 0.6.0 development candidate

The active queue includes the complete requested 0.6.0 addendum. The released 0.5.0 remains the installer baseline. The partial candidate fixes motif evolution at loop wrap, adds 10-second PLAY fullscreen with key-independent wake and transport priority, SETUP/About, Drum Presence and an oscillator-derived patch preview. Native, browser, staged installer protocol, motif boundary/Freeze, host fullscreen/workflow and archived 0.5.0 bank migration tests passed. Physical LCD, DMA and AIR are unverified. AIR is still unimplemented: the pinned BSP exposes BLE scan RSSI, but this candidate has Bluetooth disabled.

Recover the candidate under evidence/0.6.0 by concatenating each numbered set and checking SHA256SUMS.txt. Start the next run by reading NEXT_BUILD_ACTIVE.md under the existing project lock. Next implement the sequencer's independent long-form layers and sample-clock Play Along quantization, then continue mixer/UI, Vizz, waveform editing, intention motion, AIR and performance acceptance. No 0.6.0 deployment was made. If main is unchanged, fetch this exact checkpoint SHA and use it as the development base. Branch writes were rejected by GitHub while exact commit reads worked.

### 0.6.0 Play Along grid checkpoint

SOUND > Keyboard K3 selects Off (legacy default), Step or Half step. Held Play Along notes fire at the exact next selected grid boundary on the audio sample clock, retaining scale mapping. Release before the boundary cancels the note. STOP and keyboard mode changes clear the queue. The grid setting uses spare CRC-protected bank bits. Targeted ASan/UBSan timing tests passed. The matching native and WASM builds and installer package are in evidence/0.6.0/grid/. This is still a candidate. Production remains 0.5.0.

Next implement independent long-form arrangement layers and intention-aware generator quantization. Then finish mixer/UI, Vizz, waveform editing, intention motion, AIR and performance acceptance. Verify a GitHub ref can be written before release. This commit is recoverable by exact SHA.

### 0.6.0 role evolution checkpoint

Resumed from 3eab02a and verified both recovery archives. GitHub branch creation returned HTTP 422. The Work Git client had no credential helper or authenticated GitHub session, and a non-force push stopped before authentication. Production was untouched.

Added phrase-level role choices based on intention, phrase and bar, separate from sample RNG. Bass answers, pad voicings, lead accents, harmony intervals, piano/pluck figures, choir and drum downbeat accents evolve. Before first evolution or at zero variation the earlier decisions remain. Sanitizer tests cover 64 loops for every intention and loop length, reference stability and Freeze, plus motif and Play Along grid regressions. Native, WASM and staged installer artifacts are in evidence/0.6.0/roles/. Physical audio performance and long-session musical quality remain unverified.

Next implement returning motif memory and intention-aware generator quantization, then dynamic mixer, intuitive UI, flagship visualizer, AIR and full release acceptance. Fetch this exact commit if branch writes still fail. Production stays on 0.5.0 until acceptance.

### 0.6.0 motif memory and groove checkpoint

Resumed exactly from 9456729; verified both previous archives. The repository is active and the connector reports push and admin permissions. Even creating a development branch from main returns HTTP 422, so the failure is broader than the detached candidate. No production writes or forced push attempted.

The original motif returns every fourth evolution. V3 banks store the original motif, with v1/v2 migration seeding it from the saved melody. Piano and pluck offbeats use bounded scene-specific sample-clock offsets after evolution. Bass, drums and downbeats stay on the step clock. Tests checked Flow State's exact offset, motif return, Freeze and 0.5.0 bank migration. Native, browser and staged installer evidence are in evidence/0.6.0/memory/. Production still serves 0.5.0.

Next extend generator quantization to the full adjustable timing/strength profiles, then complete the dynamic mixer, navigation, flagship visualizer, AIR, performance and full acceptance. Recover the exact commit and verify the source and package hashes.

### 0.6.0 controls, visualizer and experimental AIR candidate

Resumed 2d3b6db under the lock and verified the memory archives. Added EVOLVE Groove Intention/Straight/Swing and 0–100% strength; bank-compatible defaults; All/Active adaptive musical channels; SETUP Quick Find; Resonance ribbons; separate performance timings; and intention-specific Lead/Bass motion. This mixer exposes the existing eight independently mixable musical tracks. Separate source/bus mixer controls remain open.

AIR now compiles using the pinned own BLE central stack, boots disabled, and scans compatible BLE MIDI advertisers when enabled. It has calibration, target including drums, Hold and bounded send smoothing. The platform supplies coarse RSSI buckets; actual antenna response, audio under radio load, and LCD FPS must be tested physically. The SDK-backed BLE alternative needs the full AC79 SDK; the own stack builds with the pinned packaging SDK. GitHub branch writes remain unresolved; use this exact SHA and the independent archive fallback.

Host controls, motif/grid, role evolution, stereo, AIR ramp, visual fallback, native, browser, and staged installer evidence are in evidence/0.6.0/integration/. The forced core-0 build was checked before the final intention motion presets. Production remains 0.5.0. Next complete the independent buses, full workflow/quantization acceptance and repeat final fallback/performance acceptance before any deployment. Do not treat the staged candidate as the released 0.6.0.

### 0.6.0 source buses and branch recovery

The development branch aquora-0.6.0-integration was created and verified at a12b10d without changing main or production. The candidate adds a Source Buses view with Nature, Sacred Bass, Reference, Binaural and FX controls and sampled post-routing meters. This view uses separate tabs from the eight musical channels. Binaural remains a stereo pair. Host workflow and four matched FX PCM signature comparisons passed. Native, forced core-0, browser and staged installer evidence is under evidence/0.6.0/buses/.

Production remains 0.5.0. Resume from this commit under the same lock. Next close the full generator/Play Along profile requirements and unified mixer/workflow acceptance. Then validate AIR and visual/software performance at 16-voice load, run full release acceptance and publish only the matching verified release. Physical AIR, display FPS and DMA performance remain unverified.

### 0.6.0 Play Along grid source checkpoint

Play Along now defaults to Auto for new settings and legacy v1 banks. Auto uses 1/32 at slower tempos and 1/16 otherwise. Live, 1/8, 1/16 and 1/32 can be selected. The ASan/UBSan host test passed exact onset samples, released-key cancellation and STOP clearing with leak detection disabled for this sandbox. This is a source-only update. The verified native firmware, browser emulator and installer archives under evidence/0.6.0/buses/ correspond to 924fbeb, not to the new grid source.

Next verify the new grid value through all four banks, then implement Play Along strength, feel and short-tap scheduling. Continue unified mixer and full software acceptance before building the release. Keep production 0.5.0.

Archived 0.5.0 bank migration and all four new grid settings passed an ASan/UBSan save/reboot/load round trip. Next implement Play Along strength, feel and short-tap scheduling.

### 0.6.0 short-tap and transport gate source checkpoint

Quantized key taps released before onset now play once at the chosen grid with a bounded quarter-step gate. Held keys use the existing release reconciliation. Scene changes discard queued onsets and STOP clears them. Generated note lengths now use the live transport period. ASan/UBSan grid coverage checks exact onset, one bounded tap, held key release and STOP. Next add strength and groove feel, test the actual panel path and audition the duration change against 0.5.0. The verified staged firmware and installer remain the 924fbeb package.

### 0.6.0 Play Along and unified mixer candidate

Resumed cf75fa5 under the project lock. Play Along supports timing strength and Straight/Intention feel on the shared sample clock. The archived bank fixture and four round trips passed. Panel simulation exercised key-down/release, short taps and Change Key through STOP and the next loop boundary. MIX All and Active now show musical channels and source buses together, with SELECT spanning both rows. Quick Find includes Play Along and FX / Nature. Native, forced core-0, browser and staged updater tests passed. The package preserves FM-1_73000600. The compiler still reports the pinned SDK boot/EQ reference differences.

Recover source and stage archives under evidence/0.6.0/unified and verify SHA256SUMS.txt. Production remains 0.5.0. Next complete the 240×240 feature reachability audit and editable patch-shape acceptance, extend generator profile and long-session checks, and complete release acceptance before publishing. FM-1 measurements remain open.

Follow-up acceptance on cb1c8b8: all five visual-worker modes produced the same fixed-seed PCM and frame, and successive frames evolved. Quick Find Play Along and FX/Nature, plus Drums, Banks and Performance shortcuts, passed in the panel simulator. The step scheduler passed an eight-hour equivalent simulation in 16/32/64 modes at compressed sample durations. This checks musical counters and role activity rather than eight hours of output audio or physical FPS. The matching candidate is still under evidence/0.6.0/unified/. Source and package were independently reconstructed from GitHub and matched their SHA-256 checksums. Next implement editable patch shape controls and complete the generator profiles and full release acceptance.

### 0.6.0 patch editor and release candidate

Resumed from 2f5cb36 under the lock. Added per-track brightness, attack and release controls, cached oscillator/envelope display and bank persistence. Migrated 0.5.0 banks and verified audio/preview changes. Intention and Swing apply bounded evolving lead/harmony offsets as well as piano/pluck offsets, with slower attack/tempo constraints and straight timing in Frequency Lock. Quick Find adds patch shape, Hz, binaural, Sacred Bass, New Song, display/LEDs, About and source isolation. All 17 routes passed panel checks.

Fixed a new SRAM-to-flash linker relocation by forcing the brightness helper inline. The pinned boot and EQ reference warnings remain unchanged. Native, forced core-0, WASM and staged updater checks passed. Use release/0.6.0 and evidence/0.6.0/editor for recovery. The public 0.5.0 site remains in place pending final release acceptance and deployment. Physical FM-1 checks remain open.
