# AQUORA v0.7.21 testing recovery checkpoint

Source 217184fbdcba9629b9790379ee3ecd60d1ef3b84, resumed from e7de43785cabdda2b1eab728716587ac85ae2678.
Native identity FM-1_73000721, 611496-byte FWSC. SHA-256 b9d03b37de5fee3ac9112ea1a9846b0d65439286cad1687f5eabb4e41285cd99.
Two pinned JieLi builds reproduced the exact binary. Native application 482296 bytes, static data/BSS 82524 of 98304, pool 271652 of 344064 bytes.

## Implemented

- Fullscreen K1 queues a section. K2 queues Percussion Rise or Spacious Breakdown and changes energy in the chosen direction. K3 adjusts intensity. K4 requests a vocal phrase, reusing per-song pitch degrees where captured, or switches singing off when turned left. Existing song key and chord adaptation remain active.
- Dedicated VOCALS EDIT cycle adds Space, Echo, Modulation, Expression, Tone + Tuning, and Play Along settings. Main K1–K4 retain their existing meanings. FX audition remains accessible. First edit locks a parameter and Play Along settings K4 clears the FX locks. Common buffers and low-rate vocal processing limit memory use.
- Audible vocal decay, damping, stereo width, tempo-relative repeat time, feedback, repeat tone, modulation rate, phaser, flanger, vibrato rate/depth/onset, glide, presence, compression and makeup. The existing chorus, reverb, echo and pitch correction remain accessible. Automatic profiles evolve with mode, seed and sections. K4 at minimum retains the processed baseline.
- Added vocal-only and combined gate targets. The fourth lane is applied to recorded singer samples only. Existing bank format retains its layout.
- Vocal AUTO, PLAY ALONG, and AUTO + PLAY settings share the keyboard quantizer, scale and release handling. Removed the duplicate SOUND vocal page.
- Added EDM quarter-note kick anchors in IDM, selected phrase responses around IDM or manual kick patterns, musical hook pitch memory, subtle pitch-aware gaze and a half-lid blink, preserving the approved RGB565 portrait and recorded day sample.

## Tests and limitations

The DSP suite verified that minimum and maximum settings for all 16 added parameters produce different PCM on recorded singing, including the existing human “day” sample. Prior vocal gain, recorded vocal scheduling, solo and mute masking, word articulation, portrait palette, v9/v10 migration and four-slot bank tests pass. Host panel checked all five FX pages and Play Along, enabled vocals, exercised the four fullscreen controls and found no dropped commands.

The source document cut off at IDM section 25. The four knob mapping was taken from the owner's separately stated fullscreen instructions. The broader human vocabulary, intention lyrics, expanded phonetic lip-sync, cybernetic module lighting, complete composition overhaul, and full per-instrument effects work remain unfinished. Dedicated shimmer and further per-processor parameters (including independent chorus depth, flanger rate and feedback, and multiband EQ) remain open. Delay and echo share a repeat bank. The copied melodic hook is pitch-degree memory within the current song, with no cross-song persistence. Locked FX values and the vocal play mode are session-local. Full hardware listening, DSP deadlines and display timing are pending.

Original Puya 0x856014 only. This is a testing candidate. Preserve stable v0.7.14 and prior testing routes. Installer deployment verification is recorded separately.
