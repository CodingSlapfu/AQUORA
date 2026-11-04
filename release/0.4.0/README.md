# AQUORA 0.4.0

Generative Sound Sanctuary for the M-VAVE FM-1. Updater identity `FM-1_73000400`. The existing FM-1_73 family is preserved.

- Thirteen intentions with automatic or manual reference frequencies. Reference Layer preserves the musical arrangement. Frequency Lock renders the pitched tracks at the selected exact Hz and suppresses other sources by default.
- Eight-channel mixer with level, pan, FX send, mute, solo, independent Auto Gain and Auto Level, and master FX controls.
- 16-, 32- and 64-step loops with response melodies and changing percussion bars. Play Along enforces scale notes. Change Key queues a tonic change for the next loop.
- Four arrangement banks preserve mixer, frequency and loop settings. Legacy 0.3.0 records load with the stored frequency, mixer and motion settings.
- Water Cymatics and Original visuals, a 50 ms display update floor, audible Nature beds with timed audition, stronger Bass, and separate glide and vibrato controls.
- Source Isolate can monitor each of the eight tracks, Fixed Tone, Sacred Bass, Binaural, Nature and Wet FX independently. HOME restores the mix.

## Verification

See `evidence/0.4.0/final/` for native, forced single-core, browser, installer, sanitizer, PCM, bank migration, UI and worker tests. The legacy fixture was generated with the archived 0.3.0 firmware serializer. Auto reference frequencies were measured in rendered PCM. The fixed reference intentionally retains its frequency in Manual and resumes on PLAY when its level is enabled. The exact source of the intermittent tone heard on a physical FM-1 remains unconfirmed.

## Physical FM-1 checklist

1. Verify direct update from 0.3.0, reboot and reported version, then verify recovery and rollback separately.
2. Repeat STOP/PLAY and scene changes with Reference Layer at zero and audible levels, Sacred Bass in each mode, and binaural on and off. If an unwanted tone returns, note the intention and settings, then identify the audible source using Source Isolate and Voice Inspector.
3. Save four distinct arrangements, power-cycle and reload them. Check the new mixer, loop, key, visual and frequency fields.
4. Confirm audible balance, Nature beds, and Lead/Bass vibrato. The owner previously confirmed Glide operation after changing intentions.
5. Compare actual FM-1 processing load, audio underruns and display cadence with both visualizers, with the worker enabled and with core 0 fallback.

## Recovery

Keep 0.3.0, 0.2.0 and 0.1.0 for rollback. Concatenate each set of numbered archive parts in lexical order and validate SHA256SUMS.txt. The package contains the firmware, browser installer, emulator and manual. The source includes the pinned platform, SDK, tests and build scripts.
