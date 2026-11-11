# AQUORA 0.7.4 development

The latest official release is [v0.7.3](https://github.com/CodingSlapfu/AQUORA/releases/tag/v0.7.3), identity FM-1_73000703. Its accepted software checkpoint and exact assets are recorded in validation/issue75/PUBLICATION.json.

Development branch aquora-0.7.4-development builds a separate candidate with identity FM-1_73000704. See the v0.7.4 delta in validation/NEXT_BUILD_REQUIREMENT_MATRIX.md for remaining owner requirements. The new candidate preserves prior releases and rollback.

[#76](https://github.com/CodingSlapfu/AQUORA/issues/76) remains open. The 0.7.2 and 0.7.3 packages have matching marker layouts, UFW entry layouts and byte-identical OTA loaders. Both pass container CRC and padding validation. Those checks do not reproduce the Windows vendor validator or establish hardware acceptance.

The existing Site project is appgprj_6ad6d0ae409c8191b45224ecdec724db. It still needs the matching installer, manuals, and release content after release acceptance. The prior 0.7.3 publication record and original Puya 0x856014 hardware scope still apply. Performance USB/BLE MIDI and UC25HQ80 remain deferred.
