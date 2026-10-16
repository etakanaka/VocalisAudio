#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "$root/build/TSan"
"${CXX:-g++}" -std=c++20 -g -fsanitize=thread -pthread "$root/tests/SanitizerRuntimeProbe.cpp" -o "$root/build/TSan/runtime-probe"
if ! "$root/build/TSan/runtime-probe"; then
 echo 'BLOCKED: ThreadSanitizer runtime is unavailable; no race-free claim.' >&2
 exit 77
fi
cmake -S "$root" -B "$root/build/TSan" -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
 -DCMAKE_CXX_FLAGS='-fsanitize=thread -fno-omit-frame-pointer' \
 -DCMAKE_C_FLAGS='-fsanitize=thread -fno-omit-frame-pointer' \
 -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=thread
cmake --build "$root/build/TSan" --target ThreadStress GuiTests --parallel "${JOBS:-3}"
"$root/build/TSan/ThreadStress"

"$root/build/TSan/GuiTests" "$root/artifacts/audit/tsan-gui"
