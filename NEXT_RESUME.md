# AQUORA 0.6.9 development — NOT RELEASED
Resume current `aquora-next-integration` checkpoint, preserving v0.6.8. No release claim.
Ownership checked once: no workspace lock or build process; GitHub in-progress workflows empty. Active session 56c15b7cf3b9, lock /workspace/.aquora-release.lock/owner. Original local worktree .git pointed to deleted /workspace/scratch/759587c0312e; it was left untouched. Exact durable branch was confirmed as 8b5627d (main is older). Restored clone /workspace/scratch/56c15b7cf3b9/AQUORA; complete pinned source /workspace/scratch/56c15b7cf3b9/source.

Work in complete source, then copy only edited overlay files into Git checkout and commit/push. `bash tools/prepare_next_source.sh NEW_DIRECTORY` reconstructs base + committed overlay + four platform patches, without starting a build. New headers serenity.h/song_form.h/learn.h must remain tracked.
Read NEXT_BUILD_ACTIVE.md for active status. Current host compilation log /workspace/scratch/56c15b7cf3b9/host069.log; compile is engineering feedback, not acceptance. v6 bank migration and new learn lifecycle require tests; no native 0.6.9/WASM or publication yet.
Preserve passed CI42421822671/42437228836/42438056120 and artifact13600611437; do not repeat unchanged baseline tests. Existing site only: https://parallax8-studio.jminie66.chatgpt.site/install/. Publication must wait for all software gates.
