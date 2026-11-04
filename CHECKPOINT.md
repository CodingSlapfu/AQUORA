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
