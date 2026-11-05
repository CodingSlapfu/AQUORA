# AQUORA 0.6.4 — implementation backed up, acceptance in progress

Branch aquora-0.6.4-development. Follow FAST_RESUME.md and NEXT_BUILD_ACTIVE.md.
Firmware inputs from2eaea586 passed all five platform jobs in42085609310.
Follow-up8b60a664 continues new music/load/migration and installer packaging gates,
reusing successful builds and all14 successful DSP suites. No firmware rebuild
is warranted for test-fixture/compiler-warning-only corrections.

v0.6.3 remains live and untouched until complete accepted v0.6.4 publication.
Exclusive lock /workspace/.aquora-release.lock owned by current noninteractive
session56959; do not start a concurrent writer. If this environment is lost,
check remote Actions and newest checkpoint before reacquiring ownership.

Outstanding: final software gate; publish accepted GitHub release; update source
branch aquora-showcase-site; verify native Pages and live Sites installer.
Sites project appgprj_6ad6d0ae409c8191b45224ecdec724db, existing version180.
Use existing site checkout; verify committed and packaged asset hashes, with
`git add --renormalize out` to avoid preserved-timestamp cache omissions.
