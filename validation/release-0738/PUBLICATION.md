# AQUORA v0.7.38 TESTING publication

- Installer: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.38/install/
- Matching emulator: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.38/emu/
- Audio evidence: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.38/evidence/
- Public manifest: https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.38/manifest.json
- Device source: `0d2a84c6efb05158e179f8e4eb8f8f8c24930097`
- Immutable verified backup: `beb8a72d0c8f54b35f8aa0342301d6f432d49399`
- Site 232, source `f574b497b4929790d7a296931c29fcd6fd191c16`, deployment `appgdep_6afe9329c6c08191a0f31df607d6d4df`
- Prior Site 231, source `b33b75f7584188b5abf3858475f8a72e2d14beba`, deployment `appgdep_6afe69fc1f78819181d1437d546435f8`

The downloaded firmware, WASM, worklet and word audition match the GitHub
backup byte for byte. The live installer advertises `FM-1_73000738` and
the verified firmware digest. Stable v0.7.14 and the v0.7.36 and v0.7.37
TESTING installer and binary pairs matched their prior hashes.

```
firmware eb17afd53a184844aaa5c2a06407887bbd495a2c3721e0dcc26b75dafba3ffae
wasm 9bb569077d05acc90f8bd33773f83bfa9f26a4d2e8c32c55f986cdfb13d54894
PASS live installer, emulator and evidence, GitHub bytes and all three preserved installer/firmware pairs
```

The two-slot sampler can still reclaim a finishing voice during overlapping
vocal and harmony events. The duplicate-word vowel fallback addresses the
discarded-note path. Owner listening remains important for sample-specific
retrigger or coda reports, pitch expression and diction.
