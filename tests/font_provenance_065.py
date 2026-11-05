from pathlib import Path
import hashlib,json,re,sys
header,binary,out=map(Path,sys.argv[1:]);text=header.read_text()
assert 'font set AQUORA Sans / Inter Tight derivative' in text
assert '#define AQUORA_FONT_FAMILY 0x41515335u' in text
assert 'FONT_I_DATA' in text and 'FONT_B_DATA' in text and 'FONT_T_DATA' in text
atlas=re.search(r'#define AQUORA_FONT_ATLAS_SHA256 "([0-9a-f]{64})"',text)[1]
marker=('AQUORA Sans:'+atlas).encode();assert marker in binary.read_bytes(),binary
out.write_text(json.dumps({'family':'AQUORA Sans','atlas_fingerprint':atlas,'generated_header_sha256':hashlib.sha256(header.read_bytes()).hexdigest(),'compiled_binary':binary.name,'compiled_binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'runtime_about_marker_in_exact_binary':True,'faces':['T','XS','S','B','M','L','I']},indent=2)+'\n')
print('PASS actual AQUORA Sans fingerprint retained in',binary.name,'with bold and real precomputed italic atlas')
