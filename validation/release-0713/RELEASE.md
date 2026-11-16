# AQUORA v0.7.13 — Water Mixer and arrangement controls

Firmware identity `FM-1_73000713`. Original M-VAVE FM-1 with Puya `0x856014` flash. Software verification covers the updater container and matching emulator. Physical flash, speaker balance, display and sustained real-time performance await owner testing.

- **MIX** shows eight live colored water columns. Liquid height and peak lamps follow each role’s post-mix meter. Percentages under the tanks are the stored channel volumes. Turn SELECT to move across the columns, and press FX for the channel strip.
- **Intention Frequency Match** folds each audible pitch into the octave above the selected intention’s assigned frequency. An octave-equivalent pitch gets full credit, a pure fifth up to 80%, and a pure major third up to 60%. Credit falls linearly to zero at a distance of 1/32 of the anchor frequency from each relationship. The engine’s active pitch and envelope metadata, channel volume, auto trim and pitched reference-bus meters weight the result. Silence shows N/A. Drums and other unpitched sources are excluded. This is a musical alignment indicator.
- **DRUMS** shows all sixteen parts in every view. Solid hits are played steps, hollow hits are upcoming, and the vertical stripe follows playback. In 16-PART MIX, K1 selects the part, K2 mutes, K3 solos, and K4 sets its level.
- **EVOLVE → GROOVE** exposes independent Tempo Mode and Intensity Mode controls. Turn K3 or K4 right to lock the currently selected value for new songs, or left to let new songs choose it automatically. Existing saved locks are honored. REROLL preserves the composition and BPM while changing its sounds.
- Low intensity thins piano answers, pluck responses and flourishes. Higher intensity opens pad voicing, adds piano notes and brings in harmony at climaxes. The lead hook, bass anchors, and manual drum controls remain in place.
- Recorded singers keep their gain. While a recorded phrase plays, synthetic choir and pad levels and their reverb sends ease back, then recover as the singer releases.
- EDIT prompts identify the next page and show the current position. The FX channel strip indicates that EDIT returns to MIX 1/5.

The staged v0.7.12 installer remains available for rollback. Back up banks before flashing.
