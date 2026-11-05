# Independent AQUORA 0.6.5 software evidence

Accepted source45c02ebb17e93684092227b16378277bbe3841d3.
Final gate run: https://github.com/CodingSlapfu/AQUORA/actions/runs/42128145960

Successful platform compilation/protocol/font provenance:
- Native, native-repro, single-core:42126704211 jobs127592616130/313/280.
- WASM:42126040098 job127590393567; browser inputs unchanged afterward.
- Expanded host UI/SAVE/appearance/performance/captures:42126704211 job127592616323
  completed those gates successfully before a stale separate navigation fixture failed.
- Corrected navigation regression and unreached host benchmarks:42127815685
  job127596418013. Exact executable reused, unchanged compilation inputs enforced.
- Final run revalidates exact platform artifact hashes and diffs, independently runs
  remaining sanitized DSP/migration/PCM gates, and assembles the matching package.

Nine earlier successful DSP suites are preserved through logs plus exact input diffs;
remaining eight sanitized suites run in the final gate. Corrective palette coverage
uses64-bit masks and includes all twelve FM operator patches across all three modes.
No firmware implementation changed for those test fixture corrections.

Measured legacy PCM parity:39 same-seed/intention/mode comparisons, SNR82.30dB,
maximum difference1/32768; clocks/RNG unchanged with a matched legacy bass patch.
Six new bass choices intentionally change new generated palettes.

Software acceptance includes48 real visualizer pressure/fallback cases,270 appearance
captures,30 focused typography/theme/MIX/spiral configurations, real SEL/SAVE actions,
four-bank reboot recall, fault/stall/slow save responses,400 modeled flash cut points,
CRC rollback,1404 music realizations, saturated16-voice/FX/Nature/binaural load,
and six distinct new bass PCM/STOP checks. Release packaging additionally requires
native/repro byte equality and direct updater identity transitions, including065→065.

These are software results. Host callback times and mocked flash slices do not establish
physical FM-1 FPS, callback percentiles, XRUN count, flash latency, listening or flashing.
