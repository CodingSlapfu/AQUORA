# AQUORA v0.7.35 recovery checkpoint

Candidate source descends from `02f1dce83e34014e5916ae9bdc715ca7b8f6e084`. The recovery incorporated the unfinished AUTO recorded-word work from the interrupted checkout. It calculates the opening's available time against the scheduled answer and keeps the full allocated time for words at cadences. Words are placed on nearby chord tones within a bounded sample pitch ratio when they fit. Existing banks, sample IDs, percussion code, and saving code were carried forward.

## Verification

`render.c` extends the original matched 72 BPM comparison to 14,400 × 256 frames per run, about 83.6 seconds. Compile with `cc -O2 -ffp-contract=off -I PATH_TO_DSP render.c -lm` and pass an output directory. There are 15 full mixes and 15 dry voice runs per version across five modes and three matched seed/intention pairs. The checked-in audio is one dry/full pair from the candidate. The CSVs cover all runs. Recorded-word starts count allocations.

| Full-mix metric | v0.7.34 | v0.7.35 candidate |
|---|---:|---:|
| Recorded-word starts | 14 | 58 |
| Full mixes with a recorded-word start | 1 of 15 | 11 of 15 |
| Consecutive starts more than five semitones apart | 6 | 14 |
| Clipped samples | 0 | 0 |

Two candidate runs contributed 48 of the 58 word starts. Diction, perceived emotion, register transitions, and hook quality require listening verification.

The native firmware compiled as FM-1_73000735. Application flash is 570,808 / 581,564 bytes, static RAM is 81,772 / 98,304 bytes, and the DSP pool uses 262,472 / 344,064 bytes. The matching WASM passed a 512,000-frame smoke test with a changing 240×240 panel. Validate the artifact hashes with `sha256sum -c SHA256SUMS` in this directory. The successful v0.7.34 drum, bank and portrait gates are recorded in the preceding checkpoint.

## Outstanding release checks

The artist-by-artist listening comparison including scat, separate IDM reference listening, and audible assessment of the candidate remain open. The thirteen credited existing additions are retained. No additional recording met the combined provenance, redistribution and audition requirements in this run. The NUS NHSS database conditions restrict use to internal noncommercial research. Physical FM-1 acceptance also remains open.

This is an unpublished candidate. Issue #91 makes the listening checks part of the release gate. The existing installer, stable firmware and rollback versions were left in place.
