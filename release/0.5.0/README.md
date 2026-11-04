# AQUORA 0.5.0

M-VAVE FM-1 updater identity `FM-1_73000500`.

- ARP opens dedicated DRUMS with pattern, sound/mix and mute/solo tabs. Density runs from 0 to 8 and HIT remains independent.
- SOUND has a category picker. SETUP groups display, lights and banks. PLAY EDIT switches keyboard mode; GLO opens Performance.
- EVOLVE EDIT offers stopped New Song and Reroll Sounds. Thirteen intention palettes vary compatible patches, drum kits, Nature, FX and musical material. Sound locks are stored with the arrangement.
- MIX shows four larger strips at a time, with SELECT moving between both banks. Water Cymatics uses a frequency-reactive perspective surface. Original remains selectable.
- Lighting modes are On, Keys, Off and Dance. Change Key latches the physical key indicator until the next loop commits the tonic.

## Verification

Software evidence is under evidence/0.5.0/final/. The FX comparison used seeded pitched renders with drums and Nature disabled and found matching PCM signatures against 0.4.0 in all four FX combinations. Host timings did not establish a speedup or reproduce the FM-1 DMA issue. The two reference-SDK warning inputs match the released 0.4.0 inputs byte-for-byte.

## Physical FM-1 checklist

- Compare delivered FPS, XRUN deltas and callback/FX timing with 16 voices and both visualizers. The Performance blit counter is a software measurement. The physical 20 FPS and zero-XRUN targets remain unverified.
- Check brightness, Dance response, and pending-key indication. Verify the LED clears at the actual loop transition.
- Check audible drum balance, palette variety and the LCD layout. If the intermittent tone returns, record Source Isolate and Voice Inspector.
- Verify direct upgrade from 0.4.0, rollback and four-bank persistence after a physical power cycle.

## Recovery

Concatenate the numbered archive parts by kind and verify SHA256SUMS.txt. Keep 0.1.0 through 0.4.0 for rollback.
