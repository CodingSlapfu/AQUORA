# AQUORA MIDI core / application profile v1

**Development specification only — NOT ENABLED in AQUORA v0.6.6.**
Both USB and BLE performance MIDI are explicitly owner-authorized deferred.
Revisit after KIRA-9 has a good working firmware build and owner reprioritizes.
The partial transport-neutral core and USB adapter are preserved for future
work, with performance routing and editable MIDI controls disabled. The
contracts below describe that preserved candidate, not released capabilities.

AIR remains available; neither deferred adapter may claim the radio or alter
internal playback. Firmware-update SysEx is separate and remains required.
Approval: Issue #31 comment 6702097918 / Issue #32 comment 6702099240.
KIRA-9 was not changed.

## Setup

ALGORITHM → SETUP, then EDIT cycles sections:

- **USB MIDI SYNC:** K1 Off / Master / USB Slave, K2 Clock Off / Playing /
  Always, K3 melodic source role, K4 transmit channel 1–16.
- **USB MIDI I/O:** K1 receive channel Omni / 1–16, K2 USB IN, K3 USB OUT,
  K4 Panic. Input channels retain independent note, sustain and bend state.
- **STARTUP / MIDI SEND:** K1 startup sound Off / Quiet / Normal; K2 optional
  controller/program output. These global preferences are stored after the
  transport is stopped and externally played notes have released, preserving all four arrangement banks.

Defaults: radio off, USB IN/OUT on, clock Master / Playing, Lead on channel 1,
Omni input, controller/program output off. Select exactly one clock leader.
In USB Slave mode the incoming 24-PPQN clock is authoritative; no local clock
or transport is echoed. Missing clock stops a running slave after two seconds.
USB disconnect releases incoming notes. If a peer starts Active Sensing,
350 ms without further sensing releases its notes. Reconnection flushes stale
output before new notes and resumes a running leader at a step boundary; bounded queue exhaustion uses a visible drop counter
and panic rather than leaving orphaned gates.

## Standard messages

| Message | Receive | Transmit |
|---|---|---|
| Clock F8 | Slave phase, six clocks per sixteenth step | 24 PPQN from the audio scheduler; Playing/Always/Off |
| Start FA | Restart at downbeat in Slave mode | Internal PLAY starts at downbeat |
| Continue FB | Resume preserved position in Slave mode | Resume/switch into an existing position, with SPP |
| Stop FC | Stop/release in Slave mode | Internal STOP in Master mode |
| Song Position F2 | 14-bit sixteenth-note position in Slave mode | Existing-position resume; standard 14-bit wrap |
| Note On/Off | Selected role; velocity zero is Note Off | Actual accepted engine gate events for selected melodic role |
| CC64 | Per-channel sustain | Physical held keys use ordinary gate duration |
| CC7 / CC11 | Selected-role volume / expression | Optional CC7 fader and role fade |
| CC10 | See unsupported-input note below | Optional selected-role pan |
| CC91 | See unsupported-input note below | Optional selected-role FX send |
| CC1 | Lead/bass vibrato depth | No automatic per-voice modulation export |
| CC120 / CC123 | Channel note release | Panic sends explicit Note Off and CC123 |
| Pitch bend | Per-channel ±2 semitones, including later notes | Optional A4 tuning correction, assuming receiver A4=440 and ±2-semitone bend range |
| Program Change | 0–12 select AQUORA intentions; other values ignored | Optional intention 0–12 |

Received note/control messages are not MIDI-thru. Generated notes under an
external clock remain independent musical output; disable USB OUT on followers
if the host route should carry only the leader's notes. Controller output is
optional so a generic peer keeps its own voice and mix unless explicitly
configured. Receive CC10/CC91, poly/channel pressure, unsupported system-common,
SysEx lyric/profile extensions and RPN negotiation are not implemented; unknown
messages are safely ignored. Firmware update/debug SysEx remains the platform's
separate protocol. Ordinary MIDI does not contain English lyrics: any future
vocal peer prepares lyrics locally. No KIRA-specific whitelist or lyric mapping
is embedded in the generic core.

Audio synthesis and note gates remain sample-driven. Hardware USB transmission
is buffered; software services input between display strips as well as on the
fast input path. Desktop fixtures establish bounded software behavior, not
physical iPad enumeration, cable/hub power, latency, analog sound or RF proof.
Owner hardware checks remain explicitly unverified.
