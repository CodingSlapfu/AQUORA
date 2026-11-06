# AQUORA post-0.6.5 scope audit — 2026-11-05
**Purpose:** initial evidence-based classification of **all GitHub Issues #1–#32** so no deferred work silently disappears after a scoped hotfix. **This is NOT a complete source-by-source acceptance of all historic issues; investigate unverified entries before modifying code.** Baseline firmware: accepted v0.6.5 source `45c02ebb17e93684092227b16378277bbe3841d3`. Process remedy: [#32](https://github.com/CodingSlapfu/AQUORA/issues/32).

## Grounded root cause
- v0.6.5 [acceptance matrix](../validation/integration-065/REQUIREMENTS.md) names **only #2, #16, #24, #26–#30, PR #25**. #17's latest owner's water/sand redesign was explicitly excluded from the hotfix and deferred to the next build; the root active queue then said no outstanding work without a separate post-v0.6.5 queue.
- Verified latest `device/visual_worker.c` still labels the visual `Resonance Plate`: a **17×17 sampled nodal scalar field**, up to **256 contour segments**; `device/ui.c` labels it **"Resonance Plate"**. The implementation improved speed but does not simulate visually moving sand particles/settling or illuminated water surface. Owner's latest brief [#17 comment](https://github.com/CodingSlapfu/AQUORA/issues/17#issuecomment-6667383022) specifically rejects contour-mesh variations.
- At audit, GitHub's 30 preexisting issues (#1–#31 excluding #25) were open while some work was software accepted. Open issue ≠ unimplemented, and a successful release ≠ all issues complete.

## Initial issue inventory
| Issue | Preliminary status | Important remaining acceptance / evidence |
|---|---|---|
| #1 LEDs | Partially implemented; physical unverified | Current `device/app.c` includes `lighting==1` all-key mask. Prove real supported LED hardware matrix lights correctly; don't blindly rewrite |
| #2 Navigation | Software accepted for v0.6.5 scoped fixes | Source/route tests in release; physical usability still owner's |
| #3 Nature AUTO/MANUAL/OFF | Partial / targeted verification | Current `device/ui.c` shows Nature Mode and `engine.c` chooses beds. Verify owner's intention/style, OFF persistence/crossfades/mode behavior and real audio |
| #4 SOUND+EVOLVE graphs | Targeted verification | Confirm all requested graphs use actual audio/events and respond live; don't count decorative art/APNG as proof |
| #5 Birds/Forest realism | Partial / owner listening unverified | New engine `bird_left/nature_activity` exists, subjective naturalism and long-form uniqueness still need A/B |
| #6 FM sound library | Prior implementation / targeted verification | Bass expansion #28 accepted; verify other roles/drums/nature request coverage against latest library IDs |
| #7 Broad optimization | Partially accepted / hardware unverified | v0.6.5 performance tests accepted; actual FM-1 callback/FPS/XRUN and fallback under new #17/#31 work still need checks |
| #8 Intro sound/visual | Software accepted in v0.6.5 scoped #24 | Original distinct spiral + FM chime; owner listening/skip physical acceptance unverified |
| #9 Evolution / revisit | Open evidence gap | Verify long-session role changes and fresh A→B→A unlocked patches/motif/kit with per-mode/intention tests, not just realization count |
| #10 Fullscreen | Partial / new visual pending | True edge-to-edge art composition for new #17 visual; actual LCD visual appeal/FPS owner unverified |
| #11 Battery | Existing implementation / targeted confirmation | Verify fullscreen fade, normal UI display, low-power behavior on device |
| #12 CPU/XRUN | v0.6.5 software mitigation accepted; hardware unverified | Earlier owner reported severe overload; do not claim 0-XRUN hardware |
| #13 AIR UX | Open / partial source | Actual packets, calibration, effect selection and radio ownership; new strict AIR↔BLE mutual exclusion #31 |
| #14 UI restore/LED alignment | Partial / physical unverified | Test controls after fullscreen and pending key LED behavior on real FM-1 |
| #15 Hidden graphs | Partial / targeted verification | v0.6.5 intro zero visualizer-work tests; verify every offscreen graph and fallback |
| #16 Theme/visual preferences | Software scoped typography/themes accepted; other semantics revised | Owner later prohibited normal visualizer in intro. Respect superseding rule. Check saved visual selection and bank semantics |
| **#17 SECOND VISUALIZER** | **CONFIRMED NOT DELIVERED** | Entire **ground-up water-and-sand cymatics redesign** with actual visually settling granules and reflective active water; not contour re-coloring |
| #18 19 muffled drum kits | **Owner feedback OPEN** | Later engine changes may mitigate; validate level-matched audible kit attacks/brightness before deciding extent of fix |
| #19 Showcase + emulator | Live release proof exists | v0.6.5 `LIVE_PUBLICATION.json` records GitHub Pages/site/emulator. Verify current links when release changes; avoid re-creating site |
| #20 SAVE | Software accepted for scoped #27 | v0.6.5 queued SAVE/powercut tests. Physical latency/reboot unverified |
| #21 Theme/fullscreen/UI | Partial / targeted verification | 270 appearance captures in v0.6.5; confirm owner's fullscreen color/occupancy and all white-mode legibility |
| #22 Old v0.6.4 catch-all | **Mixed outcome** | Site animations now reported as four assets in live publication; visualizer part #17 remains **OPEN**; any other latest comments must be cross-checked |
| #23 Standard/Cinematic/Neoclassical + motion | Implemented source features; targeted verification | 3 modes/auto motion appear in source/host evidence; confirm complete 13×3×revisit and glide/vibrato override tests |
| #24 Boot spiral | Software accepted in v0.6.5 | No normal visualizer during intro; real LCD/audible levels owner-unverified |
| #25 Draft PR | Not an Issue | v0.6.5 accepted design supersedes early vertical-bar draft; track actual merged/closed PR separately |
| #26 UI performance/freezes | Software mitigations accepted v0.6.5 | Physical FPS, callback and XRUN still owner-unverified; new visual must preserve budget |
| #27 SAVE hangs | Software accepted v0.6.5 | Physical flash/save timing owner-unverified |
| #28 FM bass patches | Software accepted v0.6.5 | Six distinct patches, 36 total, logged in acceptance |
| #29 Mixer legibility | Software accepted v0.6.5 | Theme/size captures and FX/control layout checks |
| #30 Duplicate controls/SEL-SAVE | Software accepted v0.6.5 | Unique registry path and button-action tests |
| #31 Generic USB/BLE MIDI | NEW, NOT IMPLEMENTED | Direct BLE, iPad USB hub, generic peers, AIR-radio exclusivity; physical owner tests |
| #32 Missing-task prevention | NEW PROCESS REQUIREMENT | Full owner-comment/release matrix, CI provenance gate, zero unexplained missing P0 tasks |

## Rules for next sole Work release owner
1. Read root `NEXT_BUILD_ACTIVE.md` and #32 before code edits; compare every issue **and its latest owner comments** with newest verified source/tests. Reconcile conflicting/superseded wording. No need to repeat already proven v0.6.5 hotfix tests unless affected by changes.
2. Convert this preliminary inventory to a **release-specific source→test→artifact matrix**. Classify software completed, open, partial, blocked and physical owner-unverified separately. Link exact file/function/commit/test or explain missing proof. Do not present this table as completed physical acceptance.
3. Treat #17 and #31 as implementation requirements, not optional document cleanup. Old visualizer performance fix #26 and website APNG #22 **do not** satisfy #17. Benchmark with real 240×240 frame-sequence visuals and actual audio/event response.
4. Any deferred item needs explicit approved destination version and a visible active-queue row. Fail release if P0 is silently dropped. Keep published 0.6.5 artifacts untouched. Push verified GitHub checkpoints, avoid concurrent builds, and leave real FM-1 hardware acceptance to the owner.
