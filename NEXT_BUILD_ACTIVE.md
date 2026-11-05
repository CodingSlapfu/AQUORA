# AQUORA 0.6.1 corrective acceptance queue

The authoritative scope is GitHub issues #1–#11 plus the second visualizer,
passive AIR and intention sound profile requirements. The previous active notes
are preserved at the v0.6.0 tag.

| Scope | State and remaining acceptance |
| --- | --- |
| #1 LED | Partial. Mode masks, quiet dim drive, event pulses and matrix-slot diagnostic are implemented. Finish the complete output and persistence test matrix. Physical output is hardware-unverified. |
| #2 Navigation | Partial. Added Quick Find destinations and HOME + EDIT access. Complete the exhaustive per-control edit, DSP feedback and persistence matrix, and document all routes. |
| #3, #5, #6 Sound | Partial. Added 13 intention profiles with four styles, procedural stereo beds, six FM patches and four FM kits. Corrective DSP test covers selections, locked/off behavior, stereo and fade. Finish Nature character controls, listening A/B and full stress coverage. |
| #4 Live graphs | Partial. Measured mono mix trace, selected patch and envelope models, source meters and observed role event timeline. Complete selected-role scope, editing context graph modes and telemetry correctness tests. |
| #7 Optimization | Partial. Half-rate interpolated Nature rendering, bounded telemetry, geometry and cached LED updates. Host timing is a proxy. Finish hotspot audit, native memory/size comparison and full performance scenario scorecard. |
| #8 Introduction | Implemented in source. Host tests cover all individual supported input skips. Finish combined-input, timing, volume and recovery cases. Physical startup audio and input feel are hardware-unverified. |
| #9 Evolution | Partial. Development levels, pace, manual next phrase, motif returns, section energy and evolving drum fills are implemented. Complete the multi-seed/level uniqueness, role and listening matrix. |
| #10 Fullscreen | Both fullscreen camera compositions and host previews implemented. Check final occupancy, clipping and audio stress. Delivered LCD FPS is hardware-unverified. |
| #11 Battery | Implemented and tested in host: unknown, low, three bars, USB, stepped fade and exit. Final native/single-core/WASM acceptance is tied to the branch tip. Physical battery indication is hardware-unverified. |
| AIR | Passive legacy BLE advertisements with raw strength, packet count, calibration, smoothing and stale signal handling. A synthetic signal regression exercises the controller. Physical hand response and RF/DMA coexistence are hardware-unverified. |
| Release | Pending full issue acceptance. Preserve 0.6.0 production, 0.5.0 rollback, verified source and artifacts. |
