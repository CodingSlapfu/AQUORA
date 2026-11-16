# AQUORA v0.7.14 recovery checkpoint

Resumed from 584345a3f0a72290f18da9af92aa4e45148b8f9c after Work command execution recovered.

- Drum HIT test passes at 0, 25, 50, 75 and 100 percent. All 16 lanes occur at the default 25 percent. FX auditions the selected part on the 16-part mixer.
- The VOCALS effects page controls independent chorus, room, echo and note-on scale pitch correction. Impulse, dry bypass and tuning tests pass. The recorded vocal regression covers all five modes with fresh default settings and verifies playback, disable/release and independence from legacy choir patch locks.
- Bank compatibility regression passed. Native build and byte-for-byte reproduction passed for FM-1_73000714. Checkpoint firmware SHA-256 is e23b5d509a9f0df066655ae7d16ffc9540a99ae2020903232286cd6a80338071.
- Pending: expanded ambient mixer effects, full drum pattern step editing and kit sound differentiation, end-to-end vocal synchronization and performance acceptance, final transport and browser installer gates, GitHub release and deployment. v0.7.14 is unpublished.

Transport fixture results: duplicate key, watchdog, Sacred Bass release, and 32 saturated stop/restart cycles passed. The legacy fixed reference isolation assertion at tests/transport_hit.c:90 failed on both 584345a and this checkpoint. Its cause still needs resolution before release.
