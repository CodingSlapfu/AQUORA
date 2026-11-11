# AQUORA v0.7.4 candidate build receipt

Native, single-core and WASM builds derive from commit ee53f9b35f2fc42fb47418ed55a8309c55d78fd7. Native identity is FM-1_73000704 and hardware scope is original Puya 0x856014 FM-1.

| Artifact | SHA-256 |
|---|---|
| AQUORA-0.7.4.fwsc | f8533c40a967870f314199730a6158609ceebc967670d66aa7164edfd299b342 |
| AQUORA-0.7.4-single-core.fwsc | d06fa5f6820ffdc9cdf44dd0f1abe6b1f26bb32cd2642822ac01866b8e79837e |
| app.wasm | 533b757bb38f354de4ca06c0781415778ccd7f9a9c4365bd86b0b660a3d30304 |
| Local installer index.html | e767972ae4c77473d56a705478b1880e74687aa60c6940ef6c889e2525731550 |
| AQUORA-0.7.4-testing.zip | ff4deaca69303342ef271687e09b098065206f9c36aa3dba6df8d7ddee1588fe |

The archive contains checksum-verified v0.7.2 and v0.7.3 rollback images, a local installer with SHA-256 verification, A/B listening renders, and full test logs. The production v0.7.3 release was preserved.

Passed byte-for-byte repeat native build, matching WASM compilation, single-core build, 504 composition cases, 84 EVOLVE/audio cases with 4,650 exact events, 16 simulated Web MIDI updater source identities, tamper rejection, 24 HOME/fullscreen HUD screenshots, 300 LED output snapshots, 24 input/load cases, startup centering, and bank/motif/transition checks.

Issue #76 remains unresolved. The official v0.7.2 and v0.7.3 FWSC files have the same marker and UFW layout, valid CRCs, and byte-identical OTA loaders. Windows M-UPGRADE and the physical FM-1 install were not validated here. The existing Site owner access succeeded but the source opening step returned `Unable to prepare the Site.` The live installer remained 0.7.2. See the v0.7.4 requirement matrix for unfinished music and FX scope.