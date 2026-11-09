# Priority musical-quality correction — listening approval pending

Owner's physical v0.6.10 report rejects its musical quality. Prior software acceptance is not aesthetic acceptance. This is corrective development on top of published v0.6.10 (2563255), not a replacement release or rollback.

## Investigation

The exact accepted v0.6.9 source f9c36e3 and v0.6.10 runtime654d725 show:

- `make_motif()` did **not** change in this transition. It independently chose nearly every scale degree; evolution still discarded the theme on three out of four changes. That pre-existing weakness cannot honestly be attributed solely to v0.6.10.
- `fresh_patch()` **did** lose mode-specific compatible pools. It used broad role membership and arbitrary scene/mode hash weights; most drum kits also gained similar baseline odds. The all36/all19-per-context coverage assertion rewarded availability without establishing musical suitability.
- v0.6.10 increased tempo variety and expanded to32 sections with added musical detail. That exposes the existing motif discontinuity and inconsistent harmonic choices more often; audibility/causality remains a listening judgment.
- Chord strums ignored section harmony while other parts used it. Default bass, pads and accompaniment also had independent degree choices. Scale membership alone does not establish chord compatibility.

## Candidate implementation

- Phrase gestures contain a repeated opening, answering contour, breathing spaces and closing cadence, with a shaped velocity arc. Four-bar extension echoes the theme instead of reversing/wrapping it into unrelated contours.
- Evolution derives from the saved home theme, changing at most one answering note and returning home every fourth development. Learned material retains precedence; Freeze and sound locks remain supported.
- Strong melody beats settle to the nearest available chord degree; passing notes retain tension. Bass, pads, piano/pluck accompaniment and strums share current section harmony.
- One weighted ensemble is selected using explicit21-intention preferences and four mode profiles: intimate wood, luminous air, chamber, velvet FM. Compatible substitutions favor similar brightness, balance a locked lead, and restrain long metallic pluck tails. Repeats are allowed; recent substitutes receive a mild penalty instead of forced novelty.
- All36 patches and19 kits remain selectable and globally reachable. Contexts owe no coverage quotas. Intention kits receive substantially higher preference.
- Existing tempo logic,32-section routes/role fades, drum synthesis/presence, nature DSP, visualizers, AIR and platform code remain intact. No bank schema or per-sample composition allocation added.

## Review audio

`audio/` contains three versions for each setup:069,0610,candidate. The version labels are visible; this is not a blinded trial. Files are real stereo renders, with actual role/event/patch/gain timelines in matching `.csv.gz` files. ANALYSIS.json records raw PCM hashes, levels and setup. All review files receive one constant level adjustment to target RMS1800; no dynamic normalization or remixing.

| Prefix | Intention | Mode | Seeds | Length |
| --- | --- | --- | --- | --- |
| intimate | Positive Healing | Ambient |777,2027|2min each|
| air | Anxiety Release | Cinematic |777,2027|2min each|
| chamber | Dreaming | Neoclassical |777,2027|2min each|
| orbit | Raise Positive Vibrations | Experimental |777,2027|2min each|
| longform | Positive Healing | Ambient |777|12min each|

Matched conditions: init12345, song seed, intention/mode,64-step loop, variation0.6, rhythm0, manual72BPM. Generated ensembles, motif, kit, effects and arrangement remain version-native: those are the behavior under review. Random-stream consumption differs, so same seed does not mean identical downstream random draws. These comparisons isolate composition from tempo; they do not claim generated tempos are matched automatically or that all possible settings have been heard.

Listen first to `intimate-s777` in all three versions, then `air-s2027`, then compare the other seed before judging. Use longform to assess whether a theme survives entrances, departures and evolving sections.

For each setup assess:

1. Can you remember or hum a theme? Does it breathe, repeat, develop and resolve?
2. Do bass, chords and melody agree, including section transitions?
3. Do instruments blend in tone/register, leaving space rather than competing?
4. Does it feel warm, emotional, sensual and immersive? Does the mode/intention feel appropriate?
5. Across minutes, is there purposeful development without losing identity or becoming monotonous?

Report preferred version and troublesome timestamps. **Human approval is required. No aesthetic acceptance is inferred from metrics or automated passes.** The agent has not listened to or judged the audio aesthetically.

## Engineering evidence and remaining gates

STRUCTURE.log checks43008 generated songs, phrase/cadence/rest identity, <=1-note development, strong-beat harmony in32 sections, locked reroll and global36/19 availability. DEVELOPMENT.json compares2688 real evolution triggers per version; released versions average10.0625 changed motif positions, candidate0.6875 (maximum1). These are structural measurements, not quality scores.

COMPOSITION.log preserves the corrected gate:84 contexts,12386304 timeline steps, all32 sections, tempo/manual locks, Freeze and2432 real drum-boundary PCM windows. Host simulator and SR_DEVICE syntax checks pass. Heavy16-voice host timing/PCM checks passed; these are not FM-1 CPU/deadline results.

Still required before any release: owner's human listening approval and corrections from it; native pi32v2/repro/single-core/WASM and affected updater/integration verification; actual FM-1 timing and physical listening. This environment has no installed JieLi toolchain/AC79 SDK. No firmware was rebuilt, installed, published or relabeled. v0.6.10 and all rollback backups remain unchanged. KIRA-9 untouched; existing MIDI/#62 deferrals preserved.
