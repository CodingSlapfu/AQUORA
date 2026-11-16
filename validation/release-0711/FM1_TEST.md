# AQUORA v0.7.11 FM-1 testing candidate

Use the original Puya 0x856014 FM-1. The candidate product identity is FM-1_73000711. The package includes the verified v0.7.9 rollback. Keep the rollback available for the first flash.

## Physical checks

1. Boot at low volume and verify the intro, PLAY, and SETUP with transport stopped. Start playback and repeat SETUP, EVOLVE, and all three visualizers. Check for delayed output, clicks, and delayed knob feedback. Use the same track and volume in each comparison.
2. Turn PRESETS on PLAY, SETUP and fullscreen, while stopped and while playing. Check that labels on PLAY and fullscreen follow the knob and that a running song starts the new intention without waiting for a beat. Return to a previous intention and listen for a fresh arrangement. Repeat with a manual patch lock and a disabled Nature bed.
3. Compare a dense song in Cymatics before and after pressing REC once. REC stops Core 1's geometry worker until reboot. Geometry uses the same renderer on Core 0 during this comparison. The testing build runs FM synthesis on Core 0 in both cases.
4. In GLO diagnostics, capture the callback P95, P99 and peak, DMA XRUN delta, geometry completions and fallbacks, PRESETS ACK and PLAN BUILD. PRESETS ACK is from the detected detent to the first consuming callback. PLAN BUILD measures the scene update in that callback. Neither measures analog output or the physical encoder edge. If the display font is Small or Medium, the performance screen also shows draw time and LCD submission time. LCD submission includes the wait for previous transfers and the copy when used.
5. Listen to the same intention in two different modes, then leave your preferred mode running through a section change and a later return. Check whether the motif remains recognizable and the supporting part entries make musical sense. Verify existing bank recall on a bank you already have.

The hardware decision includes audio, controls, visual feel and mixed listening. This checklist has not been executed on a physical FM-1 in the build environment.

## Recorded vocal checks

Open SOUND and use EDIT to reach Recorded Vocals, or choose Recorded Vocals in Quick Find. Turn K1 clockwise to enable the sample voice and set K2 to Present or Featured for the first audition. A song typically waits until the opening has established its material. Start with Cinematic, then try Standard and Neoclassical. Experimental Ambient uses recorded vocals sparingly on returns. The controls are global preferences, with the arrangement determining when an actual phrase occurs. Manual choir-patch locks keep the selected patch in use.

For hardware acceptance, compare the same song with vocals Off and On through a development or reprise. Check both headphones and the speaker, Cymatics, and the GLO callback and XRUN metrics. Listen for clear entrances, natural transitions, audible harmonized replies, and a clean release when turning the feature Off. A rollback to the earlier candidate is not needed for the comparison.
