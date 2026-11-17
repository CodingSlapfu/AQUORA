# AQUORA v0.7.23 corrective implementation

Base checkpoint 3e28b38aef95022b2bcfa78717473b62dceeaafd.

Implemented and verified milestone
- Freeze no longer excludes the featured vocalist.
- Released recorded samples can be reclaimed with a short residual-signal crossfade when the two-sample limit is occupied. Intention changes no longer have to wait for both old sample tails to finish.
- Automatic vocal entry is rearmed when the scheduler becomes eligible again after a deliberate mute, source isolation, solo, zero gain or manual-only Play Along mode.
- Invalid sample playback state is retired and entry rearmed without changing intentional OFF/mute settings.
- Vocal status, starts, allocation rejection, tail reclamation, invalid-state recovery, direct signal peak and direct-audio frame diagnostics.
- Dedicated top-level Gate/Slicer page at formerly unused page 6. Removed Gate/Slicer from SOUND navigation. Preserved VOCALS page 7, gate targets and K4 styles. Animated pattern and playhead, current/next sections, bypass/open/gated states.
- Native SRAM finite checks avoid a flash-call relocation from the real-time voice preparation routine.

Tests
- Regression reproduced the original Freeze failure before the fix.
- 420 setting-change windows over approximately 22 simulated minutes. Each window required direct-vocal activity and audible PCM in the isolated singer output. Includes intentions, modes, all 20 extended vocal parameters, gain/presence, gating, mute, source isolation, solo, transport and keyboard mode changes.
- Injected invalid sample phase recovered and audible singing resumed.
- Existing featured-vocal and 20-parameter PCM response regressions passed.
- Host navigation passed page 6, K4 style, page 7, vocal enabling, ready state and zero command drops.
- First native P0 build completed. Application 487484 bytes, data+BSS 82524/98304, pool 274312/344064. FWSC 611496 bytes, identity FM-1_73000723. This is an intermediate milestone, not the final testing publication.

Next tasks
Expand recorded/approximated vowel and syllable articulation and coherent per-song styles, source licensed extra words and humming, strengthen vocal effects, implement expressive head/module performance, then final regression/reproducible native/WASM build and testing installer deployment. Physical FM-1 confirmation of the owner's shutdown symptom remains required.


Second implementation milestone
- Added recorded EH/EE and genuine humming, sung LOVE and ME, and a speech-derived pitched BABY unit. Existing DAY/YOU tables retained.
- Added a Voice articulation selector to Vocal Play Along, with 18 explicit vowel, syllable and diphthong options plus AUTO. UH and soft consonants are documented approximations.
- Six seeded per-song vocal characters, section-aware delivery, three supported multiword templates, and remembered hook vowel/onset recall.
- Stronger baseline room/echo/chorus, tempo-divided delay, longer tails and bounded combined feedback. Manual FX overrides retained.
- Added small groove-driven head nods/tilts, eyelid expression and three larger cybernetic indicator lights using the original artwork and palette.
- Restored the instrumental FM choir independently of the sampled singer. Added harmony-following choir answers, role-aware warmth/filtering and a bounded IDM hat ratchet at section transitions.
- Protect new instrumental note entrances against immediate voice stealing.
- Two held external vocal notes no longer prevent automatic singing in AUTO+PLAY ALONG. Released/sample-slot reclamation retains a residual crossfade.
- App control consumption rearms vocals immediately when a control removes an intentional blocker, including changes shorter than a musical step. Saved-bank application clears old vocal hook identity and rearms entry without changing bank schema.

Verified evidence to date
- 1200 control-change windows, more than one simulated hour, passed direct-vocal plus isolated final-PCM checks.
- 18 articulation options produced distinct audible PCM; all six primary identities appeared across song seeds, with stable identity and recalled hook articulation.
- 24 host cases with maximum vocal FX, three visualizers, four worker states and two pressure fixtures passed, with vocal audio present and no dropped input commands.
- Native extended build fits flash and SRAM. Final reproducibility, full composition traversal, final binary/WASM hashes and existing installer publication are being completed.
- Four-bank arrangement/migration tests and full 16-voice mixed-load tests passed.
- Word, vowel, humming and syllable provenance/limitations are documented in device/docs/VOCAL_CREDITS.md.

Physical FM-1 listening, vocal-shutdown reproduction, real callback deadlines,
encoder/display responsiveness and speech-derived BABY naturalness remain owner
acceptance items. Recorded intention-name vocabulary is unavailable.
