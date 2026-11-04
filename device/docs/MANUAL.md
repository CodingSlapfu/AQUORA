# AQUORA 0.2.0 — FM-1

An original generative musical instrument for reflective listening. No medical benefit is claimed. Acoustic names describe synthesized instruments, not recordings. The owner confirmed installation and general sound on 0.1.0. The changes in 0.2.0 await FM-1 verification.

## Start
Turn the physical master down, install, then press PLAY and raise the master gradually. Playback starts only when requested. PLAY stops new events immediately and fades the entire mix over 256 samples (5.8 ms at 44.1 kHz). Digital silence follows until PLAY restarts. Keys play only while transport is running. Release and press a held key again after restarting.

## Navigation
ALGORITHM selects PLAY, SOUND, EVOLVE, MIX. SELECT chooses Lead, Bass, Pads, Harmony, Piano, Plucks, Choir, or Drums. PRESET chooses a scene on PLAY, and the selected track's patch or drum kit on other pages. These controls never change their navigation meaning. HOME returns to PLAY. SEL mutes the selected role; its name dims. EDIT cycles SOUND's eight groups. The selected track number and patch remain visible.

| Page or SOUND group | K1 | K2 | K3 | K4 |
|---|---|---|---|---|
| PLAY | Scene | Energy | Fixed tone level | Nature level |
| SOUND / Instrument | Patch or drum kit | Track level | Pan | FX send |
| SOUND / Nature | Bed | Bed level | Reverb | Delay |
| SOUND / Frequency | Fixed Hz | Tone level | A4 432/440 toggle | Energy |
| SOUND / Binaural | Carrier Hz | Difference (0.5 Hz steps) | Binaural level | Enable toggle |
| SOUND / Percussion | Kit | Drum level | Density 0–3 | Enable toggle |
| SOUND / Sacred Bass | Sacred level | Off / musical / reference | Harmonic blend | Fixed tone level |
| SOUND / Lead / Bass Motion | Off / legato / always glide | Glide milliseconds | Vibrato rate in 0.1 Hz steps | Vibrato depth in cents, 0 disables |
| SOUND / Track Routing | Track level | Pan | FX send | Solo / clear |
| EVOLVE | Motif variation | Freeze toggle | Tempo | Energy |
| MIX | Track level | Pan | Reverb | Delay |

ENV toggles A4 432/440. FX toggles binaural tones. ARP toggles percussion. SEQ freezes/unfreezes motif replacement. GLO cycles the two diagnostics screens and returns to the instrument. LFO toggles Perfect Key on either diagnostics screen and solos the selected track on the instrument. Perfect Key defaults on and maps all 27 keys to ascending notes in the current scene scale. Stopped SAVE stores Perfect Key and both tracks’ motion settings. Legato glide needs an overlapping note on the same track. Always glide uses the last pitch on that track. Glide and vibrato produce brief continuous excursions around or between scale notes. REC forces single-core graphics until reboot. Turn a toggle encoder once to toggle its setting.

## Frequency and stereo
Fixed tones: 174, 285, 396, 417, 528, 639, 741, 852, 963, 111 and 136.1 Hz. Tone level defaults to zero; raise it on Frequency or PLAY. Sacred Bass has its own gain, source and harmonic blend. Musical mode follows the Bass notes with subharmonic and harmonic support. Reference mode uses an octave-reduced reference frequency independent of the fixed tone level. A4 tuning changes musical notes separately.

Binaural mode defaults OFF. Left = carrier minus half the difference; right = carrier plus half the difference. Example: carrier 220 Hz, difference 6 Hz gives left 217 Hz and right 223 Hz. Use stereo headphones; speakers and mono are not equivalent. Tones bypass shared effects to retain channel separation, then pass through the output limiter/master. Changes glide and enabling fades in. Start at low volume.

## Instruments and arrangement
24 original algorithmic patches cover bass, drones, piano, plucked strings/guitar, flutes, woodwinds, bells, choir, strings and atmospheric pads. Eight scenes vary scale, tempo, density and nature selection. Anti-Curse uses a grounded minor palette with Sacred Earth percussion. Positive Healing uses a major palette with Moonlit Felt percussion. Each changes bass progression, atmosphere and FX. These are artistic listening intentions. Motifs vary at phrase boundaries; Freeze preserves the motif while its arrangement continues. Sixteen bounded voices share seven pitched tracks. Fifteen synthesized percussion kits share the integrated eighth track. Seven procedural nature types avoid streaming/decoding loads.

## Diagnostics and engineering
The first diagnostics screen separates Audio Processing (callback time excluding measured FX) from FX Processing (timed reverb/delay passes). It shows two-second mean, p95 and peak, callback budget use, DMA xruns, CPU percentage, and core status. The second shows total callback mean, p95/p99 and peak, visual time, active voices, worker fault reason and queue drops. Percentiles use 100-microsecond bins. These are real measurements only on the device. Host numbers are not FM-1 benchmarks. Graphics reduce refresh when reported audio load exceeds 60%. Core 1 optionally computes geometry from an isolated snapshot. A 2 ms worker timeout, invalid job or fault retires the worker and recomputes on core 0. No audio callback waits for it. REC forces single-core mode until reboot. No hardware speedup is claimed. Startup master is zero until ADC input arrives. The per-sample synthesis, FX and output passes run from SRAM; no dynamic allocation or decoding occurs in the renderer.

## Update and recovery
Use Chrome or Edge with Web MIDI and SysEx permission, connect FM-1 by USB, open the installer and select its ports. The installer checks package integrity and follows the firmware/loader/restarted device identity. Keep power connected. The bundled updater path preserves the boot area; interrupted application writes still require recovery and are not dual-bank rollback.

If normal startup fails, hold OCT- while powering on to request the platform rescue path. OCT- and OCT+ held together for five seconds enter update mode on the running platform. These procedures come from the selected platform and remain to be confirmed on your FM-1. Preserve a stock backup and the existing independent recovery archives. Do not install another bootloader merely for this release.

## Current limitations
Perfect Key and Lead/Bass motion are saved explicitly with SAVE while stopped. Other settings are session-local. No sampler, Bluetooth, mirror, looper or retired-product functionality is included. One nature bed is selected at a time. Physical clock calibration, sound quality, stereo separation, sustained CPU budget, display FPS, controls, direct version-to-version updating and interrupted-update recovery still require hardware testing.
