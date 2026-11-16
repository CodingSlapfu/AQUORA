"""Create original, compact percussion one-shots for AQUORA."""
import math
import random
from pathlib import Path

RATE = 11025
LENGTH = 2048
# Oscillator frequencies, a spectral decay, noise amount, transient shape.
PARTS = [
    (54, 104, .049, .12, "kick"), (210, 330, .027, .88, "clap"),
    (1510, 2461, .057, .77, "metal"), (1980, 2633, .016, .7, "metal"),
    (720, 1220, .010, .45, "tap"), (195, 327, .045, .63, "noise"),
    (89, 143, .051, .12, "bend"), (167, 257, .047, .09, "bend"),
    (791, 2351, .070, .88, "metal"), (975, 1713, .055, .45, "metal"),
    (271, 1533, .036, .93, "noise"), (222, 463, .042, .18, "bend"),
    (440, 1209, .069, .11, "bell"),
]

def samples(part, shape):
    f1, f2, decay, noise_level, kind = shape
    rng = random.Random(712 + part)
    old, phase = 0.0, 0.0
    values = []
    for i in range(LENGTH):
        t = i / RATE
        noise = rng.uniform(-1, 1)
        high = noise - old
        old += (noise - old) * .26
        sweep = (1 + .7 * math.exp(-t * 85)) if kind in ("kick", "bend") else 1
        phase += f1 * sweep / RATE
        tone = math.sin(math.tau * phase) * .7 + .3 * math.sin(math.tau * f2 * t)
        env = math.exp(-t / decay)
        if kind == "clap":
            env += sum(.45 * math.exp(-(t - x) / .003) for x in (.008, .018) if t >= x)
        if kind == "bell":
            tone += .2 * math.sin(math.tau * 1517 * t)
        attack = min(1, i / 6)
        release = min(1, (LENGTH - 1 - i) / 170)
        values.append(round(104 * attack * release * env * ((1-noise_level)*tone + noise_level*.68*high)))
    return [min(127, max(-127, x)) for x in values]

dest = Path(__file__).resolve().parents[1] / "device/dsp/drum_waves.inc"
with dest.open("w") as f:
    f.write("/* Original AQUORA percussion rendered by tools/generate_drum_waves.py. */\n")
    f.write("enum { DRUM_WAVE_LENGTH = 2048 };\nstatic const int8_t drum_flash[13][DRUM_WAVE_LENGTH] = {\n")
    for index, part in enumerate(PARTS):
        f.write(" {\n")
        y = samples(index, part)
        for offset in range(0, LENGTH, 32):
            f.write("  " + ",".join(map(str, y[offset:offset+32])) + ",\n")
        f.write(" },\n")
    f.write("};\n")
