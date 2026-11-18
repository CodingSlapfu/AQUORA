# v0.7.25 testing publication — verified complete

- Device source checkpoint: 483224ad77c4c07106f21bc8d5b25cd65127b13a.
- Firmware/WASM/evidence backup checkpoint: b5b434c5218f449e20cbcd80ec2df076a7ee531a. Both remote Git objects were fetched and SHA-256 checked after backup.
- Site source: 650ebbd0aed4a7db677ccec5d0a9a6eae9e2a394.
- Site version: 218; appgprj_6ad6d0ae409c8191b45224ecdec724db~appgver_31c1df69c11c819180ccc172d35fb5e2.
- Deployment: appgdep_6afd085110488191abb6b68341929340, succeeded 2026-11-18T02:07:26.086533Z.
- Installer: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.25/install/
- Emulator: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.25/emu/
- Native identity: FM-1_73000725; package 611496 bytes.
- Public firmware download SHA-256: 96b6834b63d35a48b63077fd90fc091cff383b32e60ef8185365d92c006d0a22.
- Public WASM download SHA-256: f19c0ccd8f4e7275c0a6bcbd3e182bebf04c0be50ca24fc6e35c3578ae5a67b8.

Both public downloads were compared byte-for-byte with the verified local/GitHub artifacts. Public installer metadata, target identity, SHA guard and Puya confirmation guard were checked. Matching emulator HTML route loads its WASM; the same WASM separately passed boot/audio-render smoke testing. No claim of physical FM-1 timing, listening or flash acceptance is made.

Stable v0.7.14, testing v0.7.22 and v0.7.24 firmware and installer files were byte-compared with the prior site source and preserved. The promotional homepage was not redesigned; only its testing emulator/installer links and testing version label were updated. The existing visit-counter worker and database configuration remain unchanged.

No new official stable release was published. Remaining owner hardware/listening acceptance and recording approximations are listed in ACCEPTANCE.md. v0.7.24 publication was already complete when this session resumed; it was verified rather than rebuilt.
