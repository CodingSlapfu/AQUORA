# AQUORA 0.3.0 publication

- Verified GitHub release backup `556cc715d190d5cf17d407a00e17b8e1127ec441`. Reassembled both archives from GitHub and validated SHA-256. Compared the extracted firmware byte-for-byte with the native build.
- Production site commit `1b04358f002dad0a5b870b8abb8f3dddca610431`, deployment `appgdep_6aeb50f8f8308191941c24a7bf13fc16` succeeded. Existing owner-only audience preserved.
- Live assets below returned HTTP 200 and matched the tested bytes. Installer identity is `FM-1_73000300`. Both rollback installers and firmware were checked.
- Native board timing, intermittent-note reproduction, power-cycle persistence and physical update verification remain owner hardware checks.

```text
200 / 6313 bytes 1b16804271cfb16985b84c784753d4b74f2218c41828d3977ed5b1aa39ff1311
200 /install/ 69516 bytes b4dbaafbe8f416e7dcd7bab374226924b8c41fd426aa53c72ab2b5abae92471a
200 /emu/app.wasm 80788 bytes e2cc7d7cd655db817be1d19a4c4cf349479c890471abf6b59a650d16056789e3
200 /firmware/sound-healing-0.3.0.fwsc 611496 bytes 8ed7742635bb52b55274cc0ce1c8ee431ef8738871af59211818721e7c1bc6da
200 /AQUORA-0.3.0-package.tar.gz 371500 bytes 3150e9cb67277c7f86bdfbc2d93b53ace28ae7272f622a2899518259d121929f
200 /AQUORA-0.3.0-source.tar.gz 1636205 bytes 8abc1070b34727bc13100767ea0eb110f6ad7c0fc90c83607e9a990bf8bfe722
200 /rollback/0.1.0/install/ 69516 bytes 97e937dc2f6028155cd14733482a873b3c90470cebe10619eee89506bbc2d784
200 /rollback/0.1.0/firmware/sound-healing-0.1.0.fwsc 611496 bytes 35d16b6df0fe096c0aa7b205ff17f150c9e0ea0f558c42d9fe853d27fe936344
200 /rollback/0.2.0/install/ 69516 bytes 6e7154b150296b2955a032092e918e9970d6b4fde4ab5c82691719e3c2c13a72
200 /rollback/0.2.0/firmware/sound-healing-0.2.0.fwsc 611496 bytes 2af13829f89c3a40b4f3d11956fcf695c695f1df3d82fdd4c9776406ce2ee8bf
```
