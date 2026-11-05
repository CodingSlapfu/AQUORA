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
