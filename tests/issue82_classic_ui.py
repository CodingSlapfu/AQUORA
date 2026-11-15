#!/usr/bin/env python3
"""Issue #82: original v0.7.6 LCD language, transparent spiral title, v0.7.7 audio intact."""
from pathlib import Path
from PIL import Image
import os
import subprocess
import sys

source = Path("device/ui.c").read_text()
assert "/* Static panel palettes. Canvas is independently exact black or white. */" in source
for obsolete in ("metal_panel(", "cabinet_rails(", "focus_jewel(", "static void dial("):
    assert obsolete not in source, obsolete
assert "if(letters)cv_text(x,y,&FONT_L,title,RGB(119,255,225));" in source
overlay = source.split("   if(intro_active){", 1)[-1].split("if(band==2){", 1)[-1].split("if(band==4){", 1)[0]
assert "char title[7]=\"AQUORA\"" in overlay
assert "metal_panel" not in overlay and "cv_rect" not in overlay
assert "if(band==0)fullscreen_labels();" in source
assert "geometry(band*48);" in source
assert "mixer_console(band);" in source and "mixer_focus_strip(band);" in source
assert "sr_score_preview(" in source
assert "const char *shown=knob_value(control_for(knob)" in source

host = sys.argv[1]
output = Path(sys.argv[2]).resolve()
output.mkdir(parents=True, exist_ok=True)

def check(script, name, env=None):
    folder = output / name
    folder.mkdir(exist_ok=True)
    file = folder / "commands.txt"
    file.write_text("\n".join(script) + "\n")
    result = subprocess.run(
        [host, str(file), str(folder)], text=True, capture_output=True,
        env=dict(os.environ, AQUORA_TEST_SEED="1", **(env or {})))
    (folder / "host.log").write_text(result.stdout + result.stderr)
    assert result.returncode == 0, result.stdout + result.stderr
    return folder

intro = check([
    "wait 4500", "expect intro 1", "shot title.png",
    "wait 300", "shot spiral-next.png",
    "tap HOME", "expect intro 0", "expect drops 0"
], "intro", {"AQUORA_TEST_INTRO": "1"})
title = Image.open(intro / "title.png").convert("RGB")
next_frame = Image.open(intro / "spiral-next.png").convert("RGB")
assert title.size == next_frame.size == (240, 240)
# No v0.7.7 walnut rails; black true-color corners and a centered AQUORA Sans overlay.
assert title.getpixel((0, 0)) == (0, 0, 0)
assert title.getpixel((239, 239)) == (0, 0, 0)
region = [(x, y, title.getpixel((x, y))) for y in range(98, 144) for x in range(22, 218)]
teal = [(x, y) for x, y, (r, g, b) in region if 100 <= r <= 145 and g >= 245 and b >= 205]
assert len(teal) > 60, "Centered aqua title was not rendered"
xmin, xmax = min(x for x, y in teal), max(x for x, y in teal)
assert abs((xmin + xmax) / 2 - 120) < 5, (xmin, xmax)
# Pink/yellow spiral pixels in the title region show that no opaque plaque covered it.
vivid = sum(1 for x, y, (r, g, b) in region
            if (r > 170 and b > 120 and g < 170) or (r > 195 and g > 170 and b < 140))
assert vivid > 5, f"Spiral appears obscured behind wordmark ({vivid} pixels)"
assert title.tobytes() != next_frame.tobytes(), "Spiral was not animated"

pages = check([
    "master 2048", "tap PLAY", "wait 1200", "tap HOME", "shot play.png",
    "turn ALGO 1", "wait 150", "shot sound.png",
    "turn ALGO 2", "wait 150", "shot evolve.png",
    "turn ALGO 3", "wait 150", "shot mix.png",
    "expect running 1", "expect drops 0"
], "pages", {"AQUORA_TEST_AUTOSTART": "0"})
for page in ("play", "sound", "evolve", "mix"):
    image = Image.open(pages / f"{page}.png").convert("RGB")
    assert image.size == (240, 240)
    assert image.getpixel((0, 0)) == (0, 0, 0), f"Unexpected wood rail on {page}"
print("PASS Issue #82: classic 240x240 UI, centered transparent AQUORA intro, animated visible spiral, four pages, preserved v0.7.7 paths")
