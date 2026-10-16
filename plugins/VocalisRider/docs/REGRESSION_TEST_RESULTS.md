# Regression and performance results

Date: 2026-10-16. Final release decision: **READY FOR CONTROLLED TESTING**. No REAPER session was used. Every result below comes from executed local tests; unexecuted diagnostics are identified separately.

## Registered suites — VERIFIED PASS

| Suite | Final result | Coverage |
|---|---|---|
| DSP | 82 PASS checks | Original silence, level changes, detector/Spill, sensitivity, sidechain, range, Idle, modes, trim, bypass, finite values; 6-rate × 9-block matrix |
| Integration | 36 PASS checks | Parameters/presets/state, buses, real GUI capture, editor-closed capture/WRITE publication, READ, allocation probe, export, bypass |
| VST3_Module | 26 PASS checks | Native ELF entry/factory/controller, buses, actual audio, host READ gain and automatic riding, reported latency |
| Audit | 55 PASS checks | Fixed timing/reentrancy/gestures, Range teardown, malformed inputs, routing changes, all factory values, 8191-point bounded overflow, old fixture, instance independence |
| GUI | 102 PASS checks | Bindings, mouse/keys, external updates, mode buttons, presets, actual meters, restored editor, 750/1000/1400 scale, repeated lifecycle |
| DSP_Audit | 41 PASS checks | Analytical RMS/peak, no impulse delay, gain/trim accuracy, smoothing, long silence, bypass/music isolation, rapid changes, invalid rate |
| Thread_Stress | PASS | Real processing thread versus message-thread edits, repeated editor lifecycle, state save/restore and independent instance |
| Install_Safety | 3 PASS cases | Copy failure, commit failure/rollback, rapid backups; unrelated user plugin sentinel retained |
| VST3_Reload | 20 complete cycles | Same-process native module load/process/unload; 501 repeated PASS assertions (not 501 independent features) |

**9/9 suites passed, zero failures**, approximately 7.5 seconds. Original three suites passed before and after fixes. Output-directory creation was added to snapshot harness setup; existing assertions were retained. Full final output: `artifacts/final-debug/release-tests.log`.

## Baseline failures, fixes and failed diagnostic runs

- Original existing tests: 3/3 PASS; separate GUI baseline: 102 PASS.
- Expanded regression linked to checkpoint library: **10 checks, 5 FAIL**. Two failures cover the early first capture point; others are reentrant READ overwrite, missing response-button gesture and open Range gesture during teardown. Final tests pass. See `baseline-defects-expanded.log`.
- Old installer: copy-failure preservation and rapid-backup checks FAIL. Old installer has no staged-commit operation to inject; that case is UNAVAILABLE for baseline, and PASS for final.
- ASan initially found a newly authored test requesting two sidechain channels from one pointer. The test now supplies two valid pointers; assertions unchanged. Report preserved in `asan-dsp-audit-first-failure.log`.
- New overflow assertion initially assumed 8192 usable slots. JUCE documents AbstractFifo usable capacity as buffer size minus one; corrected exact assertion 8191 passes, with 809 dropped points after 9000 undrained records. No production limit changed.
- Leak-enabled runs FAIL because LeakSanitizer cannot scan under ptrace; TSan minimal probe FAILS at runtime (exit 66, unexpected memory mapping). These are **BLOCKED diagnostics**, not passes.

## Sanitizers, static analysis and limitations

**VERIFIED PASS:** fresh isolated Debug/O1 ASan + UBSan + float-cast-overflow instrumentation of processor/editor/JUCE and test executables. `dsp_tests`, `dsp_audit`, `audit_tests`, `gui_tests`, `thread_stress`, `integration_tests` all exit 0 with `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and `UBSAN_OPTIONS=halt_on_error=1`. No remaining address/undefined-behaviour diagnostic or JUCE assertion found. Logs: `asan-noleak-*.log`. Sanitized code was not installed or run inside REAPER.

**BLOCKED:** leak detection and TSan, with logs preserved. **UNVERIFIED:** Valgrind, cppcheck and clang-tidy (not available on PATH); no packages installed. **VERIFIED PASS:** Clang 18 analyser on Source/Plugin.cpp and Source/Editor.cpp, no findings (`static-analysis-final.log`); application-source warning-enabled build has no final warning/error diagnostics. These tools/results do not prove absence of races/leaks in every host/library path.

## Audio null and compatibility fixtures — VERIFIED PASS

Before edits, `AudioRegression.cpp` was linked to the original shared library and captured real processor output/state. The identical harness linked to final Release uses 44.1/48/96/192 kHz × 32/128/1024 samples × mono/stereo (24 combinations), 600 callbacks each, all eight presets, Off/Write/Read, Rider settings, trim/bypass, signal-level changes/pauses and stereo music.

`cmp` reports **byte-identical** 34,110,984-byte files. SHA-256 for both:

```text
b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647
```

Evidence: `baseline-audio-state.bin`, `final-audio-state.bin`, `null-test.log`. Audible algorithm changes are not intended. Capture timestamp/gesture/error-handling fixes are verified independently; they do not change this valid-input audio. User explicitly retained smoothed range narrowing.

`tests/fixtures/pre-debug/legacy-state.bin` and parameter-schema fixture were produced using pre-debugging code before changes, not by a new-version-only round trip. All 13 expected values restore. A final fixture producer yields identical schema and binary state. Complete VST3 manifest also matches checkpoint, including both CIDs. Old REAPER project reopening is still UNVERIFIED.

GUI uses actual processor atomics; meter/fader tests run audio. Current interface has not been redesigned. `visual-regression.log` records comparison against the checkpoint capture. Native desktop focus/chooser and REAPER embedding remain UNVERIFIED.

## Repeatable callback benchmark — VERIFIED PASS within measured conditions

Hardware: AMD Ryzen 9 5900XT 16-Core Processor, native Linux x86_64; GCC 13.3, JUCE 9.0.4. Release, stereo main + stereo music, GUI closed. Per case: 100 warm-up callbacks, then 2000 measured callbacks. Input generation is outside timing; steady_clock wall time surrounds processBlock only. Timed callback includes normal detection, riding and meter publication. No CPU affinity or real-time scheduling was applied; this is not an OS CPU-utilisation sample or hard WCET measurement. Scheduling outliers are retained.

Allocation column includes scoped direct malloc/calloc/realloc and executable-visible C++ new/new[] probes. Aligned allocators and arbitrary shared-library internals are not exhaustively intercepted; callback source inspection found no explicit allocation/locks/I/O. Every measured output is finite and every case reports zero samples latency. An analytical impulse test also shows output at sample zero: no hidden delay buffer. Smoothing time constants are response times, not reported algorithmic latency.

| Rate Hz | Block | Mean µs | p99 µs | Worst µs | Mean deadline % | Worst deadline % | Alloc calls |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 44100 | 32 | 1.557 | 1.733 | 7.524 | 0.215 | 1.037 | 0 |
| 44100 | 64 | 2.855 | 2.956 | 6.543 | 0.197 | 0.451 | 0 |
| 44100 | 128 | 6.189 | 8.636 | 30.187 | 0.213 | 1.040 | 0 |
| 44100 | 256 | 11.142 | 15.038 | 17.133 | 0.192 | 0.295 | 0 |
| 44100 | 512 | 22.136 | 26.550 | 28.213 | 0.191 | 0.243 | 0 |
| 44100 | 1024 | 44.541 | 49.834 | 58.340 | 0.192 | 0.251 | 0 |
| 48000 | 32 | 1.484 | 1.533 | 8.566 | 0.223 | 1.285 | 0 |
| 48000 | 64 | 2.862 | 2.906 | 8.316 | 0.215 | 0.624 | 0 |
| 48000 | 128 | 5.651 | 5.740 | 11.612 | 0.212 | 0.435 | 0 |
| 48000 | 256 | 11.162 | 15.008 | 17.273 | 0.209 | 0.324 | 0 |
| 48000 | 512 | 22.239 | 27.121 | 42.049 | 0.208 | 0.394 | 0 |
| 48000 | 1024 | 44.231 | 49.072 | 63.890 | 0.207 | 0.299 | 0 |
| 88200 | 32 | 1.468 | 1.542 | 6.793 | 0.405 | 1.872 | 0 |
| 88200 | 64 | 2.848 | 2.926 | 8.385 | 0.393 | 1.156 | 0 |
| 88200 | 128 | 5.628 | 5.721 | 12.053 | 0.388 | 0.831 | 0 |
| 88200 | 256 | 11.174 | 13.956 | 26.019 | 0.385 | 0.896 | 0 |
| 88200 | 512 | 22.230 | 26.921 | 34.445 | 0.383 | 0.593 | 0 |
| 88200 | 1024 | 44.276 | 49.513 | 51.387 | 0.381 | 0.443 | 0 |
| 96000 | 32 | 1.466 | 1.513 | 7.043 | 0.440 | 2.113 | 0 |
| 96000 | 64 | 2.831 | 2.895 | 7.945 | 0.425 | 1.192 | 0 |
| 96000 | 128 | 5.587 | 5.691 | 11.341 | 0.419 | 0.851 | 0 |
| 96000 | 256 | 11.112 | 11.242 | 42.279 | 0.417 | 1.585 | 0 |
| 96000 | 512 | 22.149 | 26.510 | 28.394 | 0.415 | 0.532 | 0 |
| 96000 | 1024 | 44.270 | 49.303 | 57.548 | 0.415 | 0.540 | 0 |
| 192000 | 32 | 1.528 | 1.523 | 96.602 | 0.917 | 57.961 | 0 |
| 192000 | 64 | 2.855 | 2.905 | 8.205 | 0.857 | 2.462 | 0 |
| 192000 | 128 | 5.614 | 5.671 | 13.716 | 0.842 | 2.057 | 0 |
| 192000 | 256 | 11.134 | 14.968 | 17.152 | 0.835 | 1.286 | 0 |
| 192000 | 512 | 22.077 | 26.390 | 27.792 | 0.828 | 1.042 | 0 |
| 192000 | 1024 | 44.289 | 49.313 | 52.740 | 0.830 | 0.989 | 0 |

Mean deadline utilisation: **0.1907–0.9167%**. Worst observed callback: **96.602 µs**; largest deadline ratio **57.9612%**. No observed callback exceeded its nominal buffer duration in this run. Do not extrapolate this into a guarantee under live host load. Average matched-case final/baseline callback-time ratio: **0.887**, about **11.3% lower** after hoisting constant coefficient calculations; environment noise and compiler/caller overhead still apply.

Raw final and baseline CSVs, stdout and `performance-summary.json` are in `artifacts/final-debug/`. Baseline executable uses the original callback implementation. Benchmarks were rerun sequentially after compilation activity ended. Live REAPER CPU/dropout performance remains UNVERIFIED.

## Reproduction

```bash
./scripts/rebuild.sh
ctest --test-dir build-release --output-on-failure
build-release/performance_tests artifacts/final-debug/manual-performance.csv
cmake -S . -B build-sanitize -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS_DEBUG="-O1 -g1" -DVOCALIS_SANITIZER=ADDRESS
cmake --build build-sanitize --target dsp_tests dsp_audit audit_tests gui_tests thread_stress integration_tests -j 2
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
  build-sanitize/audit_tests --final "$PWD/tests/fixtures/pre-debug/legacy-state.bin"
```

Leak scanning is deliberately disabled only because the runtime is blocked; absence of leaks is not certified. Optional separate THREAD configuration exists for a compatible environment, but was not fully built here after its minimal runtime probe failed. Actual recorded vocals/whispers, real microphones, subjective pumping/click assessment, native desktop interactions and all required REAPER project/automation acceptance steps remain UNVERIFIED; use REAPER_COMPATIBILITY.md.
