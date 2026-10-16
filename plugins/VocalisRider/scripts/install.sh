#!/usr/bin/env bash
set -euo pipefail
project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
bundle="Vocalis Rider Live.vst3"
source_bundle="$project_dir/build-release/VocalisRiderLive_artefacts/Release/VST3/$bundle"
if [[ ! -d "$source_bundle" ]]; then source_bundle="$project_dir/build/VocalisRiderLive_artefacts/Release/VST3/$bundle"; fi
if [[ ! -d "$source_bundle" && -d "$project_dir/$bundle" ]]; then source_bundle="$project_dir/$bundle"; fi
[[ -f "$source_bundle/Contents/x86_64-linux/Vocalis Rider Live.so" ]] || { echo 'Build first using scripts/rebuild.sh' >&2; exit 1; }
mkdir -p "$HOME/.vst3"
stage="$(mktemp -d "$HOME/.vst3/.vocalis-stage.XXXXXXXX")"
trap 'rm -rf -- "$stage"' EXIT
# Complete the copy before disturbing the working installation.
cp -a -- "$source_bundle" "$stage/$bundle"
destination="$HOME/.vst3/$bundle"
backup=""
if [[ -e "$destination" || -L "$destination" ]]; then
  backup="$destination.backup-$(date +%Y%m%d%H%M%S)-${stage##*.}"
  [[ ! -e "$backup" ]] || { echo 'Backup destination already exists; no files replaced.' >&2; exit 1; }
  mv -- "$destination" "$backup"
fi
if ! mv -- "$stage/$bundle" "$destination"; then
  if [[ -n "$backup" ]]; then mv -- "$backup" "$destination"; fi
  echo 'Install failed; previous installation restored.' >&2
  exit 1
fi
if [[ -n "$backup" ]]; then echo "Previous bundle backed up to $backup"; fi
echo "Installed $destination"
