# Final release recovery audit — 2026-11-08

Base verified GitHub checkpoint: 0263e6e4e364db1aec2b3a2687ebd07e5dd49b82, aquora-next-integration.
Prior recovery: Issue #62 comment 6718734372.

## Ownership
One bounded process/lock check: existing owner remains session 56c15b7cf3b9 at /workspace/.aquora-release.lock/owner. No running compiler, make, or site-workflow uploader was observed. Continued read-only inspection of its preserved checkout; no lock removed or reassigned, no firmware changes or builds.

## Website size correction verified, not deployed
Preserved archive /workspace/scratch/56c15b7cf3b9/site069-publication-25m.tar.
SHA256 9fc2eeeab304cc4d22d5973d22711e3947189ad5f73cce284dfedd0f534cddb4.
Every regular tar member is <=26214400 bytes. Largest members are two source parts of 25165824 bytes each (24 MiB). This fixes the failed deployment's per-file size cause. Archive uses dist/; do not substitute the checkout's out/ manifest blindly. Repackage after #62 changes; existing firmware cannot be published as #62-compliant.

## #62 blocked by unverified actual flash paths
Actual compiled source: /workspace/scratch/56c15b7cf3b9/source.
- vendor/fm1-base/device/main_fm1.c:897 accepts only JEDEC 0x856014. New chip currently remains RAM-only.
- vendor/fm1-base/bsp/tools/fm1_rescue.py:252 accepts only key/type/id (0x980F,3,0x856014).
- Packaged SysEx RAM updater is source-built via tools/build.py:build_loader(), using bsp/loader/loader.c and chip/fm1_flash.h. It can be changed reproducibly; it is NOT the opaque rescue loader.
- USB UBOOT rescue first uploads a DIFFERENT encrypted vendor wl82loader.bin, before querying JEDEC. Pinned URL is kagaimiq/jl-uboot-tool commit c32b28e7a04f873f526ba929cb1bae9f05442fe9, data/loaderblobs/usb/wl82loader.bin. Retrieved once, 24064 bytes, verified SHA256 d41da6126760c9d66660bcc0cac8d27d221806c5e369a8036921efe68dca5376. Its source is absent. Hash provenance does not establish UC25HQ80 support. No claim that this binary is known incompatible; its UC25HQ80 initialization/protection/write behavior is UNVERIFIED, and host simulations cannot prove it.
- device/aquora_async_store.h and bsp/chip/fm1_flash_aquora.h use P25Q80H-specific 75h/7Ah erase/program suspend/resume and SR2 mask 0x84. #62 establishes equivalence only for standard read/program/erase/status/write-enable commands, not suspend/resume or suspend-status semantics. Applying an ID-only allowlist here would assert unsupported behavior.
- fm1_flash.h basic RAM erase/program uses WEL, busy polling, page/sector and partition bounds. Preserve these and all identity/CRC/head/data guards.

Required to unblock: auditable UC25HQ80 command/status evidence or a bounded verified alternative for live persistence, plus evidence that the pinned rescue RAM loader handles this chip (or an auditable compatible replacement without foreign firmware/loader substitution). Then implement exact two-ID policy in actual app/update/rescue paths, tracked reconstruction overlay, changed-path tests, and affected native/repro/single-core packages. Physical testing remains owner work; no physical results invented.

No successful tests repeated, no affected artifact rebuilt before this blocker is resolved. No publication. Existing 067/068 rollback, accepted 069 artifacts, source, AIR, startup and KIRA-9 untouched.
