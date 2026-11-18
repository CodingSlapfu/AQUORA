# AQUORA v0.7.25 testing acceptance

This testing candidate completes the Issue #86 vocal-director gaps after v0.7.24. See ISSUE86.md for the detailed audit. No new stable release.

## Implemented

Intention/mode/seed-aware primary singing identity; purposeful humming, verse words/chant/scat/vocables, OH-AH refrains and cinematic sustained vowels; sparse breathing sections; intention/groove/energy-aware lengths and accents; all 20 automatic FX controls plus chorus/room/echo and tuning; manual overrides protected, including zero tuning and articulation during hook recall. Existing P0 reliability fixes and instrumental keyboard choir routing remain.

## Verified software evidence

- 110 intention/mode combinations produce distinct rendered output with measured direct vocal audio; 550 section profiles; every AUTO parameter varies; all per-control locks and main FX overrides win; OFF and zero presence remain respected.
- 1200 sustained setting-change windows pass isolated PCM and direct-vocal measurements without a failed recovery. Invalid sample state recovery and frozen arrangement scheduling remain verified.
- 18 audibly distinct articulation PCM hashes; six primary identities; existing multiword vocabulary and hook articulation recall.
- Keyboard OFF/AUTO/PLAY/AUTO+PLAY routing and note-off pass. All 20 individual DSP controls change actual vocal PCM. Featured-vocal gain, ducking, reprise and vocal-only slicer controls pass.
- 24 maximum-vocal-FX host cases cover three visualizers, dual-core fallback/fault/late fixtures and callback pressure. They are host simulations, not measured FM-1 CPU/XRUN results.
- Two independently invoked native builds match byte-for-byte. Native image 537912 B, data+BSS 82540/98304 B, memory pool 318692/344064 B. Compiler ISR/SRAM and hardware-register checks pass.
- Matching v0.7.25 WASM boots and renders 512000 audio frames in the Node smoke test.
- Updater simulation checks prior and same-version identities, port re-enumeration, wrong return identity and package corruption. See updater.log for result.

## Hardware acceptance still required

Physical FM-1: long setting/intention/mode changes without silence or reboot; prompt entry and recovery; Play Along and Gate/Slicer interactions; natural phrases, word intelligibility, musical identity and hook reprises; level/FX balance through speaker and headphones; portrait and graph appearance; knob/input latency and actual CPU/XRUN timing during all visualizers. Validate original Puya 0x856014 flashing/rollback on the actual unit. Other flash IDs remain unapproved for this installer.

DAY/YOU/LOVE/ME remain existing human recordings with published credits. BABY is speech-derived; UH, soft consonant vocables and AY are approximations. No additional intention names were recorded. Software PCM checks do not establish human-like singing or owner listening approval.

Rollbacks remain v0.7.24 testing, v0.7.22 testing and official v0.7.14. Saved bank formats were not changed. Do not publish a new stable release before owner physical acceptance.
