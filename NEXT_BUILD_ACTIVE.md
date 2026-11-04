# AQUORA active follow-up

The implemented 0.2.0 software scope is archived in [archive/0.2.0-requirements.md](archive/0.2.0-requirements.md). Verification logs live in evidence/0.2.0/ and artifact hashes in release/0.2.0/. Keep 0.1.0 available for rollback.

## Resolved connection issue (2026-11-04; not an updater defect)

- **User correction:** The installer displayed `No answer. Check that the FM-1 is running sound-healing.` because the FM-1 USB cable had been unplugged. User reconnected USB and reports it is working. **Withdraw prior P0 firmware-updater failure claim and repair work.** Do not spend build time investigating this as a confirmed software regression. Preserve the connection history here only for troubleshooting context.
- **Verification nuance:** Successful connection is reported; don't claim full 0.1.0 → 0.2.0 flash and subsequent device boot or rollback testing as independently confirmed unless documented by the owner.

## Owner hardware verification still open

- [ ] Confirm successful 0.1.0 → 0.2.0 in-place installation and new firmware boot on hardware, plus rollback/recovery separately. The earlier connection error was due to a disconnected USB cable, not a confirmed updater defect.
- [ ] Physical STOP/restart and master-zero safety, balanced and audible bass and drums, all controls, pitch motion, intended sound and headphone separation.
- [ ] Measure real core-1 execution, forced single-core behavior, typical and sustained stress audio/FX callback timing, peak/percentile margin, visual responsiveness and zero xruns against 0.1.0. Follow the hardware checklist in release/0.2.0/README.md.

No device measurements have been inferred from host timing. Add new software work only from verified regressions or a new requirement.
