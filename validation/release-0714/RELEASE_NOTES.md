Firmware `FM-1_73000714` for the original M-VAVE FM-1 with Puya `0x856014` flash.

- MIXER → MASTER FX cycles through SPACE, MOTION, TEXTURE and AMBIENT with FX. MOTION adds chorus, autopan, flanger and tremolo. TEXTURE adds wet filtering, bitcrush, soft drive and tempo-derived stutter. AMBIENT adds octave-up shimmer, granular freeze, and wet-return ducking. Raise FREEZE above zero to capture the next 1,024 samples of the Mixer send. Return it to zero before capturing again.
- KIRA-9’s portrait uses the recorded vocal audio signal for mouth motion. The host regression checks recorded PCM and rendered portrait frames with vocal mute, solo and source isolation.
- DRUMS → STEP EDIT supports all sixteen parts, audition, manual patterns across four banks and loading v9 banks. Banks saved in v0.7.14 use v10. Back up your v9 banks before saving if you plan to roll back to v0.7.13.

Source `ca96a17668c5ca6a4c01eb6bb848884d22c473fa`. Two native builds produced identical bytes. Browser staging verified the same firmware and rejected a modified package. Physical FM-1 testing is pending.

Firmware SHA-256 `d3de83af71ba084c61dc88b1804e814200d7ae69439480d2aecfcb886d09ec6e`.

[Previous firmware and rollback download](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.7.13)
