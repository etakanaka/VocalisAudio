#!/usr/bin/env bash
set -euo pipefail
project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cmake -S "$project_dir" -B "$project_dir/build-release" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$project_dir/build-release" --parallel "${BUILD_JOBS:-2}"
ctest --test-dir "$project_dir/build-release" --output-on-failure
