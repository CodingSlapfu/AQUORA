#!/usr/bin/env python3
"""Offline AQUORA Sans bitmap derivative of OFL Inter Tight.

Reuses the platform's packed four-bit glyph writer. No runtime rasterization.
Distinctive slash-zero, barred capital I and footed lowercase l are drawn into
the bitmap, with tabular digits and optical weight choices for LCD legibility.
The unmodified upstream vector source retains its original name and license.
"""
from pathlib import Path
import importlib.util
import sys

root = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("platform_font", root.parent / "vendor/fm1-base/tools/gen_font.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
module.FONTDIRS.insert(0, root / "fonts")
module.FONTSET = "AQUORA Sans / Inter Tight derivative"
module.SIZES = [(n, f"InterTight-source.ttf@wght={weight}", px, 1, False, 32, 126)
                for n, px, weight in [("T", 9, 650), ("XS", 11, 620),
                                      ("S", 13, 560), ("B", 13, 720),
                                      ("M", 16, 660), ("L", 27, 720)]]
original_render = module.render


def render(*args):
    height, glyphs = original_render(*args)
    first = args[-2]
    for char in "0Il":
        index = ord(char) - first
        advance, width, pixels = glyphs[index]
        lit = [(x, y) for y in range(height) for x in range(width)
               if pixels[y * width + x] >= 8]
        if not lit:
            continue
        x0, x1 = min(x for x, y in lit), max(x for x, y in lit)
        y0, y1 = min(y for x, y in lit), max(y for x, y in lit)
        if char == "0":
            for y in range(y0 + 1, y1):
                x = x1 - 1 - round((y - y0 - 1) * max(0, x1 - x0 - 2) / max(1, y1 - y0 - 2))
                pixels[y * width + x] = 13
        else:
            # Distinguish I / l / 1 in short parameter captions and counters.
            span = range(max(1, x0 - 1), min(width - 1, x1 + 2))
            for x in span:
                pixels[y1 * width + x] = 15
                if char == "I":
                    pixels[y0 * width + x] = 15
        glyphs[index] = (advance, width, pixels)
    return height, glyphs


module.render = render
module.main(Path(sys.argv[1]) / "felucca_font.h")
