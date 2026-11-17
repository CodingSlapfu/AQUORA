# AQUORA v0.7.22 testing recovery checkpoint

Recovered v0.7.21 from ded2006aa2e928fefb32637ce8cb4ef5637986a4. The surviving staged tree was byte-identical to source 217184f. New source e985699061338cf790cbfba6b887f126dbdc813b. Native updater FM-1_73000722. Both builds with the retained pinned JieLi toolchain produced the same 611496-byte FWSC, SHA-256 b77b3680aadacba533b66e2b2650f2eec50b70156f5bdfcfd11b471f296f13ae. Native application 486064 bytes, data + BSS 82524 / 98304, pool 274256 / 344064.

## Owner checklist A–J

| Item | Status and evidence | Open |
|---|---|---|
| A. Visuals | Preserved palette/portrait, retained gaze/blinks and added direct-vocal and beat-driven light on the neck module. Canvas pixel tests and timed word poses pass. | Full vocabulary-dependent phoneme coverage and hardware panel acceptance. |
| B. Vocal controls | Fresh OFF, singer gain independent of choir level, VOCALS K1–K4 unchanged, Play Along and singer-only gating software tested. Prior screen and intention scheduling preserved. | Physical keyboard feel, gain balance and continuity. |
| C. Human recordings | Two recorded lexical units, “day” and candidate “you”. Featured cadences choose dynamically and follow note pitch/duration with a looped vowel and timed mouth shapes. Sources, licenses and hashes in device/docs/VOCAL_CREDITS.md. | Baby, Love, intention names, a larger vocabulary, coherent multiword phrase construction and hardware intelligibility. |
| D. Vocal musicality | Hook recall retained. Seeded chorus answer steps avoid planned kick/manual snare where practical, with optional hat accents. Instrumental pluck responses, vibrato, and glide retained. | Adaptive microtiming, deeper motif evaluation and listening acceptance. |
| E. Vocal FX | Added Spatial page with dedicated octave shimmer, independent flanger rate and feedback, and chorus depth. All 20 extended parameters changed recorded-vocal PCM under their applicable conditions. Existing FX locks, auto profiles and processed baseline retained. | Additional room/delay detail, broader EQ, and physical acceptance across modes and words. |
| F. Instrument FX | Eight roles now feed separately weighted room and echo buses. Section energy and style adjust automatic targets, plucks favor repeats, bass stays drier, and manual track/global locks remain effective. Existing shared modulation tests pass. | Additional independently composed per-role modulation, filter, saturation and dynamics. |
| G. Slicer | Style differentiation, automatic variation, kick guard and independent singer gate affirmed in software. | Hardware click/feel and further compositional adaptation. |
| H. Fullscreen | Four control paths exercised, with no host command drops. | Physical timing, playback continuity and UI feel under load. |
| I. IDM | Preserved four kick anchors, added syncopated bass pickups and synth/vocal accents. Full-mode native structure compiles and host mix renders. | Physical danceability and deeper break/drop/glitch writing. |
| J. Composition | Local phrase and role-production changes. Verified patch, bank, and singer regressions. | The requested full composition-quality pass remains unfinished. |

Additional evidence: 24 host input/render/worker-load scenarios, 30 seconds of a full IDM mix at maximum vocal settings with zero full-scale PCM samples, V9 migration and V10 round trips, MIDI-octave/patch regressions, host LCD page navigation and matching WASM. These checks are software evidence. FM-1 listening, XRUNs, display/encoder timing and palette remain physical acceptance items. Testing only on original Puya 0x856014 hardware. The stable and earlier testing routes remain available. See PUBLICATION.md for verified live assets.
