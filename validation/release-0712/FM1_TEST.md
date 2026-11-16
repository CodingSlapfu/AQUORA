# AQUORA v0.7.12 corrective build

Source comparison: SLOOP 3 v3.3.7 at `915b54f6780564cb486b4de8a518aabbea1503a0`. Its polled DMA service renders short blocks ahead of display work, bounds overload with voice admission, samples the panel independently, and submits dirty LCD tiles asynchronously. AQUORA retains its higher-priority audio IRQ and existing 64-sample DSP subdivisions within each 256-sample DMA half. Both projects already restore equivalent FM-1 supply levels at boot. The corrective work independently implements nonblocking LCD transfer gating, stripe signatures, a 1 kHz foreground panel snapshot, reduced repetitive IRQ-masked LED bookkeeping, and callback-timed voice admission. The 512 Cymatics particles and equations remain intact.

DRUMS → 16-PART MIX displays the selected group of 16-step lanes. K1 selects any of the sixteen drum parts. K2 sets mute, K3 sets solo or clears solo, and K4 controls that part's level. The three original part settings import into v9 bank records. Original percussion routes remain available. The added sounds were generated for AQUORA, then stored as 11.025 kHz PCM. The firmware mixes active one-shots at that rate into a single interpolated bus. Bass anchors still use the primary kick score.

## Owner hardware checks

1. Cold boot the original Puya FM-1. Compare playback stopped and running in SETUP, then use PRESETS and switch pages while listening for crackles.
2. Check Original, Cymatics and Chladni, then try dense drums with Recorded Vocals both Off and On.
3. Compare GLO callback P95/P99/peak, DMA XRUN and PRESETS ACK metrics on each test. Record the page, mode and whether the audio actually crackled. Confirm every drum lane appears and responds to its own mute, solo and level controls.
4. v0.7.11 and v0.7.9 remain available for rollback. Use another bank while testing saving/reloading v0.7.12 drum settings.

Software validation measures algorithm behavior and linker budgets. It does not establish physical IRQ slack, audio quality during knob turns, actual LCD cadence, or safe bank rollback after re-saving into a newer schema.
