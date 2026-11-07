# AQUORA 0.6.8 resume

Sole current session: 0c64376c3cd0; lock /workspace/.aquora-release.lock/owner.
Git checkout /workspace/scratch/22ea018dd526/aquora-resumed.
Reconstructed full source /workspace/scratch/0c64376c3cd0/source from tools/prepare_next_source.sh.
Baseline integration 335e1fd; published 067 preserved. Read NEXT_BUILD_ACTIVE.md.

Candidate #46 adds input_revision and polls/dispatches between LCD bands; abandons partially drawn stale frame on input without clearing dirty. tests/input_bands_068.py injects a knob after first completed strip; passes all seven pages STOP/PLAY, drains queue on next audio callback and completes redraw. Deterministic 5ms strip fixture is NOT measured physical latency or proof of complete root cause. No new DSP/feature work yet; finish #46 latency/native review before load additions.
Host candidate built successfully with python3 vendor/fm1-base/tools/build.py device --host --release 0.6.8 in reconstructed tree. Test log validation/release-068/input-bands.log. Do not rerun successful unmodified 067 tests. Remaining scope listed in active queue; no publication authorized until gates pass.
Native toolchain retained at /workspace/scratch/759587c0312e/jieli-verified/jieli-linux-toolchains-20260730.1. Reuse it if needed.
