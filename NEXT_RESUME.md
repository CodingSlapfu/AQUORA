# AQUORA 0.6.10 — development, NOT accepted or published

Baseline c9813170bdc4fab29e46ed6beb23de6c27f879e3; accepted 0.6.9 stays public.
Current owner session9e3e15408fe4, lock /workspace/.aquora-release.lock/owner, Git checkout /workspace/scratch/9e3e15408fe4/AQUORA, reconstructed source /workspace/scratch/9e3e15408fe4/source. Do not overlap an active owner.
Read NEXT_BUILD_ACTIVE.md and validation/release-0610/OWNER_SCOPE.json. Latest #71 overrides earlier no-visualizer-changes wording. Initial implementation compiles host; no release acceptance claim.
Recover complete source via tools/prepare_next_source.sh once when necessary. Native toolchain pinned archive SHA256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd; local /workspace/scratch/9e3e15408fe4/jieli/jieli-linux-toolchains-20260730.1. Matching 0.6.9 host and visual kernel retained for equal-seed A/B in parent scratch; recreate from baseline if absent.
Do not repeat unchanged accepted 0.6.9 tests. Run new composition_0610.c plus affected acceptance, native/repro/single/WASM/update, visual/source backup gates. Complete all scope before publishing existing site. Hardware62/performance MIDI deferred; AIR/rollback/KIRA-9 preserved.
