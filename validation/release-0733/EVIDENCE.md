# AQUORA 0.7.33 testing

For original Puya 0x856014 M-VAVE FM-1 hardware. Firmware identity FM-1_73000733. Stable and earlier testing installers are available separately.

- **Gate** shows the actual sequencer position, active mask, gate envelope and signal meters in a colored crystal display.
- **Mixer** uses K1 to select any of the eight tracks, K2 for level, K3 for pan and K4 for send. SEL mutes, LFO solos, and EDIT clears the mute and solo state.
- **Drums** uses K1 for the part, K2 for the step, K3 to write the step and K4 for the kit. SEL mutes, LFO solos and FX auditions. Hold EDIT for group mute and solo or press EDIT+FX to clear them.
- **Composition** derives the style from the intention. The 19 kits cover different acoustic, FM, ambient and experimental voices. Kick and backbeat shapes vary by arrangement and seed.
- **Sound** keeps the live waveform and patch envelope and adds expanded live level feedback.
- **KIRA-9** favors sustained openings and connected notes in automatic singing, limits abrupt jumps and can transition to recorded words later in a phrase. WHEN, CALL and DO join the ten prior recordings. The portrait uses gold and aqua light driven by the direct vocal signal. See [recording credits and licenses](VOCAL_CREDITS.md).

The native image occupies 570,328 of 581,564 application flash bytes, leaving 11,236 bytes. Static data and BSS occupy 81,772 bytes, and the DSP pool uses 262,528 bytes. Relative to 0.7.32, the increases are 9,512 flash bytes, 16 static RAM bytes and 8 DSP pool bytes. Native firmware and WASM were rebuilt byte for byte.

Software checks cover the changed panel controls, favorites migration, generated drum timing, PCM for 19 kits and 13 recorded words, and browser emulator rendering. The host panel run recorded zero reported audio drops. Timing and input under the physical LCD/audio workload, device flashing and musical quality on the FM-1 require hardware verification. The matched vocal excerpts let listeners compare the changes. The requested artist-reference listening benchmark is outstanding.

[Open the software evidence](evidence/) · [Stable installer](/install/) · [0.7.32 testing rollback](/testing/0.7.32/install/)
1 River Hands        16 lanes peak=6278 signature=4b670289
2 Willow Brushes     16 lanes peak=6603 signature=8ab9bc39
3 Hollow Earth       16 lanes peak=8321 signature=68abffcd
4 Moonlit Felt       16 lanes peak=7324 signature=8502f8ba
5 Temple Air         16 lanes peak=8075 signature=50c01ec0
6 River Stones       16 lanes peak=6956 signature=699ded35
7 Warm Pulse         16 lanes peak=10873 signature=5f8e5dcd
8 Star Dust          16 lanes peak=7112 signature=a3527205
9 Rain Shells        16 lanes peak=7785 signature=9aec4c68
10 Willow Brush       16 lanes peak=5427 signature=1571eece
11 Crystal Taps       16 lanes peak=7757 signature=a08c8fed
12 Soft Studio        16 lanes peak=8286 signature=d2fead9c
13 Lunar Tide         16 lanes peak=11080 signature=df8b57da
14 Sacred Earth       16 lanes peak=8185 signature=bf7b5b2b
15 Feather Ritual     16 lanes peak=8492 signature=10fea3c3
16 FM Moon Kit        16 lanes peak=9536 signature=b5709bf7
17 Temple Resonance   16 lanes peak=9400 signature=717d3e01
18 Warm Pocket        16 lanes peak=10433 signature=9d1bd90d
19 Stardust FM        16 lanes peak=7427 signature=1c2ad92e
PASS all styles and 299 neighboring-seed groove changes
GO -2 semitones blocks=272 coda=46 peak=6358
GO +0 semitones blocks=238 coda=41 peak=6039
GO +2 semitones blocks=213 coda=37 peak=5857
NO -2 semitones blocks=222 coda=38 peak=6739
NO +0 semitones blocks=198 coda=34 peak=6566
NO +2 semitones blocks=173 coda=30 peak=5922
WAY -2 semitones blocks=298 coda=51 peak=6493
WAY +0 semitones blocks=265 coda=45 peak=5941
WAY +2 semitones blocks=232 coda=40 peak=5820
BE -2 semitones blocks=186 coda=34 peak=4924
BE +0 semitones blocks=166 coda=30 peak=4294
BE +2 semitones blocks=145 coda=26 peak=3550
FEEL -2 semitones blocks=229 coda=39 peak=6826
FEEL +0 semitones blocks=205 coda=35 peak=6762
FEEL +2 semitones blocks=179 coda=31 peak=6191
COLORS -2 semitones blocks=393 coda=66 peak=6108
COLORS +0 semitones blocks=351 coda=60 peak=6550
COLORS +2 semitones blocks=306 coda=52 peak=6680
GREEN -2 semitones blocks=302 coda=52 peak=6584
GREEN +0 semitones blocks=263 coda=45 peak=6123
GREEN +2 semitones blocks=235 coda=40 peak=5504
BEAUTIFUL -2 semitones blocks=363 coda=67 peak=6725
BEAUTIFUL +0 semitones blocks=323 coda=60 peak=6400
BEAUTIFUL +2 semitones blocks=283 coda=53 peak=4976
I -2 semitones blocks=249 coda=43 peak=6897
I +0 semitones blocks=222 coda=38 peak=6515
I +2 semitones blocks=193 coda=33 peak=6284
GOING -2 semitones blocks=215 coda=37 peak=6935
GOING +0 semitones blocks=188 coda=32 peak=6222
GOING +2 semitones blocks=168 coda=29 peak=6312
WHEN -2 semitones blocks=67 coda=12 peak=3411
WHEN +0 semitones blocks=59 coda=11 peak=3403
WHEN +2 semitones blocks=55 coda=10 peak=3397
CALL -2 semitones blocks=270 coda=46 peak=5877
CALL +0 semitones blocks=236 coda=40 peak=5749
CALL +2 semitones blocks=210 coda=36 peak=5254
DO -2 semitones blocks=125 coda=23 peak=4065
DO +0 semitones blocks=111 coda=21 peak=3400
DO +2 semitones blocks=96 coda=17 peak=3381
PASS all thirteen words in manual bank and AUTO choices, natural one-shot endings and direct-PCM glow

## Build and regression record

- Source 72bfee45793c11cf1ef7ebcf53200c0363a00742 follows verified base a5026cb6a36bd41ae27bc2cdd98f881ea66eb054.
- Firmware SHA-256: 076819c190e14ead729990a77e8b1aa44618e6ac02599351aa3c435ae43d7dff
- WASM SHA-256: 383eb1509ceccb5a4c0c77d8312f2d46d75a859e4c780c37db7b02b5f657f664
- Both consecutive native and WASM builds matched byte for byte. The WASM smoke test rendered 512,000 frames and changed the framebuffer.
- Score preview matched 5,709 live events across 110 scene and mode pairs, including drum velocity. The Favorites script passed 87 assertions including migration and exact recall.
- The panel replay observed zero reported drops. Host mixer, drums and gate band draw maxima were 240, 280 and 170 microseconds respectively. These timings do not measure FM-1 hardware.
- Host render evidence and matched listening excerpts are staged with the testing website. No promotion of stable is authorized until physical acceptance.
