## Current resumption checkpoint — November 7, 2026
Both performance MIDI transports are owner-deferred. Candidate preserved at 82ca1f1; non-MIDI release boundary implemented and host/native builds pass; see validation/scope-066. Remaining non-MIDI reconciliation, visual/music acceptance, single-core/repro/WASM/updater/installer and publication gates are unfinished. No release was published; v0.6.5 preserved.

**Backup blocker:** automatic approval review rejected the authorized development push to public CodingSlapfu/AQUORA, citing sensitive source egress and lack of private/organization-owned destination proof. Connector verified public repository and connected-account admin/push rights. No alternative source upload was attempted. Obtain explicit approval resolution before remote writes. Do not call local commits remote backups. Resume existing source; do not repeat passed scope tests unless source changes.

SDK packaging inputs actually exist at `/workspace/scratch/759587c0312e/recovered-base/vendor/ac79-package-files`; use that absolute --sdk path while it exists. Old relative path in instructions below is absent from current checkout. No edits to KIRA-9.

# AQUORA next integration — unfinished development checkpoint

Branch: `aquora-next-integration`. Provisional development identity: 0.6.6 /
`FM-1_73000606`. **Not released, not accepted, do not publish.**
Published v0.6.5 and installer are preserved. KIRA-9 was not accessed or changed.

## Resume without restarting

1. Fetch `aquora-next-integration` and verify its remote head. Check
   `/workspace/.aquora-release.lock` and other active build ownership before
   any edits/builds. The previous owner token was `759587c0312e`; do not
   assume a surviving lock is stale without checking its process/session.
2. Read this file, `validation/NEXT_BUILD_REQUIREMENT_MATRIX.md`,
   `validation/next-development/RESULTS.md`. Reuse the captured full issue and
   owner-comment texts in `validation/next-owner-scope.json`; query GitHub
   only for updates after that snapshot or missing context.
3. For a complete isolated source checkout, run from repository root:
   `bash tools/prepare_next_source.sh /NEW/EMPTY/OUTPUT/PATH`.
   It verifies the pinned archive, overlays current tracked source and applies
   AIR, LED duty and canvas pipeline patches. No compilation starts.
4. Use the verified JieLi **20260730.1** toolchain archive SHA256
   `a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd`.
   Extract with `tar --no-same-owner`. Set `JIELI_TOOLCHAIN` to its directory
   containing `pi32v2/` and `common/`. SDK packaging inputs are recovered
   under `vendor/ac79-package-files`.

Native build in recovered tree:
`python3 vendor/fm1-base/tools/build.py device --release 0.6.6 --sdk vendor/ac79-package-files`.
Forced single-core: same command with `FM1B_CPU2=0`.
Host: `python3 vendor/fm1-base/tools/build.py device --host --release 0.6.6`.
Run builds sequentially under one project owner.

## Preserve candidate work; finish missing scope

- #1: physical F3–G5 black/white mapping; scanner pulse-density 70/50 duty;
  KEYS active/pending keys full brightness; PLAY pulse from engine step/phase,
  independent of decorative mode. External MIDI clock acceptance is owner-deferred.
- #8: existing spiral retained, actual Sans glyph reveal and original stereo
  three-voice FM cue; 49 input skips, PCM/master mute, eight-second and hidden
  worker checks pass. Startup sound preference/migration and complete acceptance
  remain open.
- #17: independent water lighting plus 512 migrating/settling sand particles;
  music event/pitch/intention/frequency inputs. Review full required low/high,
  silence and transition sequences; finish performance/visual acceptance.
- #33: restored 33-vertex Original baseline from `128662b`; fixed/AUTO policy;
  two-strip DMA canvas pipeline. Delayed DMA ownership and sampled pixel parity
  pass. Finish comprehensive fidelity sequences, native instrumentation and
  independent acceptance. Host timing is not physical FPS.
- #34: seeded musical section role targets and bounded fades; 117 intention /
  mode / loop cases and all eight post-gain roles observed. Zero-variation
  regression was fixed and its prior corrective test rerun successfully.
  Finish stems/click/level/SAVE/New Song/Reroll and native load acceptance.
- **#31 MIDI-USB and MIDI-BLE: OWNER-AUTHORIZED DEFERRED** for v0.6.6 per comment 6702097918 / #32 comment 6702099240. Revisit after KIRA-9 has a good working firmware build and owner reprioritizes. Partial work preserved at 82ca1f1; current performance adapters and MIDI controls disabled. AIR, updater SysEx and internal-clock LEDs remain required. KIRA-9 untouched.
- Finish #13/#18/#9 and every unresolved earlier requirement through the #32
  matrix. The register is deliberately `audit_complete=false`; no prior accepted
  feature is to be rewritten merely because its audit row is still OPEN.

## Release remains blocked

Run `python3 tests/requirement_gate_next.py --self-test` for structural and
negative checks. `--release` currently **must fail**. No missing requirement,
unreviewed owner comment, incomplete P0 or unsupported completion can be waived.

Still required: complete reconciliation and implementation, independent CI,
reproducible native comparison, WASM/emulator, required AIR acceptance (USB/BLE performance MIDI owner-deferred),
flash/update/installer acceptance, source/package manifests and verified live
publication. Hardware checks remain owner-only, never software release proof.
