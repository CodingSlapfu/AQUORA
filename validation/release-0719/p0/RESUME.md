# AQUORA v0.7.19 P0 recovery checkpoint

Resume source ecb58955db64c0e9de53a8ddae5306fa3bc743c1 on aquora-v0.7.19-corrective-development. Parent 886dd131cb12a358fc23ed950cfc51bd09c86b3d. This branch isolates the new identity from the old v0.7.18 push-triggered workflow and preserves the testing backup and official rollback branches.

## Verified in software

- Retained the approved portrait art. Its canonical RGB565 pixels now go to the FM-1 canvas in the same high-byte-first order as the other UI elements. All-pixel portrait canvas comparison and recorded-vocal portrait PCM regression passed. Physical LCD acceptance is pending.
- Added next-step recorded-vowel entry when vocals are enabled or an intention is applied, with normal musical breaks after entry. Default OFF, explicit ON/presence and existing mix isolation remain intact. Tests passed for entry, intention change, featured phrasing, audition and recorded notes across all five existing modes. The PCM test measured isolated peaks 0 / 1376 / 2753 at K3 0 / 50 / 100 with the choir at zero.
- K3 now changes the singer gain independently of choir level. K4 changes vocal wet send. A reverb floor retains a processed baseline at the minimum send. Explicit choir mute/solo and isolation routing continue to govern the shared vocal lane. K3/K4 values persist across intention changes within the running session; across-power settings are not added.
- The slicer has AUTO and seven K4 styles, arrangement-dependent variation and kick protection. Pattern and binding code are in place. Style-pattern tests passed; end-to-end listening is pending.
- Host UI compiled. All 24 worker-load/input/render cases passed with no dropped commands. These are host results. Dedicated navigation-through-every-page vocal continuity, hardware audio timing and physical listening are pending.
- Native image 467576 B. Static data/BSS 82524 / 98304 B; pool 261716 / 344064 B. Pinned JieLi archive SHA-256 a1545cddbc451a06bac43eb5cd96d831470c42a97c5c0b1f98879a20fc7647bd. Two native builds produced the same 611496-byte FWSC.

## Firmware and publication

P0 engineering checkpoint only. Updater identity FM-1_73000719. SHA-256 81a51dcccb06694b1c86f34a0a42d47652dfb1bfc4e9d8e726bfc76946626e04. This branch contains the exact binary. The existing Web Installer was not updated. No release or stable tag was created.

## Continue

Use the complete owner specification in Pasted text(3).txt and the resume request. Start with genuine, redistributable human sung words or syllables and timed phoneme metadata. The current six licensed VocalSet zones are wordless A/O/U sustains. Research found NHSS and NUS-48E as leads; dataset availability does not establish redistribution rights for recordings and underlying compositions. Finish audio-timeline word and pitch lipsync, vocal FX automation/expanded processors, per-role FX improvements, composition development and listening comparisons, and the remaining final compatibility and full-load checks. Retain the tested portrait byte order and first-phrase logic. Use a fresh updater identity for any different binary. Prepare matching installer assets after those gates and retain v0.7.14 and the verified v0.7.18 rollback. The owner will verify LCD colors, performance, musical quality and intelligibility on physical hardware before any new stable release.
