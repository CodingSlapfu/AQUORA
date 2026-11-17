# AQUORA v0.7.20 testing recovery checkpoint

## Exact build

- Source 3ecd2a0794bb26e496e6380732ee4f27af43b661. Branch aquora-v0.7.20-corrective-development. Previous engineering checkpoint 565a0a3677928988d2709782902a89d81645c1ea. Verified v0.7.18 backup 2645a06 is retained separately.
- Native updater FM-1_73000720. 611496 bytes. SHA-256 04ce60bcf1621f185ebd4dbd349f97128b71b85f979e5fe784d6bfb4948acd6f.
- Two builds with pinned/reused JieLi 20260730.1 produced byte-identical FWSC packages. Native image 479092 B, data/BSS 82524 of 98304 B, pool 271428 of 344064 B. Target linker and static budget checks passed.
- Host UI and matching Emscripten 3.1.64 browser build passed. WASM instantiated. Staged web installer accepted exact bytes and rejected a one-byte change under the same identity.

## Implemented and verified in software

- Original portrait through LCD high-byte-first RGB565 canvas, pixel-for-pixel regression. Recorded audio and portrait masking checked for mute, solo, and isolated sources.
- Fresh OFF, prompt first scheduled phrase after enabling, and re-entry at intention changes. Host checks covered PLAY, SOUND, EVOLVE, MIXER, DRUMS, SETUP, VOCALS, VOCAL EFFECTS and Performance Mode using all three visualizers. Targeted tests exercise direct vocal PCM across all five existing modes. Existing mixer mute/solo and source isolation rules are still applied.
- K1 global enable, K2 presence, K3 independent singer level and K4 vocal FX send. Isolated K3 0/50/100 peaks 0/1820/3641 with instrumental choir gain zero. Knob settings survive intention changes within the session.
- Featured vocal verse/chorus/cadence, sung-to-pluck response, longer holds, delayed vibrato and selective accompaniment ducking. Musical balance still requires listening on the device.
- One real recorded word, “day,” selected for some Featured cadences. CC BY 4.0 recording credits and source and edited PCM hashes are in device/docs/VOCAL_CREDITS.md. Measured PCM, sustained sample loop, natural release and three source-position mouth poses in software. The face follows actual pitch contour and held notes while audible direct vocal activity drives articulation. Six wordless recorded vowel samples remain available.
- Mode, song, and section-aware vocal chorus/reverb/echo, manual edit locks, a nonzero baseline, stronger compressed lead gain, role-aware smoothed instrument sends, and bounded larger Cinematic space and Experimental/IDM feedback. Existing Mixer textures continue to pass their targeted regression.
- SLICER K4 selects AUTO and seven styles. Pattern distinctions, AUTO evolution and protection of current kick anchors passed. VOCALS K4 remains independent.
- V9 migration, V10 four-slot round trips, corrupt bank rejection, 16-lane drum edits, and 24 host input/render/worker load cases pass. These measurements are software only.

## Remaining owner acceptance and development

- Physical FM-1 LCD palette, install, rollback, XRUNs, controls, and mix/listening acceptance. No new stable release. Existing firmware and rollback links stay available.
- Broader sung vocabulary, expressive consonant coverage and intelligibility testing, distinct vocal mode selection in AQUORA, dedicated extra vocal processors, deeper per-track effect chains, and the full composition and listening-quality pass remain open. This version demonstrates one genuine lexical unit and targeted accompaniment changes.
- Next step is physical testing of the separately labeled testing installer plus continued development from this branch. Use a new identity for changed native bytes. Preserve v0.7.14 stable and v0.7.18 testing. V10 banks are incompatible with v0.7.13 on rollback.

## Installer

Matching assets staged for /testing/0.7.20/install/ on the existing AQUORA website. Live status is recorded separately after the public download is verified against this hash.
