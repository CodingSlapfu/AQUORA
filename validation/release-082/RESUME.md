# AQUORA Issue #82 — verified release checkpoint (2026-11-15)

**Official release complete:** https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.7.8  
**Accepted source/tag:** `406f4a64b66063fa502a6929a40c08637aa3c585`  
**Actions software acceptance:** https://github.com/CodingSlapfu/AQUORA/actions/runs/43755711533 (all build-test and publish jobs successful)  
**Firmware identity:** `FM-1_73000708` (original Puya `0x856014` FM-1 only)  
**Firmware SHA-256:** `1ddda3a8264d02be486e4e8c04eb73fbaddaf883d6be9849d7ddafd844dd8605`  
**Official ZIP SHA-256:** `c438f43210755a17154d83247730712497c5ed638aac6f49df91dd27f1687b04`  
**Unchanged v0.7.7 rollback SHA-256:** `2b72e959f5efc970e93fe1a1869399ec6150d7fd11872be3817f2e0c886b4766`.

The branch `aquora-issue82-classic-ui` was based on the verified official v0.7.7 candidate. Relative to tag v0.7.7, only `device/ui.c` changed in device sources: classic v0.7.6 palette/surfaces/knob styling returned, walnut/metal decorations removed, and centered AQUORA Sans glyphs draw without a background over the moving spiral. All v0.7.7 composition, leads, plucks, STRUM, banks, control mapping, EVOLVE, fullscreen and visualizer code remains. Native/repro/single-core, host screenshot/LCD, latency/lighting, regression, WASM and checksum-gated local installer checks passed. The validated package includes the previous official firmware for rollback.

## Remaining: EXISTING ChatGPT Site publication only

Site project `appgprj_6ad6d0ae409c8191b45224ecdec724db`  
Homepage https://parallax8-studio.jminie66.chatgpt.site/  
Web Installer https://parallax8-studio.jminie66.chatgpt.site/install/

The GitHub developer connector in this session can publish GitHub, but does not expose a ChatGPT Sites project editing/publishing action. The existing website was **not modified or live-verified** here; do not claim the public installer serves v0.7.8 yet. The repository's most recent site publication record is for v0.7.7. Do not create a replacement site.

To finish in a ChatGPT Sites-capable Work session: open the EXISTING site project above; upload/point its installer to the immutable v0.7.8 firmware SHA above; preserve the current v0.7.7 and earlier rollback installers, page layout and visitor counter; update labels, manuals and screenshots using the already verified assets in `AQUORA-0.7.8-official.zip`. Publish and fetch the actual live installer; check product identity `FM-1_73000708`, exact firmware SHA, browser checksum/tamper guard, v0.7.7 rollback path and homepage consistency. Record the site deployment ID and hashes.

Never rebuild or silently replace accepted official v0.7.8 bytes for this website-only remainder. Hardware LCD, controls, audio and real flash/rollback acceptance remain owner-tested.
