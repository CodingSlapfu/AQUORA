from pathlib import Path
import json,hashlib,re,zipfile
out=Path('/workspace/scratch/56c15b7cf3b9/site069/out');r=json.loads((out/'release.json').read_text());sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
assert r['version']=='0.6.9' and r['identity']=='FM-1_73000609'
for field in ['firmware','wasm','worklet','installer_zip']:
 x=r[field];p=out/x['url'];assert p.is_file() and sha(p)==x['sha256']
for version in ['0.6.7','0.6.8']:
 assert sha(out/f'firmware/sound-healing-{version}.fwsc')==r['rollback'][version]
 p=out/f'rollback/{version}/install/index.html';s=p.read_text();meta=json.loads(re.search(r'const meta = (\{.*?\});',s)[1]);assert meta['version']==version and (p.parent/meta['pkg']).resolve().is_file()
s=(out/'install/index.html').read_text();m=json.loads(re.search(r'const meta = (\{.*?\});',s)[1]);assert m['sha256']==r['firmware']['sha256'] and m['product']==r['identity'] and m['version']=='0.6.9' and 'Install AQUORA v0.6.9' in s
s=(out/'emu/index.html').read_text();assert r['wasm']['url'].split('/')[-1] in s and r['worklet']['url'].split('/')[-1] in s and 'AQUORA Studio companion bridge' in s
s=(out/'listening/0.6.9/index.html').read_text();audio=re.findall(r'<audio[^>]*src="([^"]+)"',s);assert len(audio)==12
for n in audio:assert (out/'listening/0.6.9'/n).read_bytes().startswith(b'OggS')
m=json.loads((out/'downloads/0.6.9/SOURCE-MANIFEST.json').read_text());h=hashlib.sha256()
for f in m['parts']:
 p=out/'downloads/0.6.9'/f['file'];assert p.stat().st_size<25*1024*1024 and sha(p)==f['sha256'];h.update(p.read_bytes())
assert h.hexdigest()==m['archive_sha256']==r['source_archive_sha256']
with zipfile.ZipFile(out/r['installer_zip']['url']) as z:assert hashlib.sha256(z.read('firmware/sound-healing-0.6.9.fwsc')).hexdigest()==r['firmware']['sha256'] and z.testzip() is None
print('PASS existing-site069 metadata/firmware/WASM/worklet/ZIP hashes, source reconstruction hash, both rollback installers, Studio bridge and12original audio files')
