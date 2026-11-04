# AQUORA 0.1.0 checkpoint

Native SRAM link fixed. The local release uses pi32v2/r3 and single precision. See evidence/ for native and forced-single-core builds, host UI, sanitized DSP, stereo, simulated worker fallback, and simulated updater results. The release/0.1.0 manifest records matching firmware and archive hashes. Physical FM-1 verification is open.

Build inputs follow FAST_RESUME.md. Source snapshot includes fm1-base 2d8ed748cdade582c65d9fc8d8cf17da4ddadec8, BSP 773b0b99f5ade1a30d1879b2618c6c3c3e6ba031, and SDK packaging files from 5fa84a9b2f723a5e9337ad79dc2ad741fc98fe2d. The SDK boot and EQ packaging files differ from the platform's historical reference; evidence/sdk-sha256.log pins the exact inputs. Physical compatibility remains to be tested.
