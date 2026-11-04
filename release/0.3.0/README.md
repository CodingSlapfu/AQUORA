# AQUORA 0.3.0

The updater family remains FM-1_73 and the image reports FM-1_73000300. Keep 0.2.0 and 0.1.0 available for rollback.

- Musical Sacred Bass follows gated Bass voices instead of holding the last pitch. Key voices reject duplicate down events and have an eight-second missed-release guard. Held keys must be released before they can trigger after STOP/PLAY. The 256-frame STOP fade and stale-effect reset remain in place. GLO Voice Inspector exposes source, age, gate, remaining time, envelope, counters and tone frequencies. SEQ there stops and clears transport.
- ARP opens Percussion and enables an audible route. K4 selects ON or OFF. OCT+ opens Percussion or HIT. HIT adds eligible optional strikes with a hard 70% cap. The screen shows actual drum state, density, level, kit, hit setting and pulse/meter.
- All pages share a live control mapping. ALGO, SELECT, PRESET and K1–K4 readouts show the current values. Knob feedback refreshes separately from animation.
- OCT− opens four arrangement banks. Select with K1, choose LOAD/SAVE with K2 and press SAVE. Arm overwrite with K3 for a populated slot. Save and load while stopped. Bank records contain musical state with a version and CRC, using two alternating flash sectors per slot in the platform data region. Existing motion and Perfect Key storage remains separate.
- Original and Water Crystals are selectable in SOUND or in Banks with K4. Water Crystals uses a bounded reactive geometry job. The stock drawing routine was unavailable in the official Open application source, so this design uses original geometry. See research/stock-animation-0.3.0.md.

## Verification

See evidence/0.3.0/ for sanitizer, voice, DSP, stereo, knob, bank, interrupted flash, worker, native, emulator and installer logs. The two visual modes and simulated worker fault modes produced bit-identical audio in the scripted test. Host A/B measurements compare 0.2.0 and 0.3.0 at the same workload inputs. They do not establish the approximately 74% hardware processing result or underrun margin.

## FM-1 checklist

1. Start with master at zero. Install in Chrome or Edge with MIDI SysEx allowed. Verify reboot and version. Keep the previous installer and stock recovery accessible.
2. Repeat STOP/PLAY with a held key, full effects, Sacred Bass and tones. If an unexpected note returns, capture Voice Inspector and both Performance pages. Try SEQ in Voice Inspector. Intentionally continuous reference, fixed and binaural tones are separately identifiable.
3. Confirm drums in the mix and solo. Test ARP, K4, density zero and HIT 0% / 70%. Check the pulse count and audible balance in all scenes.
4. Save four different banks, power-cycle, load each, and check scene, patch, mixer, rhythm, tuning, effects and visual mode. Verify overwrite confirmation and that settings save outside Banks does not change arrangements.
5. Compare both visualizers on the same arrangement with core 1 and with REC forcing core 0. Record callback/audio/FX timing, xruns and worker status during a sustained stress run against 0.2.0.
6. Verify physical direct update and rollback/recovery separately. Software tests do not establish recovery after an interrupted physical flash.

## Recover matching archives

Concatenate source.part-* in lexical order to AQUORA-0.3.0-source.tar.gz and package.part-* to AQUORA-0.3.0-package.tar.gz. Verify both with SHA256SUMS.txt. The source includes the pinned platform, SDK packaging inputs and acceptance script. The package contains the flashable firmware, browser installer, emulator, manual and licenses.
