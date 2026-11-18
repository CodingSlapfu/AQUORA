#!/usr/bin/env python3
"""Extract the opening sung word from the credited buggly preview."""
import argparse
import hashlib
import subprocess
from pathlib import Path

import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("preview", type=Path)
parser.add_argument("output", type=Path)
args = parser.parse_args()
EXPECTED = "fec0061c9e2ea3edb9c0d2405fb196075e1dc71a0f60602fac1b94c9fe2243b6"
if hashlib.sha256(args.preview.read_bytes()).hexdigest() != EXPECTED:
    raise SystemExit("Source preview checksum mismatch")
decoded = subprocess.check_output([
    "ffmpeg", "-hide_banner", "-loglevel", "error", "-i", str(args.preview),
    "-ac", "1", "-ar", "11025", "-f", "s16le", "-",
])
recording = np.frombuffer(decoded, dtype="<i2")
pcm = recording[882:7718].astype(float)  # 0.080–0.700 seconds
pcm -= pcm.mean()
pcm *= 101 / np.max(np.abs(pcm))
pcm[:66] *= np.linspace(0, 1, 66)
pcm[-110:] *= np.linspace(1, 0, 110)
# The 52-sample period straddles the stable center of the vowel.
# Match the end of the loop to the approach to its beginning.
loop, release = 2340, 2340 + 52 * 28
for n in range(104):
    w = n / 103
    pcm[release - 104 + n] = (1 - w) * pcm[release - 104 + n] + w * pcm[loop - 104 + n]
pcm = np.rint(pcm).astype(np.int8)
rows = ["  " + ",".join(map(str, row)) + "," for row in np.array_split(pcm, (len(pcm) + 31) // 32)]
args.output.write_text(
    "/* Original sung DREAMS by buggly, CC0. See docs/VOCAL_CREDITS.md. */\n"
    f"enum {{VOCAL_DREAMS_LENGTH={len(pcm)}, VOCAL_DREAMS_LOOP={loop}, "
    f"VOCAL_DREAMS_RELEASE={release}, VOCAL_DREAMS_ATTACK=960}};\n"
    "static const int8_t vocal_dreams_flash[VOCAL_DREAMS_LENGTH]={\n"
    + "\n".join(rows) + "\n};\n"
    "static int8_t vocal_dreams_pcm[VOCAL_DREAMS_LENGTH] VOCAL_POOL;\n"
)
print(hashlib.sha256(pcm.tobytes()).hexdigest())
