# AQUORA v0.7.10 FM-1 acceptance

This is a flashable release candidate for the original Puya-equipped M-VAVE FM-1. The v0.7.9 installer and rollback remain available. Keep a copy of your existing firmware and avoid overwriting occupied banks during the comparison.

## Install the candidate

1. Extract the candidate ZIP and verify the files with `sha256sum -c SHA256SUMS.txt` from the extracted directory. On Windows, `Get-FileHash -Algorithm SHA256 <path>` prints the hash for comparison.
2. From the extracted directory run `python -m http.server 8071 --bind 127.0.0.1`. Open `http://localhost:8071/installer/install/` in Chrome or Edge. Its embedded checksum must accept only the included dual-core firmware.
3. After the FM-1 restarts, confirm it reports AQUORA v0.7.10 and identity `FM-1_73000710`. Open `http://localhost:8071/installer/emu/` for the matching emulator. The ZIP includes a single-core build for comparison with the same identity. Use the normal M-UPGRADE process with that separate file if a permanently single-core comparison build is needed.
4. The official rollback is [AQUORA v0.7.9](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.7.9). Check that its native SHA-256 is `0b98bb4b7df0f18cd2079b301509b18f789dc13e4b7a3748b2bf26b0a7ec0d32`.

## Hardware gates

| Test | What to record |
|---|---|
| Dual-core FM | Start dense playback, then sustain several pitched notes. On GLO's performance screen in Large Text, check that **FM BLOCKS** increases. Record the **FALLBACK** count. A count that rises and then stops along with the block counter means audio retired the second core. |
| Deadline and display | Let a dense arrangement run for ten minutes with Cymatics. Repeat for Original and Chladni. Record GLO callback P95/P99/peak, XRUN delta, visible pauses, crackles and encoder response while switching intentions and adjusting parameters. The audio DMA half-buffer is about 5,805 µs. |
| Single-core comparison | Under the same playback conditions, press REC to disable the second core until reboot. Repeat the deadline and display test. Compare those results against the dual-core run and official v0.7.9. |
| Cymatics and LCD | Record matched quiet and dense clips with the same intention, mode, pitch and output level. Compare density, motion, lighting and responsiveness in normal and fullscreen mode. Check the five themes, three text sizes, and Black/White modes for readability on the physical panel. |
| Sound and speaker | Audition the 63 patches in low, middle and high registers against v0.7.9, with levels matched by ear or meter. Check the improved leads, plucks, STRUM, pads, choir and piano in an arrangement. Check speaker toggling, pitch stability and stereo binaural restoration on headphones. |
| Musical form and controls | Run long arrangements in each mode and change 16/32/64 during playback. Verify Fortune across four modes, drum preview/fills and individual mutes, filter/slicer bypass and recall, AIR, LEDs, supported MIDI, and EDIT navigation. |
| Flash and rollback | Verify all legacy banks load and their patch IDs and locks match. Save a test arrangement only in an unused bank. Flash the verified v0.7.9 rollback and verify the original banks again. |

Attach panel photos or short audio captures for any mismatch. Physical acceptance is pending.
