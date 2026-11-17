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
