# AQUORA v0.7.29 testing checkpoint

Firmware source db5e454df0e30f9adfdc911ac39ec22c84c01381, branch
aquora-v0.7.29-corrective-development. It extends the immutable
v0.7.28 backup faea6efd67d82face1da3d675bcd4622a74eed64. The prior site
version 221 saved successfully during the pause and had no deployment.
Keep its source, saved version, and artifacts. The stable installer remains
v0.7.14.

Do not repeat already successful build and regression checks. Obtain the
publication lease before editing or publishing. See ACCEPTANCE.md, logs
and SHA256SUMS. The original allocator, two-slot policy and dynamic voice
cap remain in place. V11 uses reserved bits 11 and 12 for Vocal Flow.
Older builds ignore those bits. Existing saved-word hooks retain their old
IDs. Physical FM-1 timing and subjective musical acceptance are open.
