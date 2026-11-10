# AQUORA 0.6.10 existing-Site publication — stopped on credential expiry

Release checkpoint: 5f6756b258beb0e2914405c69cc473e09e9b0c7a.
Prior diagnostic read: 8da76e77197deb7f30da0f716e6a8a2124a1d7f3.
One bounded ownership check found no project lock or running build/publisher. Shared app/exec servers were not treated as owners.

Existing project appgprj_6ad6d0ae409c8191b45224ecdec724db remained public, latest saved version190. Supported source opening succeeded in this Site conversation at source0d9ac7c428c9dcde5b131dcdeed6d2d9c7ab677f. Opening fetched existing source history; it did not create another Site.

Prepared current0.6.10 homepage, installer, exact accepted emulator/WASM/worklet, Studio bridge, guide/downloads/release pages,14LCD captures rendered from accepted WASM (no compilation), paired original audio and source archives. Preserved0.6.9rollback plus067/068. Recovered ignored files missing in the old Site Git tree using verified069website backup and actual public files; all old069source parts join to accepted adf7f01e9eddefb5d33512f02e925323354abc0bd49ff6304b70d465ffc727cc. Staged all out files explicitly so future source recovery includes them.

Website publication checks passed:316references inventoried, all local paths resolve; all four installer package paths/versions resolve; exact accepted firmware e31fc01b728b51e6e7ea25fc8ae389fcb35a7fb201071aed12502d4098833378, WASM50374bf175072373dd3dbe38e30416649bfe19d4d5eb7d4beb10053c5d9740cc and release ZIP hashes retained; largest file24MiB. No firmware rebuild, WASM regeneration, acceptance-suite rerun or existing GitHub release-file upload.

The single publication workflow stopped before source push/package/save/deploy. Exact helper result:

    Renew the source credential for the same Site before retrying.

Exit code1. Per explicit owner instruction, no credential renewal/retry or second publication attempt was performed. No new saved Site version or deployment was created. Public site remains0.6.9, version190; no claim of live0.6.10 verification.

Prepared local Site checkout:
 /workspace/scratch/1b74ec6feab1/site610/.sites-checkout
Prepared local source commit (not pushed):
 ab2d0741f612a88cf1c673c13c1c6bb439030028
Intended package path (not produced):
 /workspace/scratch/1b74ec6feab1/aquora610-publication.tar

Independent new website-only overlay backup:
 validation/release-0610/WEBSITE_PUBLICATION_READY_0610.zip
931668bytes, SHA256137286c43676fd221c91e7c7d00e69a1e44aba8a62e3ad04282b9dab30e38b5c.
Contains prepared current pages, real current captures, scripts and compatibility metadata; restore against the complete saved Site190 archive, then copy accepted release610 artifacts and paired audio from their existing GitHub backups. It intentionally excludes already-backed-up firmware/WASM/installer/source archive binaries. Do NOT rerun one-time stage610.py over an already610tree. Preserve all local restored assets and prepared source. Existing069fallback/public assets remain available.

A future owner-authorized publication session must obtain a fresh credential immediately before push/package. Do not repeat firmware/software acceptance. Preserve original Puya-only warning, UC25HQ80deferral, AIR, KIRA-9 and rollback. Physical tests remain owner work.
