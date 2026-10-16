#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0}"
# LeakSanitizer needs a runtime that can inspect threads; this sandbox blocks it.
if [[ "${1:-}" == '--full' ]]; then
 cmake -S "$root" -B "$root/build/ASan" -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined' \
  -DCMAKE_SHARED_LINKER_FLAGS='-fsanitize=address,undefined'
 cmake --build "$root/build/ASan" --target VoiceTests GuiTests AuditDspTests ThreadStress VocalisVoice_VST3 --parallel "${JOBS:-3}"
 "$root/build/ASan/VoiceTests"
 "$root/build/ASan/GuiTests" "$root/artifacts/audit/asan-gui"
 "$root/build/ASan/AuditDspTests"
 "$root/build/ASan/ThreadStress"
else
 mkdir -p "$root/build/Sanitized"
 for name in DspTests AuditDspTests; do
  "${CXX:-g++}" -std=c++20 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -I"$root/src" "$root/tests/$name.cpp" -o "$root/build/Sanitized/$name"
  "$root/build/Sanitized/$name"
 done
fi
