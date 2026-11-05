# AQUORA — Public Showcase & Studio

An independent, responsive marketing website plus working FM-1 browser emulator and live companion control desk. This site lives on branch \`aquora-showcase-site\` and is **separate from firmware development**.

## Verified, pinned release
- Public content and embedded WebAssembly app are pinned to **AQUORA v0.6.1** — GitHub release \`v0.6.1\`, firmware source commit \`23c8db04538fec13171a6e380501bb153f949311\`.
- The GitHub Actions build downloads \`AQUORA-0.6.1-package.tar.gz\` from that published release, preserves the actual \`emu/app.wasm\` and \`emu/worklet.js\`, and applies a tiny site-only HTML bridge with \`tools/patch_emu.py\`. It does **not** rebuild or modify the firmware.
- Screenshot assets come from the independently validated \`aquora-061-host-panel\` artifact in successful run \`41969916116\`. The older mixer image is explicitly identified as a 0.6.x test view.
- Source and site generation scripts are kept in this branch; generated output is published to the \`gh-pages\` branch by the isolated website workflow.

## Pages
If GitHub Pages has not yet been activated, visit **repository Settings → Pages → Build and deployment → Deploy from a branch → \`gh-pages\` / \`/ (root)\` → Save**. After GitHub Pages reports the deployment URL, verify the site and emulator from an unsigned browser session. GitHub's default project URL should be \`https://codingslapfu.github.io/AQUORA/\`, but only treat it as live after a successful browser check.

The standalone workflow also attempts an Actions-based Pages deployment when enabled. If Pages has not been configured and no privileged Pages token is supplied, the deploy job may fail even when the public \`gh-pages\` source branch has been generated correctly.

## Studio controls
The editor UI uses same-origin \`postMessage\` and the emulator's existing button/encoder handlers. Browser audio begins on a user click on **Switch on** inside the emulator. The editor cannot edit a connected physical FM-1 and does not provide a new preset export API; the existing firmware controls and browser store determine supported saving.

## Source credits and safety
This site has original AQUORA layout and imagery from actual validation renders; the packaged emulator and fm1-base worklet remain credited to their upstream authors (including Charles Vestal, MPL-2.0). The marketing page credits HYP-1 and ChoralRoot as independent design references only. No proprietary logos/assets are copied. Artistic intention/frequency themes are not medical claims.

## Updating the site after the next completed release
Only bump the pinned release tag, screenshots and emulator after that exact firmware version has **passed all software acceptance gates and been officially published**. Never point the public emulator at an unaccepted development branch. Preserve older release assets and installers.
