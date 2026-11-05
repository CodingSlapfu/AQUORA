# AQUORA 0.6.1 resume

Branch `aquora-0.6.1-corrective`. Fetch and start from its tip in an isolated checkout.
The verified base is tag `v0.6.0` at `cc7d663b8d74de306fea91ae1daaf2d34f51bd3a`.
The last full passing build before the final UI pass was
[`9fa1f56`](https://github.com/CodingSlapfu/AQUORA/commit/9fa1f56b072cfc1321fbd6d547306bfffbbde44d)
in [run 41968298722](https://github.com/CodingSlapfu/AQUORA/actions/runs/41968298722).
It passed both native modes, WASM, host and sanitized DSP, and staged direct update.
Check the latest branch run separately. A passing workflow proves only the tests it runs.

- CI restores vendor inputs from the checked `evidence/0.6.0/editor` source archive, overlays the current tracked source, and applies `vendor/patches/aquora-passive-air.patch`. For a local host replay, do that in an isolated directory and run `python3 vendor/fm1-base/tools/build.py device --release 0.6.1 --host`. Native release acceptance runs on GitHub Actions.
- Run `python3 tests/corrective_previews.py device/build/host/sound-healing_host /absolute/output` for battery/intro and 240×240 panel previews. See `NEXT_BUILD_ACTIVE.md` for gaps.
- Source and artifacts from 0.6.0 and the previous queue remain in the existing Git history. Leave the abandoned Work lock alone. Publish once, only after the issue acceptance and installer gates.
