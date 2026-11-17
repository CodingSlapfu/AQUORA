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
