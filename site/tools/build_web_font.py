#!/usr/bin/env python3
"""Build a native AQUORA Sans website companion from the firmware's OFL Inter Tight source.

The FM-1 uses offline 4-bit bitmap rendering. Web browsers need regular vector
font files, so this produces static WOFF2 faces with analogous 0, I, l details.
Source stays unchanged. Reuse is allowed under the included SIL OFL 1.1.
"""
from pathlib import Path
import sys
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.pens.boundsPen import BoundsPen


def outline(pen, points):
    pen.moveTo(tuple(round(c) for c in points[0]))
    for point in points[1:]:
        pen.lineTo(tuple(round(c) for c in point))
    pen.closePath()


def lcd_optical_details(font):
    """Match firmware slash-zero, barred I, and footed l in scalable outlines."""
    cmap = font.getBestCmap()
    for character in "0Il":
        glyph_name = cmap.get(ord(character))
        if not glyph_name:
            raise ValueError("Missing required character " + character)
        glyphset = font.getGlyphSet()
        bounds_pen = BoundsPen(glyphset)
        glyphset[glyph_name].draw(bounds_pen)
        if bounds_pen.bounds is None:
            raise ValueError("Empty required glyph " + character)
        x0, y0, x1, y1 = bounds_pen.bounds
        width, height = x1 - x0, y1 - y0
        pen = TTGlyphPen(glyphset)
        glyphset[glyph_name].draw(pen)
        if character == "0":
            stroke = max(12, round(height * .065))
            xa, ya = x0 + width * .22, y0 + height * .15
            xb, yb = x1 - width * .22, y1 - height * .15
            outline(pen, [(xa, ya), (xa + stroke, ya),
                          (xb + stroke, yb), (xb, yb)])
        else:
            foot = max(12, round(height * .07))
            bar_width = width * (.90 if character == "I" else .66)
            center = (x0 + x1) / 2
            outline(pen, [(center - bar_width/2, y0), (center + bar_width/2, y0),
                          (center + bar_width/2, y0 + foot), (center - bar_width/2, y0 + foot)])
            if character == "I":
                outline(pen, [(center - bar_width/2, y1 - foot), (center + bar_width/2, y1 - foot),
                              (center + bar_width/2, y1), (center - bar_width/2, y1)])
        font["glyf"][glyph_name] = pen.glyph()


def set_family_name(font, family, style):
    names = {1: family, 2: style, 4: family + " " + style,
             6: "AQUORASans-" + style, 16: family, 17: style}
    for name_id, label in names.items():
        for platform, encoding, language in ((3, 1, 0x409), (1, 0, 0)):
            font["name"].setName(label, name_id, platform, encoding, language)


def build(source, output, weight, style):
    font = TTFont(source)
    if "fvar" in font:
        axes = {axis.axisTag for axis in font["fvar"].axes}
        if "wght" not in axes:
            raise ValueError("Expected a wght axis in the firmware source")
        font = instantiateVariableFont(font, {"wght": weight}, inplace=False)
    lcd_optical_details(font)
    set_family_name(font, "AQUORA Sans", style)
    font["OS/2"].usWeightClass = weight
    font.flavor = "woff2"
    font.save(output)
    verify = TTFont(output)
    if verify["name"].getDebugName(1) != "AQUORA Sans":
        raise ValueError("Wrong web font family name")
    if verify["OS/2"].usWeightClass != weight:
        raise ValueError("Wrong weight class")
    if not all(ord(ch) in verify.getBestCmap() for ch in "0IlAa09"):
        raise ValueError("Missing latin glyphs")
    print("Generated", output, output.stat().st_size, "bytes")


def main():
    source = Path(sys.argv[1])
    destination = Path(sys.argv[2])
    destination.mkdir(parents=True, exist_ok=True)
    build(source, destination / "aquora-sans-regular.woff2", 560, "Regular")
    build(source, destination / "aquora-sans-bold.woff2", 720, "Bold")


if __name__ == "__main__":
    main()
