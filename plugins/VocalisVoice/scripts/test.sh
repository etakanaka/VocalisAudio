#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
mode="${1:-Release}"
case "$mode" in Debug|Release) ;; *) exit 2;; esac
ctest --test-dir "$root/build/$mode" --output-on-failure
"$root/build/$mode/DspTests" --benchmark
probe_args=("$root/build/$mode/VocalisVoice_artefacts/$mode/VST3/Vocalis Voice.vst3" "$root/artifacts/native-gui.png")
if [[ -n "${BASELINE_VST3:-}" ]]; then probe_args+=("$BASELINE_VST3"); fi
# Audio/state discovery still runs headlessly; the probe gates native XEmbed itself.
"$root/build/$mode/Vst3Probe_artefacts/$mode/Vst3Probe" "${probe_args[@]}"
python3 "$root/tests/InstallerTests.py"
