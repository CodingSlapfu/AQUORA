# AQUORA 0.6.2 — FM-1 guide

Generative Sound Sanctuary · Sound • Water • Resonance

Turn the master down before installation. The original eight-second FM
introduction follows the master. Deliberate key, button, encoder or master
movement skips it; the skip input is consumed. Playback starts on PLAY.
PLAY stops new notes and fades to digital silence; press again to resume.
The eight roles are Lead, Bass, Pads, Harmony, Piano, Plucks, Choir and Drums.

## Navigation and editing

ALGORITHM selects PLAY / SOUND / EVOLVE / MIX / DRUMS / SETUP / AIR.
HOME returns to PLAY. SELECT chooses a role (or a channel on MIX).
PRESET changes and auditions the selected patch; on DRUMS it auditions a kit.
K1–K4 always show their current assignments and values.

**SOUND:** EDIT cycles directly through editable sections; there is no empty
category picker. SELECT remains role selection. Instrument K1 selects patch,
K2 brightness, K3 attack, K4 release. Motion applies to Lead/Bass; select one
of those roles. Patch shape zero means the original patch shape.

**Quick Find:** hold HOME and tap EDIT, or SETUP → EDIT to Quick Find.
K1 chooses a category or individual parameter; K2 opens its editable location
and highlights the physical knob. Categories are shortcuts; the following
parameter entries are the canonical directory. All routes share the same
parameter IDs and setters. MIX owns channel level/pan/send/mute/solo; SOUND
MIX SHORTCUT and PLAY level shortcuts edit those same values. Bus shortcuts
edit the same Nature/frequency/binaural controls, without independent state.

**EVOLVE:** EDIT cycles Loop, New Song, Groove, Development. New Song and
Reroll require STOP; turn the action knob to arm and SAVE to execute.
Reroll keeps motif identity and locked sounds. Freeze holds phrase evolution.
Development is Stable / Subtle / Flowing / Adventurous with selectable pace;
Next requests a boundary evolution. The event graph shows observed events,
not predicted notes. Returning motif memory runs every fourth evolution.

**DRUMS:** ARP or OCT+ opens drums. EDIT cycles Pattern, Sound/Mix,
Mute/Solo. Pattern K1 enables, K2 density 0–8, K3 HIT 0–100%, K4 level.
Density means placement; HIT means optional-hit probability; Level is volume;
Presence means mix emphasis. None is a brightness knob. PRESET or kit K1
on Sound/Mix changes and auditions the kit. All nineteen IDs are preserved.
Kick/hand transients and kit-dependent timbres are brighter in 0.6.2.

**MIX:** eight-track console; SELECT focuses a track or source bus.
FX opens the large channel strip; FX returns to the overview.
K1 level, K2 pan, K3 FX send, K4 mute. SEL toggles mute; LFO toggles solo.
EDIT cycles All, Master FX, Reset, Active, Bus Shortcuts. Reset needs its
arm and SAVE. Master FX includes reverb, delay, Auto Gain and Auto Level.
Meters are real engine peaks. The channel strip shows level/peak/clip/pan/send.
Meters are sampled peaks, not a calibrated loudness meter.

**SOUND / Nature:** choose a manual bed, level and audition. AUTO uses the
intention/style song profile. Selecting Off locks it off; AUTO explicitly
unlocks selection. Manual beds stay fixed across new songs and intention
changes. Nature Character provides style, activity and distance. Level is
independent of activity. Birds and Forest use bounded irregular procedural
phrases, foliage and stereo depth; no field-recording authenticity claim.
The four added beds are Florida Night, Twilight Meadow, Crystal Creek and
Mountain Rain. Six appended FM patches and four FM kits preserve old IDs.

**Frequency:** reference tone is a separate precise layer. Frequency Lock
places pitched roles at the chosen fixed reference, with optional textures.
A4 432/440 is a separate tuning choice (ENV toggles it). Binaural produces
independent left/right carriers at carrier ± half the difference, after shared
FX. Headphones are required for the intended binaural presentation. Beat
range is 0.5–40 Hz; carrier 80–1000 Hz. These are musical features without
medical efficacy claims.

**Keyboard:** PLAY EDIT switches Play Along / Key Change. Perfect Key
quantizes live notes to scale. Step and half-step queue notes to boundaries;
short released taps cancel; STOP clears pending notes. Key Change commits at
a loop boundary. Strength/feel and groove are separate from key quantization.

**AIR:** opt-in receive-only BLE-strength experiment, OFF by default.
First tab K1 radio, K2 send target, K3 far calibration, K4 Hold. EDIT opens
FX mapping: K1 Reverb Bloom / Echo Throw / Bloom + Echo, K2 depth, K3 manual
modulation, K4 target. EDIT returns to the sensor tab. FX choice/depth persist;
radio, temporary output and Hold reset safely at boot. Existing manual FX
amounts are preserved; disabling or signal loss returns modulation to zero.
The shared FX bus is global; target chooses the role whose send is modulated.
When packets are absent, use manual K3. A stored calibration does not mean
live signal. Raw/filtered strength, packet rate and output are shown separately.
This is one inferred RF dimension, not XY tracking. Physical reception and
hand response still require the owner's FM-1 test. Advanced gesture presets
are not included because real gesture reliability is not established.

**SETUP:** EDIT cycles Display/Lights, Banks, About, Quick Find. In the first
tab SELECT switches Lights and Appearance. Appearance K1 selects the panel
skin (Stock/Aurora/Neon Lagoon/Solar Bloom/Midnight Prism), K2 Small/Medium/
Large AQUORA Sans, K3 Black/White background, K4 Original/Polar Bloom.
Stopped preferences save automatically after 1.2 seconds; SAVE also stores
preferences while stopped. Display choices are independent of arrangement
banks. Large values use full cells; bank confirmations occupy an upper row.

Lights On lights the fitted outputs, Keys follows keys, Off disables decorative
lights, Dance follows actual musical events. LED Test steps matrix slots;
physical LED mapping/brightness still needs owner verification.

OCT− opens four arrangement banks. K1 bank, K2 Load/Save, K3 overwrite arm;
SAVE executes while stopped. Bank CRC guards corrupt records. Old bank IDs,
patch IDs and kit IDs remain stable. New global appearance/AIR settings migrate
separately from musical banks. Bank save/recall preserves motion, locks,
Nature style/activity/distance, play grid/strength/feel and evolution pace.

**Visuals:** idle PLAY enters fullscreen after ten seconds. Deliberate panel
interaction exits without applying that first edit; keys continue to play.
Battery fades after about two seconds; normal PLAY restores full controls
for both visualizers. Polar Bloom has thirteen smoothly changing artistic
intention geometries, frequency/tuning form changes, actual role meters and
onset envelopes. These are cymatics-inspired art, not scientific water models.
Only visible graphs capture samples or generate geometry. Hidden graphs do
not stop musical evolution, effects or AIR control.

**Performance:** GLO cycles diagnostics. Mean audio/FX, callback p95/p99/max,
DMA XRUN delta, voices, graphics time and blit rate are separate quantities.
REC forces graphics to single core until reboot. Host timing is a software
proxy and cannot establish delivered LCD FPS or FM-1 XRUN performance.

## Canonical parameter directory

Open a parameter by its displayed name in Quick Find. Tab indices below are
zero-based and identify the implementation; EDIT cycles the named sections.
The directory is generated from `device/parameters.def`, which is consumed by
the actual on-device routing. Repeated performance shortcuts use the same ID.

| ID | Parameter | Canonical page / tab | Knob |
|---|---|---|---|
| 1 | SCENE | PLAY / 0 | K1 |
| 18 | ENERGY % | PLAY / 0 | K2 |
| 22 | FIXED TONE % | PLAY / 0 | K3 |
| 5 | NATURE % | PLAY / 0 | K4 |
| 2 |  | SOUND / 0 | K1 |
| 75 | BRIGHTNESS | SOUND / 0 | K2 |
| 76 | ATTACK | SOUND / 0 | K3 |
| 77 | RELEASE | SOUND / 0 | K4 |
| 21 | NATURE BED | SOUND / 1 | K1 |
| 54 | AUDITION | SOUND / 1 | K3 |
| 78 | NATURE MODE | SOUND / 1 | K4 |
| 4 | FIXED Hz | SOUND / 2 | K1 |
| 48 | PITCH MODE | SOUND / 2 | K3 |
| 45 | HZ SOURCE | SOUND / 2 | K4 |
| 8 | CARRIER Hz | SOUND / 3 | K1 |
| 7 | BEAT Hz | SOUND / 3 | K2 |
| 23 | BINAURAL % | SOUND / 3 | K3 |
| 6 | BINAURAL | SOUND / 3 | K4 |
| 30 | SACRED LEVEL % | SOUND / 5 | K1 |
| 31 | SACRED SOURCE | SOUND / 5 | K2 |
| 36 | HARMONIC % | SOUND / 5 | K3 |
| 32 | GLIDE MODE | SOUND / 6 | K1 |
| 33 | GLIDE ms | SOUND / 6 | K2 |
| 34 | VIBRATO Hz | SOUND / 6 | K3 |
| 35 | VIBRATO cents | SOUND / 6 | K4 |
| 13 | TRACK % | MIX / 5 | K1 |
| 24 | PAN % | MIX / 5 | K2 |
| 29 | FX SEND % | MIX / 5 | K3 |
| 28 | SOLO | SOUND / 7 | K4 |
| 43 | ISOLATE | SOUND / 11 | K1 |
| 47 | KEYBOARD | SOUND / 12 | K1 |
| 63 | PLAY GRID | SOUND / 12 | K2 |
| 73 | KEY SNAP % | SOUND / 12 | K3 |
| 74 | KEY FEEL | SOUND / 12 | K4 |
| 55 | EXTRA TEXTURES | SOUND / 13 | K1 |
| 14 | A4 TUNING Hz | SOUND / 13 | K2 |
| 79 | NATURE STYLE | SOUND / 14 | K1 |
| 85 | BIRD ACTIVITY | SOUND / 14 | K2 |
| 86 | BIRD DISTANCE | SOUND / 14 | K3 |
| 19 | VARIATION % | EVOLVE / 0 | K1 |
| 20 | FREEZE | EVOLVE / 0 | K2 |
| 25 | TEMPO BPM | EVOLVE / 0 | K3 |
| 46 |  | EVOLVE / 0 | K4 |
| 58 | NEW SONG | EVOLVE / 1 | K1 |
| 59 | REROLL SOUNDS | EVOLVE / 1 | K2 |
| 60 | TRACK LOCK | EVOLVE / 1 | K3 |
| 61 | NATURE / FX LOCK | EVOLVE / 1 | K4 |
| 64 | GROOVE | EVOLVE / 2 | K1 |
| 65 | STRENGTH % | EVOLVE / 2 | K2 |
| 82 | EVOLUTION | EVOLVE / 3 | K1 |
| 83 | PACE | EVOLVE / 3 | K3 |
| 84 | NEXT PHRASE | EVOLVE / 3 | K4 |
| 11 | REVERB % | MIX / 1 | K1 |
| 12 | DELAY % | MIX / 1 | K2 |
| 50 | AUTO GAIN | MIX / 1 | K3 |
| 51 | AUTO LEVEL | MIX / 1 | K4 |
| 52 | RESET MIX | MIX / 2 | K1 |
| 9 | DRUMS | DRUMS / 0 | K1 |
| 10 | DENSITY | DRUMS / 0 | K2 |
| 37 | HIT % | DRUMS / 0 | K3 |
| 17 | DRUM LEVEL % | DRUMS / 0 | K4 |
| 16 | DRUM KIT | DRUMS / 1 | K1 |
| 49 | MUTE | DRUMS / 2 | K1 |
| 62 | PRESENCE % | DRUMS / 2 | K3 |
| 38 | VISUALIZER | SETUP / 0 | K1 |
| 56 | LIGHTS | SETUP / 0 | K2 |
| 80 | LED TEST | SETUP / 0 | K3 |
| 81 | LED SLOT | SETUP / 0 | K4 |
| 90 | PANEL SKIN | SETUP / 6 | K1 |
| 91 | FONT SIZE | SETUP / 6 | K2 |
| 92 | BACKGROUND | SETUP / 6 | K3 |
| 39 | BANK | SETUP / 1 | K1 |
| 40 | ACTION | SETUP / 1 | K2 |
| 41 | OVERWRITE | SETUP / 1 | K3 |
| 66 | AIR RADIO | AIR / 0 | K1 |
| 67 | AIR TARGET | AIR / 0 | K2 |
| 68 | CALIBRATE | AIR / 0 | K3 |
| 69 | AIR HOLD | AIR / 0 | K4 |
| 57 | VISUAL FPS | SETUP / 4 | K1 |
| 42 | VOICE | SETUP / 5 | K1 |
| 93 | AIR FX | AIR / 1 | K1 |
| 94 | AIR DEPTH % | AIR / 1 | K2 |
| 95 | MANUAL AIR % | AIR / 1 | K3 |

## Owner hardware acceptance

Compare first playback immediately after updating with a complete power-off/
power-on at identical stock settings. Capture callback mean/p95/p99/max and
XRUN deltas with binaural OFF / ON / OFF, then dense drums/Nature/FX and each
visualizer. Compare the 0.6.1 cold-boot baseline (83% Audio+FX in the supplied
window), separately from its first-install 97% / 82-XRUN case. No measured
percentage improvement on physical hardware is claimed by software tests.

Check each fitted LED, battery state, hand-shadow calibration/dropout, all
nineteen drum kits on speaker/headphones, Nature realism, font legibility,
fullscreen return, bank recall, direct updater and one-hour maximum-feature
soak. Delivered 20 FPS, RF behavior, first-install state and audio quality
remain owner verification.
