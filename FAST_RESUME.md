# AQUORA fast resume

Production is v0.6.1, accepted source `23c8db04538fec13171a6e380501bb153f949311`.
Read [publication evidence](release/0.6.1/PUBLICATION.md) and
[NEXT_BUILD_ACTIVE.md](NEXT_BUILD_ACTIVE.md) before changing code.
The corrective source is tagged. The publication and evidence commits live on
`aquora-0.6.1-release`.

For reproducible builds, use the GitHub release source archive or restore
verified vendor inputs from the v0.6.0 recovery archive, overlay the corrective
tracked files and apply `vendor/patches/aquora-passive-air.patch`. The
validation workflow contains the exact CI commands, pinned toolchain digest
and all software gates. Leave the abandoned Work lock alone. Retain
both rollback installers.
