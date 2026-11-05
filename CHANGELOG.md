# AQUORA changelog

## 0.3.0

- Guarded held-key lifetime, suppressed held-key restart triggers, linked musical Sacred Bass to the active bass gate and added Voice Inspector. Evidence: transport_hit.log and UI STOP/restart tests.
- Added direct percussion activation, audible routing, 0–70% optional HIT, pulse/meter and all live control values. Evidence: next_acceptance.log, transport_hit.log, percussion_crystal.log and knob_matrix.log.
- Added four arrangement slots with validated musical snapshots, explicit overwrite and two-sector flash recovery. Evidence: arrangements.log, bank-flash.log and motion-store.log.
- Preserved Original and added Water Crystals with bounded table geometry and adaptive render cadence. Evidence: worker.log and ab.log. Both modes and simulated faults preserve audio bytes in the comparison.
- Compiled native and forced single-core, validated the updater package and matching emulator, and retained both prior builds. Evidence: native.log, singlecore.log, package.log and installer_ports.log.

All logs are under evidence/0.3.0/. The original requirement text remains in archive/0.3.0-requirements.md. Physical verification is tracked in NEXT_BUILD_ACTIVE.md.


## 0.6.1 software release

Published from `23c8db04538fec13171a6e380501bb153f949311`. The
[publication record](release/0.6.1/PUBLICATION.md) identifies the passing
independent build, reproducibility comparison, assets, Site commit,
checksums, host scorecard and verification limits.

Implemented FM patches and kits, contextual stereo Nature Beds, passive
advertisement AIR, LED modes and matrix diagnostics, direct parameter
navigation, live role telemetry and observed arrangement graphs, a
skippable startup sequence, immersive fullscreen compositions and battery
status with a two-second fullscreen fade. Added audible stopped patch/kit
preview, Nature audition, evolution levels/pace and confirmed song actions.

The active queue retains hardware acceptance and evidence gaps. Historical
scope is preserved in the v0.6.0 tag.
