# AQUORA 0.3.0 — FM-1

An original generative musical instrument for reflective listening. No medical benefit is claimed. Acoustic names describe synthesized instruments, not recordings. The owner confirmed installation and excellent general sound on 0.2.0 and reported roughly 74% processing. The changes in 0.3.0 await FM-1 verification.

## Start
Turn the physical master down, install, then press PLAY and raise the master gradually. Playback starts only when requested. PLAY stops new events immediately and fades the entire mix over 256 samples (5.8 ms at 44.1 kHz). Digital silence follows until PLAY restarts. Keys play only while transport is running. Release and press a held key again after restarting.

## Navigation
ALGORITHM selects PLAY, SOUND, EVOLVE, MIX. SELECT chooses Lead, Bass, Pads, Harmony, Piano, Plucks, Choir, or Drums. PRESET chooses a scene on PLAY, and the selected track's patch or drum kit on other pages. These controls never change their navigation meaning. HOME returns to PLAY. SEL mutes the selected role; its name dims. EDIT cycles SOUND’s eleven groups. The display shows all current knob values and highlights the latest adjustment. The selected track number and patch remain visible.

| Page or SOUND group | K1 | K2 | K3 | K4 |
|---|---|---|---|---|
| PLAY | Scene | Energy | Fixed tone level | Nature level |
| SOUND / Instrument | Patch or drum kit | Track level | Pan | FX send |
| SOUND / Nature | Bed | Bed level | Reverb | Delay |
| SOUND / Frequency | Fixed Hz | Tone level | A4 432/440 toggle |Hz Source Auto/Manual |
| SOUND / Binaural | Carrier Hz | Difference (0.5 Hz steps) | Binaural level | Enable toggle |
| SOUND / Percussion | Kit | Drum level | Density 0–3 | Enable toggle |
| SOUND / Sacred Bass | Sacred level | Off / musical / reference | Harmonic blend | Fixed tone level |
| SOUND / Lead / Bass Motion | Off / legato / always glide | Glide milliseconds | Vibrato rate in 0.1 Hz steps | Vibrato depth in cents, 0 disables |
| SOUND / Track Routing | Track level | Pan | FX send | Solo / clear |
| SOUND / Percussion HIT | HIT % (0–100) | Drum level | Density 0–3 | Drums ON/OFF |
| SOUND / Visualizer | Original / Water Crystals | — | — | — |
| SOUND / Arrangement Banks | Bank 1–4 | LOAD / SAVE | Overwrite arm | Visualizer |
| EVOLVE | Motif variation | Freeze toggle | Tempo | Energy |
| MIX | Track level | Pan | Reverb | Delay |

ENV toggles A4 432/440. FX toggles binaural tones. ARP opens Percussion and turns drums on. Enabling restores audible minimums if density, track level or drum level is too low, unmutes track 8, and clears a conflicting solo. K4 counterclockwise turns drums off. OCT+ switches between Percussion and Percussion HIT. SEQ freezes/unfreezes motif replacement. GLO cycles Performance, Callback/Core and Voice Inspector, then returns to the instrument. In Voice Inspector, K1 selects a voice and SEQ stops transport and clears it with the normal fade. LFO toggles Perfect Key on either diagnostics screen and solos the selected track on the instrument. Perfect Key defaults on and maps all 27 keys to ascending notes in the current scene scale. Stopped SAVE stores Perfect Key and both tracks’ motion settings. Legato glide needs an overlapping note on the same track. Always glide uses the last pitch on that track. Glide and vibrato produce brief continuous excursions around or between scale notes. REC forces single-core graphics until reboot. For ON/OFF encoders turn clockwise to enable and counterclockwise to disable. For Solo turn clockwise to solo the selected role and counterclockwise to clear.

## Frequency and stereo
Fixed tones: 174, 285, 396, 417, 528, 639, 741, 852, 963, 111 and 136.1 Hz. Tone level defaults to zero; raise it on Frequency or PLAY. Sacred Bass has its own gain, source and harmonic blend. Musical mode follows a gated Bass voice and releases with it. The voice inspector distinguishes that layer from the continuous reference and fixed-tone options. Reference mode uses an octave-reduced reference frequency independent of the fixed tone level. A4 tuning changes musical notes separately.

Binaural mode defaults OFF. Left = carrier minus half the difference; right = carrier plus half the difference. Example: carrier 220 Hz, difference 6 Hz gives left 217 Hz and right 223 Hz. Use stereo headphones; speakers and mono are not equivalent. Tones bypass shared effects to retain channel separation, then pass through the output limiter/master. Changes glide and enabling fades in. Start at low volume.

## Instruments and arrangement
24 original algorithmic patches cover bass, drones, piano, plucked strings/guitar, flutes, woodwinds, bells, choir, strings and atmospheric pads. Eight scenes vary scale, tempo, density and nature selection. Anti-Curse uses a grounded minor palette with Sacred Earth percussion. Positive Healing uses a major palette with Moonlit Felt percussion. Each changes bass progression, atmosphere and FX. These are artistic listening intentions. Motifs vary at phrase boundaries; Freeze preserves the motif while its arrangement continues. Sixteen bounded voices share seven pitched tracks. Fifteen synthesized percussion kits share the integrated eighth track. Seven procedural nature types avoid streaming/decoding loads.

## Diagnostics and engineering
The first diagnostics screen separates Audio Processing (callback time excluding measured FX) from FX Processing (timed reverb/delay passes). It shows two-second mean, p95 and peak, callback budget use, DMA xruns, CPU percentage, and core status. The second shows total callback mean, p95/p99 and peak, visual time, active voices, worker fault reason and queue drops. Percentiles use 100-microsecond bins. These are real measurements only on the device. Host numbers are not FM-1 benchmarks. Graphics reduce refresh when reported audio load exceeds 60%. Core 1 optionally computes geometry from an isolated snapshot. A 2 ms worker timeout, invalid job or fault retires the worker and recomputes on core 0. At elevated load animation slows before control feedback. Original keeps the existing geometry. Water Crystals uses six branches and two evolving rings tied to frequency, scene and musical activity. No audio callback waits for it. REC forces single-core mode until reboot. No hardware speedup is claimed. Startup master is zero until ADC input arrives. The per-sample synthesis, FX and output passes run from SRAM; no dynamic allocation or decoding occurs in the renderer.

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
