#!/usr/bin/env bash
set -euo pipefail
base="${1:-$HOME/.vst3}"
dest="$base/Vocalis Voice.vst3"
[[ ! -L "$dest" ]] || { echo 'Refusing to delete a symlink.' >&2; exit 1; }
[[ -e "$dest" ]] || { echo 'Vocalis Voice is not installed.'; exit 0; }
[[ -f "$dest/.vocalis-voice-owned" ]] || { echo 'Refusing to remove an unmarked bundle.' >&2; exit 1; }
[[ "$(cat "$dest/.vocalis-voice-owned")" == 'audio.vocalis.voice 1.0.0' ]] || { echo 'Ownership marker mismatch.' >&2; exit 1; }
rm -rf -- "$dest"
echo "Removed: $dest (backups and other plugins retained)"
