# AQUORA fast resume

**Post-hardware-test, next release:** Read [NEXT_BUILD_ACTIVE.md](NEXT_BUILD_ACTIVE.md) first. The user successfully installed and listened to AQUORA 0.1.0 on FM-1 on November 4, 2026, and reports great general audio, but no audible drums or bass, ~90% CPU; new perfect-key, visual, performance, two intention modes, and twelve additional ambient kits are outstanding. Maintain this as the authoritative next-build queue, and do not redo completed 0.1.0 work.

AQUORA 0.1.0 has a native and browser build. Read CHECKPOINT.md and release/0.1.0/README.md first. Recover full source from the checksummed GitHub release parts, including pinned platform and SDK packaging inputs. Do not redo completed dependency recovery or tests unless inputs changed.

Acquire the nonblocking exclusive lock /workspace/.fm1-sound-healing-build.lock before edits, builds, tests or publication. Reconcile local changes against GitHub main. Run one sequential build owner. Keep compact evidence and verify remote checkpoints before long work.

JieLi 20260730.1 archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Set JIELI_TOOLCHAIN to the extracted directory with pi32v2 and common. Verify extraction. Emscripten 3.1.64 builds the browser emulator. Use the archived vendor tree.

Native: python3 vendor/fm1-base/tools/build.py device --sdk vendor/ac79-package-files --release 0.1.0
Single-core check: use FM1B_CPU2=0 and an isolated app output directory.
Host and web: add --host or --web with the same release version.
Installer: python3 vendor/fm1-base/tools/make_site.py device device/build/sound-healing-0.1.0.fwsc 0.1.0 <output>
Identity: FM-1_73000100. Preserve updater identity. Owner physical verification remains open.
