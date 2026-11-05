#!/usr/bin/env python3
"""Site-only bridge for the pinned AQUORA v0.6.2 FM-1 WebAssembly emulator.

Never alters app.wasm or worklet.js. Adds a same-origin postMessage adapter
that maps Studio editor controls to the emulator's existing button/knob paths.
"""
from pathlib import Path
import re
import sys

path = Path(sys.argv[1])
data = path.read_text(encoding="utf-8")

assert '<title>sound-healing in the browser</title>' in data
data = data.replace('<title>sound-healing in the browser</title>',
                    '<title>AQUORA 0.6.2 — FM-1 Browser Emulator</title>', 1)
data = data.replace('<h1>sound-healing</h1>', '<h1>AQUORA <span style="font-size:.55em;opacity:.7">0.6.2</span></h1>', 1)
data = data.replace('sound-healing for the M-VAVE FM-1, in the browser:',
                    'AQUORA 0.6.2 for the M-VAVE FM-1, in the browser:', 1)
bridge = r"""
// ---------------------------------------------------------------- AQUORA Studio companion bridge (site-only)
// Same-origin parent only. All editing uses existing verified FM-1 input paths.
(function () {
  "use strict";
  const tag = "AQUORA_EMU";
  const pageNumbers = {PLAY:0,SOUND:1,EVOLVE:2,MIX:3,DRUMS:4,SETUP:5,AIR:6};
  const encoderNames = ["SELECT","ALGORITHM","PRESETS","KNOB1","KNOB2","KNOB3","KNOB4","MASTER"];
  const legalButtons = ["HOME","PLAY","EDIT","SAVE","SEL","ARP","SEQ","GLO","FX","ENV","LFO"];
  const report = (type, extra) => {
    if (window.parent !== window) window.parent.postMessage(
      Object.assign({source:tag,type:type},extra||{}), location.origin);
  };
  const statusNow = () => report("state",{on:!!node});
  const sleep = ms => new Promise(resolve=>setTimeout(resolve,ms));
  async function pulse(name) {
    const bit = BTN[name];
    if (!Number.isInteger(bit)) return;
    const id = "aquora-studio-pulse";
    btnHeldBy.set(id,bit);
    syncButtons();
    await sleep(115);
    btnHeldBy.delete(id);
    syncButtons();
    await sleep(85);
  }
  const statusNode = document.getElementById("status");
  if (statusNode) new MutationObserver(statusNow).observe(statusNode,
    {childList:true,characterData:true,subtree:true});
  window.addEventListener("message",async e => {
    if (e.origin !== location.origin || e.source !== window.parent) return;
    const m = e.data;
    if (!m || m.source !== "AQUORA_STUDIO") return;
    if (m.action === "status") { statusNow(); return; }
    if (!node) { report("off"); return; }
    if (m.action === "turn" && encoderNames.includes(m.role) &&
        Number.isInteger(m.steps) && Math.abs(m.steps) <= 12) {
      const knob = knobs[m.role];
      if (knob) turn(knob,m.steps);
      report("ack",{message:m.role+" adjusted on FM-1"});
    } else if (m.action === "button" && legalButtons.includes(m.button)) {
      await pulse(m.button);
      report("ack",{message:m.button+" pressed"});
    } else if (m.action === "page" && Object.prototype.hasOwnProperty.call(pageNumbers,m.page)) {
      await pulse("HOME"); // establish canonical PLAY page before changing ALGORITHM
      const step = pageNumbers[m.page];
      if(step)turn(knobs.ALGORITHM,step);
      report("ack",{message:m.page+" page selected"});
    }
  });
  statusNow();
})();
"""
assert data.count("</script>") == 1, "Unexpected emulator HTML structure"
data=data.replace("</script>",bridge+"\n</script>",1)
# Public-facing text only; never modify firmware binary or licensing.
data=re.sub(r"(?i)M-VAVE\s+FM-1", "FM-1", data)
data=re.sub(r"(?i)M-VAVE|HYP-1|ChoralRoot|Sony|Sega", "", data)
path.write_text(data,encoding="utf-8")
assert "AQUORA Studio companion bridge" in path.read_text()
print("Pinned FM-1 emulator branded and Studio postMessage bridge installed:",path)
