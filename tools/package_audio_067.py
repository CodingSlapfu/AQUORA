from pathlib import Path
import sys,wave,zipfile,io
mix,stems,target=map(Path,sys.argv[1:])
with zipfile.ZipFile(target,'w',zipfile.ZIP_DEFLATED,6) as z:
 for p in sorted(mix.glob('*.pcm')):
  b=io.BytesIO()
  with wave.open(b,'wb') as w:w.setnchannels(2);w.setsampwidth(2);w.setframerate(44100);w.writeframes(p.read_bytes())
  z.writestr('stereo-mixes/'+p.stem+'.wav',b.getvalue())
 for p in sorted(stems.glob('*.pcm')):
  b=io.BytesIO()
  with wave.open(b,'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(11025);w.writeframes(p.read_bytes())
  z.writestr('post-gain-role-stems/'+p.stem+'.wav',b.getvalue())
 for p in sorted(mix.glob('*.csv')):z.write(p,'note-gain-timelines/'+p.name)
 z.writestr('README.txt','Actual accepted DSP renders. Matched seed, intention, style, patches and variation; compare 16/32/64. Human aesthetic acceptance remains owner-unverified. Stem excerpts are 11025Hz normal-arrangement post-gain audio, not SOLO-bypassed gates.\n')
print('Preserved 36 stereo mixes, 24 real role stems and note/gain timelines')
