# AQUORA Issue #89 corrective checkpoint

Base is the verified v0.7.26 publication receipt 8303764301b7fe242f983622df72fc94e9838783. Preserve v0.7.26, v0.7.25 and stable v0.7.14. The corrective branch targets testing identity FM-1_73000727. Keep the publication lease fenced and check the tip before editing.

## Implemented and reproduced

- Sample allocation is transactional. The two-slot allocator selects an in-place victim, then validates and commits through the common note-start path. A rejected replacement leaves the older voice untouched. A scheduled phrase can roll over older automatic holds without waiting for both releases. An available automatic sample is preferred to taking a held manual sample. Existing AUTO + Play Along arbitration still permits borrowing a slot if both are manually held.
- The common allocator counts active voices and held instrumental roles, favors released voices, protects the last sequenced bass from other roles and preserves gated external or keyboard notes. An entering role can reclaim surplus sequenced voices at the deadline cap. The new short splice uses the displaced voice's last filtered amplitude and existing decay rather than another DSP voice. Allocation telemetry counts instrument requests, admission refusals and gated evictions, and splits sample-pool denial from the common pool or admission gate. VOCALS Play Along exposes both denial counters, current cap and time since direct vocal PCM.
- The previous native LCD renderer restarted its five-band frame on general input revision changes, even in fullscreen. Water, Cymatics and Chladni now finish the current frame across parameter edits. The dedicated label band can go first. Display mode changes still invalidate an old frame.
- SLOOP 3 reference 2228313f3abe0ad6c8f28ef0cd9b9d8167c01330, engine/engine.h and engine/sound.c. It uses per-track polyphony/protection, quietness and release-state based stealing, deadline shedding and separate 96-frame fade slots. AQUORA retains its 16-voice pool, existing deadline controller and audio worker. Its own admission code uses role occupancy, gated-source protection and the existing one-sample fade state. No SLOOP source is integrated.

## Evidence and remaining work

Run tests/voice_admission_0727.c and tests/frame_policy_next.c. The former reproduces destructive denied replacement against v0.7.26 and captures real direct vocal PCM and audible track-meter values at imposed caps in all five modes. The latter exercises 1,800 fullscreen encoder revision changes, mode changes and the same cadence/pressure policy. Neither measures native DMA time or proves what exact source state was latched on the owner's unit. The LCD test models the cancellation predicate. Inspect the PCM and endurance logs before claiming physical role recovery.

Preserved navigation, V11 Favorites, effects, vocal vocabulary and portrait. Open items include physical reproduction with pre/post no-reboot snapshots, callback and LCD DMA timing while spinning K1–K4, independent oscillator choir output under all roles, complete per-role on-device telemetry and endurance of saved-song writes. No additional genuine sung BABY recording or intention names, revised vocal composition grammar, or portrait animation work is claimed here.

Before a testing publication, verify the exact final native and emulator builds and the GitHub backup. Keep public stable at v0.7.14. A testing release remains subject to the owner’s physical FM-1 acceptance.

## Successor source and builds

621602d9b0dcb99b7fda7df24c4912b33fadcae2 adds crest replies, longer cadences, lead responses, idle portrait refinements and the two-arc-per-mode/cap PCM fixtures. It also prioritizes keeping gated bass voices over replacing a sequenced lead during a vocal entrance. It supersedes the earlier checkpoint's description of vocal composition and portrait work. See ACCEPTANCE.md for coverage and SHA256SUMS for the matching compiled outputs. Reuse those bytes unless evidence requires a source change.
