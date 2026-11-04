# AQUORA fast resume

**Corrected hardware connection report (2026-11-04):** The earlier `No answer. Check that the FM-1 is running sound-healing.` was caused by an **unplugged USB cable**. User reconnected USB and reports the installer is working. No confirmed firmware updater regression. Do not launch the prior P0 investigation. Only separate physical completion/boot and rollback verification remains as noted in `NEXT_BUILD_ACTIVE.md`.

Read CHECKPOINT.md and NEXT_BUILD_ACTIVE.md first. 0.2.0 release artifacts and recovery instructions are in release/0.2.0/. Owner hardware validation remains open. 0.1.0 is the rollback package. The full versioned source archive includes the platform and SDK files, so restore the archive instead of downloading them again.

Use a nonblocking flock on /workspace/.fm1-sound-healing-build.lock. Reconcile GitHub main and any existing local work. Verify remote checkpoints before long work. Use one active build session and avoid repeating unchanged acceptance.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory containing pi32v2 and common. Emscripten 3.1.64 builds the browser emulator. Run tests/accept-0.2.0.sh with an isolated output directory after restoring tools and source. Preserve updater family FM-1_73.
