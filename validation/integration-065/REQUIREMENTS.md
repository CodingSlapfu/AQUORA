# AQUORA 0.6.5 acceptance map

Scope: latest reviewed Issues #2, #16, #24, #26–#30 and draft PR #25.
Source: 45c02ebb17e93684092227b16378277bbe3841d3. Prior v0.6.4 remains intact.

| Requirement | Implementation | Software evidence |
|---|---|---|
| #26 audio/visual overload and freezes | Bounded 17×17 standing-wave field, 256-contour cap, band-clipped lines, adaptive finite cadence; cached legacy harmonics | 48 real renderer pressure/fallback cases; matched-palette PCM parity; three paired DSP benchmarks; three-mode saturated load |
| #27 reliable SAVE | Finite queue fence, captured bank, immutable active record, queued requests, flash priority, bounded worker retirement, timeout/error, readback, diagnostic views | Four-bank exact reboot recall; command-stream/slow/fail/stall cases; 400 power-cut boundaries; CRC rollback; no false Saved |
| #28 FM bass expansion | Six appended patches (30–35), distinct two-operator profiles; mode/intention pools, locks and anti-repeat | Six distinct bounded PCM renders and STOP; all-three-mode/intention/loop transitions; legacy IDs unchanged |
| #24 intro / PR25 | Independent multicolor rotating spiral, vibrant theme fields, AQUORA branding, skippable FM chime; no normal visualizer at startup | 30 theme/background/font intro comparisons; 49 skip inputs; zero intro geometry/worker/history jobs |
| #16 AQUORA Sans / typography | Required generated custom atlas; binary family/hash fingerprint; bold hierarchy; precomputed italic for expressive subtitles | Exact native/single-core/WASM/host binary font provenance; no runtime slant; 270 appearance captures plus focused matrix |
| #16 colorful White | Vivid aqua/lilac/gold/pink panels, white canvas, dark ink | Five skins × three sizes × two canvases; focused actual MIX and Sound captures |
| #2 / #29 mixer | Distinct MASTER FX columns; visible knob panel; Auto Gain K3 / Auto Level K4; dark contrast-safe text | All themes/sizes, stopped/playing, strip/overview, mute/solo, gain/level states |
| #30 navigation / duplicate controls | EDIT n/N NEXT SECTION hint; one canonical editable route per parameter; MIX/bus home links; Tempo/Freeze/Loop and Variation have distinct homes | Runtime route uniqueness and registry edit/reachability; current control-path regressions |
| #30 SEL/SAVE | SEL confirms armed New Song/Reroll/Reset; SAVE only banks; navigation cancels armed commands | Real button arm/SAVE/SEL/cancel tests; motif preserved for Reroll; no accidental destructive confirmation |
| Firmware / installer consistency | Product FM-1_73000605; exact native/repro equality; retained single-core fallback; matching accepted WASM | Native/single-core protocol acceptance; direct-update state machine; source and artifact SHA256 manifests |

Host measurements and modeled flash interrupt slices are not physical FM-1 timings.
Physical LCD FPS, DMA/XRUN behavior, battery/flash timing, listening and flashing remain
owner-unverified. Publication is gated on the complete independent software run;
failed historic runs are retained as provenance, not represented as passing runs.
