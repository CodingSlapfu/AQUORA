# Original visualizer baseline

The authoritative pre-reduction geometry is commit
`128662b` in `device/visual_worker.c::visual_job`: 33 vertices / 32 segments
for each of eight role curves. Commit `7416185` reduced these to 17 vertices /
16 segments. Radius, harmonic shape, event phase and meter pulse equations
were retained. The next development source restores the original 33-vertex
sampling, with the same equations and band-clipped rasterizer.

This restores source-level geometry fidelity, not an assertion of physical
LCD appearance or performance acceptance. Required sequence comparisons,
per-page native measurements and owner acceptance remain open under #33.
