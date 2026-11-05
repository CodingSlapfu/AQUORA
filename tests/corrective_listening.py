"""Produce comparable PCM listening examples from the pinned and corrective DSP."""
from array import array
from pathlib import Path
import math
import subprocess
import sys
import wave

before, after, directory = sys.argv[1:]
output = Path(directory)
output.mkdir(parents=True, exist_ok=True)


def render(exe, kind, sound_id, filename):
    raw = subprocess.check_output([exe, kind, str(sound_id)])
    samples = array("h")
    samples.frombytes(raw)
    rms = math.sqrt(sum(s * s for s in samples) / len(samples))
    peak = max(map(abs, samples))
    assert rms > 0 and peak > 0
    gain = min(1400 / rms, 28000 / peak)
    with wave.open(str(output / f"{filename}.wav"), "wb") as wav:
        wav.setparams((2, 2, 44100, 0, "NONE", "not compressed"))
        wav.writeframes(array("h", (int(s * gain) for s in samples)).tobytes())
    print(f"{filename} original RMS {rms:.1f}, peak {peak}, applied gain {gain:.3f}")


for kind, prior, current, label in [
    ("nature", 4, 4, "forest"),
    ("nature", 6, 6, "birds"),
    ("patch", 8, 24, "glass-keys"),
    ("drums", 3, 15, "moon-kit"),
]:
    render(before, kind, prior, f"{label}-060")
    render(after, kind, current, f"{label}-061")
for index, label in enumerate(("florida-night", "twilight-meadow", "crystal-creek", "mountain-rain"), 8):
    render(after, "nature", index, label)
for index, label in enumerate(("stable", "subtle", "flowing", "adventurous")):
    render(after, "evolve", index, f"evolve-{label}")
