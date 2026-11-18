# AQUORA v0.7.25 testing — Issue #86

Branch aquora-v0.7.22-corrective-development. Device sources compiled from implementation checkpoint 483224ad77c4c07106f21bc8d5b25cd65127b13a; subsequent changes refine test fixtures and evidence only. See ISSUE86.md and ACCEPTANCE.md.

All software gates passed: new 110-case direct-vocal PCM/550-profile director tests, 1200 setting-change windows, 18 articulations and hook memory, 20 DSP PCM responses, keyboard routing, featured voice gain/slicer, 24 host performance/fallback cases, prior/same-version updater simulation, native byte reproducibility and matching WASM boot/render. Static native resources: image 537912, data+BSS 82540/98304, pool 318692/344064 bytes.

Native FM-1_73000725 SHA-256: 96b6834b63d35a48b63077fd90fc091cff383b32e60ef8185365d92c006d0a22.
WASM SHA-256: f19c0ccd8f4e7275c0a6bcbd3e182bebf04c0be50ca24fc6e35c3578ae5a67b8.

Remaining publication task: deploy additive testing/0.7.25 installer/emulator to the existing Site, verify public hashes, write PUBLICATION.md and release owned publication lease. Native firmware and matching WASM are backed up with this checkpoint. Do not rebuild them without source changes.

v0.7.24 publication was already complete and verified; see release-0724/PUBLICATION.md. Do not overwrite it, v0.7.22 testing or v0.7.14 stable. No new stable release before owner physical FM-1 acceptance. Outstanding hardware/listening and recording limitations are itemized in ACCEPTANCE.md.
