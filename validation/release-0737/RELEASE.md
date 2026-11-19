# AQUORA v0.7.37 TESTING

Firmware identity `FM-1_73000737`. This release builds on the verified v0.7.36 source. The original Puya 0x856014 FM-1 is the installer target.

## Music modes

On HOME or in the fullscreen visualizer, hold EDIT and turn ALGO to cycle Standard, Cinematic, Neoclassical, Experimental Ambient, and IDM. Plain ALGO still selects the track on HOME. Tapping EDIT still changes the keyboard mode. The new mode appears immediately and commits at the next bar if playing. PRESETS selects intentions independently on every page. New Song and Reroll retain the chosen mode.

The last selected mode is saved in existing global preferences. Loading a bank restores the arrangement's stored mode and uses it as the current preference. Pre-mode banks load as Standard. Banks saved during a pending mode change retain the currently realized mode and arrangement. No save schema changed.

## Sound and KIRA-9

- Intention motifs retain their original contours as one option. New seeds can choose from additional contours, with the existing musical timing, cadence and role planner. The automated A→B→A check exercised eight adjacent revisits for every mode and intention.
- Sax, reed, flute and choir patches use more distinct FM ratios and envelopes within the existing oscillator and patch IDs. Other patch families retain their existing profiles. The isolated PCM audit includes all 63 patches at MIDI 48, 66 and 84, with peak, RMS, attack, tail, crossings and clipping counts. This is an engineering comparison, with perceived differentiation and full-band balance pending physical audition.
- TUNE % defaults to 100. The sampler interpolates offline voiced-pitch measurements for 15 added recorded words and applies limited playback-rate centering at the selected strength. The 0/50/100 checks measure actual PCM cursor advance. Sustained vowels still use the existing MIDI note-target mechanism, glide and vibrato. This bounded correction changes playback rate and does not provide formant-preserving studio retuning or microphone processing.
- Repeated finite-word starts are suppressed while the same automatic word is still active. Phase-continuous vowel ornaments are retained. Morena’s real spoken YES was added to WORDS and AUTO under CC BY 4.0. See vocal credits for attribution and sample provenance.
- The portrait eases into a small lift during sustained expression. Lips and the seven cybernetic lights are still driven by the voice. Panel movement and music are ready for owner review. Artist-by-artist video and listening comparisons are pending.

## Verification

The native application uses 576,400 / 581,564 flash bytes, 81,772 / 98,304 static RAM bytes, and 262,472 / 344,064 DSP pool bytes. The matching WASM passed 512,000 audio frames and a changing 240×240 framebuffer. The panel regression covers mode gestures, bank recall and migration, reboot, A→B→A, New Song, Reroll, next-bar application and fullscreen. Recorded-word tests cover all 15 additions at three pitches, through their coded finishes. Check the artifact hashes with `sha256sum -c SHA256SUMS`.

On hardware, back up the FM-1, verify the build ID, and load an existing bank. Try each mode, turn PRESETS between intentions during playback, and audition winds, choir, KIRA’s quick runs and YES. Check response and listen for clicks or drops. Stable v0.7.14 and prior TESTING v0.7.36 provide rollback.
