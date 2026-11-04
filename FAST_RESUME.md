# AQUORA fast resume

**Additional hardware P0/P1 (2026-11-04):** Owner reports **Bass also missing/inaudible on AQUORA 0.3.0**, in addition to Drums. Diagnose separate musical Bass track 2 and Sacred Bass routing/level on real FM-1; add a controlled bass audition with post-mix signal diagnostics, and do not equate generated notes with audible output. User cannot work out Glide: SOUND → Lead / Bass Motion requires **turning SELECT knob** to **Lead track 1 / Bass track 2** (pressing SEL *mutes*). Make this instruction and real glide values/demo unmistakable on screen. All acceptance in `NEXT_BUILD_ACTIVE.md`. Preserve working music, completed 0.3.0 features, and processing headroom.

**New FM-1 hardware feedback after AQUORA 0.3.0 install (2026-11-04):** Owner says overall build works great, but **drums STILL cannot be activated or heard**, despite 0.3.0's simulated tests/ARP enable path; treat as release-blocking P0. Alternate Water Crystals looks like a snowflake, not vibrating water: replace six branching crystal stems with continuously changing, ripple/standing-wave/interference-based **Water Cymatics**, preserving the original visualizer. These are new open requirements in `NEXT_BUILD_ACTIVE.md`. Do not repeat implemented 0.3.0 bank, knob, note-inspector, or visualizer-selection work unless tests show regressions. Protect excellent sound and processing headroom.

Read CHECKPOINT.md and NEXT_BUILD_ACTIVE.md first. 0.3.0 release files are under release/0.3.0/. Owner hardware validation remains open. 0.2.0 and 0.1.0 are rollback builds. Recover the complete checksummed source archive instead of downloading vendor dependencies again.

Use a nonblocking flock on /workspace/.fm1-sound-healing-build.lock. Reconcile GitHub main and local work. Verify remote checkpoints before long work and avoid repeating unchanged acceptance.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory containing pi32v2 and common. Emscripten 3.1.64 builds the browser emulator. Run tests/accept-0.3.0.sh with an isolated output directory. Preserve updater family FM-1_73.
