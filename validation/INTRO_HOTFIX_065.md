# AQUORA v0.6.5 — intro-animation hotfix source checkpoint

Base: published v0.6.4 firmware/tag `50b8a1e02e215aff1e0a7b738d40f63769d4f502`; project source branch `aquora-0.6.4-development` at `5962e9bd456d71c6690d83352e782034bd0890cb`. This topic branch deliberately does **not** alter v0.6.4 binaries or live installer.

Confirmed v0.6.4 defect: startup UI became completely static after an overly broad "no visualizer during intro" change. This branch restores a **distinct AQUORA-branded animated intro** using fixed-size precomputed 21-stroke upper/lower pulse motifs and a lightweight 8-second progress line, while the wordmark remains still and the actual chosen visualizers stay hidden. No DSP engine, FX, song, bank or startup audio code changes.

Targeted host regressions: `tests/corrective_previews.py` now requires startup frames visibly change in both decorative bands, stable brand text and zero normal visualizer geometry/worker/history jobs. Maintain all 49 skip tests, 8-second timeout and master-mute tests.

**Status: source candidate, not independently compiled/flashed/released.** Validate through GitHub Actions, compare LCD animation under both black/white UI, confirm no XRUN/regression, then publish as a matching **v0.6.5** firmware/installer and site only when all gates pass. Physical FM-1 acceptance remains with owner. Check exclusive build ownership before continuing. Issue: #24.

## Owner hardware performance and intro-background update

Owner supplied actual v0.6.4 LCD photos showing audio lag and freezing visualizers: callback mean 5186 of 5805 µs, P95/P99 5400, geometry 6156 µs, UI draw 58266 µs, 17.4 FPS observed and 1 worker fallback. **Separate P0 performance blocker: Issue #26.** This topic branch changes ONLY the intro draw path, not the nodal/Resonance Plate renderer; **DO NOT MERGE, RELEASE, OR CLAIM THIS HOTFIX COMPLETE** until #26's animation freeze and audio latency are investigated, fixed, and verified through valid native and host performance acceptance. The existing inherited 0.6.4 PR CI cannot reuse unchanged pre-built firmware after UI source modifications; recompile matching artifacts when authorized.

Owner additionally requested vivid colors for the formerly empty intro. Intro rendering now fills that negative space using one of five precomputed, saturated flat RGB565 backgrounds, with dark-ink text, 42 bounded pulse strokes, and dark intro progress; selected Black/White UI background is restored on entering PLAY. These simple colors are introduced as *unverified source changes*, with host screenshot assertions for visible color and stable wordmark.
