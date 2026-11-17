# AQUORA v0.7.22 live testing verification

- Live installer https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.22/install/
- Emulator https://parallax8-studio.jminie66.chatgpt.site/testing/0.7.22/emu/
- Existing site source 0e9bcd5ebc895cca5f052756b0c8d0b944110ee0, saved version 214, succeeded deployment appgdep_6afc11ee5d9c8191b7e9a1050f1ea55d.
- Product FM-1_73000722. Public FWSC SHA-256 b77b3680aadacba533b66e2b2650f2eec50b70156f5bdfcfd11b471f296f13ae, 611496 bytes. It matched the byte-reproduced native build, staged archive and fetched GitHub backup. The page accepted these bytes and rejected a one-byte altered copy under the same identity.
- Public WASM SHA-256 9d29fc33c34407377cd898086b7e6943e6e6c14690ea4f5117878775879e6ea6, exact to staged emulator.
- Existing stable installer HTML and v0.7.14 and v0.7.21 firmware download hashes match their prior verified values.
- Twelve loaded host cases combining active vocal spatial processing with three visualizers and four simulated FM worker states passed. Physical hardware acceptance and unfinished requirements remain in RESUME.md.

Testing firmware only. No new stable release.
