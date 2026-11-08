# AQUORA 0.6.9 — active outstanding queue

- BLOCKED #62: actual application, SysEx RAM updater and USB rescue compatibility for 0x856014/0xB36014. Existing software gate covers through #61 only. See validation/release-069/FINAL_RECOVERY_BLOCKED.md for exact source locations, pinned encrypted rescue-loader audit and Puya-specific live-save suspend/status verification gap. Do not publish before changed-path gates pass.
- PARTIAL website recovery: prepared site069-publication-25m.tar independently inspected; every member <=25MiB (largest24MiB). Package SHA256 9fc2eeeab304cc4d22d5973d22711e3947189ad5f73cce284dfedd0f534cddb4. Reuse staged assets, repackage matching #62 artifacts after compatibility passes, then deploy EXISTING public Site and verify actual HTTPS installer/firmware/emulator.
- Preserve all passed software tests and exact artifacts through #61; evidence validation/release-069/SOFTWARE_GATE.json, STAGE.json, RUNTIME_SOURCE.json. No blanket rebuild/retest.
- OWNER-HARDWARE-UNVERIFIED: physical old/new flash devices, direct update/rollback, LEDs, RF/AIR, FPS/XRUN, controls and artistic listening/visual acceptance.

Both performance MIDI transports deferred; AIR required; KIRA-9 untouched. Keep 067/068 rollback. No recovery loops.
