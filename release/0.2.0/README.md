# AQUORA 0.2.0

FM-1 hardware-test release. The updater family remains FM-1_73. This image identifies as FM-1_73000200. The owner verified 0.1.0 installation and liked its general sound. Keep that version available for rollback.

- PLAY stops generation immediately and fades the whole mix to silence over 256 frames. Restart begins with fresh voices and effect history. Playalong requires running transport.
- SELECT now exposes seven pitched tracks and Drums on track 8. Bass has more presence and Sacred Bass has independent level, source and harmonic blend. Drums now offers 15 synthesized kits and a clear mixer route.
- Anti-Curse and Positive Healing use different scales, arrangements, kits, bass progressions, nature and FX. They are artistic intentions.
- Perfect Key defaults on. LFO toggles it from GLO diagnostics. Lead and Bass have independent glide mode/time and vibrato rate/depth in SOUND → Motion. STOP and SAVE to retain these settings.
- Larger colored geometry reacts to role note events and drum activity. GLO separates measured Audio and FX processing and exposes callback timing, DMA xruns and core/worker diagnostics.

## Evidence and limits

Native and forced one-core builds, sanitizer checks, stereo tests, 15 isolated kit renders, all eight mixer routes, the 27 keys in all eight scenes, motion persistence and five simulated worker modes are covered in evidence/0.2.0/. See ab.log for 0.1.0 versus 0.2.0 under identical host workload inputs. The engine outputs differ by design. Host timing is a comparison only and cannot establish FM-1 CPU headroom. No physical 0.2.0 load, direct upgrade, recovery or actual dual-core execution has been verified.

## Hardware checklist

1. Keep the 0.1.0 rollback package and stock recovery available. In Chrome or Edge allow MIDI SysEx for the installer. Start with physical master at zero.
2. Install 0.2.0. Confirm AQUORA version and silent startup. Raise the master gradually. Repeatedly STOP under full bed/FX/drums and binaural load with a key held, then restart. Check audible bass and Drums, kit selection, Perfect Key and Lead/Bass motion persistence.
3. Capture both GLO screens on the same typical and stress scenes on 0.1.0 and 0.2.0. Compare Audio/FX/callback means, p95, peak, xruns, voice count, and core-1 busy and fallback data. Repeat after REC forces core 0. Seek stable typical CPU under 70%, peak callback well below 5.805 ms and zero DMA xruns during a 60-minute stress soak. Record device and bootloader details.
4. Listen on stereo headphones at low volume for proper binaural separation. Compare the new musical balance to 0.1.0. Record any clicks, artifacts or underruns. Verify direct update and rollback on the physical device before calling recovery verified.

## Recover matching archives from GitHub

Concatenate source.part-* in lexical order to AQUORA-0.2.0-source.tar.gz, and package.part-* to AQUORA-0.2.0-package.tar.gz. Check both against SHA256SUMS.txt. The source includes pinned platform/SDK files and acceptance scripts. The package includes the flashable firmware, installer, emulator, manual and licenses. Preserve release/0.1.0/ for rollback.
