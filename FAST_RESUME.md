# AQUORA fast resume

**Additional next-build scope (2026-11-04):** User requested a dedicated Percussion `HIT %` adjuster hard-capped at 70%, **four independent arrangement SAVE/LOAD banks**, and a **second music-reactive water-frequency/crystal visualizer inspired artistically by Masaru Emoto**. The original visualizer must remain selectable; apply stock FM-1 animation insights only after inspecting authentic reference/source. Preserve musical quality and existing ~74% reported load. Full acceptance criteria and persistence/CPU safeguards are in `NEXT_BUILD_ACTIVE.md`. These are **open requirements**, not implemented features.

**Hardware follow-up (2026-11-04; AQUORA 0.2.0):** User has installed the new build on FM-1, calls sound engine amazing, and reports processing approximately 74% (previously ~90%). **P0 new issue:** drums still cannot be heard or reliably activated; percussion K3 Density and K4 On/Off provide no live values. **P1 new scope:** display every knob's function and live value across all pages/groups, including toggles; keep performance and sound quality. Read `NEXT_BUILD_ACTIVE.md` first for the precise active acceptance queue; do not repeat completed 0.2.0 work.

**Corrected hardware connection report (2026-11-04):** The earlier `No answer. Check that the FM-1 is running sound-healing.` was caused by an **unplugged USB cable**. User reconnected USB and reports the installer is working. No confirmed firmware updater regression. Do not launch the prior P0 investigation. Only separate physical completion/boot and rollback verification remains as noted in `NEXT_BUILD_ACTIVE.md`.

Read CHECKPOINT.md and NEXT_BUILD_ACTIVE.md first. 0.2.0 release artifacts and recovery instructions are in release/0.2.0/. Owner hardware validation remains open. 0.1.0 is the rollback package. The full versioned source archive includes the platform and SDK files, so restore the archive instead of downloading them again.

Use a nonblocking flock on /workspace/.fm1-sound-healing-build.lock. Reconcile GitHub main and any existing local work. Verify remote checkpoints before long work. Use one active build session and avoid repeating unchanged acceptance.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory containing pi32v2 and common. Emscripten 3.1.64 builds the browser emulator. Run tests/accept-0.2.0.sh with an isolated output directory after restoring tools and source. Preserve updater family FM-1_73.
