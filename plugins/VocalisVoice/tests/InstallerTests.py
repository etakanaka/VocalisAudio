#!/usr/bin/env python3
import hashlib
from pathlib import Path
import subprocess
import tempfile
root = Path(__file__).resolve().parents[1]
def run(script, dest, ok=True):
    result = subprocess.run([str(root / 'scripts' / script), str(dest)], capture_output=True, text=True)
    print((result.stdout + result.stderr).strip())
    if bool(result.returncode == 0) != ok:
        raise RuntimeError(result.stderr)
with tempfile.TemporaryDirectory(prefix='vocalis-voice-installer-', dir='/tmp') as directory:
    dest = Path(directory)
    rider = dest / 'Vocalis Rider Live.vst3'
    rider.mkdir()
    sentinel = rider / 'sentinel'
    sentinel.write_text('must remain unchanged')
    run('install.sh', dest)
    bundle = dest / 'Vocalis Voice.vst3'
    original_bundle = root / 'Vocalis Voice.vst3'
    if not original_bundle.exists():
        original_bundle = root / 'build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3'
    original = original_bundle / 'Contents/x86_64-linux/Vocalis Voice.so'
    installed = bundle / 'Contents/x86_64-linux/Vocalis Voice.so'
    assert hashlib.sha256(original.read_bytes()).digest() == hashlib.sha256(installed.read_bytes()).digest()
    run('install.sh', dest)
    assert len(list(dest.glob('Vocalis Voice.vst3.backup-*'))) == 1
    (bundle / '.vocalis-voice-owned').unlink()
    run('install.sh', dest)
    assert len(list(dest.glob('Vocalis Voice.vst3.backup-*'))) == 2
    run('uninstall.sh', dest)
    assert not bundle.exists()
    assert sentinel.read_text() == 'must remain unchanged'
    bundle.mkdir()
    run('uninstall.sh', dest, ok=False)
    run('install.sh', dest, ok=False)
    bundle.rmdir()
    bundle.symlink_to(rider, target_is_directory=True)
    run('uninstall.sh', dest, ok=False)
    run('install.sh', dest, ok=False)
    assert sentinel.read_text() == 'must remain unchanged'
print('PASS install hash / reinstall backup / uninstall / unmarked and symlink protection / Rider untouched')
