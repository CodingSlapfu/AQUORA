# AQUORA v0.7.7 hardware audition candidate

Firmware identity **FM-1_73000707**. This package targets the original M-VAVE FM-1 with Puya flash **0x856014**. Owner listening, LCD, physical flash and timing acceptance are pending. The public release and installer remain at v0.7.6 until that acceptance.

## Try the candidate

1. Keep the v0.7.6 rollback package and back up any banks you need before testing.
2. Unzip the candidate. Serve the `local-installer` folder over localhost, for example with `python -m http.server 8077 --bind 127.0.0.1 --directory local-installer` from the extracted folder. Open [the local candidate Web Installer](http://127.0.0.1:8077/install/) in desktop Chrome or Edge.
3. Connect the original FM-1 by USB and follow the installer. Check that it identifies the candidate as **FM-1_73000707**. The browser verifies the exact firmware checksum before it enables installation. The normal dual-core firmware is also in `firmware/AQUORA-0.7.7.fwsc`. The single-core image is included for comparison and fallback testing.
4. The included `rollback/AQUORA-0.7.6-rollback.fwsc` is the exact published v0.7.6 image. The [official AQUORA Web Installer](https://parallax8-studio.jminie66.chatgpt.site/install/) serves the production update path.

## Physical acceptance

- Play an intention in Standard and Cinematic. Hear the lead establish an identifiable theme and the plucks answer it. Switch A → B → A while playing. Check that the return builds a fresh song at the next beat and that the interface shows the selection promptly.
- Compare AUTO LEVEL on and off at the same master volume, including a dense section and an exposed sustained note. Listen for stable balance and clean attacks. Confirm that mute, solo and manual levels behave as expected.
- Check Original, Cymatics and Chladni in both viewports. Verify screen edges, unboxed text and proportions in fullscreen, and exercise the physical PRESETS knob during EVOLVE and visualizer playback.
- Check all four saved banks, confirm physical flash and rollback, and observe FM-1 audio and LCD timing during dense playback. Older bank files retain their saved data and use the v0.7.7 composition rules during playback.

`qa/AQUORA-0.7.7-screen-review.png` shows the software LCD captures. `SHA256SUMS.txt` verifies the packaged files. The separate listening archive compares eight matched v0.7.6/candidate examples and includes the isolated roles.
