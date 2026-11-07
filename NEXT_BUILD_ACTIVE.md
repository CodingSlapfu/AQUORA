# AQUORA 0.6.8 corrective release — active queue

Owner scope: Issue #32 and latest owner requirements #45–#54, 2026-11-07.
Baseline: integration 335e1fd; preserve v0.6.6/v0.6.7 and accepted gates.

- PARTIAL #46: dispatch controls between LCD bands; abort stale frame after input. 448 STOP/PLAY strip-arrival fixtures pass (all seven encoders and HOME across seven pages, four strip boundaries, audio preemption). Native dual/single-core compile passes. Hardware scan/IRQ, queue/state/LCD p50/p95/p99 and native/single-core acceptance still open. Fix/verify before new playback processing.
- PARTIAL #45: restored 066 Cymatics color values; soft-ceiling gain and 2.5x onset displacement. Original 77,616 vertices exactly match 067. Chladni kernel passes. Full side-by-side/fallback/zero-master and perceptual acceptance remain. Remaining: restore 066 Cymatics aesthetics, stronger source-driven Cymatics/Chladni response; unchanged Original and master-zero behavior.
- PARTIAL #47/#48: hush retains quiet bass/drone targets; 189 profiles/567 form scenarios pass. Post-gain dead-zone timelines, original listening comparisons and expanded form remain. Remaining: musical continuity, deliberate multiscale form, original melodic ambient electronica and stem/listening evidence.
- PARTIAL #49: actual event markers enlarged from 3x1 to 3x3; six theme/visual renderer fixtures pass. Dense/sparse/font-size matrix and final contrast review remain.
- PARTIAL #50 / OPEN #52: AUTO motion durations shortened; connected neighboring phrase answers only, delayed sustained-note vibrato, MANUAL preserved and bank AUTO refreshed. 63 expression profiles pass; PCM A/B and motion/bank regression pending. Remaining: restrained context-aware AUTO glide/vibrato, chord/strum implementation now present with exact timing and 20 migration fixtures; manual/Freeze/voice-budget/key/long-PCM validation pending.
- PARTIAL #51/#53: optional rhythm/transition controls, bounded scheduled fills and progressive multi-bar bloom implemented; three PCM rhythm contrasts pass. 19-kit attack, long form/stems/headroom and listening acceptance remain. Original scope: optional Auto/Experimental IDM/Ambient Broken rhythms, clear 19-kit kicks, subtle coherent rolls/builds/breaks/returns, persistence and audible evidence.
- PARTIAL #54/#13: pulse detector, queued evolution/fill/harmony, continuous swell and FX Tone/Pulse implemented, calibration prefs and live UI. Core and UI fixtures pass; last small calibration/drum FX edits need verification. RF reliability remains owner-unverified. Audio A/B, targeting, telemetry and persistence acceptance remain. Original scope: audible selective AIR effects; calibrated 1D gestures with cooldown, queued evolution/fill/harmony/swells, truthful UI and safe persistent mappings.
- OPEN #32: complete targeted software acceptance, native/repro/single-core/WASM/updater/migration, source/artifact backups, publication and live verification only after all blockers pass.
- DEFERRED: USB/BLE performance MIDI. AIR required. KIRA-9 untouched. Physical acceptance owner-unverified.

Exact resume: NEXT_RESUME.md. Never treat host fixture times as measured FM-1 latency.
