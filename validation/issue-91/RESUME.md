# Issue #91 corrective development checkpoint

Parent b89b200e368f8c45896deffc9aa9e47f2bd39734 is the verified v0.7.29
publication. No deployment from this branch is authorized until the
ten-new-word gate passes. Stable promotion remains subject to the owner's
physical FM-1 approval.

## Implemented

- Removed all ten Flite word samples and the Flite generation route from
  the active build. The archived v0.7.29 tree and downloads retain their
  licenses and bytes. Legacy BABY remains addressable for compatibility,
  but the phrase selector omits it and AUTO substitutes LOVE for a
  recalled BABY hook without modifying the saved bank.
- Manual WORDS uses rotating sung phrases. In VOCALS Play Along, the
  first five keys select DAY, YOU, LOVE, ME, DREAMS, repeating across the
  keybed. These are all existing words and count as zero new takes.
- Re-registered larger violet emitters to both ear plates and the throat
  servo. The existing portrait frame snapshot and decay stay in place.
  The bloom starts from direct vocal PCM on the main VOCALS portrait.
  Fullscreen visualizers do not render the portrait.
- The transactional allocator, two-slot policy, voice budget and existing
  nonlexical recordings were left intact. A matched dry and full-mix
  listening comparison against v0.7.27 is still required.

## Source leads

The public high-quality previews were downloaded. SHA-256 values below
identify those preview files, not the original FLAC/MP3 downloads.
Licenses were checked on the individual sound pages. Candidate words have
not been individually isolated, listened to or accepted for redistribution
in firmware.

| Freesound recording | Credit | License | Preview SHA-256 |
| --- | --- | --- | --- |
| [417410](https://freesound.org/people/TheScarlettWitch89/sounds/417410/) | TheScarlettWitch89 | [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/) | 451a67b26ff5ee787d48bd2b7c67117e71f8644d79a8c567bfa6cc7dd089b633 |
| [541220](https://freesound.org/people/womb_affliction/sounds/541220/) | Katarina Rose | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) | 8ceefb3ed96762ff4be7c3b40c711e9cc7f4fa2350cf0f9c5fb53397667a3cd0 |
| [476829](https://freesound.org/people/womb_affliction/sounds/476829/) | Katarina Rose | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) | f458b3687cbd7eaa4a9b872299bf431cb067074b7d4cb9e7c4f151a68ce0effa |
| [613851](https://freesound.org/people/womb_affliction/sounds/613851/) | Katarina Rose | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) | 447409b9cadebfd14b88a36c89a58d7ca6a165689cc2647e4202075c395bea28 |

Start with dry lead vocal clips. Cut at actual word boundaries, retaining
attacks, vowel transitions and consonant releases. The layered 613851
preview may be unsuitable for exposed solo singing. A clean cut, distinct
PCM, musically useful transposition and intelligibility are individual
acceptance checks. Capture original and derived hashes and sample offsets
for each selected word.

## Remaining gates

Obtain and audition at least ten new individual sung word takes. Add each
to manual Play Along, the phrase selector and appropriate AUTO contexts.
Render per-word direct PCM and matched v0.7.27 dry/full-mix comparisons.
After the vocabulary gate, rebuild native and WASM, verify hashes and
the updater and publish to a versioned testing route. Owner listening,
purple glow visibility and physical FM-1 timing remain open.
