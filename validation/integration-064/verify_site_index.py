from pathlib import Path
import json,hashlib,subprocess
root=Path('/workspace/scratch/d79377b46aa6/site');expected=json.loads(Path('/tmp/site064-expected.json').read_text())
for path,sha in expected.items():
 content=subprocess.check_output(['git','show',':out/'+path],cwd=root)
 assert hashlib.sha256(content).hexdigest()==sha,path
print('Verified all',len(expected),'STAGED blobs against independent expected hashes')
