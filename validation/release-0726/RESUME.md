# AQUORA v0.7.26 testing checkpoint

Start with Issue #88 and ACCEPTANCE.md. Source commit 3d78cb88f38c68024be7e46da5fccc2c0da7cc36 on aquora-v0.7.26-corrective-development, based on verified v0.7.25 receipt 907e2935b65e425501c8696af4d0bc2f9a4e7f32. Keep prior testing and stable rollbacks.

Use the backed-up native/WASM files and SHA256SUMS. If publication is incomplete, reuse these exact artifacts for the testing-only installer and emulator. The intended paths are /testing/0.7.26/install/ and /testing/0.7.26/emu/ on the existing AQUORA Site. Verify live binary hashes and record a deployment receipt before marking publication complete. Do not rebuild identical artifacts.

The protected-voice admission path, SELECT/ALGORITHM navigation, mode tempos, portrait rest motion, and 16 Favorites are covered by host checks. Physical vocal silence, timing, musical judgment and on-device storage/flash acceptance remain open. The detailed acceptance document records the V11 rollback limitation.
