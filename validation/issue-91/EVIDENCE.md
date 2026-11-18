# v0.7.30 development build evidence

Source checkpoint 175ad29d5bb89ed7754ff3cab678dfb94e01f006.

| Artifact (development backup only) | SHA-256 |
| --- | --- |
| AQUORA-0.7.30-development.fwsc | ce58a342fa0af708a2fa5dd447ff875265d2d22bd523fc61406f46bef21286b9 |
| app.wasm | 276590e9934897ad22659220e3885dda486e3a46e80f3d29270601313a0c3345 |

The FM-1_73000730 native package reproduced byte for byte. App image 550,504 of 581,564 bytes, .data + .bss 81,756 of 98,304 bytes, and pool 325,960 of 344,064 bytes. The identity-specific simulated updater passed installation, resume, mismatch and corruption checks. WASM passed 512,000 audio frames, moving 240x240 framebuffer and 20 flash objects. Vocal stress passed 1,200 settings-change PCM windows. The host glow fixture measured 1,105 changed pixels across the three modules under direct PCM and no singing glow with choir muted. Host rendered DAY, YOU, LOVE, ME, DREAMS through manual word selection and the allocator to distinct unclipped PCM.

Matched host comparison used seed 729, Dreaming, Cinematic, 12,027 frames per tick, master 0.8, vocal presence 4, and 921,600 frames per take. AUTO and manual WORDS were each rendered as full mix and choir-isolated with zero vocal send. AUTO was byte-identical between v0.7.29 and this checkpoint in this fixture and differed from v0.7.27 in full mix and isolation. All twelve takes had six accepted vocal onsets. This establishes a remaining default AUTO difference. The isolated PCM peak was 3,990 in v0.7.27 and 8,084 in both later builds. Production and timing changed between the baseline and v0.7.29. The result does not isolate their individual contributions or establish subjective musical quality.

The Freesound original-download URLs all returned the login page. Public previews were retrieved and hashed. New word takes verified and shipped: 0. This development checkpoint has no testing installer deployment. No prior deployment or stable release was changed.
