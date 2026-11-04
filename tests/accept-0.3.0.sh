#!/usr/bin/env bash
set -euo pipefail
out="${1:?isolated evidence directory required}"
mkdir -p "$out"
out="$(realpath "$out")"
cc_cmd=(cc -Wall -Wextra -Werror -g -DSR_DEVICE -DPLAT_HOST -Idevice/dsp -Ivendor/fm1-base/include)
for test in transport_hit next_acceptance dsp_acceptance stereo; do
  "${cc_cmd[@]}" -O1 -fsanitize=address,undefined device/dsp/engine.c "tests/$test.c" -lm -o "$out/$test"
  ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 "$out/$test" > "$out/$test.log" 2>&1
done
python3 vendor/fm1-base/tools/build.py device --host --release 0.3.0 > "$out/host-build.log" 2>&1
host=device/build/host/sound-healing_host
"$host" tests/ui.txt "$out" > "$out/ui.log" 2>&1
"$host" tests/motion_store.txt "$out" > "$out/motion-store.log" 2>&1
cc -O2 -Ivendor/fm1-base/include tests/bank_flash.c -o "$out/bank_flash"
"$out/bank_flash" > "$out/bank-flash.log"
for test in arrangements knob_matrix percussion_crystal; do
  "$host" "tests/$test.txt" "$out" > "$out/$test.log" 2>&1
done
! cmp -s "$out/crystals.png" "$out/crystals-later.png"
sed 's/^spin K4 1$/spin K4 -1/;s/^expect visual 1$/expect visual 0/' tests/percussion_crystal.txt > "$out/original.txt"
mkdir -p "$out/original"
"$host" "$out/original.txt" "$out/original" > "$out/original.log" 2>&1
cmp "$out/crystal.wav" "$out/original/crystal.wav"
! cmp -s "$out/crystals.png" "$out/original/crystals.png"
for mode in ok off late bad fault; do
  mkdir -p "$out/worker-$mode"
  cp tests/percussion_crystal.txt "$out/worker-$mode/commands.txt"
  expected_on=0
  expected_fallbacks=0
  case "$mode" in ok) expected_on=1;; late|bad|fault) expected_fallbacks=1;; esac
  printf 'expect worker %s\nexpect fallbacks %s\n' "$expected_on" "$expected_fallbacks" >> "$out/worker-$mode/commands.txt"
  FM1_WORKER_TEST="$mode" "$host" "$out/worker-$mode/commands.txt" "$out/worker-$mode" > "$out/worker-$mode.log" 2>&1
  cmp "$out/crystal.wav" "$out/worker-$mode/crystal.wav"
  cmp "$out/crystals.png" "$out/worker-$mode/crystals.png"
done
echo 'PASS two distinct, evolving visualizers, bit-identical audio and deterministic crystal fallback across worker modes' > "$out/worker.log"
python3 vendor/fm1-base/tools/build.py device --sdk vendor/ac79-package-files --release 0.3.0 > "$out/native.log" 2>&1
single="$(mktemp -d "$out/single-core-XXXXXX")"
tar --exclude=build -cf - -C device . | tar -xf - -C "$single"
FM1B_CPU2=0 python3 vendor/fm1-base/tools/build.py "$single" --sdk vendor/ac79-package-files --release 0.3.0 > "$out/singlecore.log" 2>&1
python3 vendor/fm1-base/tools/build.py device --web --release 0.3.0 > "$out/web.log" 2>&1
node vendor/fm1-base/tests/installer_ports_test.mjs device/build/sound-healing-0.3.0.fwsc > "$out/installer_ports.log" 2>&1
