# DEMAKE sound editing — techniques for AQUORA (reviewed 2026-11-04)

Source: https://github.com/InigoHernandez/demake-synth (DEMAKE 0.4.24, GPL-3.0-only)
Live demo: https://inigohernandez.github.io/demake-synth/
Scope: source-led inspiration for AQUORA's own original implementation. Do not copy GPL-3.0-only code, fonts, artwork or the author's pedal recordings into MPL-2.0 AQUORA without a deliberate license review.

## Verified findings

1. **SOUND sample preview** (`firmware/src/dmk_ui.c::stage_sound`, ~1824–1871): the large waveform-like silhouette is derived from `sm->env[48]`, NOT a real-time audio oscilloscope. `tools/gen_samples.py` ~286–287 computes **48 peak-amplitude bins** from each source recording *at build time*. UI expands those bins into ~200 horizontal columns, overlays markers for start/loop boundaries and active playback positions; the large shape is updated as applicable sound/loop controls change. It is an inexpensive informative display because the analysis was precomputed.

2. **Every major sound parameter gets a geometric explanation** (`dmk_ui.c::stage_filter` ~1874–1920, `stage_amp` ~1924–1960, `stage_lfo` ~1962–1988, `stage_voice` ~1776–1822): on-device filter curve with resonance/cutoff marker, ADSR shape with moving note-envelope point, LFO wave displaying rate/depth/shape/fade, and animated glide/velocity curves. Even abstract DSP values have immediate physical/visual meaning.

3. **Large, clear 240x240 control layout** (`dmk_ui.c` ~1–61): four top K1–K4 fields paired to four real knobs; nearly half screen as a single large illustration. The lit/selected parameter is obvious. The UI graphics include cached thermal glow and lightweight moving sparks (`dmk_gfx.c::holo_done` ~62–124; `dmk_ui.c::graph_life` ~1746–1763). **Do not copy the expensive per-pixel distance-transform glow**: its code describes ~13,000 marked pixels for a graph and additional passes/CPU yields; AQUORA already faces 90% audio/FX processing budget and visual-frame pressure.

4. **Discoverability patterns**: related sound controls are kept in contiguous SOUND / FILTER / AMP / LFO pages, curves point to actual parameters, and screens show labels close to their controls. This approach can inspire AQUORA's main SOUND navigation and contextual help, but AQUORA should retain its own simpler grouped architecture, not clone DEMAKE's many pages.

## Proposed AQUORA next-build feature (P1): SOUND SHAPE / PATCH VISUAL EDITOR

**User's explicit priority:** “I particularly like the way it shows the waveform as you edit the sound.” Build an immediately reactive, large, true-to-audio **waveform/shape panel** in AQUORA's SOUND editor, not another ambient visualizer. This belongs to the editable selected musical track (Lead/Bass/Pads/Harmony/Piano/Plucks/Choir); Drum editor may have a separate compact transient/envelope view.

- Provide an easily reached `SOUND > PATCH / SHAPE` page. Clear title: track + patch. Show K1–K4 *actual editable* values and a large, vibrant waveform representation occupying most of the 240x240 display. Selected knob/control should highlight the exact feature it changes on the shape. At a minimum, patch selection changes the wave; make actual existing compatible tone/brightness and envelope shape controls live-editable where practical without compromising current presets/sound quality, and display dynamic articulation, glide/vibrato/FX feedback in appropriately labeled subviews. Do NOT draw visual changes for a parameter that does not audibly affect the sound.
- **Two honest kinds of graphics:** (A) `PATCH SHAPE` = preview of the selected synthesized patch over one/two periods using the existing `sr_patch` `wave/brightness` and `engine.c::wave()` formula; (B) optional `LIVE AUDIO` = a genuinely measured but low-cost, decimated waveform from the *selected track* (not a fake master-level pulse). Label the modes. Non-sinusoidal harmonics and struck transient/envelope should correspond to their real sound. `Frequency Lock` should show its intentionally pure waveform and make pitch-affecting controls visibly inactive.
- **Cheap implementation:** For synthesized patches, build a 128–192-point single-cycle or short temporal preview *only on patch/parameter changes* on UI/control thread or safe optional core-1 job, never inside audio DMA callback. Use shared lookup tables, fixed point/int where beneficial, short static arrays and bounded raster lines. For future sample layers, precompute ~48–64 peak bins at build/pack time as DEMAKE does; do not decode samples to draw every frame. No FFT or 240x240 heavy shader. Cache unchanged preview geometry and redraw only dirty bands/columns; overlay playhead, note-on/ADSR point, low-cost peak bars and K1–K4 focus at visible UI refresh rate, avoiding expensive texture re-renders. Avoid any audio-thread waiting on graphics or LED drivers, locks, reallocations, or long-running DSP preview jobs.
- Maintain AQUORA's independently selected **>=20 delivered FPS normal visualizer target** and prioritize zero-new-DMA-XRUN audio. The patch editor's big static graph need not be recomputed 20x/second: animate only small indicators while knob/keyboard edits update the actual wave immediately. Preserve efficient core fallback.
- **Save and compatibility:** Preserve per-track selected patches, user-intention weighted palette pools, manual sound locks, 4 banks and generation/Reroll behavior. Patch shaping controls (if added) must be stored/versioned safely. NEW SONG must not override manually locked sounds; `REROLL SOUNDS` changes the shown patch shape instantly, without changing retained motifs.
- **Acceptance:** Actual 240x240 screenshot comparison for all 24 original patch shapes with visible distinctions; knob stress on the seven pitched roles, several glide/vibrato settings, attack/release, and checked live audio A/B; silence/mute/solo, 16 voices, source isolate, loop/key mode, Frequency Lock, saved-bank restore and Reroll. Profile generated-graph CPU, memory, LCD draw time, audio callback p95/p99, delta XRUN and on-device achieved FPS, and prove the UI isn't doing a per-frame analysis of every track. Owner verifies legibility and musical correspondence on physical FM-1.

## Additional design ideas (secondary, not scope creep)

- Use a **filter-response card** with highlighted cutoff/resonance on any real, added filter controls; don't invent controls solely for a drawing.
- Use animated **ADSR, glide and vibrato curves** rather than text-only values so the player can tell if the setting is working.
- Use a slim **playhead and loop-start/end indicator** on any samples or sustained phrase view where the data exists.
- Use contextual K1–K4 legends and focused control highlights consistently across SOUND, DRUMS, MIX and key-mode screens.
- Do not adopt DEMAKE's large chain of effects, copied recordings or elaborate lighting engine: AQUORA must remain a CPU-efficient healing-inspired generative instrument.

Reference provenance: DEMAKE README, `firmware/src/dmk_ui.c`, `firmware/src/dmk_gfx.c`, `tools/gen_samples.py`; see upstream repo for actual source. This is *research and requested future work*, not an implemented AQUORA release.
