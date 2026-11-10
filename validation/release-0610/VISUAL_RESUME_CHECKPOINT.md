# 0.6.10 visual comparison correction — stop on preserved publication blocker

Resumed exact remote2d471707fbb069419a03630b0b80c4337d9ca753; clean local checkout, no project lock or active build observed in one check. Acquired own lock for this bounded correction; released on stop. No firmware rebuild or passed composition/native test rerun.

Root cause confirmed in app.c: AQUORA_TEST_SEED stabilizes song_nonce, while cold-boot song selection additionally mixes host clock()/time() unless AQUORA_TEST_BOOT_SEED is set. The old capture script used only the former; each master run could generate a different song. Autoplay also remained enabled alongside explicit PLAY. Corrected script pins BOOT_SEED730610 and disables autoplay, leaving the explicit scripted PLAY as the sole start. Production firmware is unchanged.

Corrected tests/visual_0610.py PASS:864 real240x240 LCD frames, three modes, dark/white, master0/50/100; baseline and candidate each have exact master0/full framebuffer equality. Captured36 lossless24-frame animations and STOP images. Comparison sheet visually inspected; distinct Original/Cymatics/Chladni identities and both palettes remain. The screenshot/archive is evidence, not owner aesthetic acceptance.

VISUAL_CAPTURE_EVIDENCE.json records the joined ZIP SHA256127c2d1d9ae5838d37f8373a2495a5f461c135efa30d5d3e474d385befa0bb06 and10ordered parts, each individually hashed. Join part01..part10 verbatim to obtain the ZIP. APNGs preserve the complete864-frame sequences. No 2x/performance or silent-settling acceptance inferred: old/new composer outputs differ by design, so identical-PCM telemetry/kernel input comparison remains needed for numeric sensitivity.

STOP: existing-site source-opening failure from BLOCKED_CHECKPOINT.md remains unresolved; no Sites recovery attempt or deployment was repeated this turn, per owner instruction. Supported helper's only reported failure was `Unable to prepare the Site.`; underlying infrastructure cause has not been established. Repeated invocation without concrete recovery is forbidden. Publication remains blocked, public0.6.9 untouched.

Still open: identical-stimulus visual motion/morphology/quiet/STOP checks, V3>=2x evidence,20FPS/p95/input/worker fallback, final bank/tempo-lock and score/stem/listening acceptance, final native/repro/single/WASM/updater610, complete requirements reconciliation and staged/live publication. Preserve prior passing tests/source/native development archive. No claim that all #64–#71 software gates pass. AIR/KIRA-9/rollback and #62/performance-MIDI deferrals unchanged.
