# AQUORA 0.7.0 next-build testing

Exclusive build owner: aaa451777333. Public 0.6.10 and musical correction 7395c75 preserved.

Source: aquora-0.7.0-development. Initial backup d3f07f7; first CI source a840953. Final source/evidence checkpoint recorded in completion manifest. Never install an older intermediate 0.7.0 binary solely because it has the same identity.

New original implementation: bounded 32-section lookahead with harmonic/common-role/energy/style weighting and planned reductions/re-entry/return; actual shared kick/bass grammar; capped section-local density without global knob mutation; auto mixing ON on cold initialization and explicit mix reset, intentional OFF saved/restored; max drum level/density/presence on cold default settings; 12 append-only FM patch IDs36–47; EVOLVE 144px music display plus role detail, actual timed note/duration/pitch and actual percussion instruments, committed scheduled events, fixed playhead, brief glint and atomic NOW/NEXT snapshot. Existing motif development, Sonic Candy, ostinato/frisson, Learn, strum, AIR and all three visualizer algorithms retained.

Targeted evidence: arranger.log, features.log, musical.log, load.log, graphics.log/graphics/SUMMARY.json, native.log, wasm.log, updater.log. Host timing is not FM-1 hardware timing. Sanitizers exclude LeakSanitizer because this runtime is ptraced; engine adds no dynamic allocation. Original068 strum fixture failed its whole-engine voice-count assumption when autonomous sequencer notes entered; performance_features_070 isolates scheduled chord timing using external clock, retains all sample-exact assertions.

Site publication currently blocked: get_site confirms existing public Site appgprj_6ad6d0ae409c8191b45224ecdec724db at version192. Source helper returned "Unable to prepare the Site." First attempt used a nonempty task directory. One corrected attempt from empty /workspace/sites/aquora-next-testing failed identically before creating a checkout. No more access/credential retries without concrete diagnosis. No public source/assets changed.

Standing deferrals: performance USB/BLE MIDI (#31), UC25HQ80 0xB36014 (#62). Puya 0x856014 only. Owner physical flash/rollback, prolonged timing/FPS/XRUN/RF, listening and aesthetic acceptance remain pending. No recordings generated/encoded/uploaded. No KIRA-9 changes.

Remaining: complete final-source independent CI and artifact hash verification; save flashable package; publish testing installer only when existing Site source can be opened. Do not promote public release.
