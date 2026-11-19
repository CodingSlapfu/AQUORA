# Recorded vocals

The included takes are from *VocalSet: A Singing Voice Dataset* by Julia Wilkins, Prem Seetharaman, Aalok Wahl, and Bryan Pardo (ISMIR, 2018). The original recordings are licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). The recordings retain that license in source and firmware.

- [VocalSet v1.2 dataset](https://zenodo.org/records/1442513)
- [Sustained originals used at mirror commit c48ab544e94e9788fec11be52b365026bb4a017c](https://github.com/mblackman/vocalset-vowel-voicebanks/tree/c48ab544e94e9788fec11be52b365026bb4a017c/source/vocalset)

Edits include mono resampling to 11,025 Hz, segment selection, DC correction, level normalization, an entrance fade, and a phase-matched crossfade into the sustain loop before 8-bit quantization. Firmware sequences the edited tones into new wordless melodies. The source IDs are the dataset's anonymized singer labels. There are six samples of sung A, O and U vowels from male and female registers.

| Source WAV stem | Source SHA-256 | Edited PCM SHA-256 |
| --- | --- | --- |
| m4_long_straight_a | `09eb6dd466b2b9c13cf2a39e4710359b9e1d3aafb43521b59335b92c36245872` | `ca92a17e0a980eb42279184faa27ad477c24318b668846408edb99c776abc5fc` |
| m5_long_straight_o | `ccfdfaa4cda651f4a92c39da3d481b26cef37c137189098c0cd73d0f593fda8b` | `2e31718fe31c2b408e93143bc16bdaa34b65463237cefd8892be2d059ba8af3b` |
| m4_long_straight_u | `5807c32d2e062a6ea0228ecd979394b2a3b275676d440af23699806bce7cdd73` | `5e6b75bae634c18d495d5ca936b7848965bb01b802eb1fa1e0182614cf4d37d9` |
| f4_long_straight_a | `4491c79131c68b10ec77aacf015339c1fa8bb795d3810e4383adbf8aa2679e73` | `6262df9eea3e4f08b43d983f9dfb86ca51cb460d5fb390ce6a5a90686aeef147` |
| f4_long_straight_o | `6f22e12f8e2833cc88654d3c0c605c0ec6a3ed27e04490def5b8f0534931d636` | `08eb1433c8e1d6c0f117d56931cc36142233b266bfd6cff9b7c0c8cd95b041fe` |
| f5_long_straight_u | `d6cf7a184f0aaad8d964191f4a6bce2b8631f31a7da2b33943847ca5205cc69d` | `5940b8207339a4b4f051b190b07c7eff1700ba02f72b5e43ddc4047b8b36ee00` |

## Recorded word pilot

The word **“day”** is sung by Katy and recorded by digifishmusic. The
[Freesound recording](https://freesound.org/people/digifishmusic/sounds/40617/)
is licensed [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).
The source is the public high-quality preview, SHA-256
`24824a09753d6d69166499c4e423929dc1d8506c32dfef3afb762b0798b46528`.

AQUORA uses a shortened performance of the word. The mono audio was resampled
to 11,025 Hz, trimmed to the onset and final diphthong, pitch-aligned at the
splice, crossfaded and quantized to signed 8-bit PCM. The edited sample SHA-256
is `5d882da9280d3aff23e0f9d591a77251c839dab355cec3e16d4786189aa7be3d`.
The firmware picks the pitch and gate, sustains the recorded vowel, and plays
the original edited tail after release. The portrait follows source positions
for the brief onset, sustained vowel and closing diphthong. This first lexical
sample is used for selected Featured-presence cadences alongside wordless singing.

## “You” candidate

First sung word excerpted from *You said you loved me, oh, you lied (Cleaned with reverb by Erokia)*, performed/uploaded by TheScarlettWitch89 and processed by Erokia. [Source recording, CC BY 3.0](https://freesound.org/people/TheScarlettWitch89/sounds/427201/). The public HQ preview was converted to mono at 11,025 Hz, trimmed to 0.226 seconds, phase-aligned for vowel looping, faded, and quantized to signed 8-bit PCM. Verify intelligibility on FM-1.

- Preview SHA-256: `58a706a4d73443bbac5c03e59571a89a4d2f14a8b396b9ed39e56952a796cb6a`
- Edited PCM SHA-256: `0dd9b3f7af903bbd9a274b01d01de53e4c6e707cba049c8ef6c406e2bbe9f6b4`

## v0.7.23 recorded extensions and articulation

| Unit | Recording and license | Edit |
| --- | --- | --- |
| EH / EE | VocalSet, female f4 sustained e/i, CC BY 4.0, same pinned mirror above | Mono 11025 Hz, steady segment, normalization, matched sustain loop, fades and signed 8-bit PCM |
| HUM | PSsoundproject, “woman humming,” CC0, https://freesound.org/people/PSsoundproject/sounds/183565/ | A closed-mouth hummed segment, pitched and looped |
| LOVE | Ehved, “female singing name of love,” CC0, https://freesound.org/people/Ehved/sounds/566926/ | Sung LOVE excerpt at 8.78–9.84 seconds, retaining attack and final consonant |
| BABY | TheScarlettWitch89, “Midnight_Baby,” CC BY 3.0, https://freesound.org/people/TheScarlettWitch89/sounds/432783/ | Spoken BABY excerpt at 1.24–2.04 seconds adapted to pitched playback and final-vowel sustain |
| ME | TheScarlettWitch89 / Erokia, same “You said you loved me” source above, CC BY 3.0 | Sung ME excerpt at 1.34–2.32 seconds, retaining nasal entrance and sustained vowel |

DAY and YOU retain their prior samples and credits. Source WAV edit hashes,
PCM hashes, pitch calibration and exact segment/loop coordinates are in
device/dsp/vocal_extensions.json. The generator at
device/tools/build_vocal_extensions.py reproduces the tables from those
decoded source recordings.

AH, OH, OOH, EH and EE use distinct human recordings. Humming uses a recorded
closed-mouth voice. UH is an approximation made by darkening recorded EH.
DO/RE/MI, LA, NA, BA, DA and HA use recorded vowel bodies with synthesized
or filtered soft consonant entrances. These syllables are approximations.
OH/AH, OO/EE, AY/OH, EH/OH and MM/OOH transitions crossfade separately pitched
recorded vowels. AY uses an EE-to-OH approximation. BABY is derived from speech,
and its conversion is less natural than the sung word units.

Automatic lexical arrangements include BABY–LOVE–YOU, YOU–LOVE–ME and
LOVE–YOU. DAY can appear alone at a cadence. The small vocabulary does not
support arbitrary lyrics or recorded intention names. Physical FM-1
intelligibility and musical acceptance remain to be checked.

## v0.7.28 startup motif

The original E5–B4–A4 motif uses the same credited female VocalSet OH, AH and
OOH assets above. Startup interpolates and loops the existing samples and adds
bounded stereo reflection and echo taps. It includes no new source recording.

## v0.7.28 DREAMS pilot

[DREAMS](https://freesound.org/people/buggly/sounds/752369/) is the opening word
of “dreams are breezy,” an original vocal performance recorded and released by
buggly under CC0. This is human singing. The HQ preview SHA-256 is
`fec0061c9e2ea3edb9c0d2405fb196075e1dc71a0f60602fac1b94c9fe2243b6`.
The generator `device/tools/build_dreams_word.py` extracts 0.080–0.700 s,
downsamples, normalizes, phase-matches a vowel loop and quantizes to 6,836 bytes
of signed PCM. Edited PCM SHA-256
`5102229c96aa9f73fd194a9c892d17e50838bc6eb0e004a6f1b9f2b67ba5dd03`.
The director can use it as a Dreaming cadence. Its intelligibility remains a
physical listening item. It is one pilot addition, and the larger vocabulary
and multiword composition expansion remain open.

## v0.7.31 live sung word additions

GO, NO and WAY are excerpted from TheScarlettWitch89’s
[Vocal Sample 2 (Variation)](https://freesound.org/people/TheScarlettWitch89/sounds/417513/).
GOING is from her [Vocal sample](https://freesound.org/people/TheScarlettWitch89/sounds/417410/).
BE and BEAUTIFUL are from her [Do you wanna be beautiful, take 3](https://freesound.org/people/TheScarlettWitch89/sounds/417936/).
All three recordings are [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).

FEEL, COLORS, GREEN and I are excerpted from Katarina Rose (womb_affliction)’s
[female vocal](https://freesound.org/people/womb_affliction/sounds/476829/),
[CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

Firmware samples are edited from the lossy public previews. Edits include mono
fold-down, word-boundary trimming, DC removal, pitch-preserving time
compression for longer takes, fades, level normalization, resampling and
5-bit companding. The originals' pitch contours and final consonants play once
through, without looping. Source offsets, preview and PCM hashes, pitch and
quantization measurements are in `device/dsp/human_words.json`. Recreate the
bank with `device/tools/build_human_words.py` and the same four previews.

## v0.7.33 recorded additions

WHEN and CALL are excerpts from TheScarlettWitch89’s [I hang up when you
call, take 1](https://freesound.org/people/TheScarlettWitch89/sounds/417935/).
DO is from the same performer’s [Do you wanna be beautiful, take 3](https://freesound.org/people/TheScarlettWitch89/sounds/417936/).
Both recordings use [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).
The sample manifest records the exact source offsets and hashes. The three
added cuts use the same one-shot encoder as the previous bank. AUTO
chooses a recorded word where the note fits its source register, with a
recorded sung-vowel fallback at other pitches.

HELLO is a cue from [Valérie Vivegnis’s self-recorded Hello](https://freesound.org/people/Valerie-Vivegnis/sounds/759142/), licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). AQUORA trims, downmixes, resamples, fades, and quantizes this spoken performance and pitches it in the sampler. It is available on the manual WORDS keyboard and in AUTO.

YES is by Morena, from [MatteusNova’s recording The word yes](https://freesound.org/people/MatteusNova/sounds/157506/), licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). AQUORA trims, resamples, fades, and quantizes this spoken cue. It is playable in WORDS and eligible in AUTO.
