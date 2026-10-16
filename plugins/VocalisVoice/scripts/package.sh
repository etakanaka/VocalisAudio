#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
python3 - "$root" <<'PY'
from pathlib import Path
import sys, tarfile, hashlib
root = Path(sys.argv[1])
bundle = root / 'build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3'
if not (bundle / 'Contents/x86_64-linux/Vocalis Voice.so').is_file():
    raise SystemExit('Build Release before packaging')
out = root / 'dist'
out.mkdir(exist_ok=True)
name = 'VocalisVoice-1.0.0-audited-linux-x86_64'
archive = out / (name + '.tar.gz')
with tarfile.open(archive, 'w:gz') as package:
    package.add(bundle, arcname=name + '/Vocalis Voice.vst3')
    for relative in ('README.md', 'LICENSE.md', 'NOTICE.md', 'CMakeLists.txt', '.clang-format', '.gitignore', 'src', 'tests', 'scripts', 'docs'):
        package.add(root / relative, arcname=name + '/' + relative)
    for relative in ('gui.png', 'gui-80-percent.png', 'gui-layout-1-1.png', 'gui-layout-1-2.png', 'gui-layout-2-2.png', 'dsp-results.txt', 'processor-results.txt', 'gui-results.txt', 'vst3-results.txt', 'full-validation.txt', 'sanitizer-results.txt', 'install-test.txt', 'debug-validation.txt'):
        path = root / 'artifacts' / relative
        if path.exists():
            package.add(path, arcname=name + '/artifacts/' + relative)
    audit = root / 'artifacts/audit'
    for path in sorted(audit.iterdir()):
        if path.is_file() and path.name not in ('package.txt', 'package-verification.txt') and path.suffix in ('.txt', '.json', '.png'):
            package.add(path, arcname=name + '/artifacts/audit/' + path.name)
checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
(archive.parent / (archive.name + '.sha256')).write_text(checksum + '  ' + archive.name + '\n')
print(archive)
print('SHA256 ' + checksum)
PY
