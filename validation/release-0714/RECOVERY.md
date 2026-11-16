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
