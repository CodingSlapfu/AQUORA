# AQUORA 0.6.5 checkpoint

Exclusive release lock: /workspace/.aquora-release.lock, live owner session42993.
Latest implementation and test source:45c02ebb17e93684092227b16378277bbe3841d3.
Independent acceptance:https://github.com/CodingSlapfu/AQUORA/actions/runs/42128145960.
Native/repro/single-core and complete host-panel acceptance already succeeded and
are reused with source-diff guards and exact SHA256 verification; WASM unchanged
and similarly verified. Remaining sanitized DSP, migration, parity/A-B and package
are running. Do not restart development or rebuild these unchanged targets.

After all gates pass: publish exact accepted tag using aquora-065-publish workflow;
publish prepared showcase with matching package/captures, then deploy existing
Sites installer appgprj_6ad6d0ae409c8191b45224ecdec724db and verify live.
Current public stable is0.6.4 until that process succeeds. Physical FM-1 tests owner.
