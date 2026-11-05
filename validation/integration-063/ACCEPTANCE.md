# 0.6.3 software acceptance

Accepted source: 0f2716ba0499def94884860248f9b0e887b0fd0d.
Actions 42022969585: host/DSP sanitizer+legacy bank migration, host panel,
WASM validity, native, native reproducibility, forced single-core, staged matching
package and direct-updater identities: all passed. No source DSP changes.
Local additional evidence: 60 theme/font/background/visualizer combinations,
180 full-resolution normal/fullscreen/return frames; exact RGB565 canvas in each
fullscreen band. Intro geometry_jobs=worker_jobs=0 before timeout.

The production async flash transaction and RAM slice were exercised through a
P25Q80H SPI model under ASAN/UBSAN: 100 interruption boundaries in each of four
banks plus CRC corruption retain the previous valid save. Modeled max 106us is
NOT physical timing. Native disassembly verifies RAM driver has no XIP calls.
P25Q80H v1.7 primary specification: https://www.puyasemi.com/download_path/数据手册/Flash%20芯片/P25Q80H_Datasheet_V1.7.pdf
100us progress between 75h suspend / 7Ah resume; WIP must clear before XIP returns.
A failed suspend takes the existing bounded completion/fault path; uninterrupted
music is not promised during hardware flash failure. Header-last CRC A/B format
and original bank IDs/patch mappings remain intact. OTA/UBOOT waits for live save.

Physical FM-1 SAVE latency/stability and power-cut behavior, post-install/cold
boot, sound quality, LEDs, RF, delivered FPS and direct upgrade are owner-unverified.
