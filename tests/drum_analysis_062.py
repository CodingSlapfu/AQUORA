"""Level-matched all-kit actual-DSP A/B metrics; listening remains subjective."""
from array import array
from pathlib import Path
import csv,math,subprocess,sys,wave,hashlib
before,after,destination=sys.argv[1:];out=Path(destination);out.mkdir(parents=True,exist_ok=True)
rows=[];hashes=set()
for kit in range(19):
 for part in range(5):
  metrics=[]
  for name,exe in [('061',before),('062',after)]:
   data=subprocess.check_output([exe,str(kit),str(part),'0'])
   samples=array('h');samples.frombytes(data)
   channels=1 if part<3 else 2
   rms=math.sqrt(sum(s*s for s in samples)/len(samples));peak=max(map(abs,samples))
   diff=math.sqrt(sum((samples[i]-samples[i-channels])**2 for i in range(channels,len(samples)))/(len(samples)-channels))
   edge=samples[:int(.008*44100)*channels]
   attack=math.sqrt(sum(s*s for s in edge)/len(edge))
   assert rms>0 and peak<32767,(kit,part,name,rms,peak)
   metrics.append((rms,peak,diff/rms,attack/rms))
   gain=min(1400/rms,28000/peak)
   with wave.open(str(out/f'kit-{kit:02}-{part}-{name}.wav'),'wb') as f:
    f.setparams((channels,2,44100,0,'NONE','not compressed'))
    f.writeframes(array('h',(int(s*gain) for s in samples)).tobytes())
   if name=='062' and part<3:hashes.add(hashlib.sha256(data).hexdigest())
  rows.append([kit,part,*metrics[0],*metrics[1]])
with (out/'metrics.csv').open('w') as f:
 w=csv.writer(f);w.writerow(['kit','part','old_rms','old_peak','old_diff_rms','old_attack_rms','new_rms','new_peak','new_diff_rms','new_attack_rms']);w.writerows(rows)
assert len(hashes)>=50,len(hashes)
for part in range(3):
 subset=[x for x in rows if x[1]==part]
 print(f'part {part}: brighter normalized HF proxy in {sum(x[8]>x[4]*1.03 for x in subset)}/19 kits; attack emphasis in {sum(x[9]>x[5]*1.03 for x in subset)}/19')
# All 13 intentions and 19 preserved kits through real mixed output.
for scene in range(13):
 for kit in range(19):
  raw=subprocess.check_output([after,str(kit),'4',str(scene)])
  pcm=array('h');pcm.frombytes(raw)
  assert max(map(abs,pcm))<32767,(scene,kit)
print(f'PASS 19 kits x 5 contexts old/new, {len(hashes)} distinct kernel renders; 247 intention/kit mixed renders below PCM clip; level-matched WAVs + CSV')
