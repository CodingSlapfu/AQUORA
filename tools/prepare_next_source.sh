#!/usr/bin/env bash
set -euo pipefail
# Recover a complete, isolated source tree from the pinned archive + current git overlay.
# Does not acquire build ownership or start compilation.
repo_root=$(git rev-parse --show-toplevel)
output_dir=${1:?Usage: tools/prepare_next_source.sh NEW_OUTPUT_DIRECTORY}
mkdir "$output_dir"
output_dir=$(cd "$output_dir" && pwd)
(cd "$repo_root/evidence/0.6.0/editor" && sha256sum --check --strict SHA256SUMS.txt)
tar --no-same-owner -xzf "$repo_root/evidence/0.6.0/editor/AQUORA-0.6.0-editor-source.tar.gz" -C "$output_dir"
git -C "$repo_root" archive HEAD | tar --no-same-owner -xf - -C "$output_dir"
cd "$output_dir"
for patch_file in aquora-passive-air.patch aquora-led-duty.patch aquora-canvas-pipeline.patch aquora-host-midi-packets.patch; do
  patch --batch --forward -p1 < "vendor/patches/$patch_file"
done
printf 'Recovered source: %s\n' "$output_dir"
