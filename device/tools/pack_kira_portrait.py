#!/usr/bin/env python3
"""Convert original KIRA RGB565 run triples to palette-coded row runs."""
import argparse
from collections import Counter
from pathlib import Path
import re


def values(source, name):
    match = re.search(rf"{name}\[\].*?=\{{(.*?)\}};", source, re.S)
    return [int(value) for value in re.findall(r"\d+", match[1])]


def array(name, datatype, data, columns):
    return f"static const {datatype} {name}[]={{\n" + "".join(
        "  " + ",".join(map(str, data[i:i + columns])) + ",\n"
        for i in range(0, len(data), columns)
    ) + "};\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("original", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    original = args.original.read_text()
    data = values(original, "k9_art_data")
    rows = values(original, "k9_art_rows")
    runs = [(n, low | high << 8) for n, low, high in zip(data[::3], data[1::3], data[2::3])]
    palette = [color for color, _ in Counter(color for _, color in runs).most_common(255)]
    indices = {color: index for index, color in enumerate(palette)}

    offsets, packed = [], []
    position = 0
    for row in rows:
        assert row % 3 == 0
        for count, color in runs[position:row // 3]:
            index = indices.get(color, 255)
            packed.extend((count, index))
            if index == 255:
                packed.extend((color & 255, color >> 8))
        offsets.append(len(packed))
        position = row // 3
    for count, color in runs[position:]:
        index = indices.get(color, 255)
        packed.extend((count, index))
        if index == 255:
            packed.extend((color & 255, color >> 8))

    # Verify every row's full sequence before generating the header.
    for row, start in enumerate(offsets):
        end = offsets[row + 1] if row + 1 < len(offsets) else len(packed)
        restored = []
        cursor = start
        while cursor < end:
            count, code = packed[cursor:cursor + 2]
            cursor += 2
            if code == 255:
                color = packed[cursor] | packed[cursor + 1] << 8
                cursor += 2
            else:
                color = palette[code]
            restored.append((count, color))
        original_end = rows[row + 1] if row + 1 < len(rows) else len(data)
        assert restored == runs[rows[row] // 3:original_end // 3]

    regions = original[original.index("static const uint16_t k9_art_regions"):]
    args.output.write_text(
        "/* Lossless palette-coded RGB565 row runs. See tools/pack_kira_portrait.py. */\n"
        + array("k9_art_colors", "uint16_t", palette, 16)
        + array("k9_art_data", "uint8_t", packed, 32)
        + array("k9_art_rows", "uint32_t", offsets, 12)
        + regions
    )
    print(f"Verified {len(rows)} rows. Original {len(data)} bytes; "
          f"packed {len(packed) + 2 * len(palette)} bytes including palette.")


if __name__ == "__main__":
    main()
