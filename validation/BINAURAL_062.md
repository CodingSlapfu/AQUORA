# Binaural optimization and retained acceptance limits

Recovered source: 5ee7a22333d23685e7d6742dedecbb021e825208.
Its full platform/staged-installer run 41991259083 passed. Local replays of
appearance_062, controls_060 and mixer_062 also pass, including FONT SIZE and
BACKGROUND persistence. Do not repeat completed earlier milestones.

The optimized binaural steady path caches target, stereo phase increments and
amplitude per internal block. Transitional controls retain sample-rate smoothing
and independent phase. Smoothing snaps only when float arithmetic otherwise
stalls at a representational limit; settled values reach the requested frequency.
No buffers, synth voices or FX crossfeed were added. Disabled binaural remains
behind the existing OFF gate and its fade terminates at exact zero.

`tests/binaural_062.c` exercises actual output PCM at carrier 80/432/1000 Hz and
beat 0.5/6/40 Hz, OFF silence, solo fade, rapid toggles/sweeps, and the exact
oscillator kernel for 476,280,000 frames (three hours). At 1000/0.5 Hz the
three-hour measured L/R averages were 999.750370 / 1000.250278 Hz; difference
0.499907 Hz. Stereo regression rejection was 81.1 / 86.1 dB at 217/223 Hz.
This is a software oscillator soak, not a three-hour physical full-system run.

`benchmark_binaural_062.c` holds musical seed/settings and exercises 16 voices
with drums, Nature and FX through OFF/ON/OFF-after-ON. Repeated host runs have
substantial scheduling variance; they do not establish an AC791N speedup or
zero DMA XRUNs. No hardware percentage improvement is claimed. Physical
post-install/cold-boot callback p95/p99/max and XRUN measurements remain owner
verification, as authorized. The previous severe first-post-install default
playback report must remain in the hardware checklist.

The custom AQUORA Sans bitmap generator is a separately testable offline
Inter Tight derivative with its OFL source/license, tabular figures and explicit
0/I/l differentiation. Host build, appearance/mixer/control tests and full
corrective startup/battery/LED previews pass with the new font. Final native,
single-core and WASM acceptance must target the commit containing these changes.
