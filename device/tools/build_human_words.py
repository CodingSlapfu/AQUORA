#!/usr/bin/env python3
"""Build the finite, signed PCM word bank from credited Freesound previews."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import subprocess

import numpy as np
from scipy.signal import resample_poly

RATE = 6800
POSITIVE = [1, 2, 4, 6, 9, 12, 16, 21, 26, 32, 39, 47, 56, 66, 77]
LEVELS = np.array([0, *POSITIVE, 88, *(-x for x in POSITIVE)], dtype=np.int8)
SOURCES = {
    759142: ("Valerie-Vivegnis", "4.0", "ed0949fb5bc6e7143c9cd75928210de68126f36f12572d155a02d4d09731d4cd"),
    417410: ("TheScarlettWitch89", "3.0", "451a67b26ff5ee787d48bd2b7c67117e71f8644d79a8c567bfa6cc7dd089b633"),
    417513: ("TheScarlettWitch89", "3.0", "3f163644ea154d9dbb039679fae1878e28f9e4bf0b9b4fd390f2dc43a1851a4f"),
    476829: ("womb_affliction", "4.0", "f458b3687cbd7eaa4a9b872299bf431cb067074b7d4cb9e7c4f151a68ce0effa"),
    417935: ("TheScarlettWitch89", "3.0", "fe59dfdb02874fc8ab20f0058f8c4e4447858e470737575c57bcaeb001b13a91"),
    417936: ("TheScarlettWitch89", "3.0", "52a550b79ecd4114542b60fc39c5cf57bf2de6d7295063e5911eb7daf90da764"),
}
# Word, source, decoded-preview bounds, median voiced frequency, mouth poses.
WORDS = [
    ("GO", 417513, 7.925, 8.615, 330.5, (4, 2, 3)),
    ("NO", 417513, 6.560, 7.125, 281.5, (4, 2, 3)),
    ("WAY", 417513, 2.915, 5.160, 362.5, (3, 1, 5)),
    ("BE", 417936, 1.86, 2.35, 396.0, (4, 5, 5)),
    ("FEEL", 476829, .968, 1.565, 275.5, (6, 5, 4)),
    ("COLORS", 476829, 1.550, 3.005, 279.6, (4, 2, 6)),
    ("GREEN", 476829, 4.240, 5.465, 214.5, (4, 5, 4)),
    ("BEAUTIFUL", 417936, 2.54, 3.96, 391.0, (4, 5, 4)),
    ("I", 476829, .290, .935, 185.0, (1, 1, 5)),
    ("GOING", 417410, 1.017, 1.549, 225.0, (4, 2, 4)),
    ("WHEN", 417935, 1.595, 1.785, 410.0, (3, 5, 4)),
    ("CALL", 417935, 1.935, 2.645, 428.0, (4, 2, 4)),
    ("DO", 417936, .605, .939, 370.0, (4, 3, 3)),
    ("HELLO", 759142, .56, 1.18, 245.0, (1, 2, 3)),
]


def ffmpeg(*args, data=None):
    return subprocess.run(
        ["ffmpeg", "-v", "error", *args], input=data,
        stdout=subprocess.PIPE, check=True,
    ).stdout


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("previews", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    decoded = {}
    for source, (_, _, expected) in SOURCES.items():
        path = args.previews / f"{source}.mp3"
        if sha(path.read_bytes()) != expected:
            raise ValueError(f"Preview hash changed for {source}")
        decoded[source] = np.frombuffer(ffmpeg(
            "-i", str(path), "-ar", "48000", "-ac", "1", "-f", "f32le", "-"
        ), dtype="<f4")

    entries, lines, manifest = [], [], []
    for name, source, begin, end, pitch, mouth in WORDS:
        original = decoded[source][round(begin * 48000):round(end * 48000)]
        mono = original.copy()
        mono -= np.mean(mono)
        # Pitch-preserving compression keeps the complete performance; the
        # soft fades remove edit clicks without masking word attacks.
        limit = 1.01 if name in ("BEAUTIFUL", "COLORS") else .785
        speed = max(1., (end - begin) / limit)
        if speed > 1:
            mono = np.frombuffer(ffmpeg(
                "-f", "f32le", "-ar", "48000", "-ac", "1", "-i", "-",
                "-af", f"rubberband=tempo={speed:.8f}:transients=crisp:formant=preserved",
                "-f", "f32le", "-", data=mono.astype("<f4").tobytes(),
            ), dtype="<f4").copy()
        divisor = math.gcd(RATE, 48000)
        mono = resample_poly(mono, RATE // divisor, 48000 // divisor)
        fade_in, fade_out = 5 * RATE // 1000, 24 * RATE // 1000
        mono[:fade_in] *= np.linspace(0, 1, fade_in)
        mono[-fade_out:] *= np.linspace(1, 0, fade_out)
        peak = np.max(np.abs(mono))
        pcm = mono * (77. / peak)
        codes = np.argmin(np.abs(pcm[:,None] - LEVELS[None,:]), axis=1).astype(np.uint8)
        quantized = LEVELS[codes]
        bits = ((codes[:, None] >> np.arange(5)) & 1).astype(np.uint8)
        packed = np.pad(np.packbits(bits.ravel(), bitorder="little"), (0, 1)).view(np.int8)
        snr = 10*np.log10(np.sum(pcm**2)/np.sum((pcm-quantized)**2))
        symbol = f"human_{name.lower()}_pcm"
        lines.append(f"static const int8_t {symbol}[{len(packed)}]={{")
        for i in range(0, len(packed), 24):
            lines.append("  " + ",".join(str(x) for x in packed[i:i + 24]) + ",")
        lines.append("};")
        attack = min(len(quantized) // 6, RATE * 5 // 100)
        release_pose = len(quantized) * 5 // 6
        entries.append(
            f"  {{{symbol},{len(quantized)}u,0u,{release_pose}u,{attack}u,"
            f"{{{','.join(map(str,mouth))}}},{pitch:.1f}f}},"
        )
        user, license_version, preview_hash = SOURCES[source]
        manifest.append(dict(word=name, attribution=user if user != "womb_affliction"
                             else "Katarina Rose (womb_affliction)",
                             source=f"https://freesound.org/people/{user}/sounds/{source}/",
                             license=f"https://creativecommons.org/licenses/by/{license_version}/",
                             preview_sha256=preview_hash, decoded_cut_sha256=sha(original.tobytes()),
                             offset_seconds=[begin, end], pcm_sha256=sha(quantized.tobytes()),
                             sample_rate=RATE, samples=len(quantized), packed_bytes=len(packed),
                             packed_sha256=sha(packed.tobytes()), codec_snr_db=round(float(snr),2), median_hz=pitch,
                             tempo_ratio=round(speed,8),
                             **({"performance": "spoken"} if source == 759142 else {})))
    (args.output / "human_words.inc").write_text(
        "/* Generated by device/tools/build_human_words.py. See VOCAL_CREDITS.md. */\n"
        + "static const int8_t human_word_levels[]={"+",".join(str(x) for x in LEVELS)+"};\n"
        + "\n".join(lines) + "\n\n#define AQUORA_HUMAN_WORDS \\\n"
        + " \\\n".join(entries) + "\n"
    )
    (args.output / "human_words.json").write_text(json.dumps(manifest, indent=2)+"\n")


if __name__ == "__main__":
    main()
