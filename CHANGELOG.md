# AQUORA changelog

## 0.6.6 — 2026-11-07

- Completed non-MIDI software scope: accurate physical LED duty and internal-tempo PLAY pulse, AQUORA Sans cinematic intro and original FM signature, full Original geometry, moving/settling Cymatics sand/water, audible-pitch response, musical role entrances/exits and fades, AIR states and page-specific performance telemetry.
- Reconciled 45 requirements and 74 owner comments. USB/BLE performance MIDI explicitly deferred and disabled; AIR and update SysEx preserved. Physical/aesthetic/listening checks remain owner-unverified.
- Software acceptance run 42353932681 passed, reusing exact native/repro/single-core and updater proofs. Published release v0.6.6 at 295c1d1; verified all 11 public assets, inner/outer hashes and independent archive. Live Site v184 and exact gh-pages mirror deployed. Evidence: validation/release-066/. Prior releases/backups preserved; KIRA-9 untouched.

## 0.3.0

- Guarded held-key lifetime, suppressed held-key restart triggers, linked musical Sacred Bass to the active bass gate and added Voice Inspector. Evidence: transport_hit.log and UI STOP/restart tests.
- Added direct percussion activation, audible routing, 0–70% optional HIT, pulse/meter and all live control values. Evidence: next_acceptance.log, transport_hit.log, percussion_crystal.log and knob_matrix.log.
- Added four arrangement slots with validated musical snapshots, explicit overwrite and two-sector flash recovery. Evidence: arrangements.log, bank-flash.log and motion-store.log.
- Preserved Original and added Water Crystals with bounded table geometry and adaptive render cadence. Evidence: worker.log and ab.log. Both modes and simulated faults preserve audio bytes in the comparison.
- Compiled native and forced single-core, validated the updater package and matching emulator, and retained both prior builds. Evidence: native.log, singlecore.log, package.log and installer_ports.log.

All logs are under evidence/0.3.0/. The original requirement text remains in archive/0.3.0-requirements.md. Physical verification is tracked in NEXT_BUILD_ACTIVE.md.
