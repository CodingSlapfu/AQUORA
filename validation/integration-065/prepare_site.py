from pathlib import Path
import shutil,hashlib,json,subprocess
root=Path('/workspace/scratch/d79377b46aa6/site065');out=root/'out';public=Path('/tmp/public065');pkg=Path('/tmp/package065')
assert (public/'assets/mixer-065.apng').exists()
if out.exists():shutil.rmtree(out)
shutil.copytree(public,out)
for name in ('install','manual','firmware'):
 target=out/name
 if target.exists():shutil.rmtree(target)
 shutil.copytree(pkg/name,target)
# Independently verify all matching firmware installer paths, and unchanged accepted WASM.
for name in ('install','manual','firmware'):
 for p in (pkg/name).rglob('*'):
  if p.is_file():assert p.read_bytes()==(out/p.relative_to(pkg)).read_bytes(),p
assert (out/'emu/app.wasm').read_bytes()==(pkg/'emu/app.wasm').read_bytes()
expected={str(p.relative_to(out)):hashlib.sha256(p.read_bytes()).hexdigest() for p in out.rglob('*') if p.is_file() and p.name!='SITE-SHA256SUMS.txt'}
(out/'SITE-SHA256SUMS.txt').write_text(''.join(f'{sha}  ./{path}\n' for path,sha in sorted(expected.items())))
expected['SITE-SHA256SUMS.txt']=hashlib.sha256((out/'SITE-SHA256SUMS.txt').read_bytes()).hexdigest()
Path('/tmp/site065-expected.json').write_text(json.dumps(expected,indent=2)+'\n')
print('Prepared',len(expected),'current public/installer paths, exact firmware and WASM')
