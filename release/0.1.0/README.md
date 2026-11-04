# AQUORA 0.1.0

First M-VAVE FM-1 hardware-test build. The package identity is FM-1_73000100. AQUORA includes 24 synthesized patches, six generative scenes, three percussion kits, seven nature beds, 11 selectable fixed tones, optional A4=432 Hz, adjustable stereo binaural tones, and a four-page interface with a reactive visualizer.

Native, host, and browser emulator builds use the same source. The native renderer runs in SRAM. The second core handles visual geometry and falls back to core 0 on failure. Real-device timing, output and flashing still need owner verification.

## Reconstruct the backup

From this directory, concatenate `source.part-*` in lexical order to create `AQUORA-0.1.0-source.tar.gz`. Concatenate `package.part-*` in lexical order to create `AQUORA-0.1.0-package.tar.gz`. Run `sha256sum -c SHA256SUMS.txt` before extracting. The source archive includes the pinned platform and the SDK packaging files used by this build. The package archive includes firmware, emulator, installer, licenses, and manual.

## Hardware checklist

1. Preserve a stock backup and use a powered USB connection. Start with physical master at zero. Use the standard application installer in Chrome or Edge.
2. Confirm the device returns as AQUORA and starts silently. Raise master gradually. Check PLAY, all four pages, live keys, and clean Stop.
3. With stereo headphones at low volume, test the binaural channel pair. Check drums, nature, reverb and delay at high density.
4. Check GLO diagnostics for sustained load and underruns. Compare the geometry with REC forcing one core.
5. Confirm the normal updater handshake. Recovery and direct upgrades require physical verification. An interrupted write can require recovery.
