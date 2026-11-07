# AQUORA 0.6.8 corrective development checkpoint — NOT RELEASED

Read NEXT_BUILD_ACTIVE.md. Resume outstanding scope; do not rebuild 067.
Checkout: /workspace/scratch/22ea018dd526/aquora-resumed.
Reconstructed full source: /workspace/scratch/0c64376c3cd0/source.
Ownership: session 0c64376c3cd0 releases its own lock at handoff. Next session checks once before editing. No competing builds.

Implemented candidates:
- #46 controls dispatch between LCD strips, abandon stale frame on input. 448 deterministic strip-arrival fixtures: STOP/PLAY, 7 pages, all 7 encoders/HOME, four boundaries, audio callbacks between strips. This is not a hardware event-scan/IRQ latency measurement.
- #45 066 Cymatics colors, soft-ceiling response, stronger onset particle displacement; Original exactly unchanged in 294 normal/fullscreen snapshots (77,616 vertices); Chladni convergence/pitch/bounds/settling pass. Perceptual 150% improvement not established.
- #49 3x3 EVOLVE markers and light-theme contrast. Actual renderer inspected; full matrix pending.
- #47 hush tonal anchor targets (quiet bass + drone), not yet post-gain dead-zone acceptance.
- #50 restrained AUTO glide/vibrato, manual preserved, bank AUTO profiles refreshed. 63 targeted expression profiles and 189 profile/567 form scenarios pass. PCM A/B and full migration/control regressions pending.

Both final native builds pass (dual/single-core); source image 263956 / 261944 bytes. Candidate packages and SHA256 in validation/release-068/artifacts and SHA256SUMS.txt. They are development packages only: NOT updater/release/owner-accepted. No live publication.
Builds run sequentially. Toolchain: /workspace/scratch/759587c0312e/jieli-verified/jieli-linux-toolchains-20260730.1. AC79_SDK points at reconstructed vendor/ac79-package-files. Host binary device/build/host/sound-healing_host.

Tests: tests/input_bands_068.py, tests/expression_068.c, tests/visual_response_068.py BASELINE_WORKER OUTPUT. Exact logs in validation/release-068. Old tests/visual_semantics_066.py Original-reference assertion fails identically on unchanged 067 and candidate; direct candidate-vs-067 comparison passes. Do not call that an introduced regression or discard the direct comparison.

NEXT: finish #46 input/state/LCD latency instrumentation and queue/button/load/fallback matrix; targeted post-gain/audio/control/migration validation for changed code. Then implement #51/#52/#53/#54 and finish #45–#50/#48 acceptance, requirement matrix, WASM/native/repro/updater/migration/release gates. NO publication until ALL blockers pass. USB/BLE performance MIDI deferred; AIR required; KIRA-9 untouched. User owns physical acceptance.
