# AQUORA v0.7.21 live testing verification

- Installer https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.21/install/
- Emulator https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.21/emu/
- Site source f6dd439711da25b3a4dfba43336a84a423943fa5, saved version 213, deployment appgdep_6afc0ebac1108191b715807241dcce9e.
- FM-1_73000721, 611496 bytes. SHA-256 b9d03b37de5fee3ac9112ea1a9846b0d65439286cad1687f5eabb4e41285cd99. Live bytes match the twice-reproduced firmware, staged archive and GitHub backup.
- The live installer accepts the exact bytes and rejects a one-byte change with the same identity. Live WASM SHA-256 5c77c1e75211dfea1146908306cf2156836334671fa9b501b6eb55280c709407 matches the staged module.
- Stable installer HTML and v0.7.14 stable and v0.7.20 testing firmware hashes match their prior verified values. All prior tracked website assets were retained in the deploy archive.
- Supplemental checks passed: 24 host input/render/worker cases, 12 loaded vocal FX and visualizer cases, 30 seconds of full IDM mix at maximum vocal-control settings with a peak of 23002/32768 and zero full-scale samples. These are host results. Physical FM-1 timing, audio and display acceptance remain open.

See RESUME.md for source, covered behavior and outstanding requirements.
