# Development validation — NOT release acceptance

Current source fingerprints: `source-input-sha256.json`. Firmware artifact
fingerprints: `artifacts/SHA256SUMS.txt`. Baseline release remains v0.6.5.

Verified during development:

- Native and forced-single-core pi32v2 builds produce provisional
  `FM-1_73000606` packages. Native image 254324 bytes, data/BSS 80436/98304,
  pool 96688/344064. Forced single-core image 252628 bytes, data/BSS 77248.
  Both pass toolchain SRAM-call, multiply/load, float and MMIO checks.
- 17 existing sanitized DSP tests passed; see `dsp-final-summary.json` and
  individual `dsp/*.log`. Initial corrective failure is preserved in
  `dsp-initial-summary.log`; the zero-variation defect was fixed and
  `dsp/corrective_061.log` records the successful targeted rerun.
- `role-evolution.log`: 117 intention/mode/loop cases, changing role targets,
  all eight genuine post-gain role meters observed across suitable contexts,
  bounded fade slopes, Freeze and STOP. This is not a full audible-stem or
  hardware transition-quality acceptance.
- `visual-pressure.json`: 48 actual renderer cases under normal/pressure and
  healthy/off/bad/late worker fixtures. Host timing is not FM-1 timing.
- `canvas-pipeline.log`: 1000 delayed-DMA frames, repeated begin/cancel,
  partial blits and synchronous one-shots. Forming/settling/fullscreen sample
  frames have exact 240×240 RGB equality before/after the canvas pipeline.
  Full Original baseline animation/fidelity acceptance remains incomplete.
- `startup.log` / `startup-analysis.json`: 49 skip inputs consumed without
  starting transport; exact hosted firmware `app_render` PCM at master 0,
  2048 and 4096; muted output zero, nonzero bounded stereo cue, low DC,
  eight-second completion, zero intro worker/geometry jobs. Not a physical
  DAC/speaker recording. Startup sound setting/migration remains unfinished.
- Frame-policy and LED mapping/duty/beat deterministic tests pass. External
  MIDI synchronization has not been implemented, so its LED acceptance is open.
- Requirement-gate negative tests reject omitted issues, unsupported completion
  and deferred work absent from the next queue. Release mode also rejects
  unreviewed owner comments, unresolved software and incomplete reconciliation.

**No universal MIDI implementation, no complete acceptance, no publication.**
Read `NEXT_RESUME.md`; do not flash these unaccepted development artifacts as a
completed release. Physical performance, brightness, RF, listening and direct
hardware updating remain owner-unverified.
