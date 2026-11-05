# AQUORA public website and Studio — v0.6.2

Standalone website branch: `aquora-showcase-site`. Built publication branch: `gh-pages`. No firmware branches are changed.

**Verified content:** Published [v0.6.2 release](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.6.2), [successful software and host-panel validation](https://github.com/CodingSlapfu/AQUORA/actions/runs/41998659763), source commit `b6d452202bb12c955c29f1726fea5a02bcc91e61`. Exact WebAssembly SHA256: `d9bd8745bed889ee34106febdda5b07ce16db1977c88b0bf0212369837dbb2c2`.

Current firmware sources define 30 named patches, 19 drum kits, 13 intentions, eight roles and 83 Quick Find parameters. Nine genuinely new 240×240 gallery images plus a PLAY hero capture come only from the successful v0.6.2 host-panel artifact, including the redesigned mixer, visualizers, sound editor, drum controls and appearance screen. The independent site build deletes all prior generated output and copies only new assets.

The publication workflow checks package version, checksum, PNG dimensions, displayed copy, excluded public names and JavaScript/WebAssembly compatibility, then publishes the independent `gh-pages` branch and backup artifact. Original framework licenses and legal notices remain in project source. GitHub's native Pages deployment run [42002935806](https://github.com/CodingSlapfu/AQUORA/actions/runs/42002935806) successfully published the new `gh-pages` commit `77f6293551b195fb56401fb744e0f13b547a0dc1`. The duplicate direct Pages deployment job was removed because this repository uses branch-based publication.

Expected hosting URL: https://codingslapfu.github.io/AQUORA/ . Repository owner may need Settings → Pages → Deploy from a branch → `gh-pages` / `/(root)`.

Studio sends same-origin physical-control events to the verified emulator, not to connected hardware. Browser audio requires a user gesture. Use headphones for stereo binaural listening. Artistic frequency themes are not medical claims.

## Homepage slideshow of accepted FM-1 screens

The homepage hero cycles automatically through **all ten accepted v0.6.2 screen captures** (PLAY, SOUND, EVOLVE, full MIXER, mixer channel, DRUMS, AIR FX, Appearance, Original visualizer and Polar Bloom). The first frame is the actual normal PLAY page, `assets/play-062.png`, captured in `panel-062/original-normal.png` by acceptance run 41998659763. Screens are authentic software-rendered captures, not website mockups or moving on-device video.

The user can select previous/next or Pause/Play under the screen; the slideshow holds while hovered or focused, pauses in background tabs, and respects reduced-motion preferences. It uses the ten existing approved v0.6.2 captures, preserves the separate gallery, and never reinstates the floating "VISIBLE MUSIC" label. Client behavior is in `site/screen-carousel.js`; all ten image paths and syntax are checked by the publication workflow.
