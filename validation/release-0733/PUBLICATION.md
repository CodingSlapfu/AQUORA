# AQUORA v0.7.33 TESTING publication

Published November 18, 2026 to the existing AQUORA website as Site version 229. Deployment `appgdep_6afe0bc9f1408191b992edda074b26ee` succeeded from Site source `54b64cfcc7fb077fdbc4980ee03621a88bd5ee1d`. GitHub backup `3d7696e9e6c2445681fd568a34527858e5ebf7c9` was fetched independently and checked against the local and public builds.

| Verified asset | SHA-256 |
|---|---|
| [v0.7.33 firmware](https://parallax8-studio.jminie66.chatgpt.site/firmware/sound-healing-0.7.33.fwsc) | `076819c190e14ead729990a77e8b1aa44618e6ac02599351aa3c435ae43d7dff` |
| [v0.7.33 WASM](https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.33/emu/app.wasm) | `383eb1509ceccb5a4c0c77d8312f2d46d75a859e4c780c37db7b02b5f657f664` |
| v0.7.14 stable firmware | `d3de83af71ba084c61dc88b1804e814200d7ae69439480d2aecfcb886d09ec6e` |
| v0.7.32 testing firmware | `a0dd25af57d641de7dc0c1851cead8f77fd4df46c60f95033cfe777215b24cb8` |

- [Testing installer](https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.33/install/) and [emulator](https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.33/emu/) resolve on the public site.
- [Evidence](https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.33/evidence/) contains panel captures, 19 drum samples, and matched dry/full-mix vocal excerpts.
- 16 simulated browser updater checks passed, including loader return, previous-identity mismatch and single-bit corruption rejection.
- Across 15 native-style render cases the direct signal lit KIRA in 40,885 blocks. The pose signaled closed eyelids in 18,271 blocks and reopened 137 times. Every closed-lid block had direct vocal light above threshold.
- The [stable installer](https://parallax8-studio.jminie66.chatgpt.site/install/) matched its previous bytes. The site commit changes only homepage testing links and adds v0.7.33 assets. The [v0.7.32 rollback](https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.32/install/) still serves its firmware and identity.
- Physical FM-1 timing, LCD behavior, flashing and listening acceptance remain open. See [benchmark](BENCHMARK.md) for audio measurements and the outstanding artist listening assessment.
