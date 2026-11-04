# AQUORA — FAST RESUME (minimum-work handoff)

**Use this before any broad project scan.** This is a narrow navigation/checkpoint sheet, not replacement for [ACTIVE_SCOPE.md](ACTIVE_SCOPE.md), [CHECKPOINT.md](CHECKPOINT.md), or the mandatory [FM1_HARDWARE_OPTIMIZATION_STANDARD.md](FM1_HARDWARE_OPTIMIZATION_STANDARD.md). Newer verified source/checkpoints supersede the status below.

## Do first (no work duplication)
1. Official origin: `https://github.com/CodingSlapfu/AQUORA`; old `CodingSlapfu/FM-1-Firmware-Projects/FM1-Sound-Healing/` is backup only. Compare latest remote head with any live paused workspace/unpushed edits; preserve and reconcile newer work. Do not overwrite, discard, or start a parallel build.
2. Acquire the project-exclusive **nonblocking** build lock (last known workspace lock: `/workspace/.fm1-sound-healing-build.lock`) before code edits/build/test/deploy. If held by another session, exit without changes. Do not assume the old filesystem/lock survives a new workspace.
3. Read `CHECKPOINT.md` and `evidence/native-build.log` and only the affected source first. Full active scope and hardware standard must still be followed; archives are fetched on demand, not bulk-ingested.

## Exact last documented software blocker (2026-11-04)
- Latest saved native link failed: `R_PI32V2_LONG_JUMP_23M2` against `fm_fabsf` from `.ram_text` in `device/dsp/engine.c` (full renderer section near line 281). See `evidence/native-build.log`.
- `device/dsp/engine.c` maps `fabsf -> fm_fabsf` on-device near the top; audio meter/limiter computations call `fabsf` near lines 309 and 329. First inspect relocation and the actual assembler/linked-section placement; apply the smallest semantically correct linker/inline/math-helper fix, then check for additional cross-section branches. Preserve safety checks, limiter behavior, float correctness, and audio timing. **Do not bypass errors or publish stale prior binaries.**
- Re-run narrow link first. Then host DSP/UI tests and sanitizers, stereo spectral/channel separation and worker fault/fallback scenarios, forced single-core variant, native package, installer checks. Record exact pass/fail evidence and hashes. Physical FM-1 verification remains with owner.

## Dependency/environment cache: do not re-download blindly
- JieLi Linux toolchain `20260730.1`, archive SHA256 `a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd`; ensure extracted `pi32v2 clang` is intact (`-mcpu=r3 -mfprev1`). Prior tar ownership errors truncated extraction; Python extraction recovered it.
- `fm1-base` pinned `2d8ed748cdade582c65d9fc8d8cf17da4ddadec8`; BSP `773b0b99f5ade1a30d1879b2618c6c3c3e6ba031`. The AQUORA GitHub snapshot has **only selected vendor files**, not a complete buildable vendor tree. Retrieve missing pinned upstream components only if required.
- AC79 SDK package files from pinned source `5fa84a9b2f723a5e9337ad79dc2ad741fc98fe2d` / Library package `EIN1-AC79-SDK-5fa84a9b-verified.zip` (toolchain/SDK assets only, **not** retired firmware code). SDK boot/config hashes differ from older reference; verify provenance and compatibility before release.
- Emscripten `3.1.64` install had been interrupted during a ~301 MB binary download. Prefer verified existing caches; install/retrieve only when emulator/installer phase requires it. Preserve cache/paths/checksums across runs where possible.
- Last known native command: `JIELI_TOOLCHAIN=<toolchain-root> python3 vendor/fm1-base/tools/build.py device --sdk vendor/ac79-package-files --release 0.1.0`
- Host command: `python3 vendor/fm1-base/tools/build.py device --host --release 0.1.0`

## Cheap execution discipline
- Single sequential build owner, Sol Light for routine coding; no speculative parallel agents/repeated source archaeology. Run focused checks first, full acceptance only after relevant changes.
- Automate mechanical steps in scripts; keep logs on disk and send only the first actionable error + short context to the model (not megabytes of compiler output).
- Use remote verified GitHub commits at meaningful milestones and **before** lengthy downloads, compilation, tests, or deploys. Record remote SHA, exact command, result, artifacts/hash, and next failing step in a short checkpoint. Don't confuse a local commit with a remote backup.
- If dependencies/environment are unavailable, save an exact recovery manifest and preserve progress rather than repeatedly rebuilding the environment or restarting from scratch.
- Avoid publishing until matching firmware + tested installer + release archive/hashes + source backup are verified. Current GitHub source is developmental, not an accepted flashable release.

## Known migration cleanup (not the linker blocker)
`device/fm1.toml` still points to the previous repository and uses the old user-facing product name. Update repository URL/branding when appropriate, **but do not casually change `id_prefix` or updater identity** without upgrade-path analysis.
