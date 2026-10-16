#!/usr/bin/env python3
"""Strict float32 audio regression: no rounding or relaxed tolerance."""
import array, hashlib, json, sys
from pathlib import Path
if len(sys.argv) != 3:
    raise SystemExit('Usage: CompareAudio.py baseline.f32 candidate.f32')
a_path, b_path = map(Path, sys.argv[1:])
a, b = array.array('f'), array.array('f')
a.frombytes(a_path.read_bytes()); b.frombytes(b_path.read_bytes())
if len(a) != len(b):
    raise SystemExit('FAIL: sample count differs')
differences = [abs(x-y) for x, y in zip(a,b)]
result = dict(samples=len(a), different_samples=sum(x!=y for x,y in zip(a,b)),
              max_absolute_error=max(differences,default=0),
              rms_error=(sum(x*x for x in differences)/max(1,len(a)))**.5,
              tolerance=0.0, baseline_sha256=hashlib.sha256(a_path.read_bytes()).hexdigest(),
              candidate_sha256=hashlib.sha256(b_path.read_bytes()).hexdigest())
print(json.dumps(result,indent=2))
raise SystemExit(0 if result['different_samples']==0 else 1)
