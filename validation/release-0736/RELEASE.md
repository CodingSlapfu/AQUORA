# AQUORA v0.7.36 TESTING

Firmware identity `FM-1_73000736`. This testing candidate extends the verified v0.7.35 checkpoint. Stable and prior testing installers remain available.

KIRA-9 now fits automatic words within MIDI 62–77 and checks the interval from the preceding vocal tone. A word that cannot fit the chord, register, pitch ratio, and time slot gives way to a vowel. Compact phrases can open with HELLO. This is a pitched human spoken cue by Valérie Vivegnis under CC BY 4.0. Its source, processing, sample hashes and attribution are in the vocabulary manifest and vocal credits. The manual WORDS slot is appended, so previous sample IDs and saved bank formats are preserved.

## Technical checks

| Full mixes, 15 matched seeds and intentions at 72 BPM | v0.7.35 | v0.7.36 |
|---|---:|---:|
| Recorded-word starts | 58 | 42 |
| Consecutive sampled starts more than five semitones apart | 14 | 7 |
| Sampled starts outside MIDI 62–77 | 31 | 0 |
| Clipped samples | 0 | 0 |

Two runs accounted for 36 of the 42 word starts. Metrics show sample allocations and pitch transitions. They cannot establish intelligibility or musical quality. The renderer is in the v0.7.35 validation directory. The matching dry and full renders for an example are included here.

The 14 added words passed the one-shot tests at three pitches, with audible-level signal through their ending, direct-signal lamp activity, and reachability in manual WORDS and AUTO phrase choices. The native application uses 573,384 / 581,564 flash bytes, 81,772 / 98,304 static RAM bytes, and 262,472 / 344,064 DSP pool bytes. WASM rendered 512,000 audio frames with a changing 240×240 panel. The v0.7.34 drum, bank and control-path evidence still applies to those unchanged code paths.

## Source and reference review

- Valérie Vivegnis's self-recorded [Hello](https://freesound.org/people/Valerie-Vivegnis/sounds/759142/) was downloaded and verified against the pinned preview hash. It is the one added recording.
- [tim.kahn's Hello](https://freesound.org/people/tim.kahn/sounds/99471/) and [digifishmusic's Female Hello](https://freesound.org/people/digifishmusic/sounds/54922/) are credited speaking alternatives for the same word. The selected rendition is self-recorded and a single utterance.
- The preview for [buggly's Dreams Are Breezy](https://freesound.org/people/buggly/sounds/752369/) was downloaded. No individual words were verified for integration. The NUS NHSS license limits the English sung dataset to internal noncommercial research.
- The approved singer references remain a guide for original phrasing, including soft entries and builds, meaningful rests, and discrete cadences. The instrumental references remain a separate guide for interlocking rhythms, recurring hooks, and harmonic atmosphere. Playback assessment of those tracks and comparative listening are pending. No artist audio is in the firmware or emulator.

## FM-1 acceptance

Back up your device before updating. Use the existing stable installer or the prior v0.7.33 testing route for rollback. After flashing, verify the displayed version, load an existing bank, and check transport and knob response during playback. Audition WORDS and AUTO in several intentions, listen for HELLO, diction, phrase contour, groove and timing, and watch for lip sync, singing-only lamps and any audio dropouts. Physical acceptance is pending.
