# Vocalis Voice main-version synchronisation

Date: 2026-10-18
Status: **Completed and validated; uncommitted.**

## Scope and provenance

- Approved source: `/home/enos/AudioDevelopment/VocalisVoice`
- Central destination: `plugins/VocalisVoice`
- Prepared overlay: `/tmp/VocalisAudio-sync-staging/plugins/VocalisVoice`
- Existing destination backup retained: `/tmp/VocalisVoice-central-destination-before-sync-20261018.tar.gz`
- Central Git working tree was clean before synchronisation.

All 81 staged plugin files were checked against the current source. Only `artifacts/author-attribution/about.png` differed; staging was refreshed from the current source before copying. All source files under `src`, `tests`, `scripts`, and `docs` were present in staging. The overlay includes CMake, README, formatting/ignore configuration, licences/notices, the approved reference image, and selected GUI screenshots. Every copied file was verified byte for byte against both source and refreshed staging after validation.

The copy was additive: existing destination-only historical images, checkpoints and artifacts were retained. The central plugin is therefore an exact match for the approved 81-file overlay, not an exact directory mirror of source build/history directories. No build cache, compiled object, binary bundle, temporary file, log or local Git metadata was copied. Build outputs and validation logs are under `/tmp/VocalisVoice-central-validation` and `/tmp/VocalisVoice-central-build.log`.

## GUI and compatibility preservation

The default editor is **960 × 617**, with the approved appearance and sizes 840 × 540 and 1200 × 771 preserved. Compiled GUI tests assert the default and render all three sizes. The resulting 960 × 617 image was visually inspected. Test-generated About screenshot output was restored to the approved source screenshot after tests, preserving exact source parity.

`newdesign.png` SHA-256: `c6fbe6854699a0bd1353cb148dd869a501de3646621846d754bbe6d18c2944c7`.

CMake, PluginProcessor, all DSP files, and parameter/preset/state sources are byte-identical to central HEAD. Plugin identity remains version 1.0.0, manufacturer/product codes `Vcls` / `Vvce`, bundle ID `audio.vocalis.voice`.

- Processor class ID: `ABCDEF019182FAEB56636C7356766365`
- Controller class ID: `ABCDEF011234ABCD56636C7356766365`

All unrelated files were hashed before synchronisation and verified unchanged afterwards, including ignored files and every other plugin. **Vocalis Rider Live was not changed.** No existing user changes were present or overwritten.

## Validation from the actual central source

Fresh configuration and build succeeded:

```sh
cmake -S /home/enos/AudioDevelopment/VocalisAudio/plugins/VocalisVoice -B /tmp/VocalisVoice-central-validation -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=/home/enos/AudioDevelopment/JUCE -DVOCALIS_BUILD_TESTS=ON
cmake --build /tmp/VocalisVoice-central-validation --parallel 8
ctest --test-dir /tmp/VocalisVoice-central-validation --output-on-failure
```

- CTest: **7/7 passed** — voice_tests, dsp_tests, gui_tests, design_parity, audit_dsp_tests, thread_stress, callback_allocation.
- DSP benchmark: **28 passed, 0 failed; audio allocations 0**.
- VST3 probe: discovery, all 11 host parameter names/order, mono/stereo layouts, measured automation, state roundtrip and repeated load/unload passed.
- Legacy compatibility: all ten parameters and preset metadata restored from the staged prior 1.0.0 release bundle.
- AudioRegression and CompareAudio: **5,255,600 samples; zero differing samples; maximum and RMS error 0**, compared with the previously validated staged build. Both SHA-256 values: `b4ba52654f1adf73488eb4720520224158eb6d9a739c07f3a3ea6a19ff34afda`.
- InstallerTests.py: install hash, reinstall backup, uninstall, unmarked/symlink protection and Rider sentinel preservation passed. An initial test setup using a bundle symlink correctly triggered installer symlink protection; rerun using a temporary build-directory link passed. Temporary links were removed.
- `git diff --check` passed.

Native VST3 editor attachment remains **unverified** because `XOpenDisplay` failed in this environment. Offscreen compiled-editor rendering and GUI/design tests passed. No DAW session was opened.

## Final state

Synchronisation is complete. The only working-tree changes are within `plugins/VocalisVoice` and this report. Existing destination-only history remains intact. No permission restriction occurred. No commit, push, tag, release or publication was performed.
