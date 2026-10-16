# Final debugging and release validation

Date: 2026-10-16. Decision: **READY FOR CONTROLLED TESTING**. This is not production approval: REAPER desktop integration, actual WRITE envelope recording and old-project reopening remain **UNVERIFIED**. No unresolved crash or essential audio failure was observed after the fixes below.

## Restoration point and baseline

`git status` confirmed this project has no Git repository; no reset, discard or history replacement occurred. Before edits, the complete project and installed GUI-redesign plugin were preserved at:

```text
/home/enos/AudioDevelopment/VocalisRider-checkpoints/final-debug-baseline-20261016-062517/
```

`project.tar.gz`, `SHA256SUMS` and `manifest.json` preserve original source, uncommitted work, reference images, tests, artifacts and build products. `installed-Vocalis Rider Live.vst3` preserves the original installation. Extract to a separate directory for restoration/review. The GUI redesign checkpoint and previous release archives also remain intact.

Baseline: existing Release build succeeded; original CTest 3/3 passed (82 DSP, 36 integration, 26 native VST3 checks), and the existing separate 102-check GUI harness passed. Baseline failures below were found by additional probes, not by silently changing those results. See `artifacts/final-debug/baseline-*.log`.

## Defects and corrections

| Severity / finding | Root cause and minimal change | Status / evidence |
|---|---|---|
| High: WRITE can overwrite a reentrant switch to READ | Timer cached Write before beginChangeGesture; a host gesture callback could select Read and set Rider, then the timer published stale automatic gain. Re-read mode after host callback, close the gesture and return when no longer Write. | **FIXED AND RETESTED**; failed against checkpoint library, passes release and ASan/UBSan Audit. |
| High: installation copy failure removes active installation | Old bundle was moved before copying replacement. Stage a complete copy first, then rename; restore backup on commit failure. | **FIXED AND RETESTED**; injected copy/commit failures preserve old installation and unrelated plugins. |
| Medium: colliding installation backups | Timestamp-only names collided during rapid reinstalls. Add a unique staging suffix and refuse an existing backup destination. | **FIXED AND RETESTED**; three rapid replacements preserve three distinct backups. |
| Medium: early automation capture point | Countdown started at zero and recorded after the first callback chunk. Initialize to one complete sample-rate-derived 20 ms interval. | **FIXED AND RETESTED**; baseline first 128-sample point was 2.667 ms at 48 kHz; final first point is 20 ms. Audio remains identical. |
| Medium: GUI response/automation/bypass edits omit host gestures | Direct notification without begin/end. Add balanced user-edit gestures, preserving values/IDs and mode semantics. | **FIXED AND RETESTED**; button listener test and full GUI suite. Actual REAPER recording remains unverified. |
| Medium: Range drag teardown leaves gesture open | Custom component lacked cancellation cleanup. Close an active gesture in destructor; Range reset brackets edits but reuses an existing drag gesture to avoid nested gestures. | **FIXED AND RETESTED**; checkpoint fails drag-close test; final double-click and close-mid-drag tests pass, including Debug ASan. |
| Safety: invalid sample-rate conversion | Processor used unchecked host rate for integer countdown and division; engine clamp did not handle NaN. Validate finite positive rate before clamping and deriving coefficients/countdown. | **FIXED AND RETESTED**; zero/NaN/infinite-rate cases, UBSan float-cast instrumentation. Normal supported rates unchanged. |
| Safety: malformed bus layout / short host buffer | Bus slot reads assumed 2 input buses/1 output bus and enough channel pointers. Reject malformed layouts before access; clear undersized buffers before obtaining bus pointers. | **FIXED AND RETESTED**; malformed layouts, undersized buffer and inactive/mono/stereo auxiliary routing tests pass under ASan. These are defensive cases, not a claim that REAPER supplied invalid buffers. |
| Safety: nonfinite choice snapshot | Integer conversion of a nonfinite raw mode could be undefined. Use one validated atomic snapshot before conversion; validate preset index similarly. | **FIXED AND RETESTED**; injected nonfinite raw mode case and UBSan. No valid parameter range/default changed. |
| Low: misleading indentation | Adjacent independent detector flush conditions and unbraced preset loop obscured control flow. Separate the conditions / brace the loop. | **FIXED AND RETESTED**; warning-enabled application sources compile without warnings. |

The original expanded baseline regression linked to the checkpoint shared library reports 10 checks, 5 failed (two timing assertions, reentrant READ, response gesture, drag teardown). Installer baseline copy and backup-collision assertions fail; staged-commit injection is unavailable for the old non-staged installer. Final corresponding cases pass.

Two new test-harness defects were corrected without changing production behaviour or weakening assertions: DSP audit supplied one pointer while requesting two music channels (ASan detected this); FIFO-overflow test initially expected 8192 usable slots, but JUCE AbstractFifo explicitly reserves one slot (correct exact expectation: 8191). First sanitizer failure is retained in `asan-dsp-audit-first-failure.log`. Snapshot tests now create their output directories rather than depending on previous artifacts.

## Preserved behaviour and performance work

No changes to the detector architecture, target/deadband, voice threshold/hysteresis/hold, music transfer curve, gain time constants, range/Idle semantics, output trim, linked gain, presets, state tree format, parameter names/IDs/defaults/ranges or VST3 identity. Current Vocalis interface appearance is preserved; only gesture/lifetime logic changed.

Per-sample exponential coefficient computation and constant music-sensitivity conversion were hoisted to per-process-call constants. No formula or coefficient precision changed. This optimisation is justified by the measured callback-cost reduction and the exact null test: **34,110,984 captured audio/state bytes match baseline**, SHA-256 `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`.

The user explicitly selected **retain existing smoothing** during this audit. Narrowing Range therefore continues to smooth toward its new bound rather than instantaneously clamping gain; temporary excursions outside newly narrowed bounds are an accepted behaviour tradeoff, not silently changed.

## Build, analysis and real-time review

**VERIFIED PASS:** clean Release configuration/build in `build-release`, GCC 13.3 / C++20 / Ninja / local JUCE **9.0.4** (Git `454fe891acc8204f99f4ef1e3a28c84b1a4da145`). Subsequent minimal fixes were recompiled and retested in that clean build tree. CMake now enables **VST3 only**; no standalone or other plugin format is enabled. Application source warnings (`-Wall -Wextra -Wpedantic`): no final diagnostics. Clang 18 static analysis of processor and editor: exit 0, no findings; this is not proof that all defects are absent.

Callback review: bounded sample loops, stack channel pointers, lock-free atomic parameter/meter access, preallocated FIFO. No explicit allocation, lock, disk/network/GUI work, logging or parameter notification in audio processing. Host edit notifications remain on processor-owned message timer, including with editor closed. State serialization and CSV/vector allocation remain outside audio callback. Denormal guard and finite sanitisation remain present.

**VERIFIED PASS:** scoped malloc/calloc/realloc and executable-visible C++ new probes observe zero callback allocations; stress tests exercise audio versus UI/parameter/state work. **UNVERIFIED:** exhaustive shared-library allocation/lock behaviour and all host scheduling/interleavings. TSan is **BLOCKED**, not passed: even a minimal atomic-thread probe exits 66 with unexpected memory mapping.

## Final test results

- **VERIFIED PASS:** 9/9 registered Release suites, zero failures: original DSP/integration/VST3 plus Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety and VST3_Reload.
- **FIXED AND RETESTED:** 55 Audit checks; 102 GUI checks; 41 additional analytical DSP/stability checks. Twenty actual native module load/process/unload cycles pass.
- **VERIFIED PASS:** six separately instrumented ASan/UBSan executables, with float-cast-overflow checks and leak detection disabled; no remaining sanitizer diagnostic.
- **BLOCKED:** LeakSanitizer scan aborts under the environment's ptrace restriction. Initial leak-enabled failures are retained; leak freedom is not claimed. Valgrind, cppcheck and clang-tidy were unavailable on PATH; no packages were installed.
- **VERIFIED PASS:** legacy parameter schema and binary state fixtures produced by pre-debugging code match; all 13 legacy parameter values restore. All eight complete factory configurations and independent processor instances tested.
- **VERIFIED PASS:** 30-case performance matrix. Mean deadline utilisation 0.1907–0.9167%; worst observed callback 96.602 µs, worst deadline utilisation 57.9612%; finite output, zero monitored allocation and zero reported latency in every case. This is callback wall time divided by buffer duration, not OS CPU sampling. No measured deadline miss; not a hard real-time guarantee. Mean matched-case cost ratio final/baseline 0.887 (about 11.3% lower), subject to scheduling noise.

Details, per-case measurements and limitations: REGRESSION_TEST_RESULTS.md. Host gaps: REAPER_COMPATIBILITY.md. Accepted/remaining limits: KNOWN_ISSUES.md.

## Artifacts and installation

Current compiled bundle:

```text
/home/enos/AudioDevelopment/VocalisRider/build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3
```

Installed bundle: `/home/enos/.vst3/Vocalis Rider Live.vst3`. Native ELF64 x86-64 `.so`; all linked dependencies resolved, no sanitizer runtime in Release. Build and installed bundles compare equal. Binary SHA-256: `fa6d9627cdc1d4165f9e635945e352a78d77a8f044054d3556ed458a8c3e60ff`.

Complete VST3 manifest matches baseline, including component `ABCDEF019182FAEB56636C7356724C76`, controller `ABCDEF011234ABCD56636C7356724C76`, product/vendor and version 1.0.0. Native loading is actually executed by the module tests. Previous installation backup is recorded in `artifacts/final-debug/install.log`.

```bash
cd ~/AudioDevelopment/VocalisRider
./scripts/rebuild.sh
./scripts/install.sh
```

Rebuild uses `build-release`; installer stages/backs up only the named user VST3. Uninstall: `./scripts/uninstall.sh`. No system installation or REAPER configuration/project modification occurred. Older build directories and release packages are retained as historical artifacts and are not the final validated release.
