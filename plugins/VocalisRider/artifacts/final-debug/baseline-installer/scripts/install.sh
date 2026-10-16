#!/usr/bin/env bash
set -euo pipefail
project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
bundle="Vocalis Rider Live.vst3"
source_bundle="$project_dir/build/VocalisRiderLive_artefacts/Release/VST3/$bundle"
if [[ ! -d "$source_bundle" && -d "$project_dir/$bundle" ]]; then source_bundle="$project_dir/$bundle"; fi
[[ -d "$source_bundle" ]] || { echo 'Build first using scripts/rebuild.sh' >&2; exit 1; }
mkdir -p "$HOME/.vst3"
if [[ -e "$HOME/.vst3/$bundle" ]]; then
  backup="$HOME/.vst3/$bundle.backup-$(date +%Y%m%d%H%M%S)"
  mv -- "$HOME/.vst3/$bundle" "$backup"
  echo "Previous bundle backed up to $backup"
fi
cp -a -- "$source_bundle" "$HOME/.vst3/$bundle"
echo "Installed $HOME/.vst3/$bundle"
