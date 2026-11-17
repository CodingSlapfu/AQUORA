# Verified v0.7.20 testing deployment

- Live installer https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.20/install/
- Browser emulator https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.20/emu/
- Site source 487cc1aa62676d940d3d2a5b5696f14ac0ec97e2, saved version 212, successful deployment appgdep_6afbfb29d6148191bb2e2b52dea3a83e.
- Public .fwsc 611496 bytes, FM-1_73000720, SHA-256 04ce60bcf1621f185ebd4dbd349f97128b71b85f979e5fe784d6bfb4948acd6f. Public download, twice-reproduced native output, archive and fetched GitHub blob are byte-equivalent. The live installer accepts these bytes and rejects a modified same-identity file.
- Live WASM SHA-256 1f393c05adbc52d2cdfe1dcf0fb4a00fb87b0d505e90254386affc49e1b9e012 matches the staged emulator.
- Stable installer HTML is byte-identical to pre-deployment. Stable v0.7.14 SHA-256 d3de83af71ba084c61dc88b1804e814200d7ae69439480d2aecfcb886d09ec6e and v0.7.18 testing rollback SHA-256 cae435655f4a7387193dc2779d88ad6e423c8a5cbdd747e590ee5eaae2154272 both verified over public HTTP. Prior tracked site client assets retained. No new stable release.
- Supplemental software checks passed for both context-specific K4 controls and 12 loaded host cases combining enabled vocals, ensemble, mixer FX, three fullscreen visualizers and four worker conditions. These results do not establish physical FM-1 timings or intelligibility.

For engineering state, incomplete scope and next steps, see RESUME.md in this directory.
