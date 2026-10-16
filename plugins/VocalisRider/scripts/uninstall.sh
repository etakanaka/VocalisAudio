#!/usr/bin/env bash
set -euo pipefail
bundle="$HOME/.vst3/Vocalis Rider Live.vst3"
if [[ -d "$bundle" ]]; then
 rm -r -- "$bundle"
 echo "Removed $bundle"
else
 echo 'Vocalis Rider Live is not installed in ~/.vst3/'
fi
