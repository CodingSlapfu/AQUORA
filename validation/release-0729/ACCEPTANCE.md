# AQUORA v0.7.29 testing acceptance

Original Puya 0x856014 FM-1. Firmware FM-1_73000729. Source
db5e454df0e30f9adfdc911ac39ec22c84c01381.

| Issue #90 | Status |
|---|---|
| A. Individual AUTO | Preserved. Host tested FX AUTO, manual zero, manual values and Favorites recall. |
| B. Singer patches | All ten preserved. Physical listening remains open. |
| C. Live phrasing and register | Partial. Vocal Flow shapes gate lengths, subdivisions, vibrato onset and depth, glide and space. Synthesized words centre on E4 with bounded octave placement. Humanlike delivery and formants require listening acceptance. |
| D. Vocabulary | Ten newly synthesized words plus the prior recorded DREAMS. Firmware PCM fixture checks distinct output at the normal register, release-tail output and non-clipping for every added word. See credits and lexical fixture. Subjective intelligibility is pending. |
| E. AUTO level | Preserved. Physical level balance remains open. |
| F. Effects and fixed send | Partial. All twenty automatic parameter ranges and manual locks passed. Vocal Flow adjusts automatic delay division and room, echo and decay for faster or more sustained delivery. Existing per-role sends and unity vocal send are preserved. Full per-stem and physical max-load effects evaluation remains open. |
| G–K. Lamps, VOCALS, startup, shutdown, Gate | v0.7.28 implementations preserved. Hardware appearance and timing need acceptance. |
| L. Layout and workflow | Partial. Added VOCAL FLOW to the existing VOCALS Play Along tab on K3. Existing V11, encoder behavior and Favorites are covered by host tests. Full cross-page typography review remains open. |
| M. Fullscreen performance | Partial. Host stress covered 1,200 encoder edges across all three visualizers with zero drops and direct vocals. Physical LCD DMA, input-to-photon latency and callback p95/p99 need owner testing. |
| Vocal Flow | AUTO, HALF, NORMAL, DOUBLE. Uses mode, intent, section, energy and tick length. In the controlled fixture, half/normal/double yield 2/3/6 onsets at the same tempo and opening MIDI pitch. Vocal Flow is stored in V11 and uses the same sample admission budget. |

New words: SHINE, FREE, FLY, STAY, AWAY, HOME, HOPE, ALIVE, TONIGHT, FOREVER.
The generator uses Flite CMU US SLT with tuned excitation and a loop in a
periodic vowel, followed by four-bit companded storage. Firmware grammar
selects Shine Tonight, Fly Away, Stay Home, Stay Alive, Forever Free or a
solo Hope cadence based on song character and mode. The arranger places
each word on its current chord or cadence anchor. Nonlexical singing and
manual articulation remain available. No new word IDs enter persisted hook
memory. DREAMS retains its credited CC0 human recording.

Native: 574,072 bytes of 581,564-byte app slot. Main data/BSS 81,756 of
98,304 bytes. Pool 325,960 of 344,064 bytes. The firmware was reproduced
byte for byte. WASM ran 512,000 frames and advanced the 240×240 framebuffer.
The updater fixture tested identity, install, resume and corruption detection.
This is a testing candidate with no stable promotion.

Physical checks: listen to the lexical refrains at all three flow speeds
and across modes. Check chord alignment, consonant clarity, register and
balance. Exercise repeated fullscreen controls and page changes with all
eight roles, vocals and high FX. Check LCD band completion, DMA XRUNs,
callback p95/p99 and vocal sample/budget denials. Verify Play Along, Favorite
recall and update/rollback on hardware.
