# 0.6.2 checkpoint reconciliation

Primary: e281f618d0bba51d8e8972720acaf815511aa9fa.
Recovery: c056115889101bef3cf3e092bd2437ad2b1be6c0.
Both remote branches are preserved. Integration starts at primary.

Only the inspected binaural helper/output diff was selected from recovery.
The exact-settling steady path eliminates per-sample smoothing and tuning math
once targets settle. Transition fades retain the original per-sample one-poles.
The primary stereo/fade/frequency-lock test remains as binaural_stereo_062.c;
recovery kernel/corner/sweep/solo/three-hour test remains binaural_062.c.
Both benchmarks are retained. No physical performance gain is claimed.
Independent font source, generator, license and fm1.toml gen entry are retained.
Previous uncommitted checkout was left untouched.
