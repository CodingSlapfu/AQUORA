# AQUORA 0.3.0 software checkpoint

Matching release archives and instructions are under release/0.3.0/. Read NEXT_BUILD_ACTIVE.md for the remaining FM-1 validation. The 0.2.0 and 0.1.0 archives remain rollback options. Check publication status in release/0.3.0/PUBLICATION.md.

Use nonblocking flock on /workspace/.fm1-sound-healing-build.lock before editing, compiling or publishing. Reconcile remote main and local changes before editing. Restore the checksummed source archive to recover pinned vendor and SDK files. Preserve updater family FM-1_73.

## 0.4.0 development checkpoint

Resume from this development commit under the exclusive lock. The installer still serves the published 0.3.0. The 0.4.0 partial candidate built with JieLi 20260730.1 and passed the targeted transport/HIT ASan and UBSan run. Logs and candidate hash are in evidence/0.4.0/. Recover the toolchain from the verified EIN1-JieLi-20260730.1.tar.xz backup (same pinned SHA in FAST_RESUME.md), extracting with tar --no-same-owner. Recover SDK and vendor from the 0.3.0 source archive as before.

HIT now supports 100% across the DSP, controls and bank restore. At 100% the host test hit all 7000 eligible steps. Other probability, kit, and full acceptance remain in the active queue.

The isolated fixed-reference oscillator reproduces the STOP/PLAY and scene-change signature in PCM when explicitly enabled. There were 528 rising crossings before the scene change, after the scene change and after restart. At zero level the isolated output was silent. The unit's active tone source is still unknown. Next implement independent source isolation controls and PCM checks for Sacred Bass Reference, binaural, generated tracks, and FX. Do not close the reported P0 from this reproduction alone. Continue with all outstanding requirements before publishing.

The display scheduler now enforces a 50 ms minimum between display updates while retaining slower geometry updates under load. Host Percussion, Arrangement and Knob Matrix scripts passed. The updated partial candidate is backed up as evidence/0.4.0/candidate.part-*; concatenate in numeric order and verify candidate-sha256.txt. It is a development artifact, not a release. Physical frame cadence and full release acceptance remain open.
