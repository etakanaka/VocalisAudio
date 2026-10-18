# Voice audio and control regression

## Baseline

Full accessible workspace checkpoint: `checkpoints/VocalisVoice-checkpoint-20261016-073330/workspace.tar`. Git status failed because the mounted .git is not a usable repository. Source hashes, previous Release/Debug binaries, untracked files, archives and previous GUI captures were preserved before modifications. Baseline build and existing Release tests executed successfully; logs are in `artifacts/audit/baseline-build.txt` and `baseline-tests.txt`.

## Numerical audio comparison — VERIFIED PASS

`AudioRegression` runs the actual VoiceProcessor callbacks, not a GUI meter or a dry DSP substitute. It starts fresh state for each case: 44.1/48/96/192 kHz, all three layouts, all ten presets, variable blocks 32/64/127/256/511/1024. Deterministic input includes silence, quiet harmonic input, syllabic/pitched excitation, noise and a loud transient. Intensity automation and Bypass transitions are scheduled by block. Input is synthetic and is not labelled a recording of human speech or singing.

The original baseline is reproduced twice to establish deterministic output. The styling-only build and final safety/state-fix build are compared to that baseline. Each output contains **5,255,600 float32 samples**. Both comparisons found **0 differing samples, maximum absolute error 0, RMS error 0**, with tolerance exactly zero. SHA256 is `b4ba52654f1adf73488eb4720520224158eb6d9a739c07f3a3ea6a19ff34afda`. See `artifacts/audit/audio-comparison.json` and `final-audio-comparison.json`.

The fractional-wrap safety fix affects only the previously invalid one-past-end read condition. The broad audio fixture did not encounter that condition; its bit-identical result does not substitute for the dedicated boundary test. For the boundary reproducer, original behaviour was undefined and generated an ASan heap-buffer-overflow. Fixed behaviour wraps the rounded endpoint to zero and yields the first circular-buffer sample. No meaningful audible equivalence can be required for undefined original memory reads. Valid read arithmetic, algorithms, gains, modulation, feedback and coefficients are unchanged.

The state fix rejects malformed, duplicate, incomplete or nonnumeric payloads atomically. Valid version-1 XML serialization is unchanged. Actual old-checkpoint VST3 state restoration is tested through the exported VST3 host, checking all ten parameters and preset metadata. A corrupt payload that previously changed input trim is now rejected without changing parameters.

## Repeat commands

```bash
cmake --build build/Release --target AudioRegression
build/Release/AudioRegression artifacts/audit/candidate.f32
python3 tests/CompareAudio.py artifacts/audit/baseline.f32 artifacts/audit/candidate.f32
ctest --test-dir build/Release --output-on-failure
```

The local baseline .f32 files are retained. Packaged release evidence includes hashes/comparison JSON rather than duplicating the large raw files. The full checkpoint can restore the original source/binary into a separate writable directory to regenerate a baseline.

## Functional coverage

The original 28 DSP checks remain. Six audit DSP suites add: the fractional-wrap reproducer; measured 10 ms RMS detector/10 ms attack/120 ms release time constants at maximum intensity; calibrated RMS/peak decay/clip hold; late harmonic engagement, odd symmetry and third-harmonic generation; bit-identical audio across fixed and variable block partitions; and noisy, syllabic, singing-like/transient fixtures at 44.1–192 kHz and blocks 32–1024 including non-power-of-two sizes.

Processor tests retain trim accuracy, all ten preset semantics, bus layouts, closed-editor processing, host bypass and state roundtrips; malformed-state validation was added. The VST3 probe checks discovery, all ten controls plus the wrapper Program selector, all three layouts, old/new state compatibility and repeated loading. These are harness tests, not REAPER integration evidence.

GUI tests execute all six slider attachments, all four toggle attachments, keyboard adjustment, host feedback, numerical Label parsing and the double-click reset handler. Repeated create/resize/paint/destroy is exercised for 20 editors while a separate thread processes real audio callbacks using the correct JUCE owner lifecycle. The compiled editor reports 960 x 617 by default; offscreen snapshots cover 840 x 540, 960 x 617 and 1200 x 771, and GUI meter configuration is checked for mono, mono→stereo and stereo layouts. Native physical typing, pointer drag/focus and desktop popup interaction remain UNVERIFIED because X11 is inaccessible.

Callback allocation tests measure 1,800 actual normal/bypassed callbacks over all layouts, 44.1/48/88.2/96/176.4/192 kHz and variable 32–1024 blocks. Recorded new/new[]/aligned-new and wrapped malloc/calloc/realloc counts are zero. Wrapping covers linked code calls, not private allocations inside arbitrary external system libraries; the reviewed callback calls no such allocating services.

Concurrent actual processor callbacks, APVTS automation and atomic meter reads execute in ThreadStress without numerical errors. This is a stress check, not proof of race absence. TSan runtime is BLOCKED by an unexpected memory mapping (exit 66), even on a minimal atomic/thread program. No race-free claim is made.

Real recorded speech/singing and subjective listening remain UNVERIFIED: no recorded vocal corpus was supplied, and no synthesis tool was available in this environment. No naturalness or production-mix approval is inferred from synthetic tests.

## Sanitizer reproduction

`./scripts/sanitize.sh` builds both standalone DSP suites with ASan/UBSan. `./scripts/sanitize.sh --full` builds the instrumented JUCE-linked processor, editor, audit DSP and thread-stress tools, plus the instrumented VST3. Leak detection defaults off because the executed minimal LSan probe failed; override ASAN_OPTIONS only on a supporting runtime. `./scripts/thread-sanitize.sh` first executes a minimal runtime probe, returns 77 if blocked, and only then builds/runs a fully instrumented ThreadStress. Its current exit is 77, not a plugin race-test pass.

The actual VST3 compatibility probe accepts an optional fourth CLI argument pointing to the checkpoint bundle. For `scripts/test.sh`, set `BASELINE_VST3` to that bundle path. It flushes queued host parameter changes through real callbacks before serializing and uses declared parameter-step values; comparison tolerance was not relaxed. Exported +6 dB automation is verified against measured samples with intensity zero and all returns disabled.
