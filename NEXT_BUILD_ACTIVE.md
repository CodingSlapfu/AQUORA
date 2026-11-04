# AQUORA active follow-up

The 0.5.0 software evidence is in evidence/0.5.0/final/. The full development scope is archived in archive/0.5.0-requirements.md.

- [ ] Measure sustained physical LCD delivery for both visualizers, aiming for at least 20 FPS under representative 16-voice playback, and record XRUN deltas and callback/FX timing. Host results do not establish a speedup or resolve the reported DMA issue. Use the FM-1 results to identify the next bottleneck.
- [ ] Verify Dance brightness, the physical pending-key latch, mixer readability, drum balance, and the artistic quality and variety of intention palettes on FM-1.
- [ ] Verify direct 0.4.0 upgrade, power-cycle bank persistence and rollback/recovery on hardware.
- [ ] Capture Source Isolate and Voice Inspector if the intermittent tone is reproduced.


## P1 — Live visual SOUND editor inspired by DEMAKE (owner preference, 2026-11-04)

- [ ] **Large reactive waveform as sound is edited.** Owner specifically likes how DEMAKE shows the sound shape while changing knobs. Add original AQUORA `SOUND > PATCH / SHAPE` waveform/shape view with true-to-audio patch preview, big legible 240×240 graph and highlighted K1–K4 controls. Editing patch/timbre/envelope and compatible motion/FX should cause an immediate, meaningful visual difference; no decorative/fake waveform. See detailed source comparison and implementation contract in [research/DEMAKE_SOUND_EDITOR_REVIEW.md](research/DEMAKE_SOUND_EDITOR_REVIEW.md).
- [ ] **Highly optimized and audio-safe:** DEMAKE's sample visualization is drawn from 48 *precomputed peak bins*, not a continuously recomputed audio scope. AQUORA synthesizes its own 24 patches, so build bounded 128–192-point shapes from its actual oscillator/wave function only when a parameter changes; optionally show a separate, clearly labeled decimated selected-track live waveform if profiling permits. Use cached geometry, partial redraw and lightweight playhead/envelope markers, with audio callback free of UI/DMA work and no big per-pixel glow shader. Test alongside 16 voices, 20 delivered FPS visualizations, callback timing and XRUN deltas.
- [ ] **Make editing genuinely useful:** Expose only actual editable sound properties, show their effects immediately, add legible envelope/filter/LFO/glide previews where corresponding DSP exists, preserve manual sound locks, Reroll Sounds, 13 intention palette choices, 4-bank persistence, physical-key modes and frequency purity. Preserve existing musical presets by default. Do not copy DEMAKE's GPL-3.0-only implementation/assets; independently design an AQUORA-native graphics and DSP path. Acceptance requires on-device screenshots, real knob/audio A/B and budget measurements; leave final hardware verification to owner.

