# AQUORA 0.6.0 publication

- Firmware source checkpoint: `5cb184f1ea150c2226d2416d91dd6bc0b08dab69`. Release tag `v0.6.0` resolves to CI commit `cc7d663b8d74de306fea91ae1daaf2d34f51bd3a`, which adds validation workflows to the same firmware source.
- [Acceptance run](https://github.com/CodingSlapfu/AQUORA/actions/runs/41960642450) passed native and forced single-core compilation, WASM compilation and module validation, nine sanitized DSP tests, six host panel workflows, host timing, and staging with a simulated 0.5.0 to 0.6.0 direct update.
- [Publication run](https://github.com/CodingSlapfu/AQUORA/actions/runs/41960911998) created the [GitHub release](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.6.0), verified the tag and downloaded and checksummed all release assets.
- [Live installer](https://parallax8-studio.jminie66.chatgpt.site/install/) serves `FM-1_73000600`. Existing public deployment `appgdep_6aebed41641881919cc9056ca54a9215`, Site commit `509bd2777d5e6806210f64a6375b948435bf724f`. Public installer, firmware, WASM, and manual match the release package. The rebuilt native firmware and WASM match the live assets byte for byte.
- [0.5.0 rollback](https://parallax8-studio.jminie66.chatgpt.site/rollback/0.5.0/install/) was verified against the 0.5.0 publication checksums.

| Asset | SHA-256 |
| --- | --- |
| Firmware | `9f3198b7efbffed498e46bdab3e1e04d2890d064ab7e1e78aca67128dfffc64b` |
| WASM | `a37f35a106a1111a437ff9dbef2372875f55c8fca546a688790b44935c89383d` |
| Package archive | `054e68b966c56ecf514eaf7ba6d457973d6a09adcd9d6823672d023bb1ca6f83` |
| Source archive | `5b97cb48548ebb432ac11be54aed3f2d28b80ad811001dcfded14bd6c0dcabde` |

Physical flashing and rollback, AIR hand response, LCD FPS, DMA stability, key feel, and power-cycle persistence remain unverified. Host timing is a software measurement. The pinned SDK boot and EQ reference warnings remain.
