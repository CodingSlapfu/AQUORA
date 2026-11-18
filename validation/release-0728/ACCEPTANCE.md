# AQUORA v0.7.28 testing

FM-1_73000728 for the original Puya 0x856014 FM-1. Source
`6c0b94c3b52902ab2d211be54b8d675b146e1a77`. Physical FM-1 acceptance is pending.

| Issue #90 requirement | Status and evidence |
|---|---|
| A. Independent vocal AUTO | Implemented for 20 extended FX, chorus, room, echo, tuning and level. One detent reaches manual zero from AUTO and another left from zero releases the single lock. Host fixture covers every encoder position and Favorites recall. Reset-all remains available. |
| B. Dedicated patches | Implemented. K4 selects AUTO and ten singer-only patches. The director selects by song seed, mode, intention and scale. The patch biases smoothed processing targets and leaves manual overrides intact. Ten unique isolated DAY PCM renders. Perceived differences and mix fit need listening. |
| C. Live singing and register | Partial. Automatic nonlexical lead now uses the female VocalSet zones for the main vowels and the matching transition targets. Existing legato, vocal director, hook memory and voice admission are retained. Wider phrasing, register and direct/mix listening acceptance remain open. |
| D. Sung vocabulary | Partial. One new human-sung DREAMS pilot, 6,836 bytes, from buggly's CC0 recording. It is eligible for selected Dreaming cadences. A substantial multiword bank and intelligibility evaluation remain open. |
| E. AUTO level | Implemented. Previous-block direct PCM and the backing track meters drive a two-second adaptation with a 0.76–1.25 gain range. Silence holds the target. Manual zero and Favorites recall work. Speaker and headphone balance and pumping need physical validation. |
| F. FX and fixed send | Partial. Normalized unity input to the vocal FX path, independent dry path, ten production characters, and stronger auto room and echo mixes and per-role sends. Manual routing overrides remain. Full stem-by-stem and maximum-load hardware DSP evaluation remain open. |
| G. Singing lamps | Implemented. All three module lamps use a bounded purple halo with intensity derived from direct sample PCM and pitch and a short visual decay. Physical color and appearance pending. |
| H. VOCALS home | Implemented. Portrait spans the full page, with shadowed labels and dedicated patch readout. Physical readability and all popup combinations pending. |
| I. Startup | Implemented. E5–B4–A4 uses the credited female VocalSet vowels with stereo taps. Host renders verify all 49 simulated skip inputs, real master mute, headroom and completion. |
| J. STOP | Implemented on VOCALS home. STOP still requests transport stop immediately. A 720 ms lid animation and fading module lights run independently of the audio. PLAY cancels the shutdown. Physical viewing pending. |
| K. Gate/Slicer | Implemented. The gem cells use the real pattern bits, step and phase. Existing control and DSP routing are retained. FM-1 visual timing pending. |
| L. Global UI | Partial. VOCALS and Gate changed, shared filter cutoff and emphasis labeled more clearly. SELECT, ALGORITHM, SOUND SEL and 16 Favorites covered in host fixtures. Exhaustive typography and workflow review across pages remains open. |
| M. Fullscreen/page performance | Partial. The fullscreen K1 callback updates queued section state only. Feedback now acknowledges the revision actually drawn by the feedback band, reducing avoidable waiting after mid-frame input. Host stress covered 1,200 encoder edges across Water, Cymatics and Chladni with zero drops and vocal PCM. Device LCD DMA, input-to-photon and audio callback p95/p99 are physically unverified. |

Native app 549,768 bytes. Main data/BSS 81,756 of 98,304 bytes and pool
325,952 of 344,064 bytes. Native reproducibility is byte for byte.
The WASM smoke test checks 512,000 frames, finite audio and moving
240×240 framebuffer output. V11 keeps its wire size and CRC positions.
v0.7.28 stores patch and level AUTO in unused bits older versions ignore.
Legacy V11 non-default levels remain manual. Legacy level 100 migrates to AUTO.

Test Standard → IDM → Cinematic → IDM with KIRA-9 and the full backing
ensemble. Hold fullscreen K1–K4 on every visualizer for 30–60 seconds and
record callback p95/p99, XRUNs, PCM age and LCD responsiveness. Exercise
Play Along, STOP/PLAY, Gate, 16 Favorites and direct update/rollback.
Listen to the ten patches, DREAMS cadence, startup and AUTO level on the
FM-1 speaker and headphones. Stable promotion requires owner approval.
