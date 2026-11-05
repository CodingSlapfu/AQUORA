"""AQUORA 0.6.2 isolated host-panel regression: normal PLAY/fullscreen parity."""
from pathlib import Path
import subprocess
import sys
from PIL import Image

host, destination = sys.argv[1], Path(sys.argv[2])
destination.mkdir(parents=True, exist_ok=True)
commands = [
    "master 2048", "tap PLAY", "wait 250", "shot original-normal.png",
    "turn ALGO 5", "spin K1 1", "tap HOME",
    "expect visual 1", "expect page 0", "expect fullscreen 0",
    "wait 2400", "shot polar-normal.png",
    "wait 10200", "expect fullscreen 1", "shot polar-fullscreen.png",
    "turn ALGO 1", "expect fullscreen 0", "expect page 0",
    "shot polar-return.png", "expect drops 0"
]
script_path = destination / "regression.txt"
script_path.write_text("\n".join(commands) + "\n")
run = subprocess.run([host, str(script_path), str(destination)],
                     text=True, capture_output=True)
(destination / "regression.log").write_text(run.stdout + run.stderr)
if run.returncode:
    raise RuntimeError(run.stdout + run.stderr)

def image(name):
    im = Image.open(destination / name).convert("RGB")
    assert im.size == (240, 240), (name, im.size)
    return im

original = image("original-normal.png")
polar = image("polar-normal.png")
fullscreen = image("polar-fullscreen.png")
restored = image("polar-return.png")

def changed_fraction(a, b, top=144):
    return sum(a.getpixel((x, y)) != b.getpixel((x, y))
               for y in range(top, 240) for x in range(240)) / (240 * (240 - top))

# Both normal-mode visualizers must show the same functional K1-K4 lower panel.
# The decorative visuals are deliberately only in bands 1 and 2.
normal_diff = changed_fraction(original, polar)
assert normal_diff < .20, f"PLAY lower control bands differ {normal_diff:.1%}"

# Fullscreen must replace the panel with artwork, then restore every control band.
full_diff = changed_fraction(polar, fullscreen)
assert full_diff > .05, f"Polar fullscreen did not replace controls: {full_diff:.1%}"
restore_diff = changed_fraction(polar, restored)
assert restore_diff < .20, f"Returning from fullscreen failed to restore PLAY: {restore_diff:.1%}"
print(f"PASS Polar normal/fullscreen/return, diff {normal_diff:.1%}, {full_diff:.1%}, {restore_diff:.1%}")
