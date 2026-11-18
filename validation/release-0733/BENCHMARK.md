# KIRA-9 v0.7.33 audio comparison

## Native-style engine regression

Each version rendered 15 full mixes using the same three seed and intention pairs across Standard, Cinematic, Neoclassical, Experimental Ambient and IDM. Each render was 27.86 seconds at 44.1 kHz. This test inspected the direct vocal signal and tracked new allocated sample voices. The matching Cinema dry and full-mix excerpts are on the public testing evidence page.

| Measured behavior | v0.7.27 | v0.7.32 | v0.7.33 |
|---|---:|---:|---:|
| Consecutive sample-start pitch changes greater than 5 semitones | 46 / 176 | 50 / 176 | 3 / 174 |
| New voices with less than 250 ms of remaining gate at the sample point | 0 | 5 | 0 |
| Longest continuous above-threshold direct vocal run, median per case | 1.65 s | 1.64 s | 4.10 s |
| Blocks with direct vocal peak above 0.002 | 43.9% | 42.8% | 56.7% |
| New sample voice starts outside MIDI 53–73, including harmony voices | 7 | 28 | 59 |
| Clipped stereo samples in the mix | 0 | 0 | 0 |

Consecutive starts can include harmony entries and breath boundaries. These measurements show increased signal continuity and fewer large consecutive changes, alongside more out-of-band voice starts under this specific threshold. They do not certify diction, naturalness, phrasing quality, or comfort of the register. No speech-recognition gate was used.

## Apple Music reference excerpt measurements

Ten official 30-second preview recordings were decoded temporarily for analysis. No reference audio is included in the firmware, emulator or evidence downloads. Dynamics are 95th minus 10th percentile 20 ms mono RMS, attack peaks are rises above 4 dB in 20 ms after 100 ms deduplication, and center energy is mono averaged power relative to stereo channel average. These are whole-mix proxies. Accompaniment contributes to all three measures and the excerpts do not systematically cover the opening, verse, chorus and ending. An artist-by-artist listening and phrase-level assessment is still outstanding.

| Artist | Recording | Dynamics (dB) | Attack peaks/min | Center energy |
|---|---|---:|---:|---:|
| Michael Jackson | [Human Nature](https://music.apple.com/us/album/human-nature/273048762?i=273048790&uo=4) | 11.3 | 128.1 | 80.6% |
| Michael Jackson | [Billie Jean](https://music.apple.com/us/album/billie-jean/269572838?i=269573364&uo=4) | 16.5 | 190.2 | 95.5% |
| Mariah Carey | [Vision of Love](https://music.apple.com/us/album/vision-of-love/169605890?i=169605940&uo=4) | 10.0 | 86.1 | 94.3% |
| Mariah Carey | [Hero](https://music.apple.com/us/album/hero/190173909?i=190174911&uo=4) | 11.2 | 60.0 | 95.7% |
| No Doubt | [Don't Speak](https://music.apple.com/us/album/dont-speak/1440845400?i=1440845557&uo=4) | 7.2 | 72.1 | 89.8% |
| Gwen Stefani | [Cool](https://music.apple.com/us/album/cool/1452869898?i=1452870050&uo=4) | 10.0 | 128.1 | 92.8% |
| Portishead | [Roads](https://music.apple.com/us/album/roads/1440653096?i=1440653203&uo=4) | 11.0 | 142.1 | 100.0% |
| Portishead | [Glory Box](https://music.apple.com/us/album/glory-box/1440653096?i=1440653206&uo=4) | 10.1 | 92.1 | 99.0% |
| Björk | [Jóga](https://music.apple.com/us/album/j%C3%B3ga/20833577?i=20833584&uo=4) | 7.2 | 38.0 | 90.0% |
| Björk | [Hyperballad](https://music.apple.com/us/album/hyperballad/262483556?i=262483634&uo=4) | 14.8 | 82.1 | 99.7% |
