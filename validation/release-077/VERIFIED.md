# v0.7.7 software verification

Source commit `4033a95918802b514e5ad7b35ec37f19410f7933`, tree `81602815ada3a918562bcf876baaceeabce0b89e`. Released baseline v0.7.6 at `a35c667f2f4f432457501f054e84c840fbce1043`. The host, two independent dual-core native builds, single-core native build, and Emscripten WASM build used the same firmware sources. The dual-core packages are byte-identical. Node compiled the WASM module. Native identity is `FM-1_73000707` for original Puya `0x856014` FM-1 units. Native RAM, pool, flash, hot-path and arithmetic checks passed.

- 84 intention/mode combinations passed A → B → A, intentional reselect, deterministic replay, sound lock, safety cap and timbre reroll checks.
- 504 composition configurations and 7,272 cross-boundary transition cases passed. The score preview matched 4,809 rendered event timestamps, pitches and durations across all 84 intention/mode pairs.
- AUTO LEVEL on and off produced identical score scheduling in 84 stress pairs. Stable calibrated trim did not track individual notes or rests. Test PCM peaked at 19,935 of 32,767 with wet FX, Nature, binaural and high drum density active. AUTO GAIN uses a fixed master lift and the peak limiter remains shared.
- Four saved banks, V7/V6 migration, ARRANGE, intensity, motif memory and musical response checks passed. Older patterns retain their saved data and use the new accompaniment rules.
- Exact 240×240 screenshots cover all pages and three text settings. Fullscreen and normal Original, Cymatics and Chladni interaction traces, a centered startup title, moving EVOLVE, and 24 worker/load scenarios passed. Device audio and LCD timing must be measured on hardware. Under concurrent host runs one callback had a 34 ms outlier. The isolated run had a maximum of 2.72 ms and p99 under 0.78 ms.
- The staged installer accepted the exact candidate and rejected a one-byte change. Mock Web MIDI checked the v0.7.6 → v0.7.7 → v0.7.6 paths using the packaged identities. The FWSC layout and OTA loader payload match v0.7.6. Software simulated the transfer. The physical transfer is pending.
- The listening package includes level-matched host audio and role isolation. Owner approval of the musical result, physical flash and rollback, display and on-device performance remain open.

The existing official release and public Web Installer stay on their accepted production path until the physical gates are satisfied.
