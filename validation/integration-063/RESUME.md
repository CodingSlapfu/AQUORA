# AQUORA 0.6.3 active development
Base: da64632 documentation / b6d4522 accepted v0.6.2 firmware. Originals preserved.
Owner: current Work run, exclusive /workspace/.aquora-release.lock.
Branch: aquora-0.6.3-development. This checkpoint is NOT a release.
Implemented pending acceptance: global SAVE snapshot/async A-B flash transaction;
100us progress slices with P25Q80H suspend, header-last/readback; pure black/white;
continuous mixer wells; one PLAY preview; scoped SEL/display instructions;
no intro visualizer jobs; steady On/Keys/Dance button masks; standing-wave Polar.
Local passed: 270 appearance views; all-page SAVE and canvas/navigation checks;
four-bank recall/overwrite/failure; async 400 power-cut boundaries + CRC fallback
under ASAN/UBSAN. Timing of 106us is a mock, NOT device evidence.
Next: inspect CI native/platform jobs and fix failures; audit async interactions,
refresh captures/manual, final acceptance, versioned release backup, both websites.
Existing 0.6.2 deployment remains untouched. Physical FM-1 testing belongs to owner.
