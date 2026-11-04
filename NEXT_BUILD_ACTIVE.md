# AQUORA active follow-up

The implemented 0.2.0 software scope is archived in [archive/0.2.0-requirements.md](archive/0.2.0-requirements.md). Verification logs live in evidence/0.2.0/ and artifact hashes in release/0.2.0/. Keep 0.1.0 available for rollback.

## P0 — Confirmed hardware failure: 0.1.0 → 0.2.0 browser upgrade (2026-11-04)

- [ ] **Fix direct AQUORA-to-AQUORA update with no stock restore.** On FM-1 hardware running the preceding AQUORA build (reported as 0.1.0), user attempted the just-published 0.2.0 browser installer and received **\"No answer. Check that the FM-1 is running sound-healing.\"**. Treat 0.1.0 → 0.2.0 as **FAILED/UNVERIFIED on hardware**, not completed. User had previously successfully installed and played 0.1.0. The new 0.2.0 package is published as identity `FM-1_73000200`; 0.1.0 is `FM-1_73000100`; `device/fm1.toml` remains `name = "sound-healing"`, `id_prefix = "FM-1_73"`. A matching family does not prove a functioning MIDI SysEx handshake or bootloader upgrade path.
- [ ] **Investigate actual transport and protocol, not only packaging tests:** Determine whether no-answer occurs in device discovery/HELLO, input/output pairing, name matching, SysEx permissions, USB device claim, previous firmware's responder, or loader handoff. Capture browser installer diagnostics with stage, selected ports, transmitted SysEx command, response/timeout, old firmware build identity, loader identity, and USB re-enumeration. Check source in pinned `fm1-base` / `felucca-bsp` and the **actual 0.1.0 installed updater code** (backup archive), not just 0.2.0 tests. Test the 0.1.0 → 0.2.0 compatibility path against a real or byte-identical 0.1.0 protocol fixture; host simulated discovery tests alone did not catch this failure. Confirm if recovery/update mode can be entered safely without a stock restore, and document button combination only after checking platform firmware behavior.
- [ ] **Repair and verify:** Publish an installer that accepts the installed 0.1.0 response or offers verified alternate port/update-mode guidance, refuses wrong devices, identifies the exact step on failure and never claims success without boot verification. Preserve updater family `FM-1_73` and rollback, guard half-written updates, and verify direct 0.1.0 → fixed-release upgrade plus rollback on physical FM-1 with owner testing. Do not silently recommend stock restore as default. Do not claim the bug is fixed solely because generated installer, .fwsc and tests pass.

## Owner hardware verification still open

- [ ] Direct upgrade and rollback/recovery from the failed 0.1.0 → 0.2.0 path **once repaired**.
- [ ] Physical STOP/restart and master-zero safety, balanced and audible bass and drums, all controls, pitch motion, intended sound and headphone separation.
- [ ] Measure real core-1 execution, forced single-core behavior, typical and sustained stress audio/FX callback timing, peak/percentile margin, visual responsiveness and zero xruns against 0.1.0. Follow the hardware checklist in release/0.2.0/README.md.

No device measurements have been inferred from host timing. Add new software work only from verified regressions or a new requirement.
