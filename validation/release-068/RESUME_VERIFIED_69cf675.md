# AQUORA 0.6.8 release verification checkpoint

## Verified without rebuilding
- Source/backup checkpoint: 69cf67588d211597057911e6dba6f05df8bbd465; aquora-next-integration head confirmed before this record.
- Final corrected CI: https://github.com/CodingSlapfu/AQUORA/actions/runs/42438056120 — completed/success.
- Tested source: f0d7f43f39703f3f3e9f776cff58cf6818d71da3. 69cf675 adds listening/archive/scope documents and data.
- Independently downloaded artifact 13600611437 (aquora-068-platform). ZIP SHA256 d586db1e5542af8ad63732982d55b2f199fc25d99f43619ea3caf57045bba31d matches GitHub digest. All 30 SHA256SUMS entries verified.
- Native firmware: 611496 bytes, SHA256 86527d3167c125ee3e9e433b3663f335f102e98b53bb89e00e1fd5091c15a525.
- WASM: 206755 bytes, SHA256 4a52496f2610f056674e058259ff393bad8e59420069988c5e936ac62c58250c.
- Matching source tar.gz: 38673954 bytes, SHA256 481a8a0377c46efadd829a9cfac1132ea6ff9c89589e8dd6231b669b2b48f253.
- Preserve earlier passed runs 42437228836 and 42421822671; no tests/builds rerun.
- One bounded ownership check found no active AQUORA builder or existing lock; temporary release-verification ownership was acquired. No competing build started.

## Exact environment blocker
The Sites publishing skill requires scripts/site-workflow.mjs to open/synchronize source and package the deployment. This runtime exposes the skill text but does not contain that helper in the available plugin locations (searched /opt, /root, /workspace, /home, /mnt). No supported source-helper executable is available.
The previous checkout's .git points at a missing worktree: /workspace/scratch/759587c0312e/AQUORA/.git/worktrees/aquora-resumed. Previous session0c64376c3cd0 source, ci068 and site068 directories are absent in this restored workspace.
Sites get_site succeeded for appgprj_6ad6d0ae409c8191b45224ecdec724db; public audience retained. Fresh source credentials were obtained but are session-only and not recorded.
Publication and staged installer acceptance were NOT completed. All software release gates have NOT been declared complete.

## Resume only remaining work
1. Use an environment with the installed Sites source helper; renew session credential and open the existing project, keeping its public audience. Do not create a replacement site.
2. Reuse final artifact 13600611437 and the hashes above; download again only if local copies are unavailable. No rebuild required.
3. Reconcile requirement/register evidence against OWNER_SCOPE.json and preserved tests; finish any remaining software verification only. Listening comparison archive is backed up at 69cf675; no subjective approval was invented.
4. Restore ignored 0.6.7 production firmware from its preserved installer backup before modifying site. Stage final native firmware and matching emulator WASM/worklet/index. Identity FM-1_73000608; bind metadata to actual firmware SHA above. Do not use legacy package068.py.
5. Verify staged installer/emulator/Studio and source/package/hash backups; publish existing site only when all software gates pass. Verify resulting live installer.
6. Update NEXT_RESUME.md and NEXT_BUILD_ACTIVE.md to reflect actual verified release state, not assumptions.
Preserve 0.6.7, all backups and KIRA-9. USB/BLE performance MIDI deferred; AIR required. Physical device tests remain owner work.

Local verified recovery files (ephemeral): /workspace/scratch/39b5bfc73923/ci, final-ci.zip, source. Durable authority is GitHub and CI artifact.
