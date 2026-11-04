# AQUORA active follow-up

The implemented 0.2.0 software scope is archived in [archive/0.2.0-requirements.md](archive/0.2.0-requirements.md). Verification logs live in evidence/0.2.0/ and artifact hashes in release/0.2.0/. Keep 0.1.0 available for rollback.

## Owner hardware verification still open

- [ ] Direct 0.1.0 → 0.2.0 flash, updater re-enumeration and rollback/recovery.
- [ ] Physical STOP/restart and master-zero safety, balanced and audible bass and drums, all controls, pitch motion, intended sound and headphone separation.
- [ ] Measure real core-1 execution, forced single-core behavior, typical and sustained stress audio/FX callback timing, peak/percentile margin, visual responsiveness and zero xruns against 0.1.0. Follow the hardware checklist in release/0.2.0/README.md.

No device measurements have been inferred from host timing. Add new software work only from verified regressions or a new requirement.
