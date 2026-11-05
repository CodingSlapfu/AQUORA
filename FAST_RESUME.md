# AQUORA 0.6.2 — resume this integration

Branch: aquora-0.6.2-release-integration. Verified remote integration parent:
128662b4e32a1e5d534c24b1f64026f42f03fa39, retains primary e281f618 and recovery
c056115 as parents. Use the newest verified tip of the integration branch.

Check active writers and acquire /workspace/.aquora-release.lock nonblocking
before edits/build/publication. Current owner uses /workspace/scratch/eccf36adefd2/aquora
and isolated vendor/build tree /workspace/scratch/eccf36adefd2/build. Never
modify the previous checkout /workspace/scratch/8dbedffa0b9d/aquora.

Read NEXT_BUILD_ACTIVE.md and validation/integration-062/RECONCILIATION.md.
Both binaural tests remain: binaural_062.c includes engine.c and must compile
alone; binaural_stereo_062.c links engine.c normally. Run long kernel mode
with `long`. Font generator uses checked-in OFL Inter Tight source offline.

Git CLI reads work but push lacks credentials. Use GitHub plugin Git-data
trees/commits and update_ref with expected_sha; verify remote by fetching and
comparing exact trees. Preserve both original remote branches. Do not blindly
merge recovery or rewrite the development branch. Never publish until gates pass.

CI .github/workflows/aquora-062-validation.yml supports the integration branch;
a changed validation/trigger-062.txt starts the isolated complete matrix.
Host artifacts/results are under /workspace/scratch/eccf36adefd2/results.
Site project appgprj_6ad6d0ae409c8191b45224ecdec724db is the existing public
parallax8-studio.jminie66.chatgpt.site installer. Source checkout is
/workspace/scratch/eccf36adefd2/site; hosting static directory is out.
Use renewed Sites credentials via stdin; never store them. Site source
opening requires git init in an otherwise empty local directory in this runtime.

Next: complete matrix, audit results, back up source and artifacts, package
matching 0.6.2, publish and verify installer firmware identity FM-1_73000602
and exact bytes. Physical FM-1 checks are the owner's responsibility.
