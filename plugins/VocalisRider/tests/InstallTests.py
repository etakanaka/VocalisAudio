#!/usr/bin/env python3
"""Isolated HOME fault-injection tests; never touches the user's installation."""
import os,pathlib,subprocess,tempfile,sys
script=pathlib.Path(sys.argv[1]).resolve() if len(sys.argv)>1 else pathlib.Path(__file__).resolve().parents[1]/'scripts/install.sh'
failures=0
for fault in ['copy','commit','normal']:
 if fault=='commit' and 'stage=' not in script.read_text():
  print('UNAVAILABLE installer commit injection: baseline has no staged commit');continue
 with tempfile.TemporaryDirectory(prefix='vocalis-install-') as d:
  home=pathlib.Path(d);dest=home/'.vst3/Vocalis Rider Live.vst3';dest.mkdir(parents=True);(dest/'old-marker').write_text('original')
  unrelated=home/'.vst3/Other.vst3';unrelated.mkdir();(unrelated/'keep').write_text('user')
  tools=home/'tools';tools.mkdir()
  if fault=='copy':
   f=tools/'cp';f.write_text('#!/bin/sh\nexit 86\n');f.chmod(0o755)
  if fault=='commit':
   f=tools/'mv';f.write_text('#!/bin/sh\nif [ "$1" = "--" ]; then shift; fi\ncase "$1" in */.vocalis-stage.*/*) exit 87;; esac\nexec /usr/bin/mv "$@"\n');f.chmod(0o755)
  env=dict(os.environ,HOME=d,PATH=str(tools)+':'+os.environ['PATH']);r=subprocess.run([str(script)],env=env,capture_output=True,text=True)
  ok=(r.returncode!=0 and (dest/'old-marker').exists()) if fault!='normal' else r.returncode==0 and (dest/'Contents/x86_64-linux/Vocalis Rider Live.so').is_file()
  ok &= (unrelated/'keep').read_text()=='user'
  if fault=='normal':
   for _ in range(2):ok &=subprocess.run([str(script)],env=env,capture_output=True).returncode==0
   backups=list((home/'.vst3').glob('Vocalis Rider Live.vst3.backup-*'));ok &=len(backups)==3 and any((p/'old-marker').exists() for p in backups)
  failures+=not ok;print(('PASS' if ok else 'FAIL')+' installer '+fault+' preserves installation/unrelated data')
sys.exit(1 if failures else 0)
