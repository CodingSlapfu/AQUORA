# Resume AQUORA 0.6.4

Branch aquora-0.6.4-development. Check exclusive ownership before editing.
Parent v063 checkpoint 2e7049fb; accepted v063 firmware 0f2716ba / Actions42022969585.
Read NEXT_BUILD_ACTIVE.md. Issues22/23 and latest ground-up cymatics directive are scope.
Implementation in current checkpoint; 1404 music cases and UI captures passed locally.
New field is Resonance Plate; no startup geometry. Four genuine APNGs + three PCM previews
are reproducible with tests/capture_ui_064.py. CI workflow aquora-064-validation.yml
runs independent compilation/acceptance. Inspect latest run before repeating tests.
Complete remaining queue through verified publication; no further owner permission needed.

Independent run42085609310: host-panel, WASM, native, native-repro and single-core
passed on2eaea586. DSP stopped at obsolete fixed-kit assertion in next_acceptance.
Corrected assertion checks valid kit; mode suite enforces freshness. Subsequent
workflow reuses those exact five build artifacts only after source diff/hash checks.
Do not recompile them unless firmware inputs change.
