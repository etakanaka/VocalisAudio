#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
# An alternate directory supports staging and installer tests without touching the DAW.
base="${1:-$HOME/.vst3}"
source_bundle="$root/Vocalis Voice.vst3"
if [[ ! -d "$source_bundle" ]]; then
 source_bundle="$root/build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3"
fi
[[ -f "$source_bundle/Contents/x86_64-linux/Vocalis Voice.so" ]] || { echo 'Build Release first.' >&2; exit 1; }
mkdir -p -- "$base"
dest="$base/Vocalis Voice.vst3"
[[ ! -L "$dest" ]] || { echo 'Refusing to replace a symlink.' >&2; exit 1; }
if [[ -e "$dest" && ! -f "$dest/.vocalis-voice-owned" ]]; then
 # Recognise earlier manually installed Voice builds by their independent identity.
 # Unknown bundles remain protected; matching builds are backed up, never deleted.
 if ! python3 - "$dest" <<'PYIDENTITY'
from pathlib import Path
import re, sys
bundle=Path(sys.argv[1])
manifest=bundle/'Contents/Resources/moduleinfo.json'
try:
    text=manifest.read_text()
    valid=all(re.search(r'"CID"\s*:\s*"'+cid+r'"',text) for cid in
              ['ABCDEF019182FAEB56636C7356766365','ABCDEF011234ABCD56636C7356766365'])
    valid=valid and (bundle/'Contents/x86_64-linux/Vocalis Voice.so').is_file()
except OSError:
    valid=False
raise SystemExit(0 if valid else 1)
PYIDENTITY
 then
  echo 'Existing unmarked bundle is not identifiable as Vocalis Voice; refusing replacement.' >&2; exit 1
 fi
fi
staging="$(mktemp -d "$base/.vocalis-voice-install.XXXXXXXX")"
trap 'rm -rf -- "$staging"' EXIT
cp -a -- "$source_bundle" "$staging/Vocalis Voice.vst3"
printf '%s\n' 'audio.vocalis.voice 1.0.0' > "$staging/Vocalis Voice.vst3/.vocalis-voice-owned"
if [[ -e "$dest" ]]; then
 backup="$dest.backup-$(date +%Y%m%d%H%M%S)-$$"
 mv -- "$dest" "$backup"
 echo "Previous Voice bundle retained: $backup"
fi
mv -- "$staging/Vocalis Voice.vst3" "$dest"
echo "Installed: $dest"
