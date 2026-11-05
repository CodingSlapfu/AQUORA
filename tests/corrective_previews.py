"""Exercise the actual host panel and collect 240x240 corrective previews."""
import os
from pathlib import Path
import subprocess
import sys

from PIL import Image


def panel(name, commands, **fixtures):
    folder = output / name
    folder.mkdir(parents=True, exist_ok=True)
    script = folder / "panel.txt"
    script.write_text("\n".join(commands) + "\n")
    env = dict(os.environ, AQUORA_TEST_SEED="1", **fixtures)
    result = subprocess.run([host, str(script), str(folder)], env=env,
                            text=True, capture_output=True)
    (folder / "panel.log").write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(f"{name}: {result.stdout}{result.stderr}")
    return folder


def clip(name, setup, **fixtures):
    commands = list(setup)
    for frame in range(30):
        commands.extend([f"shot {frame:02}.png", "wait 67"])
    folder = panel(name, commands, **fixtures)
    files = sorted(folder.glob("*.png"))
    frames = [Image.open(path).convert("RGB") for path in files]
    assert len(frames) == 30 and all(frame.size == (240, 240) for frame in frames)
    assert len({frame.tobytes() for frame in frames}) == 1 if name == "startup" else len({frame.tobytes() for frame in frames}) > 5
    frames[0].save(folder / "preview.gif", save_all=True, append_images=frames[1:],
                   duration=67, loop=0)
    for path in files[1:]:
        path.unlink()


host = sys.argv[1]
output = Path(sys.argv[2])

for level in range(-1, 5):
    commands = [f"expect battery_level {level}", "shot normal.png"]
    if level in (0, 4):
        commands += ["wait 10100", "expect fullscreen 1", "expect battery_fade 4",
                     "shot fullscreen-entry.png", "wait 1580", "expect battery_fade 2",
                     "shot fullscreen-dim.png", "wait 1400", "expect battery_fade 0",
                     "shot fullscreen-clear.png", "tap HOME", "expect fullscreen 0",
                     "expect battery_fade 4", "shot returned.png"]
    folder = panel(f"battery-{level}", commands, AQUORA_TEST_BATTERY=str(level))
    normal = Image.open(folder / "normal.png").convert("RGB")
    assert normal.size == (240, 240)
    # The top border is a known flat segment of the glyph and otherwise blank.
    assert (normal.getpixel((220, 5)) != normal.getpixel((216, 5))) == (level >= 0)
    if level in (0, 4):
        entry = Image.open(folder / "fullscreen-entry.png").convert("RGB")
        dim = Image.open(folder / "fullscreen-dim.png").convert("RGB")
        clear = Image.open(folder / "fullscreen-clear.png").convert("RGB")
        assert sum(entry.getpixel((220, 5))) > sum(dim.getpixel((220, 5)))
        assert sum(dim.getpixel((220, 5))) > sum(clear.getpixel((220, 5)))

buttons = "FX SEL ENV LFO EDIT GLO HOME SAVE ARP SEQ PLAY REC OCT- OCT+".split()
knobs = "SELECT ALGO PRESET K1 K2 K3 K4".split()
for number, skip in enumerate([f"tap {button}" for button in buttons]
                              + [f"turn {knob} 1" for knob in knobs]
                              + [f"tapkey {key}" for key in range(27)] + ["master 1024"]):
    panel(f"intro-skip-{number}", ["expect intro 1", "wait 80", skip, "wait 55",
                                  "expect intro 0", "expect page 0", "expect running 0"],
          AQUORA_TEST_INTRO="1")

panel("intro-timeout", ["expect intro 1", "wait 500", "expect peak_db_max -27",
                       "wait 9700", "expect intro 0", "expect running 0"],
      AQUORA_TEST_INTRO="1")
panel("intro-mute", ["expect intro 1", "wait 80", "master 0", "peakreset", "wait 400",
                    "expect intro 0", "expect peak_db_max -110"],
      AQUORA_TEST_INTRO="1")
led_commands = ["wait 70", "leds", "turn ALGO 5", "spin K2 1", "expect lighting 1",
                "key 6 down", "wait 40", "leds", "key 6 up", "wait 40", "leds",
                "spin K2 1", "expect lighting 2", "leds", "wait 1500", "reboot",
                "expect lighting 2", "turn ALGO 5", "spin K2 1", "expect lighting 3",
                "tap PLAY", "wait 40", "leds", "tap PLAY", "wait 200",
                "spin K3 1", "expect led_slot 0", "leds"]
for slot in range(1, 42):
    led_commands += ["spin K4 1", f"expect led_slot {slot}", "leds"]
led_folder = panel("led-modes", led_commands)
leds = [line for line in (led_folder / "panel.log").read_text().splitlines() if "leds:" in line]
assert "white ################ black ###########" in leds[0]
assert leds[1].count("#") == 27 and leds[2].count("#") == 27
assert "white ................ black ..........." in leds[3]
assert "PLAY" in leds[4] and "FX SEL ENV" in leds[4]
assert all(line.count("#") == 1 for line in leds[5:32])
assert all("buttons |" not in line for line in leds[32:])

clip("startup", ["wait 100"], AQUORA_TEST_INTRO="1")
clip("sound", ["tap PLAY", "turn ALGO 1", "wait 300"])
clip("evolve", ["tap PLAY", "turn ALGO 2", "wait 1000"])
clip("original", ["tap PLAY", "wait 12300"])
clip("resonance-plate", ["turn ALGO 5", "spin K1 1", "tap HOME", "tap PLAY", "wait 12300"])
print("PASS six battery states, fade pixels, 49 individual startup inputs, and five panel animations")
