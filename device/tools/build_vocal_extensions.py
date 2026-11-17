#!/usr/bin/env python3
"""Build bounded 8-bit vocal units from credited source recordings.
Requires NumPy and SciPy. Input directory contains the source WAV files named
below. MP3 previews are decoded to mono 11025 Hz WAV with ffmpeg beforehand.
The source hashes and edit parameters are emitted beside the generated table.
"""
import argparse
import hashlib
import json
from pathlib import Path
import numpy as np
from scipy.io import wavfile
from scipy.signal import resample_poly, correlate

parser=argparse.ArgumentParser()
parser.add_argument("sources",type=Path)
parser.add_argument("output",type=Path)
args=parser.parse_args()
specs=[
    ("eh","eh.wav",None,.38,1,False),
    ("ee","ee.wav",None,.38,5,False),
    ("hum","hum.wav",None,.40,4,False),
    ("love","love.wav",8.78,1.06,1,True),
    ("baby","baby.wav",1.24,.80,5,True),
    ("me","you.wav",1.34,.98,5,True),
]
metadata=[]
tables=[]
for name,file,start,duration,mouth,word in specs:
    source=args.sources/file
    rate,a=wavfile.read(source)
    if a.ndim==2:
        a=a.astype(np.float64).mean(axis=1)
    a=a.astype(np.float64)
    a=resample_poly(a,11025,rate)
    a/=max(np.max(np.abs(a)),1)
    if start is None:
        energy=np.convolve(a*a,np.ones(220)/220,mode="same")
        onset=np.flatnonzero(energy>max(energy)*.15)[0]
        start=max(0,(onset+300)/11025)
    data=a[round(start*11025):round((start+duration)*11025)].copy()
    data-=np.mean(data)
    data*=.82/max(np.max(np.abs(data)),.01)
    fade=min(66,len(data)//8)
    data[:fade]*=np.linspace(0,1,fade)
    data[-fade:]*=np.linspace(1,0,fade)
    # Sustain the final syllable in BABY, the middle vowel in single words.
    middle=round(len(data)*(.69 if name=="baby" else .54))
    analysis=data[max(0,middle-512):middle+512]
    ac=correlate(analysis,analysis,mode="full")[len(analysis)-1:]
    low,high=round(11025/700),round(11025/100)
    period=low+int(np.argmax(ac[low:high]))
    hz=11025/period
    # Match a long, integer-period loop, then crossfade its entrance.
    cycles=max(2,min(24,(middle-96)//period,(len(data)-middle-96)//period))
    loop=middle-period*cycles
    end=middle+period*cycles
    blend=min(period*2,loop)
    for n in range(blend):
        mix=n/blend
        data[end-blend+n]=data[end-blend+n]*(1-mix)+data[loop-blend+n]*mix
    pcm=np.round(data*127).astype(np.int8)
    attack=round(len(pcm)*(.67 if name=="baby" else .16))
    rows=["  "+",".join(str(int(v)) for v in pcm[i:i+32])+"," for i in range(0,len(pcm),32)]
    tables.append(f"static const int8_t vocal_{name}_flash[{len(pcm)}]={{\n"+"\n".join(rows)+"\n};\n")
    tables.append(f"static int8_t vocal_{name}_pcm[{len(pcm)}] VOCAL_POOL;\n")
    metadata.append(dict(name=name,length=len(pcm),loop=loop,release=end,
                         attack=attack,hz=hz,mouth=mouth,word=word,
                         source_file=file,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
                         start_seconds=start,duration_seconds=duration,
                         pcm_sha256=hashlib.sha256(pcm.tobytes()).hexdigest()))
args.output.write_text("/* Credited recordings. See docs/VOCAL_CREDITS.md. */\n"+"".join(tables))
args.output.with_suffix(".json").write_text(json.dumps(metadata,indent=2)+"\n")
print(json.dumps(metadata,indent=2))
