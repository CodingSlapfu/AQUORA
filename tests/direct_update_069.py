"""Exercise actual browser updater, including same-identity in-place reflash."""
from pathlib import Path
import subprocess,sys
root=Path.cwd();source=root/'vendor/fm1-base/tests/installer_ports_test.mjs'
original=source.read_text()
# A mismatching return must stay distinct even when source/target identities match.
original=original.replace('["FM-1_7010300", "mismatch"]','["FM-1_73999999", "mismatch"]')
for prior in ('FM-1_73000600','FM-1_73000601','FM-1_73000602','FM-1_73000603','FM-1_73000604','FM-1_73000605','FM-1_73000606','FM-1_73000607','FM-1_73000608','FM-1_73000609'):
 target=source.with_name('aquora_direct_update.mjs')
 target.write_text(original.replace('FM-1_7010300',prior).replace('FM-1_7010400','FM-1_73000609'))
 print(f'UPDATER {prior} -> FM-1_73000609',flush=True)
 subprocess.run(['node',str(target),sys.argv[1]],check=True)
target.unlink()
