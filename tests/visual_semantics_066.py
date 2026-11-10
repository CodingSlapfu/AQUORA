from pathlib import Path
import subprocess,sys
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
s=(root/'device/visual_worker.c').read_text();(out/'visual-kernel.inc').write_text(s[:s.index('static void prepare_geometry')])
(out/'original-reference.inc').write_text((root/'tests/original_reference_066.inc').read_text())
subprocess.run(['gcc','-O2','-I'+str(out),str(root/'tests/visual_semantics_066.c'),'-lm','-o',str(out/'visual-semantics')],check=True)
r=subprocess.run([str(out/'visual-semantics')],capture_output=True,text=True);(out/'visual-semantics.log').write_text(r.stdout+r.stderr);print(r.stdout+r.stderr);assert r.returncode==0
