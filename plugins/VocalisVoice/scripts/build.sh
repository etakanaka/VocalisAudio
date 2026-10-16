#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
mode="${1:-Release}"
case "$mode" in Debug|Release) ;; *) echo 'Usage: build.sh [Debug|Release]' >&2; exit 2;; esac
cmake -S "$root" -B "$root/build/$mode" -G Ninja -DCMAKE_BUILD_TYPE="$mode" -DJUCE_PATH="${JUCE_PATH:-$HOME/AudioDevelopment/JUCE}"
cmake --build "$root/build/$mode" --parallel "${JOBS:-3}"
