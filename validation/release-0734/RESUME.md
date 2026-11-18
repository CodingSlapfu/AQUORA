# AQUORA v0.7.34 development checkpoint

Branch aquora-v0.7.34-corrective-development, source 0d450a405f4e21a70ba672961845bab3499d5804, parent 1946f0c5a8986585ed921d0659abc3c45b047d3a. This is an unpublished testing candidate. The live testing track is v0.7.33 and the stable track is v0.7.14.

## Implemented and verified

- The existing 16/32/64 loop-length control sets automatic drum pattern length as well as song loop length. Drum TIME independently scales the drum grid against the song clock. The part editor has become a read-only 16-step viewport with a page counter. K1 selects the lane, K2 sets length, K3 sets HIT % and K4 sets drum TIME. The kit is accessible in SOUND / MIX.
- Backbeat and kick anchors stay in the groove when HIT % is reduced. Higher settings introduce ride and subdivision patterns, ghost notes and fills. All 19 existing kits and the per-part/group mixer actions are retained. Existing banks load with NORMAL drum time and the new time choice fits the existing save format.
- KIRA's side ribbons are removed. Seven gold and aqua LED points on the portrait follow the direct vocal signal and blink in a staggered pattern. A full-frame pixel test verifies localized changes, pulse variation, and darkness on silence and mute. Automatic singing now reserves a short breath at formal cadences and constrains supporting harmony to the intended lead register.
- Host playback verified all nine drum length/rate combinations with wrap and live/forecast parity. All 19 PCM kit signatures remained distinguishable. The host panel script passed lane selection, mute/solo, HIT %, TIME and zero reported audio drops. The Favorites script passed all 16 Favorites and legacy migration with the existing post-reboot intro delay. The browser smoke test rendered 512,000 audio frames and a changing 240×240 framebuffer.
- Both native and WASM builds reproduced their byte hashes. The native application uses 570,280 / 581,564 flash bytes, 81,772 / 98,304 static RAM bytes and 262,472 / 344,064 DSP pool bytes.

| Artifact | SHA-256 |
|---|---|
| FM-1_73000734 | bed0c62fbac0f05f8dc9ae213b22eb21607bde24a4de32c61f9e75cb71422804 |
| WASM | 800b634af800bc0bd831ce55ace01cb863517af0bd8fa2b1a1b7df7a10ef89f7 |

## Vocabulary retained

v0.7.33 already ships thirteen credited recordings added after DREAMS. GO, NO, WAY, BE, FEEL, COLORS, GREEN, BEAUTIFUL, I and GOING were in v0.7.31/32. WHEN, CALL and DO joined in v0.7.33. The provenance manifest at device/dsp/human_words.json contains performer, URL, license, offsets, source and PCM hashes for each. The host PCM regression passed for all thirteen in the WORDS bank and selection tests, including three pitches and direct-voice lamp gating. Physical diction and further additions have not been verified.

## Matched dry and full-mix measurements

Compile render.c against the device/dsp directory of each selected checkout with cc -O2 -ffp-contract=off -I PATH_TO_DSP render.c -lm. Its argument is a directory to hold 44.1 kHz signed stereo PCM. The benchmark uses three matched seeds and intentions across five styles, at 72 BPM, tonic MIDI 48, manual vocal FX at zero, and fixed presence. Each run is 4,800 blocks at 256 frames per block. Word starts are counted at voice allocation, which does not prove diction or complete phrase playback.

| Full-mix measure, 15 runs per version | v0.7.27 | v0.7.33 | v0.7.34 |
|---|---:|---:|---:|
| Sample starts | 130 | 130 | 130 |
| Consecutive starts more than five semitones apart | 35 | 5 | 5 |
| Starts outside MIDI 62–77 | 78 | 0 | 0 |
| Median longest uninterrupted direct-vocal run | 1.68 s | 3.42 s | 3.42 s |
| Recorded word starts | 5 | 2 | 2 |
| Clipped output samples | 0 | 0 | 0 |

The sampled windows show continuity gains over v0.7.27 and no measured gain over v0.7.33 in these aggregates. The low count of word starts in both later versions warrants longer lexical section coverage. The reference TSV contains ten decoded official previews from the approved pop vocalists and matched engine render metrics. The preview figures measure the complete recordings, including instruments. They cannot score a singer's diction, legato, breath, emotional impact or hook quality. No commercial preview audio is included.

## Remaining release gate

The prescribed artist-by-artist listening and comparative phrasing assessment is incomplete, including the scat references and the separate instrumental listening request. No additional validated human-sung sources were integrated in this checkpoint. Complete those requirements before publishing the combined #91/#92 testing build. The source and both build artifacts are available to resume directly; earlier site deployments and rollback assets are untouched. Actual FM-1 visual brightness, audio timing, flash behavior and listening acceptance remain unverified.
