# AQUORA v0.7.32 testing verification

Source ca312d34117d2a238d90bb27504a575994263c4f, based on v0.7.31 checkpoint 6b99b235232469bf5935f4a6ae5fc6322227ba3d. FM-1_73000732, original Puya 0x856014 hardware.

| Allocation | v0.7.31 | v0.7.32 | Recovered |
|---|---:|---:|---:|
| Application flash | 581,500 / 581,564 | 560,816 / 581,564 | 20,684 bytes |
| DSP pool RAM | 325,960 / 344,064 | 262,520 / 344,064 | 63,440 bytes |
| .data + .bss | 81,756 / 98,304 | 81,756 / 98,304 | 0 |

Standalone flash packing trial measured 561,024 bytes. Final audio FX storage isolation saved a further 208 flash bytes and 63,440 pool bytes. Legacy drum/vocal storage payload went from 126,513 to 105,378 bytes with reversible predictive coding. Decoder runs once before audio. Score preview retains the full scheduler state and no longer allocates reverb, delay or vocal-FX history on hardware. The linked-symbol audit inspected the portrait, fonts, sample tables, screen buffers, audio effects and score state. Required artwork, font sizes, legacy word IDs and FX lengths were preserved. No Flite code or synthetic word assets return.

Validation: exact equality of 174,855 sample bytes against the raw-table reference, including the unchanged ten new samples. Four matched Cinematic/Neoclassical dry and mix captures were byte-identical to v0.7.31, retaining its restored v0.7.27-style phrasing. All ten recorded human words played cleanly at source pitch and ±2 semitones with verified endings and direct-PCM glow. Automatic and manual vocabulary checks passed. The software score preview matched 5,912 events across 110 cases. All 16 Favorites saved and recalled across simulated reboot, and V1–V10 bank migration passed. The test script allowed auto-start to finish and stopped playback before recall. WASM rendered 512,000 audio frames and a moving 240×240 framebuffer. Simulated updater detected the release identity and exercised update, resume, mismatched identity and corruption detection. No recognition-based acceptance gate was applied. No physical FM-1 callback timing or listening result is claimed.

Independent native and WASM builds reproduced these exact hashes.

- AQUORA-0.7.32-testing.fwsc a0dd25af57d641de7dc0c1851cead8f77fd4df46c60f95033cfe777215b24cb8
- app.wasm 2c73c3da79c08c7f4e5c52aa6faaf8cbd55c8907a997c673d6a7601f45e7c3d0
