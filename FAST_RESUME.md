# AQUORA fast resume

**New P0 hardware regression (2026-11-04):** Real FM-1 0.1.0 → 0.2.0 web installer failed at connection with `No answer. Check that the FM-1 is running sound-healing.` This supersedes the old checklist item that called direct upgrade untested. Read the P0 in `NEXT_BUILD_ACTIVE.md`; verify 0.1.0 updater SysEx responder and 0.2.0 installer port pairing/HELLO/loader handoff against real old firmware, and deliver a safe in-place path without stock restore. Do not confuse simulated installer-port passes with hardware verification.

Read CHECKPOINT.md and NEXT_BUILD_ACTIVE.md first. 0.2.0 release artifacts and recovery instructions are in release/0.2.0/. Owner hardware validation remains open. 0.1.0 is the rollback package. The full versioned source archive includes the platform and SDK files, so restore the archive instead of downloading them again.

Use a nonblocking flock on /workspace/.fm1-sound-healing-build.lock. Reconcile GitHub main and any existing local work. Verify remote checkpoints before long work. Use one active build session and avoid repeating unchanged acceptance.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory containing pi32v2 and common. Emscripten 3.1.64 builds the browser emulator. Run tests/accept-0.2.0.sh with an isolated output directory after restoring tools and source. Preserve updater family FM-1_73.
