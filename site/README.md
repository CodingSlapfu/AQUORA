# AQUORA public website and Studio — v0.6.2

Standalone website branch: `aquora-showcase-site`. Built publication branch: `gh-pages`. No firmware branches are changed.

**Verified content:** Published [v0.6.2 release](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.6.2), [successful software and host-panel validation](https://github.com/CodingSlapfu/AQUORA/actions/runs/41998659763), source commit `b6d452202bb12c955c29f1726fea5a02bcc91e61`. Exact WebAssembly SHA256: `d9bd8745bed889ee34106febdda5b07ce16db1977c88b0bf0212369837dbb2c2`.

Current firmware sources define 30 named patches, 19 drum kits, 13 intentions, eight roles and 83 Quick Find parameters. Nine genuinely new 240×240 gallery images plus a PLAY hero capture come only from the successful v0.6.2 host-panel artifact, including the redesigned mixer, visualizers, sound editor, drum controls and appearance screen. The independent site build deletes all prior generated output and copies only new assets.

The publication workflow checks package version, checksum, PNG dimensions, displayed copy, excluded public names and JavaScript/WebAssembly compatibility, then publishes the independent `gh-pages` branch and backup artifact. Original framework licenses and legal notices remain in project source. GitHub's native Pages deployment run [42002935806](https://github.com/CodingSlapfu/AQUORA/actions/runs/42002935806) successfully published the new `gh-pages` commit `77f6293551b195fb56401fb744e0f13b547a0dc1`. The duplicate direct Pages deployment job was removed because this repository uses branch-based publication.

Expected hosting URL: https://codingslapfu.github.io/AQUORA/ . Repository owner may need Settings → Pages → Deploy from a branch → `gh-pages` / `/(root)`.

Studio sends same-origin physical-control events to the verified emulator, not to connected hardware. Browser audio requires a user gesture. Use headphones for stereo binaural listening. Artistic frequency themes are not medical claims.

## Homepage slideshow of accepted FM-1 screens

The homepage hero cycles automatically through **all ten accepted v0.6.2 screen captures** (PLAY, SOUND, EVOLVE, full MIXER, mixer channel, DRUMS, AIR FX, Appearance, Original visualizer and Polar Bloom). The first frame is the actual normal PLAY page, `assets/play-062.png`, captured in `panel-062/original-normal.png` by acceptance run 41998659763. Screens are authentic software-rendered captures, not website mockups or moving on-device video.

The hero has no extra controls or captions. All ten v0.6.2 screens advance automatically every four seconds, looping continuously even when hovered. Rotation stops in background browser tabs; reduced-motion preferences remove transitional movement but not automatic cycling. The separate gallery remains unchanged; the old floating visualizer label stays removed. Client behavior is in `site/screen-carousel.js`; all ten image paths and syntax are checked by the publication workflow.

## Matching AQUORA Sans website typography

The website and Studio use a self-hosted **AQUORA Sans Web** companion font. The firmware's actual LCD font is a packed bitmap adaptation of the SIL OFL 1.1 licensed Inter Tight source. The site build starts from the **same repository font source**, creates native browser WOFF2 faces at Regular (optical weight 560) and Bold (weight 720), and applies distinguishing slashed zero and footed/barred I/l shapes to match the firmware's designed details. The web version is a vector companion (not the tiny pixel-identical LCD bitmaps) for high-quality responsive text. The build is reproducible using `site/tools/build_web_font.py`; the source is copied without alteration from the v0.6.2 firmware's `device/fonts/` files. Its original SIL OFL is published as `assets/OFL.txt`.

The homepage headline is **“A universe made of sound.”**, replacing the previous water reference. The independent `site/style.css` stylesheet also covers the Studio page. No firmware font, original source, emulator, or release binaries are modified.
