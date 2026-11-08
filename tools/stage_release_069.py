from pathlib import Path
import hashlib,json,re,shutil,zipfile,html
repo=Path(__file__).resolve().parents[1];site=Path('/workspace/scratch/56c15b7cf3b9/site069');out=site/'out';final=Path('/workspace/scratch/56c15b7cf3b9/final069')
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
fw=final/'native-exact/sound-healing-0.6.9.fwsc';wasm=final/'emu/app.wasm';worklet=final/'emu/worklet.js'
assert fw.read_bytes()==(final/'native-repro/sound-healing-0.6.9.fwsc').read_bytes()
prior={n:sha(out/f'firmware/sound-healing-{n}.fwsc') for n in ['0.6.7','0.6.8']}
install=out/'install/index.html';old=install.read_text();m=re.search(r'const meta = (\{.*?\});',old);assert m
meta=json.loads(m[1]);assert meta['version']=='0.6.8'
rollback=out/'rollback/0.6.8/install';rollback.mkdir(parents=True,exist_ok=True)
rm=json.loads(m[1]);rm['pkg']='../../../firmware/sound-healing-0.6.8.fwsc';rm['boot']['pkg']='../../../firmware/m-bootloader-1.2.fwsc'
(rollback/'index.html').write_text(old[:m.start(1)]+json.dumps(rm)+old[m.end(1):])
shutil.copy2(fw,out/'firmware'/fw.name)
wn=f'app-0.6.9-{sha(wasm)[:12]}.wasm';jn=f'worklet-0.6.9-{sha(worklet)[:12]}.js'
shutil.copy2(wasm,out/'emu'/wn);shutil.copy2(worklet,out/'emu'/jn)
e=out/'emu/index.html';s=e.read_text().replace('0.6.8','0.6.9');s=s.replace('app.wasm?v=0.6.9',wn).replace('worklet.js?v=0.6.9',jn);assert wn in s and jn in s and 'AQUORA Studio companion bridge' in s;e.write_text(s)
meta.update(version='0.6.9',product='FM-1_73000609',pkg='../firmware/'+fw.name,sha256=sha(fw))
s=old[:m.start(1)]+json.dumps(meta)+old[m.end(1):];s=s.replace('Install AQUORA v0.6.8','Install AQUORA v0.6.9')
s=s.replace('</main>','<p><a href="../rollback/0.6.8/install/">Install preserved v0.6.8 rollback</a></p></main>',1);install.write_text(s)
p=out/'index.html';s=p.read_text().replace('0.6.8','0.6.9').replace('FM-1_73000608','FM-1_73000609')
section='<section class="content"><div class="container"><div class="section-head"><span class="eyebrow">NEW / AQUORA 0.6.9</span><h2>Play a phrase.<br><span class="gradient-text">Let it evolve.</span></h2><p>Teach AQUORA your original melody with Learn from Playing. Explore Experimental Ambient, sixteen evolving song sections, clearer drums, more responsive Chladni motion and steadier AIR control. Each startup opens a fresh musical arrangement; press PLAY to stop whenever you want.</p><div class="buttons"><a class="button primary" href="listening/0.6.9/">Hear the original A/B listening pack ↗</a><a class="button secondary" href="downloads/0.6.9/AQUORA-0.6.9-installer.zip">Download the installer archive ↗</a></div><p class="little-note">Software checks passed. Physical FM-1 performance and artistic preference remain for the owner to evaluate. Both 0.6.7 and 0.6.8 installers remain available for rollback.</p></div></div></section>'
s=s.replace('</main>',section+'</main>') if '</main>' in s else s.replace('</body>',section+'</body>');p.write_text(s)
for p in (out/'studio').rglob('*.html'):
 p.write_text(p.read_text().replace('0.6.8','0.6.9').replace('../emu/"','../emu/?v=0.6.9"'))
# Accessible level-matched original listening comparisons, no third-party recordings.
lp=out/'listening/0.6.9';lp.mkdir(parents=True,exist_ok=True)
for p in (final/'listening').glob('*.ogg'):shutil.copy2(p,lp/p.name)
shutil.copy2(final/'listening/ANALYSIS.json',lp/'ANALYSIS.json')
names=['warm-pulse','interlocking-lights','quiet-broken-orbit','slow-drift','chamber-answer','experimental-orbit'];cards=[]
for n in names:
 cards.append('<section><h2>'+html.escape(n.replace('-',' ').title())+'</h2><p>Original 180-second renders. '+('Experimental Ambient is compared with the previous Standard mode.' if n=='experimental-orbit' else 'Same seed, intention, mode and loop length; release algorithms differ.')+'</p>'+''.join('<label>'+v+'<audio controls preload="none" src="'+n+'-'+v[2:]+'.ogg"></audio></label>' for v in ['0.6.8','0.6.9'])+'</section>')
# Filenames use release shorthand068/069.
page='<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>AQUORA 0.6.9 · Listening comparisons</title><style>body{background:#071c23;color:#e3f5f2;font:17px/1.6 system-ui;margin:0}main{max-width:900px;margin:auto;padding:40px 22px}a{color:#86e7d6}h1{font-size:42px;line-height:1.1}section{border-top:1px solid #31535a;padding:25px 0}label{display:inline-block;width:46%;margin:1%}audio{display:block;width:100%;margin-top:8px}p{color:#b4d0d0}@media(max-width:600px){label{width:100%}}</style><main><a href="../../">← AQUORA</a><h1>Listen. Compare.<br>Choose what moves you.</h1><p>Twelve original, level-matched recordings compare the previous release with the new composition engine. These are actual firmware-DSP renders, with no artist samples. They passed objective headroom checks; musical preference is yours to judge. Use a comfortable volume.</p>'+''.join(cards)+'<p><a href="ANALYSIS.json">Measured levels and original PCM hashes</a></p></main></html>'
page=page.replace('-6.8.ogg','-068.ogg').replace('-6.9.ogg','-069.ogg');(lp/'index.html').write_text(page)
d=out/'downloads/0.6.9';d.mkdir(parents=True,exist_ok=True)
archive=final/'AQUORA-0.6.9-source.tar.gz';data=archive.read_bytes();parts=[]
for at in range(0,len(data),28*1024*1024):
 p=d/f'AQUORA-0.6.9-source.tar.gz.part{at//(28*1024*1024):03}';p.write_bytes(data[at:at+28*1024*1024]);parts.append({'file':p.name,'bytes':p.stat().st_size,'sha256':sha(p)})
(d/'SOURCE-MANIFEST.json').write_text(json.dumps({'source_commit':'dc3db77347bdf9abfad1045c5d9b562b99fd1abf','archive_sha256':sha(archive),'restore':'Concatenate parts in listed order, verify SHA256, then extract tar.gz.','parts':parts},indent=2))
z=d/'AQUORA-0.6.9-installer.zip'
with zipfile.ZipFile(z,'w',zipfile.ZIP_DEFLATED) as f:
 f.write(install,'install/index.html');f.write(fw,'firmware/'+fw.name);f.write(out/'firmware/m-bootloader-1.2.fwsc','firmware/m-bootloader-1.2.fwsc');f.write(repo/'device/docs/RELEASE_069.md','RELEASE.md');f.write(d/'SOURCE-MANIFEST.json','SOURCE-MANIFEST.json')
record={'version':'0.6.9','identity':'FM-1_73000609','source_commit':'dc3db77347bdf9abfad1045c5d9b562b99fd1abf','software_acceptance':'passed','hardware_acceptance':'owner-unverified','firmware':{'url':'firmware/'+fw.name,'bytes':fw.stat().st_size,'sha256':sha(fw)},'wasm':{'url':'emu/'+wn,'bytes':wasm.stat().st_size,'sha256':sha(wasm)},'worklet':{'url':'emu/'+jn,'sha256':sha(worklet)},'installer_zip':{'url':'downloads/0.6.9/'+z.name,'sha256':sha(z)},'source_archive_sha256':sha(archive),'rollback':prior}
for n in ['release.json','release-0.6.9.json']:(out/n).write_text(json.dumps(record,indent=2)+'\n')
for n,h in prior.items():assert sha(out/f'firmware/sound-healing-{n}.fwsc')==h
for n in names:
 for v in ['068','069']:assert (lp/f'{n}-{v}.ogg').is_file()
(final/'STAGE.json').write_text(json.dumps(record,indent=2)+'\n');print(json.dumps(record,indent=2))
