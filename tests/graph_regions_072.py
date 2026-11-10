from pathlib import Path
from PIL import Image
import sys
root=Path(sys.argv[1]);cases=0
for d in root.iterdir():
 if not d.is_dir():continue
 frames=[Image.open(d/f'f{i}.png').convert('RGB') for i in range(6)]
 assert all(x.size==(240,240) for x in frames)
 footer=[x.crop((0,208,240,240)).tobytes() for x in frames]
 assert len(set(footer))==1, f'graph modified footer: {d.name}'
 assert len({x.crop((35,48,233,208)).tobytes() for x in frames})>=3
 # Shared knob panels formerly started x6,y146; the outer graph margin
 # must retain pure selected background rather than the colored panel.
 white=int(d.name.split('-')[0]);bg=(255,255,255) if white else (0,0,0)
 assert all(x.getpixel((6,152))==bg for x in frames),d.name
 cases+=1
print(f'PASS {cases} exact 240x240 sequences: invariant footer208..239, moving graph48..207, no shared panel backgrounds')
