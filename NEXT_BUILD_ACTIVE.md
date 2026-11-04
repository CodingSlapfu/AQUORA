# AQUORA active follow-up

The implemented 0.2.0 software scope is archived in [archive/0.2.0-requirements.md](archive/0.2.0-requirements.md). Verification logs live in evidence/0.2.0/ and artifact hashes in release/0.2.0/. Keep 0.1.0 available for rollback.

## Resolved connection issue (2026-11-04; not an updater defect)

- **User correction:** The installer displayed `No answer. Check that the FM-1 is running sound-healing.` because the FM-1 USB cable had been unplugged. User reconnected USB and reports it is working. **Withdraw prior P0 firmware-updater failure claim and repair work.** Do not spend build time investigating this as a confirmed software regression. Preserve the connection history here only for troubleshooting context.
- **Verification nuance:** Successful connection is reported; don't claim full 0.1.0 → 0.2.0 flash and subsequent device boot or rollback testing as independently confirmed unless documented by the owner.

## Newly confirmed 0.2.0 FM-1 hardware results (2026-11-04)

- **Confirmed by owner:** AQUORA 0.2.0 is installed and running on the physical FM-1. Overall sound engine is described as **amazing / very nice**. The owner's reported processing gauge is now approximately **74% during playback**, down from approximately 90% on 0.1.0. Preserve the current musical character, performance improvement and functioning UI/installer. Do not claim 74% is a controlled CPU-core benchmark, complete stress test, or zero underruns; no hardware evidence yet for those.
- **Still a P0 usability/audibility failure:** The owner still cannot reliably hear or activate drums and cannot tell whether percussion K3 density/K4 enabled toggles work. Do not mark drums fully hardware-verified from simulated isolated-kit tests.

## Priority 0 — Make percussion audible, discoverable and visibly controllable

- [ ] **Investigate why owner cannot hear drums on FM-1:** In 0.2.0 `sr_init()` sets `drums_enabled=1`, `drum_level=.66`, `drum_density=2` and Drums track volume to 1.0, so a default-on path exists in source. It is not proof of audible device output. Check actual `drum_trigger` events, envelopes, chosen kits, track 8 mute/solo, effects/mix levels, master/limiter, channel routing, K4 and ARP toggles, user-selected kit, and scene changes. Add low-cost diagnostics: percussion ON/OFF, selected kit, density 0-3, level and **actual drum-trigger/meter activity**. Test isolated drum solo, natural mix and every kit at intended hardware volume, in every scene. Preserve soft ambient character but make percussion clearly audible. Capture owner verification before closing.
- [ ] **Fix percussion group K3/K4 live feedback:** Current `device/ui.c` renders values for K1 Kit and K2 Level but **only a hint string** for K3 Density and K4 On/Off. Show `K3 DENSITY: 0/1/2/3` and `K4 DRUMS: ON/OFF` with true live device state, immediate visibly updated feedback after any turn and at-rest value visibility. Toggle must have a positive/negative explicit state, not a silent ambiguous action. Display when track 8 is muted/soloed, and distinguish global percussion enable, density zero and track volume/mute so user can find the actual reason for silence.

## Priority 1 — Live function and current value on EVERY knob

- [ ] **Every currently active knob must show its function AND its changing value on the FM-1 display:** K1, K2, K3 and K4 across all four pages and all eight SOUND groups, including mixer, effects, intentions, frequency, binaural, motion/glide/vibrato, Sacred Bass, tracks, evolution and percussion. Display meaningful values/units (%, Hz, ms, cents, 0-3 density, text patch/kit, Off/Legato/Always, ON/OFF, source/mode) and update immediately when changed by user or internal scene changes. Include ALGORITHM (current page), SELECT (selected track number/name) and PRESET (scene, patch or kit, as applicable). Labels cannot merely list control names; real values/status are mandatory.
- [ ] **Design for actual 240×240:** Keep four clear K1-K4 value readouts visible at rest (compact 2×2 or equivalent) with an easy-to-read temporary larger adjustment feedback when a knob turns; preserve a usable reactive visualizer. Ensure legible font, no overlap/truncation, no stale labels after page/group/track changes, no excessive redraw or audio stutter at the user's roughly 74% reported processing load. Apply encoder changes through the existing audio-owned command queue; display confirmed state after the command is applied. Toggle encoders should change exactly once per intended detent, remain responsive, and make failure/limit reached apparent.
- [ ] **UI acceptance matrix:** Exercise all pages, SOUND groups, knob rotations in both directions, zero/max, mute/solo, scenes changing dependent values, toggle state, rapid knob turns during playback, and STOP/PLAY with screenshot/automated text verification plus hardware listening. Measure incremental UI CPU/display cost; preserve audio deadline and current soundtrack quality.

## Owner hardware verification still open

- [x] Owner reports AQUORA 0.2.0 installed, boots, and plays excellent audio on hardware after reconnecting USB. Exact updater steps and rollback were not separately logged; do not claim interrupted-flash recovery validated.
- [ ] Separately verify in-place update and rollback/recovery details when needed; the prior USB connection error was not a firmware defect.
- [ ] Physical STOP/restart and master-zero safety, balanced and audible drums (currently not heard/activation unclear), all controls, pitch motion, headphone separation and extended listening. Owner confirmed overall sound quality is excellent.
- [ ] Measure real core-1 execution, forced single-core behavior, typical and sustained stress audio/FX callback timing, peak/percentile margin, visual responsiveness and zero xruns against 0.1.0. Owner reports approximately 74% processing during playback (informal hardware observation); do not substitute this for the complete benchmark. Follow release/0.2.0/README.md.

No device measurements have been inferred from host timing. Add new software work only from verified regressions or a new requirement.
