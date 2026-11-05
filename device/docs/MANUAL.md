# AQUORA 0.6.0 — FM-1

An original generative musical instrument for reflective listening. Patch names describe synthesized instruments.

## Start
Turn the physical master down, install, then press PLAY and raise the master gradually. Playback starts only when requested. PLAY stops new events immediately and fades the entire mix over 256 samples (5.8 ms at 44.1 kHz). Digital silence follows until PLAY restarts. Keys play only while transport is running. Release and press a held key again after restarting.

## Navigation
Turn ALGORITHM to choose a page. HOME returns to PLAY. ARP jumps to DRUMS, OCT− opens arrangement banks, and GLO opens Performance. Use SELECT to focus a track or category. On SOUND, EDIT opens the category picker. Turn SELECT to choose, then press EDIT to open it. Knob assignments and values appear at the bottom of the screen.

| Page or SOUND group | K1 | K2 | K3 | K4 |
|---|---|---|---|---|
| PLAY | Scene | Energy | Fixed tone level | Nature level |
| SOUND / Instrument (pitched) | Patch | Brightness | Attack | Release |
| SOUND / Track Routing | Track level | Pan | FX send | Solo / clear |
| SOUND / Nature | Bed | Bed level | Reverb | Delay |
| SOUND / Frequency | Fixed Hz | Tone level | Reference Layer / Frequency Lock | Hz Source Auto/Manual |
| SOUND / Binaural | Carrier Hz | Difference (0.5 Hz steps) | Binaural level | Enable toggle |
| SOUND / Percussion | Kit | Drum level | Density 0–8 | Enable toggle |
| SOUND / Sacred Bass | Sacred level | Off / musical / reference | Harmonic blend | Fixed tone level |
| SOUND / Lead / Bass Motion | Off / legato / always glide | Glide milliseconds | Vibrato rate in 0.1 Hz steps | Vibrato depth in cents, 0 disables |
| SOUND / Track Routing | Track level | Pan | FX send | Solo / clear |
| SOUND / Percussion HIT | HIT % (0–100) | Drum level | Density 0–8 | Drums ON/OFF |
| SOUND / Visualizer | Original / Resonance | — | — | — |
| SOUND / Arrangement Banks | Bank 1–4 | LOAD / SAVE | Overwrite arm | Visualizer |
| EVOLVE | Motif variation | Freeze toggle | Tempo | Loop steps 16/32/64 |
| MIX / Channels | Track level | Pan | FX send | Mute |
| MIX / Master FX | Reverb | Delay | Auto Gain | Auto Level |

ENV toggles A4 432/440. FX toggles binaural tones. ARP opens Percussion and turns drums on. Enabling restores audible minimums if density, track level or drum level is too low, unmutes track 8, and clears a conflicting solo. K4 counterclockwise turns drums off. OCT+ switches between Percussion and Percussion HIT. SEQ freezes/unfreezes motif replacement. GLO cycles Performance, Callback/Core and Voice Inspector, then returns to the instrument. In Voice Inspector, K1 selects a voice and SEQ stops transport and clears it with the normal fade. LFO toggles Perfect Key on either diagnostics screen and solos the selected track on the instrument. Perfect Key defaults on and maps all 27 keys to ascending notes in the current scene scale. Stopped SAVE stores Perfect Key and both tracks’ motion settings. Legato glide needs an overlapping note on the same track. Always glide uses the last pitch on that track. Glide and vibrato produce brief continuous excursions around or between scale notes. REC forces single-core graphics until reboot. For ON/OFF encoders turn clockwise to enable and counterclockwise to disable. For Solo turn clockwise to solo the selected role and counterclockwise to clear.

## Frequency and stereo
Fixed tones: 174, 285, 396, 417, 528, 639, 741, 852, 963, 111 and 136.1 Hz. Tone level defaults to zero; raise it on Frequency or PLAY. Sacred Bass has its own gain, source and harmonic blend. Musical mode follows a gated Bass voice and releases with it. The voice inspector distinguishes that layer from the continuous reference and fixed-tone options. Reference mode uses an octave-reduced reference frequency independent of the fixed tone level. A4 tuning changes musical notes separately.

Binaural mode defaults OFF. Left = carrier minus half the difference; right = carrier plus half the difference. Example: carrier 220 Hz, difference 6 Hz gives left 217 Hz and right 223 Hz. Use stereo headphones; speakers and mono are not equivalent. Tones bypass shared effects to retain channel separation, then pass through the output limiter/master. Changes glide and enabling fades in. Start at low volume.

## Instruments and arrangement
24 original algorithmic patches cover bass, drones, piano, plucked strings/guitar, flutes, woodwinds, bells, choir, strings and atmospheric pads. Eight scenes vary scale, tempo, density and nature selection. Anti-Curse uses a grounded minor palette with Sacred Earth percussion. Positive Healing uses a major palette with Moonlit Felt percussion. Each changes bass progression, atmosphere and FX. These are artistic listening intentions. Motifs vary at phrase boundaries; Freeze preserves the motif while its arrangement continues. Sixteen bounded voices share seven pitched tracks. Fifteen synthesized percussion kits share the integrated eighth track. Seven procedural nature types avoid streaming/decoding loads.

## Diagnostics and engineering
The first diagnostics screen separates Audio Processing (callback time excluding measured FX) from FX Processing (timed reverb/delay passes). It shows two-second mean, p95 and peak, callback budget use, DMA xruns, CPU percentage, and core status. The second shows total callback mean, p95/p99 and peak, visual time, active voices, worker fault reason and queue drops. Percentiles use 100-microsecond bins. These are real measurements only on the device. Host numbers are not FM-1 benchmarks. Graphics reduce refresh when reported audio load exceeds 60%. Core 1 optionally computes geometry from an isolated snapshot. A 2 ms worker timeout, invalid job or fault retires the worker and recomputes on core 0. At elevated load animation slows before control feedback. Original keeps the existing geometry. Water Cymatics uses six branches and two evolving rings tied to frequency, scene and musical activity. No audio callback waits for it. REC forces single-core mode until reboot. No hardware speedup is claimed. Startup master is zero until ADC input arrives. The per-sample synthesis, FX and output passes run from SRAM; no dynamic allocation or decoding occurs in the renderer.

## Arrangement banks

Stop playback, press OCT− to open Banks, then choose a slot with K1. K2 selects LOAD or SAVE. Press SAVE to perform the shown action. For a slot marked SAVED or INVALID, arm overwrite with K3 before pressing SAVE. Arming clears after an action, a slot change or an action change. K4 chooses the current visualizer. The status line reports empty, saved, loaded or failed.

Banks store instrument assignments and mixer state, scene, motif and generation state, rhythm and HIT, tempo, effects, nature, reference and binaural settings, motion, Perfect Key and visualizer. Loading restores these settings while stopped. Start with PLAY for a fresh downbeat. Live voice, effect history and playback cursor are excluded. Outside the bank page, stopped SAVE still saves motion and Perfect Key.

## Update and recovery
Use Chrome or Edge with Web MIDI and SysEx permission, connect FM-1 by USB, open the installer and select its ports. The installer checks package integrity and follows the firmware/loader/restarted device identity. Keep power connected. The bundled updater path preserves the boot area; interrupted application writes still require recovery and are not dual-bank rollback.

If normal startup fails, hold OCT- while powering on to request the platform rescue path. OCT- and OCT+ held together for five seconds enter update mode on the running platform. These procedures come from the selected platform and remain to be confirmed on your FM-1. Preserve a stock backup and the existing independent recovery archives. Do not install another bootloader merely for this release.

## Current limitations
Four arrangement banks and the separate motion/Perfect Key settings are persistent. Unsaved changes reset on reboot. Keyed notes have an eight-second missed-release guard. A total voice age guard keeps malformed or unexpectedly long voices bounded. No sampler, Bluetooth, mirror, looper or retired-product functionality is included. One nature bed is selected at a time. Physical clock calibration, sound quality, stereo separation, sustained CPU budget, display FPS, controls, direct version-to-version updating and interrupted-update recovery still require hardware testing.

### Sound Source Isolate

In SOUND, use EDIT to reach **Source Isolate**. K1 selects the full mix, a dry
track, Fixed tone, Sacred Bass, Binaural, Nature or Wet FX. Other audio sources
continue running internally, so Wet FX can be heard with its normal inputs.
Existing mute and solo settings still apply. HOME returns to the full mix.
K2 edits the reference Hz and switches to Manual. K3 adjusts the fixed tone
level. K4 selects the Sacred Bass source. Choose Auto on the Frequency page to
follow the intention again. Reference tone is off until its level is raised.

### Loop length and keyboard

EVOLVE K4 selects 16, 32 or 64 steps. During playback the new length applies
at the start of the next full loop. Longer loops keep the same pulse and
derive distinct response phrases from the stored motif.
Press EDIT on PLAY to switch between Play Along and Change Key. For timing controls, open SOUND > Keyboard or choose Play Along in SETUP > Quick Find. K1 changes mode, K2 selects Live, 1/8, 1/16, 1/32 or Auto, K3 sets timing strength, and K4 selects Straight or Intention feel. Auto uses 1/32 at slower tempos and 1/16 otherwise. At 100%, a note waits for the next grid position. Lower strength shortens the wait. Live and 0% play immediately. Intention feel shifts offbeats for pitched instruments other than Bass. SELECT chooses the Play Along instrument. In Change Key, the keys map chromatically from F
and select the new tonic without sounding a note. The last choice applies
on the next loop downbeat. If transport stops first, the choice applies on
the next PLAY. The active key and pending choice are shown on the screen.

Frequency Lock uses the exact selected Hz for every pitched track, including
Bass. Track timing, envelopes, level and pan still shape the unison. Drums,
Nature, binaural, Sacred Bass, fixed reference and FX are suppressed while
locked. Reference Layer restores the ordinary arrangement. ENV still toggles
A4 tuning for Reference Layer.

On MIX, use EDIT to cycle the views. On Reset, turn K1 to arm and press SAVE to restore default mix settings. Auto Gain gently trims measured track levels. Auto Level applies role-specific balance and a lower peak ceiling. You can run both together.

Resonance displays reactive ribbons on PLAY. Select Original or Resonance under SETUP. After 10 seconds without navigation input, PLAY enters fullscreen. Musical keys continue to play in fullscreen. Turn a navigation control to return to PLAY.

The Lock Textures group can explicitly add drums and Nature to Frequency Lock.
This introduces other frequencies. Leave Extra Textures off for strict output.
On Nature, turn K3 right to audition for four seconds. Turn it left or press
HOME to exit. An intention change selects that intention's default Nature bed.

### New songs and sound palettes

From EVOLVE, press EDIT to open the song controls. Stop transport before
turning K1 right for **New Song**, or K2 right for **Reroll Sounds**. Reroll
keeps the motif and musical RNG state. SELECT chooses the track for K3's
sound lock, including the percussion kit on track 8. K4 locks Nature and FX
together. Turn a lock control left to unlock. Editing a Nature bed also locks
that selection so a deliberately disabled bed stays off. Sound locks travel
with the arrangement bank. PLAY and STOP preserve the current motif and
sounds. A stopped intention change draws a fresh palette for that intention.

### Navigation

ALGORITHM selects PLAY, SOUND, EVOLVE, MIX, DRUMS, and SETUP. HOME returns
to PLAY. ARP jumps to DRUMS without changing the drum enable setting. EDIT
cycles Pattern, Sound / Mix, and Mute / Solo. OCT+ is a DRUMS shortcut.
On SOUND, EDIT opens the category picker, SELECT chooses the category, and
EDIT opens it. SETUP groups lighting and visualizer controls with arrangement
banks on its second tab. OCT− jumps to banks. GLO opens Performance, with
K1 selecting the visual pacing policy. The displayed PLAY blit rate is a
software count and still needs confirmation on the physical LCD.

### Groove, MIX and experimental AIR

EVOLVE EDIT cycles Loop, New Song, Groove and Development. On Groove, K1 chooses Intention,
Straight or Swing. K2 scales the timing offset from 0 to 100 percent. K3 is
loop length and K4 is Freeze. The timing profile applies to evolving piano, pluck, lead and harmony offbeats. Lead and harmony use smaller offsets with slow patches or at slow tempos. Frequency Lock uses straight timing. Bass, drums and downbeats stay on the transport clock.

MIX opens with musical channels and source buses together. Turn SELECT through the tracks and buses to change the focused controls. EDIT cycles All Channels, Master FX, Reset Mix, Active Channels and the taller Source Buses view. Use All Channels to recover muted or disabled sources. SEL and LFO mute and solo musical tracks. Bus controls retain their own routing. For example, Binaural keeps its stereo separation.

AIR is an opt-in experiment. See Corrective firmware controls for calibration and diagnostics.

SOUND / Instrument shows a cached oscillator and attack/release preview. Brightness affects held notes. Attack and release apply to newly triggered notes. Set each control to zero to use the patch default. The four banks save these edits per track.

SETUP EDIT also opens Quick Find. K1 chooses a destination and K2 opens it.
Performance separates geometry time from UI draw time and reports average
Audio and FX times, callback percentiles, DMA XRUNs and active voices.

Each intention has Lead and Bass glide/vibrato defaults. Lock the relevant
track under EVOLVE / New Song to retain its manually selected motion when
changing intention. Saved arrangements recall their exact motion values.

MIX EDIT also offers Source Buses. SELECT chooses Nature, Sacred Bass,
Reference, Binaural, or FX, and the encoders adjust that bus's existing
controls. Meters show sampled post-routing signal activity. Binaural keeps
its stereo carrier routing and does not expose a pan control.

### Corrective firmware controls

The introduction starts after initialization. A key, button, panel encoder or
master-volume movement dismisses it. The first skip input is consumed. The
startup chime follows the master level.

Hold HOME and tap EDIT to open Quick Find. K1 selects a destination and K2
opens it. The directory continues from category shortcuts to individual
parameters and highlights the knob that edits the selected setting. On SOUND, EDIT toggles the category list. SELECT chooses a category
while the list is open and a track while editing.

| Destination | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| SOUND / Instrument | Patch | Brightness | Attack | Release |
| SOUND / Nature | Bed | Level | Audition | Auto / Off |
| SOUND / Nature Character | Style | Bird activity | Bird distance | Audition |
| EVOLVE / Development | Evolution | Variation | Pace | Next phrase |
| SETUP / Lights and Display | Visualizer | Lighting | LED test | LED slot |
| AIR | Radio | Target | Calibrate | Hold |

Editing a patch or kit locks that choice and auditions it while stopped.
To generate a New Song or Reroll Sounds, turn its knob to arm the action,
then press SAVE while stopped. Release it in EVOLVE / New Song by
selecting the role and turning Track Lock left. A manually selected Nature
Bed remains selected until you return Nature to Auto. On Nature K4, turn
right for Auto or left for Off. Turn Audition right to isolate
the bed for four seconds. Turn left or press HOME to end the audition.

The SOUND Instrument graph shows the measured selected role over its
patch model during play. The attack and release panel is a model of the
selected patch's envelope. EVOLVE plots observed role events by step. A
blank lane is an interval with no captured events.

Select Original or Polar Bloom under SETUP / Lights and Display. After ten
seconds of PLAY inactivity the selected scene uses the full LCD. Piano keys
continue to play. A deliberate button or encoder input exits fullscreen.

On illuminates supported keys and controls at low brightness. Keys shows
held keys and the pending key change. Off extinguishes decorative lighting.
Dance uses note and percussion events to trigger brief pulses. PLAY uses
its green LED when running. LED Test steps the 27 piano keys, 14 button
outputs and PLAY green. On FM-1 the diagnostic prints the active matrix
column and packed row to the platform log. Compare with the pinned board
map and confirm each light independently. Leave test mode before playback.

AIR listens for legacy Bluetooth LE advertisements. Calibrate with your
hand away while packets are arriving. The raw signal and packet count show
whether there is a usable source. A drop below the calibrated reference
increases FX wet modulation. Stale signal returns the modulation toward zero.
AIR and physical FM-1 response are hardware-unverified.

The battery glyph reports the FM-1's coarse reading. An empty red outline
means low battery. The lightning glyph means power from a USB host. ABOUT
spells out the current reading. Fullscreen fades the glyph out by two
seconds. A new low battery reading can bring it back briefly.

### Quick Find parameter directory

The same control can appear on more than one page. This table gives its
direct Quick Find destination. Device settings are saved by pressing SAVE
while stopped outside the arrangement and mix confirmation pages. Bank
fields are saved with the arrangement.

| ID | Parameter | Direct destination | Knob | Range or action | Saved scope |
| --- | --- | --- | --- | --- | --- |
| 1 | Scene | PLAY / Main | K1 | 13 intentions | Bank |
| 18 | Energy % | PLAY / Main | K2 | 0–100% | Bank |
| 22 | Fixed Tone % | PLAY / Main | K3 | 0–10% | Bank |
| 5 | Nature % | PLAY / Main | K4 | 0–100% | Bank |
| 2 | Patch | SOUND / Instrument | K1 | 30 melodic patches | Bank |
| 75 | Brightness | SOUND / Instrument | K2 | −7 to +7 | Bank |
| 76 | Attack | SOUND / Instrument | K3 | −7 to +7 | Bank |
| 77 | Release | SOUND / Instrument | K4 | −7 to +7 | Bank |
| 21 | Nature Bed | SOUND / Nature | K1 | Off or 11 beds | Bank |
| 54 | Audition | SOUND / Nature | K3 | 4-second audition | Session |
| 78 | Nature Mode | SOUND / Nature | K4 | Auto, manual, off | Bank |
| 4 | Fixed Hz | SOUND / Frequency | K1 | 13 reference tones | Bank |
| 48 | Pitch Mode | SOUND / Frequency | K3 | Reference or frequency lock | Bank |
| 45 | Hz Source | SOUND / Frequency | K4 | Auto or manual | Bank |
| 8 | Carrier Hz | SOUND / Binaural | K1 | 80–1000 Hz | Bank |
| 7 | Beat Hz | SOUND / Binaural | K2 | 0.5–40 Hz | Bank |
| 23 | Binaural % | SOUND / Binaural | K3 | 0–10% | Bank |
| 6 | Binaural | SOUND / Binaural | K4 | On/off | Bank |
| 30 | Sacred Level % | SOUND / Sacred Bass | K1 | 0–20% | Bank |
| 31 | Sacred Source | SOUND / Sacred Bass | K2 | Off, musical, reference | Bank |
| 36 | Harmonic % | SOUND / Sacred Bass | K3 | 0–100% | Bank |
| 32 | Glide Mode | SOUND / Motion | K1 | Off, legato, always | Settings and bank |
| 33 | Glide Ms | SOUND / Motion | K2 | 10–800 ms | Settings and bank |
| 34 | Vibrato Hz | SOUND / Motion | K3 | 0.5–8 Hz | Settings and bank |
| 35 | Vibrato Cents | SOUND / Motion | K4 | 0–50 cents | Settings and bank |
| 13 | Track % | SOUND / Track Routing | K1 | 0–100% | Bank |
| 24 | Pan % | SOUND / Track Routing | K2 | −100 to +100% | Bank |
| 29 | Fx Send % | SOUND / Track Routing | K3 | 0–100% | Bank |
| 28 | Solo | SOUND / Track Routing | K4 | Off or selected role | Bank |
| 43 | Isolate | SOUND / Source Isolate | K1 | Full mix, role or source | Session |
| 47 | Keyboard | SOUND / Keyboard | K1 | Play Along or Change Key | Bank |
| 63 | Play Grid | SOUND / Keyboard | K2 | Live, 1/16, 1/32, Auto, 1/8 | Bank |
| 73 | Key Snap % | SOUND / Keyboard | K3 | 0–100% | Bank |
| 74 | Key Feel | SOUND / Keyboard | K4 | Intention or straight | Bank |
| 55 | Extra Textures | SOUND / Lock Textures | K1 | On/off | Bank |
| 14 | A4 Tuning Hz | SOUND / Lock Textures | K2 | 432/440 Hz | Bank |
| 79 | Nature Style | SOUND / Nature Character | K1 | 4 styles | Bank |
| 85 | Bird Activity | SOUND / Nature Character | K2 | 0–4 | Bank |
| 86 | Bird Distance | SOUND / Nature Character | K3 | 0–4 | Bank |
| 19 | Variation % | EVOLVE / Loop | K1 | 0–100% | Bank |
| 20 | Freeze | EVOLVE / Loop | K2 | On/off | Bank |
| 25 | Tempo Bpm | EVOLVE / Loop | K3 | 30–120 BPM | Bank |
| 46 | Loop Steps | EVOLVE / Loop | K4 | 16/32/64 | Bank |
| 58 | New Song | EVOLVE / New Song | K1 | Arm then SAVE | Session |
| 59 | Reroll Sounds | EVOLVE / New Song | K2 | Arm then SAVE | Session |
| 60 | Track Lock | EVOLVE / New Song | K3 | On/off | Bank |
| 61 | Nature / Fx Lock | EVOLVE / New Song | K4 | On/off | Bank |
| 64 | Groove | EVOLVE / Groove | K1 | Intention, straight, swing | Bank |
| 65 | Strength % | EVOLVE / Groove | K2 | 0–100% | Bank |
| 82 | Evolution | EVOLVE / Development | K1 | 4 levels | Bank |
| 83 | Pace | EVOLVE / Development | K3 | Intention or 2/4/8 loops | Bank |
| 84 | Next Phrase | EVOLVE / Development | K4 | Queue next phrase | Session |
| 11 | Reverb % | MIX / Master FX | K1 | 0–80% | Bank |
| 12 | Delay % | MIX / Master FX | K2 | 0–70% | Bank |
| 50 | Auto Gain | MIX / Master FX | K3 | On/off | Bank |
| 51 | Auto Level | MIX / Master FX | K4 | On/off | Bank |
| 52 | Reset Mix | MIX / Reset Mix | K1 | Arm then SAVE | Session |
| 9 | Drums | DRUMS / Pattern | K1 | On/off | Bank |
| 10 | Density | DRUMS / Pattern | K2 | 0–8 | Bank |
| 37 | Hit % | DRUMS / Pattern | K3 | 0–100% | Bank |
| 17 | Drum Level % | DRUMS / Pattern | K4 | 0–100% | Bank |
| 16 | Drum Kit | DRUMS / Sound / Mix | K1 | 19 kits | Bank |
| 49 | Mute | DRUMS / Mute / Solo | K1 | On/off | Bank |
| 62 | Presence % | DRUMS / Mute / Solo | K3 | 0–100% | Bank |
| 38 | Visualizer | SETUP / Lights / Display | K1 | Original or Polar Bloom | Bank |
| 56 | Lights | SETUP / Lights / Display | K2 | On, Keys, Off, Dance | Settings |
| 80 | Led Test | SETUP / Lights / Display | K3 | On/off | Session |
| 81 | Led Slot | SETUP / Lights / Display | K4 | 42 outputs | Session |
| 39 | Bank | SETUP / Banks | K1 | 1–4 | Session |
| 40 | Action | SETUP / Banks | K2 | Load/save | Session |
| 41 | Overwrite | SETUP / Banks | K3 | Arm/disarm | Session |
| 66 | Air Radio | AIR / AIR | K1 | Off/passive | Session |
| 67 | Air Target | AIR / AIR | K2 | All or selected role | Session |
| 68 | Calibrate | AIR / AIR | K3 | Capture reference | Session |
| 69 | Air Hold | AIR / AIR | K4 | On/off | Session |
| 57 | Visual Fps | SETUP / Performance | K1 | Auto or 20 fps target | Settings |
| 42 | Voice | SETUP / Voice Inspector | K1 | 1–16 | Session |
