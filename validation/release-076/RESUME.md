# AQUORA v0.7.6 development checkpoint

This is an in-progress candidate, not an official release or a deployed installer.

Baseline: v0.7.5, d0bde846544f388be16107ae8dab2610a130b38f.
Development source: 270717f6b50216ff80fa529ab6ef7ed2cc19368d (tree 5143e4f83c036f363bcc3b627093dbd73fd01efd).
Candidate native identity: FM-1_73000706. Target remains original FM-1 Puya 0x856014.
Native 611496 byte SHA-256: 7af7cb5723e2af8c376c1782efa8d6c7ff6ec8908dc6d7e1a5cf5720761a4632.
WASM SHA-256: 1c9150e9aa91c7d871c9d93a8fda484bffc5d9c0fb63f6f0713fe529b485422e.
Single-core packaging SHA-256: 93fe4188dc3d13d2865dd63907e82b649d1c57f446784a7c47774e08cbd4060a.

Changes include Lead echo, complementary Pluck cadences, coordinated Neoclassical piano registers, patch-sensitive wind articulation, 180–230 ms automatic total string-roll spread with manual setting preserved, and reductions to EVOLVE render arithmetic with an extra input poll before score preview.

Checks passed: byte-identical independent native rebuild, single-core compile, WASM compile/module load, host LCD/lighting/startup and 24 input/load cases, arranger, transitions, exact score-vs-audio preview, 504 long-form composition configurations, 40 automatic string rolls, 84 intention/mode cap settings, and one-byte tamper rejection in the locally staged browser installer. Physical flash, listening, LCD and audio deadlines have no hardware measurements for this candidate.

Remaining: finish per-bed Nature expansion and complete instrument-aware performance and piano pedal handling. Profile EVOLVE, all three visualizers and physical controls during real FM-1 playback, and finish outstanding acceptance in Issues #6, #72, #73, #75 and #76. Preserve and independently verify the existing 0.7.5 Web Installer, all rollback routes, and KIRA-9. Existing site returned 0.7.5 / FM-1_73000705 with a matching 0f5e1e133f48f34fa8a0a3682db6749a71f8ed79bd517872e60942e3cd0542d7 firmware package. No official 0.7.6 release or Site update was made at this checkpoint.
