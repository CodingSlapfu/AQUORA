"""Independent public-site hash verification and current-website backup; no audio."""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import hashlib,json,sys,urllib.request,urllib.parse,zipfile,re,html
m=json.loads(Path('validation/release-071/WEBSITE_FILES.json').read_text());dest=Path(sys.argv[1]);dest.mkdir(parents=True,exist_ok=True)
transformed={}
def fetch(pair):
 name,expected=pair
 assert '..' not in Path(name).parts and not Path(name).is_absolute()
 request=urllib.request.Request(m['base_url']+urllib.parse.quote(name),headers={'User-Agent':'AQUORA-GitHub-Release-Verification/0.7.1'})
 with urllib.request.urlopen(request,timeout=45) as r:
  assert r.status==200,name;raw=r.read()
 actual=hashlib.sha256(raw).hexdigest()
 if actual!=expected:
  # Sites may inject hosting runtime markup into HTML. Validate the exact
  # page identity and release/product markers; binary/JS/CSS/JSON remain strict.
  assert name.endswith('.html'),name
  text=raw.decode();title=re.search(r'<title>(.*?)</title>',text,re.S)
  expectation=m['html_expectations'][name]
  assert title and html.unescape(title[1])==expectation['title'],(name,text[:1000])
  assert all(x in text for x in expectation['markers']),name
  transformed[name]={'source_sha256':expected,'served_sha256':actual}

 p=dest/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(raw)
 return name
with ThreadPoolExecutor(max_workers=6) as pool:names=list(pool.map(fetch,m['files'].items()))
r=json.loads((dest/'release.json').read_text());assert r['version']=='0.7.1' and r['identity']=='FM-1_73000701'
raw=(dest/r['firmware']['url']).read_bytes()
product=''.join(chr((raw[i*48+47]-i-1)&255) for i in range(20) if raw[i*48+47]!=125)
assert product=='FM-1_73000701'
for k in ('firmware','wasm','worklet'):assert hashlib.sha256((dest/r[k]['url']).read_bytes()).hexdigest()==r[k]['sha256']
report={'version':'0.7.1','identity':product,'verified_https_files':len(names),'site_source_commit':m['site_source_commit'],'hashes':{k:r[k]['sha256'] for k in ('firmware','wasm','worklet')},'historical_assets':m['historical_assets'],'hosting_html_transformations':transformed}
Path('validation/release-071/LIVE_PUBLICATION.json').write_text(json.dumps(report,indent=2)+'\n')
(dest/'BACKUP-PROVENANCE.json').write_text(json.dumps(report,indent=2)+'\n')
archive=dest.parent/'AQUORA-0.7.1-website.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(dest.rglob('*')):
  if p.is_file():z.write(p,p.relative_to(dest))
archive.with_name('WEBSITE-SHA256SUMS.txt').write_text(hashlib.sha256(archive.read_bytes()).hexdigest()+'  '+archive.name+'\n')
print(json.dumps(report,indent=2))
