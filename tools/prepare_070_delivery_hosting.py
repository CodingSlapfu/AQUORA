from pathlib import Path
import shutil,hashlib,json
r=Path('passed/testing-installer')
out=Path('_site')
shutil.copytree(r,out/'testing-070',dirs_exist_ok=True)
p=out/'testing-070/install/index.html';s=p.read_text()
s=s.replace('<h1>NEXT BUILD TESTING INSTALLER</h1>','<h1>AQUORA v0.7.0 — TESTING ONLY</h1>')
s=s.replace('<main>','<main><p><label><input type="checkbox" id="puya-confirm"> I have verified that my FM-1 has the original Puya 0x856014 flash. Newer UC25HQ80 units and unknown flash types are unsupported.</label></p><p>The browser cannot automatically read the flash-chip ID. Leave this unchecked if the chip type is unknown.</p>',1)
s=s.replace('async function run(img, product, isStock, sha256) {','async function run(img, product, isStock, sha256) {\n  if (!document.getElementById("puya-confirm").checked) { say("Installation blocked: verify original Puya 0x856014 hardware first."); return; }')
s=s.replace('$("go").disabled = false;', '$("go").disabled = !document.getElementById("puya-confirm").checked;')
s=s.replace('let writing = false;', '''document.getElementById("puya-confirm").addEventListener("change", () => { $("go").disabled = !document.getElementById("puya-confirm").checked || !image; });
let writing = false;''')
p.write_text(s)
(out/'index.html').write_text('''<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>AQUORA v0.7.0 — TESTING ONLY</title><style>body{background:#071f28;color:#e7f8f3;font:18px/1.6 system-ui;max-width:760px;margin:10vh auto;padding:24px}a{color:#8ae9d3}</style><h1>AQUORA v0.7.0 — TESTING ONLY</h1><p>Physical hardware and listening acceptance pending. Original Puya 0x856014 FM-1 only.</p><p><a href="testing-070/install/">Open the testing installer</a></p><p><a href="https://parallax8-studio.jminie66.chatgpt.site/install/">Official AQUORA installer and rollback options</a></p><p><a href="https://github.com/CodingSlapfu/AQUORA/releases/tag/next-testing-0.7.0-1a1b46d">Testing firmware ZIP, source, manifest and checksums</a></p>''')
assert hashlib.sha256((out/'testing-070/firmware/sound-healing-0.7.0.fwsc').read_bytes()).hexdigest()=='680b6d61ce251d6fe7f0c92f70047b1b10f45776304a88d3969e952ab3f9ad6b'
print('Prepared separate hosting assets; verified unchanged firmware')
