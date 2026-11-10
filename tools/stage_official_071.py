"""Stage a separate testing installer; never writes the existing hosted Site."""
from pathlib import Path
import sys,re,json,hashlib,shutil
root=Path(sys.argv[1]);repo=Path(__file__).resolve().parents[1]
p=root/'install/index.html';s=p.read_text();m=re.search(r'const meta = (\{.*?\});',s);meta=json.loads(m[1])
fw=p.parent/meta['pkg'];assert fw.is_file() and meta['product']=='FM-1_73000701'
meta['sha256']=hashlib.sha256(fw.read_bytes()).hexdigest()
s=s[:m.start(1)]+json.dumps(meta)+s[m.end(1):]
if 'async function candidateHashOk' not in s:
 s=s.replace('let writing = false;', '''async function candidateHashOk(raw) {
  const hash = Array.from(new Uint8Array(await crypto.subtle.digest("SHA-256", raw)), b => b.toString(16).padStart(2, "0")).join("");
  return hash === meta.sha256;
}
let writing = false;''')
 s=s.replace('    if (productOf(pkg) !== meta.product)', '    if (!await candidateHashOk(pkg)) { sayK("badpkg"); return; }\n    if (productOf(pkg) !== meta.product)')
s=s.replace('<h1>Install sound-healing</h1>','<h2>Install AQUORA 0.7.1</h2>')
if 'AQUORA 0.7.1 — OFFICIAL RELEASE' not in s:
 s=s.replace('<main>','<main><h1>AQUORA 0.7.1 — OFFICIAL RELEASE</h1><p>Official firmware 0.7.1 / FM-1_73000701. Original Puya 0x856014 FM-1 only; UC25HQ80 is unsupported. Previous firmware remains available for rollback.</p><p><a href="https://parallax8-studio.jminie66.chatgpt.site/install/">Public firmware and rollback installers</a></p>',1)
p.write_text(s)
(repo/'validation/release-071/INSTALLER_MANIFEST.json').write_text(json.dumps({'version':'0.7.1','identity':meta['product'],'firmware_sha256':meta['sha256'],'firmware_bytes':fw.stat().st_size,'installer_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'publication':'Official release artifact; website deployment verified separately'},indent=2)+'\n')
print(json.dumps(meta))
