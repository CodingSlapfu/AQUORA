# AQUORA v0.7.14 recovery checkpoint

Resumed from 584345a3f0a72290f18da9af92aa4e45148b8f9c after Work command execution recovered.

- Drum HIT test passes at 0, 25, 50, 75 and 100 percent. All 16 lanes occur at the default 25 percent. FX auditions the selected part on the 16-part mixer.
- The VOCALS effects page controls independent chorus, room, echo and note-on scale pitch correction. Impulse, dry bypass and tuning tests pass. The recorded vocal regression covers all five modes with fresh default settings and verifies playback, disable/release and independence from legacy choir patch locks.
- Bank compatibility regression passed. Native build and byte-for-byte reproduction passed for FM-1_73000714. Checkpoint firmware SHA-256 is e23b5d509a9f0df066655ae7d16ffc9540a99ae2020903232286cd6a80338071.
- Pending: expanded ambient mixer effects, full drum pattern step editing and kit sound differentiation, end-to-end vocal synchronization and performance acceptance, final transport and browser installer gates, GitHub release and deployment. v0.7.14 is unpublished.

Transport fixture results: duplicate key, watchdog, Sacred Bass release, and 32 saturated stop/restart cycles passed. The legacy fixed reference isolation assertion at tests/transport_hit.c:90 failed on both 584345a and this checkpoint. Its cause still needs resolution before release.

## Transport fixture recovery: GitHub-hosted verification

A subsequent Work attempt had no usable execution environment, so it made no source changes.
A separate GitHub-connected correction was committed without changing firmware DSP:

- `040cd4e050de7b368287197768ca81601d02a1a3`: fix `tests/transport_hit.c` to set `tone_level=0` before asserting the initial all-muted silence (the independent reference oscillator must not be conflated with instrument tracks), then allow 64 render blocks for the documented click-free reference-gain ramp to settle before asserting silence after tone disable. Keep enabled-frequency, STOP silence and remaining transport assertions.
- `6bcb3a27f33f0dec59b7335de0f0525869f2dd15`: add targeted workflow `.github/workflows/aquora-0714-transport.yml` for GitHub-hosted compilation and execution of the complete transport fixture. Workflow run: https://github.com/CodingSlapfu/AQUORA/actions/runs/43968874123.

**Verification status:** The workflow was in progress when this handoff was written; do not treat it as a PASS without reading its actual result. No native firmware was rebuilt after the fixture-only correction, and no official release or installer deployment was performed. Existing open gates above remain open.

### First GitHub Actions result and follow-up fixture correction

- GitHub Actions run [43968874123](https://github.com/CodingSlapfu/AQUORA/actions/runs/43968874123) **compiled** the current transport harness and passed the previously failing fixed-reference isolation, then uncovered an additional **obsolete test assumption** at `tests/transport_hit.c:130`: the historical hard-coded expected frequencies array has 13 entries, but the engine has `SR_SCENES=22`; `sr_set_scene` can also automatically reduce/disable its tone level and the reference pitch slews smoothly.
- Commit `ddf9023e54237ab63654713dacc89d4d987c4d0c` updates this test to hold reference-level settings constant, derive expected Hz from the actual `sr_scenes[scene].frequency` mapping for **all 22 intentions**, and allow the intentional frequency glide before stable-frequency measurement. It preserves reference, Sacred Bass, binaural and STOP assertions.
- GitHub-hosted verification for this revision: https://github.com/CodingSlapfu/AQUORA/actions/runs/43969234311 . At documentation time this run's outcome was not yet confirmed. **Never report it passed without reading the workflow result.**

### VERIFIED transport regression gate — PASS

After modernizing outdated assumptions in the legacy fixture while retaining all substantive assertions, the complete `tests/transport_hit.c` host regression suite **passed** on GitHub-hosted Ubuntu:

- Verified source fix: `040cd4e050de7b368287197768ca81601d02a1a3` initial fixed-reference silence and click-free gain-ramp behavior.
- `ddf9023e54237ab63654713dacc89d4d987c4d0c` all 22 intention frequencies from current mapping, with reference pitch glide settled.
- `5a1a05a5fc5a1417bd19453ddfa100beacbcca03` valid 8/14/20% Nature progression and 50/100% clamp at the mandatory 20% maximum.
- `966322fb0dfe46053cd50cd23c1f92d1dc9354b9` deliberate immediate manual Lead/Bass vibrato test and modern mode-dependent optional drum HIT cap assertions.
- **Successful GitHub Actions run:** https://github.com/CodingSlapfu/AQUORA/actions/runs/43970123244 , head `966322fb0dfe46053cd50cd23c1f92d1dc9354b9`, conclusion `success`; the engine and full transport test compiled and executed, including duplicate-key watchdog, repeated STOP/restart, isolated reference PCM, Sacred Bass and binaural isolation, all intention frequencies, loop boundary key changes, Frequency Lock, Nature, percussion repetition, vibrato, and HIT cap/zero.

All changes in this recovery were to tests, CI, and documentation; **NO firmware DSP implementation, binary, official release or existing Web Installer was changed**. This PASS clears the previously reported transport fixture gate only. Outstanding work remains expanded ambient Mixer FX, full 16-lane drum step editing and kit sound differentiation, vocal/KIRA-9 visual sync and hardware-oriented performance verification, final native/browser installer validation, official v0.7.14 release and existing Sites deployment. Resume from latest verified branch HEAD; preserve previous native reproducibility result (firmware checksum from prior checkpoint) but do not misrepresent it as an artifact from this CI.

### GitHub-connected drum-kit differentiation — VERIFIED PASS

A Work execution runner was again unavailable. Rather than retry it, a scoped GitHub-connected change was made with an exclusive publication lease.

- Drum DSP commit `5fd65e8f6461cd7c6d40ed063e645d2193c9723a` replaces the previous modulo-seven one-shot sample playback-rate mapping with a bounded, kit-specific mapping for all 19 kits. This changes only sampled percussion timbre, not bank format, MIDI note assignments or the previous 16-lane graph.
- Added deterministic PCM host test `tests/drum_kits_0714.c` and GitHub-hosted workflow `.github/workflows/aquora-0714-drums.yml`. The test generates complete PCM one-shots, requires all 19 kit outputs to be mutually different, and verifies audible samples on every one of the 16 lanes.
- **Verified green CI:** https://github.com/CodingSlapfu/AQUORA/actions/runs/43972494465 (head `b66e469cfdd49d02c6eb488df09b28d03d7e4db6`). Host PCM tests passed all 19 distinct kits and 16 audible lanes, complete transport regression passed, and `release_controls_0714` passed five automatic BPM ranges plus drum HIT coverage.
- This does **not** establish subjective kit quality on the physical FM-1, nor complete the user's requirement for independent 16-lane step editing/programming. No release, Sites deployment or new native package was created by this CI-only scoped change.

Remaining: implement and persist complete 16-lane step edits, expanded mixer FX, end-to-end human singer/KIRA-9 sync, production performance, native + installer packaging, official v0.7.14 release and existing Sites update. Resume the latest branch HEAD; do not restart old work, or conflate the passing drum audio test with complete drum acceptance.
### 512-byte bank persistence constraint for drum step editing

The still-open full per-step editor cannot safely be implemented merely by appending arrays to the existing bank: `vendor/fm1-base/device/aquora_async_store.h` allocates `payload[512]` and rejects `len>512`; the current `device/arrangement.c` record is 480 bytes. Preserve the existing validated bank format and CRC/torn-write rollbacks; design explicit versioned compact steps or separate collision-safe persistent storage with migration tests. The new 19-kit PCM differentiation does not change bank schemas or count as completion of per-lane step editing.

### GitHub-hosted step editor, Mixer space controls and browser staging

- Firmware source checkpoint `2fc5b3cbd3ad337bd9712667a3f403249d495365`. DRUMS → STEP EDIT uses K1 for lane, K2 for step, K3 to set/clear, K4 for manual/auto, and FX to audition. Manual patterns repeat per bar, respect track-level mute/solo/level and bypass automatic HIT gating. The 512-byte V10 format uses versioned space reclamation and a terminal CRC; prior V9 banks still load. Previous firmware cannot read newly written V10 banks.
- [Drum and bank CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43981203030) passed all 16 manual lanes over 64 steps, all four bank slots, V9 migration, corrupt payload rejection, 19-kit PCM differentiation, transport and controls.
- MASTER FX now opens SPACE FX with the FX button. Separate controls expose smoothed reverb decay, echo feedback, echo damping and stereo width. Chorus/modulation, broader filtering and experimental processors requested in #83 are still open.
- KIRA-9 portrait rendering takes one recorded-vocal snapshot per complete UI frame and honors meter, mute, solo and source isolation. [Audio and performance CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43981620929) passed recorded-vocal PCM and vocal effects tests and 24 simulated visualizer/control cases. End-to-end verification that audible phrases and portrait frames align is still open.
- [Native CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43981203002) reproduced `FM-1_73000714` byte-for-byte. The [staged browser installer](https://github.com/CodingSlapfu/AQUORA/actions/runs/43981456484) used that exact native artifact, compiled matching WebAssembly, rendered the actual step tab, and rejected a one-byte modified FWSC. Verified SHA-256 `03723331e7d8e940a8c7f7e98276c5345bd5d7fe61ec0e4fa32015d78f3dcebe`. CI artifacts have 30-day retention.
- v0.7.14 remains unpublished. The existing production installer was not deployed in this session. Remaining release gates include broader Mixer FX and end-to-end visual synchronization; simulated latency is not physical FM-1 evidence.

### Motion/Texture and recorded-vocal portrait software gates

- Firmware source `e00569b443b71aa6fdcaf77f3442c5ab1bf97f4a`. MOTION FX offers chorus, autopan, flanger and tremolo. TEXTURE FX offers wet filter, bitcrush, soft drive and tempo-derived stutter. New controls default to zero and short-delay effects reuse the existing FX ring.
- [Audio/UI gate](https://github.com/CodingSlapfu/AQUORA/actions/runs/43983924061) passed all eight Mixer PCM checks, recorded vocal FX and recorded PCM/portrait-band agreement, including mute, solo and source isolation. Its 24 latency scenarios ran on the host simulator.
- [Drum/bank CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43983924263) and [transport](https://github.com/CodingSlapfu/AQUORA/actions/runs/43983924087) passed. [Native reproduction](https://github.com/CodingSlapfu/AQUORA/actions/runs/43983924168) built two byte-identical `FM-1_73000714` packages. [Browser staging](https://github.com/CodingSlapfu/AQUORA/actions/runs/43984063880) matched the native bytes and rejected a one-byte tampered package. SHA-256 `04fef2404bd75f1085ea8538d61f98fddc85a24dee98afa8319abcca28756615`.
- Site target remains the existing public project `appgprj_6ad6d0ae409c8191b45224ecdec724db`. Its source uses a separate authenticated Git endpoint. This session had no executor to check out, update, and package it. The browser staging workflow has the exact matching installer artifact for the eventual in-place update. Preserve every rollback route, including v0.7.13, and compare the deployed firmware SHA-256 to the GitHub release asset before claiming live verification.

### Official publication attempt

[Release workflow 43984315330](https://github.com/CodingSlapfu/AQUORA/actions/runs/43984315330) passed immutable input, native/staged byte equality, v0.7.13 rollback, and lease checks. GitHub then rejected release creation with HTTP 403, `Resource not accessible by integration`. The v0.7.14 release endpoint returned 404 after the attempt. Use the existing verified bytes with a GitHub account authorized to create Releases. No rebuild is needed.

The durable [binary backup](https://github.com/CodingSlapfu/AQUORA/blob/6816df7fbd820286d8a7d6ed30f736ccab20c96a/validation/release-0714/assets/AQUORA-0.7.14.fwsc) is on `aquora-v0.7.14-verified-assets` at `6816df7fbd820286d8a7d6ed30f736ccab20c96a`. [Backup CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43984694877) fetched the pushed branch and reproduced the staged firmware SHA-256 from its remote blob. This is the exact package to upload when release creation is available. Deploy matching staged assets to the existing Site only after release and retain the v0.7.13 rollback path.

### Ambient effects candidate supersedes the earlier firmware for publication

- Firmware source `ca96a17668c5ca6a4c01eb6bb848884d22c473fa`. MIXER → MASTER FX → AMBIENT FX adds octave-up reverb shimmer, a two-grain hold of a 1,024-sample send capture, and dry-signal-keyed wet-return ducking. All start bypassed. Release FREEZE to zero before capturing a fresh buffer.
- [Affected audio/UI regression](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989720448), [transport](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989720406), and [drums and banks](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989720444) passed. [Native CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989720495) reproduced `FM-1_73000714` byte-for-byte. RAM .data+.bss is 82,764 of 98,304 bytes, and the pool is 261,316 of 344,064 bytes. Audio deadline testing with the effects enabled on physical FM-1 hardware remains open.
- [Browser staging](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989896947) verified native byte equality and rejected a one-byte changed package. [Backup CI](https://github.com/CodingSlapfu/AQUORA/actions/runs/43989896404) fetched and hashed the published backup Git blob. [Exact candidate firmware](https://github.com/CodingSlapfu/AQUORA/blob/e8c959e4b71b34e0a2164ba4a63197af761400d8/validation/release-0714/assets/AQUORA-0.7.14.fwsc) SHA-256 `d3de83af71ba084c61dc88b1804e814200d7ae69439480d2aecfcb886d09ec6e`. Retain the previous verified backup at `6816df7` and the v0.7.13 rollback.
- The official v0.7.14 release was absent on entry. The connected GitHub interface has no release creation operation; the previous Actions credential returned 403 and was not retried. The existing Site is accessible as owner, but the executor needed to open, update, and push its separate authenticated source checkout is unavailable. Publish from a release-authorized session, then deploy and verify the live bytes against this checksum. Use the updated release notes. The historical publication workflow points to the previous candidate and must not be rerun.
