# AQUORA 0.6.1 publication

- Source tag `v0.6.1` and accepted commit `23c8db04538fec13171a6e380501bb153f949311`.
- [Independent acceptance](https://github.com/CodingSlapfu/AQUORA/actions/runs/41969916116) passed sanitized DSP, native, forced single-core, WASM, host panel and staged direct update from 0.6.0. Two independent native builds matched byte for byte.
- [Publication run](https://github.com/CodingSlapfu/AQUORA/actions/runs/41970146909) verified the GitHub tag and all downloaded release asset digests. [GitHub release](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.6.1)
- The initial Site publication retained stale installer/WASM/manual files. Re-staging and checking the Git index and deployment archive corrected the mismatch.
- Site commit `dbb1838261e7208ddcc5136c037ef319b03b3b67`, deployment `appgdep_6aec0c5e965c819184a21aa9ef663f59`. The [public installer](https://parallax8-studio.jminie66.chatgpt.site/install/) serves `FM-1_73000601`.
- The panel evidence archive contains the 240×240 screens and animation previews, six battery states and fullscreen fade captures, LED output checks and WAV comparisons. [Panel evidence](https://github.com/CodingSlapfu/AQUORA/releases/download/v0.6.1/AQUORA-0.6.1-panel-evidence.tar.gz)
- Live rollback installers and firmware were compared with the preserved 0.6.0 and 0.5.0 site assets. [0.6.0 rollback](https://parallax8-studio.jminie66.chatgpt.site/rollback/0.6.0/install/) · [0.5.0 rollback](https://parallax8-studio.jminie66.chatgpt.site/rollback/0.5.0/install/)

| Asset | SHA-256 |
| --- | --- |
| Firmware | `7b9ee88a1be0fe58736ddc89887892a57d7aef674db84c072da6dd3e25dead08` |
| WASM | `ccf0be980820dd8f5a331b4fd9fdf126811a26be94c607fbe39563a274b07cf7` |
| Installer HTML | `f2ed3e60a4c419eb055802b9814c078a74434798d8891d5effc645f7b5c9961a` |
| Manual HTML | `6c38e8e497048f946c11d9ecf02800258f071e31631c7abe5fc02bcc8f36a140` |
| Package archive | `9331490a4943c78699180e246411073ea3b5ebeb6597caf2cd0d65e1a61a8059` |
| Source archive | `7f2d204dda45a3896cb4a75a165a67c2185c30d674e8a2153bd038c6de1f557c` |

## Before and after

Host times are per 256 frames on the same CI runner. DSP and FX timing are
software proxies for FM-1 headroom.

| Metric | 0.6.0 | 0.6.1 |
| --- | ---: | ---: |
| Pitched dry callback p95/p99 | 41.2 / 46.6 µs | 41.0 / 47.5 µs |
| All + wet callback p95/p99 | 52.4 / 57.7 µs | 53.1 / 57.4 µs |
| All + wet audio p95/p99 | 48.2 / 53.6 µs | 48.7 / 52.9 µs |
| All + wet FX p95/p99 | 4.1 / 4.4 µs | 4.2 / 5.4 µs |
| Native image | 191,496 B | 203,560 B |
| Native data + BSS | 40,132 B | 40,372 B |
| Native pool | 72,956 B | 73,260 B |
| Flashable package | 611,496 B | 611,496 B |

HARDWARE-UNVERIFIED includes physical flashing and rollback, battery status,
startup audio and skips, LED output, AIR response, LCD FPS, DMA/XRUN stability,
key feel and listening acceptance. The pinned SDK boot and EQ reference
warnings remain. Host coverage includes control routing and engine-queued
edits. It does not individually instrument the downstream signal path and
storage outcome for every control in every operating combination.
