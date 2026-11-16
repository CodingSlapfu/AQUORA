# AQUORA v0.7.14 recovery checkpoint

Resumed from 584345a3f0a72290f18da9af92aa4e45148b8f9c after Work command execution recovered.

- Drum HIT test passes at 0, 25, 50, 75 and 100 percent. All 16 lanes occur at the default 25 percent. FX auditions the selected part on the 16-part mixer.
- The VOCALS effects page controls independent chorus, room, echo and note-on scale pitch correction. Impulse, dry bypass and tuning tests pass. The recorded vocal regression covers all five modes with fresh default settings and verifies playback, disable/release and independence from legacy choir patch locks.
- Bank compatibility regression passed. Native build and byte-for-byte reproduction passed for FM-1_73000714. Checkpoint firmware SHA-256 is e23b5d509a9f0df066655ae7d16ffc9540a99ae2020903232286cd6a80338071.
- Pending: expanded ambient mixer effects, full drum pattern step editing and kit sound differentiation, end-to-end vocal synchronization and performance acceptance, final transport and browser installer gates, GitHub release and deployment. v0.7.14 is unpublished.

Transport fixture results: duplicate key, watchdog, Sacred Bass release, and 32 saturated stop/restart cycles passed. The legacy fixed reference isolation assertion at tests/transport_hit.c:90 failed on both 584345a and this checkpoint. Its cause still needs resolution before release.

## Transport fixture recovery: GitHub-hosted verification

A subsequent Work attempt had no usable execution environment, so it made no source changes.
A separate GitHub-connected correction was committed without changing firmware DSP:

- `040cd4e050de7b368287197768ca81601d02a1a3`: fix `tests/transport_hit.c` to set `tone_level=0` before asserting the initial all-muted silence (the independent reference oscillator must not be conflated with instrument tracks), then allow 64 render blocks for the documented click-free reference-gain ramp to settle before asserting silence after tone disable. Keep enabled-frequency, STOP silence and remaining transport assertions.
- `6bcb3a27f33f0dec59b7335de0f0525869f2dd15`: add targeted workflow `.github/workflows/aquora-0714-transport.yml` for GitHub-hosted compilation and execution of the complete transport fixture. Workflow run: https://github.com/CodingSlapfu/AQUORA/actions/runs/43968874123.

**Verification status:** The workflow was in progress when this handoff was written; do not treat it as a PASS without reading its actual result. No native firmware was rebuilt after the fixture-only correction, and no official release or installer deployment was performed. Existing open gates above remain open.
