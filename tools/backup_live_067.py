from pathlib import Path
from urllib.request import urlopen
from concurrent.futures import ThreadPoolExecutor
import hashlib,sys,json
out=Path(sys.argv[1]);out.mkdir(exist_ok=True,parents=True)
base='https://parallax8-studio.jminie66.chatgpt.site/'
manifest=urlopen(base+'SITE-SHA256SUMS.txt',timeout=40).read();expected=json.loads(Path('validation/release-067/LIVE_PUBLICATION.json').read_text())['site_manifest_sha256'];assert hashlib.sha256(manifest).hexdigest()==expected
(out/'SITE-SHA256SUMS.txt').write_bytes(manifest)
def fetch(line):
 digest,name=line.split('  ',1);assert not name.startswith('/') and '..' not in Path(name).parts
 data=urlopen(base+name,timeout=40).read();assert hashlib.sha256(data).hexdigest()==digest,name
 p=out/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
with ThreadPoolExecutor(max_workers=6) as pool:list(pool.map(fetch,manifest.decode().splitlines()))
print('PASS independently fetched every live installer manifest file')
