# AQUORA 0.6.2 — exact continuation after concurrent writer detected

## State

Do not publish this recovery candidate or replace the development branch.
The branch `aquora-0.6.2-development` advanced independently during this
session from 5ee7a22333d23685e7d6742dedecbb021e825208 to
e281f618d0bba51d8e8972720acaf815511aa9fa. Its new commits include
1bb497a4 (eight-sample binaural optimization), 35b24841 (stereo tests),
661b5d00 (sanitized fixtures/timing), and e281f618 (full CI trigger).
Those are not this session's writes. The expected-head update was rejected;
the active development branch was not overwritten. Single release-publisher
ownership must be reconciled before further integration or publication.

This session's contribution is preserved independently on
`aquora-0.6.2-recovery-font-binaural`, initial commit
e2b690a1c9f81523d3d7c7fa5021a91d5f9b2340. It is based on 5ee7a22.
It contains an alternative exact-settling binaural steady-path optimization,
a three-hour actual oscillator-kernel regression, matched OFF/ON/OFF host
benchmark, and AQUORA Sans offline font source/generator/license.
The font is not present in the competing e281f618 changes and can be
integrated independently after confirming publisher ownership.

## Preserve and integrate

1. Confirm the other active release session has stopped or explicitly owns
   integration. Do not infer an abandoned session merely from a quiet branch.
2. Fetch both branch tips into isolated checkouts. Read this file, then the
   current Issues #1–#18 and latest comments. Never use the historical 0.6.1
   FAST_RESUME instructions as the current branch selector.
3. Do not cherry-pick the recovery commit wholesale: `engine.c`,
   `tests/binaural_062.c`, workflow and trigger now overlap. Compare the two
   binaural implementations and select/reconcile one with its matching tests.
   The recovery's `tests/binaural_062.c` includes engine.c to exercise static
   kernel helpers; compile that test alone, not alongside engine.c.
4. Integrate independent font paths `device/fonts/*`,
   `device/tools/gen_aquora_font.py` and the `gen` entry in device/fm1.toml.
   The verified vendor source archive already contains the platform font writer.
5. Preserve completed host-panel fixes. FONT SIZE/BACKGROUND persistence,
   controls_060 and mixer_062 passed at 5ee7a22 and with this font. The old full
   CI/staged installer run 41991259083 passed all seven jobs for 5ee7a22.
6. Complete the current issue-to-source acceptance audit. Known source gaps
   observed at 5ee7a22 include AIR's missing FX chooser/depth mappings, opaque
   SOUND category picker/canonical ownership, Polar Bloom's missing thirteen
   intention-specific geometry profiles, incomplete font-size adaptation across
   every page, and stale manual/active queue. Do not mistake green CI for
   completion of these requirements. No incomplete publication is authorized.
7. Run targeted regressions after reconciliation, then full native/repro,
   single-core, WASM, host/DSP, migration, software performance and updater
   acceptance for the integrated exact commit. Back up matching artifacts and
   verify live installer bytes/identity only after software gates are complete.

## Verified local evidence

- `validation/recovery-062/` contains actual targeted test logs.
- `binaural-062.log`: nine carrier/difference corner combinations, silence,
  solo, rapid sweeps and three hours of exact kernel phase advancement.
  Mean long-run channels: 999.750370 / 1000.250278 Hz; difference 0.499907 Hz.
- stereo.log: 217/223 Hz opposite-channel rejection 81.1 / 86.1 dB.
- appearance/control/mixer logs: actual host panel mutation and persistence.
- previews.log: battery pixels/fades, 49 startup skips, LED diagnostic and
  animated SOUND/EVOLVE/visualizer previews.
- corrective_061.log and transport_hit.log: DSP regressions under ASan/UBSan.
- Timing logs are noisy host proxies; do not claim a physical speedup or XRUN
  pass. Native/single-core/WASM have not been rerun for the recovery changes.

Physical FM-1 first-post-install/cold-boot audio, AIR reception/hand response,
LED output, delivered LCD FPS and listening remain owner verification.
No 0.6.2 release or public installer was published by this recovery session.
