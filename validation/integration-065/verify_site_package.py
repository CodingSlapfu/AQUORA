"""Verify committed Git blobs and deployment archive against independently staged files."""
from pathlib import Path
import json,hashlib,subprocess,tarfile,sys
root=Path('/workspace/scratch/d79377b46aa6/site065')
expected=json.loads(Path('/tmp/site065-expected.json').read_text())
archive=Path(sys.argv[1])
for name,sha in expected.items():
 blob=subprocess.check_output(['git','show','HEAD:out/'+name],cwd=root)
 assert hashlib.sha256(blob).hexdigest()==sha,('committed',name)
with tarfile.open(archive) as tar:
 members={m.name.removeprefix('./'):m for m in tar.getmembers() if m.isfile()}
 for name,sha in expected.items():
  content=tar.extractfile(members['dist/'+name]).read()
  assert hashlib.sha256(content).hexdigest()==sha,('archive',name)
 manifest=json.load(tar.extractfile(members['.openai/hosting.json']))
 assert manifest['project_id']=='appgprj_6ad6d0ae409c8191b45224ecdec724db'
 assert manifest['static']['directory']=='dist'
print('Verified all',len(expected),'COMMITTED and PACKAGED bytes including installer/firmware/WASM')
