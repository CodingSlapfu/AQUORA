# AQUORA fast resume

**Latest owner correction (2026-11-04):** AQUORA 0.3.0 **Drums are audible** and **musical Bass is audible but weak**. Replace prior misleading “drums/bass missing” P0 investigation with the active next-build scope: **expand Percussion HIT from 0–70 to 0–100%** (optional strike probability, not overall volume) and **strengthen the Bass mix/timbre** while retaining the excellent overall sound, DSP performance and current working percussion. Current Lead/Bass Motion UI remains confusing: SOUND → Lead/Bass Motion, turn **SELECT encoder** to track 1 Lead or track 2 Bass; the SEL button toggles mute. Improve explanatory in-device prompts. See `NEXT_BUILD_ACTIVE.md` for acceptance tests.

**Water Cymatics visualizer still open (2026-11-04):** User finds 0.3.0 alternate too much like a snowflake; replace radial ice branches with moving, water-like standing waves, interference and ripples while preserving the Original visualizer. Do not repeat already completed 0.3.0 bank, knob, note-inspector or mode-selection work unless evidence warrants it.

Read CHECKPOINT.md and NEXT_BUILD_ACTIVE.md first. 0.3.0 release files are under release/0.3.0/. Owner hardware validation remains open. 0.2.0 and 0.1.0 are rollback builds. Recover the complete checksummed source archive instead of downloading vendor dependencies again.

Use a nonblocking flock on /workspace/.fm1-sound-healing-build.lock. Reconcile GitHub main and local work. Verify remote checkpoints before long work and avoid repeating unchanged acceptance.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory containing pi32v2 and common. Emscripten 3.1.64 builds the browser emulator. Run tests/accept-0.3.0.sh with an isolated output directory. Preserve updater family FM-1_73.
