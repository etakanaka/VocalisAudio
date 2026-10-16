# Vocalis Rider Live — post-build verification

Date: 2026-10-16. Project: `/home/enos/AudioDevelopment/VocalisRider`.
Scope: existing native Linux VST3, REAPER for Linux only. No DSP, GUI, test-suite, script or CMake implementation changes were made. This pass added this report and verification evidence, ran existing tests, and installed the existing current bundle. Historical Ardour documentation was not used to establish support.

Classification meanings: **VERIFIED** = supported by the stated executed check; **FAILED** = a stated criterion failed; **NOT TESTED** = no execution evidence; **NEEDS ATTENTION** = implementation limitation, review finding or incomplete coverage. A passing library/module test does not establish operation inside REAPER.

## 1. Build status

**VERIFIED** — existing Release build is current according to the CMake/Ninja dependency graph. Executed:

```bash
cmake --build build --target VocalisRiderLive_VST3 dsp_tests integration_tests vst3_smoke -j 2
```

Exit 0; the VST3 manifest helper was checked and Ninja reported no work needed. No plugin source recompilation was necessary. CMake cache points to this exact project and `/home/enos/AudioDevelopment/JUCE`, with `/usr/bin/c++`, Release and C++20. The plugin binary timestamp (05:22:49 +0200) follows the latest implementation sources (05:22:44 +0200). CMake lists `Source/Plugin.cpp` and `Source/Editor.cpp`; `Engine.h` is a tracked compile dependency. Source/test/script and binary SHA-256 values are recorded in `artifacts/post-build-verification/SHA256SUMS`.

**NEEDS ATTENTION** — `FORMATS VST3 Standalone` remains in CMake. VST3 is the only plugin format enabled, but there is also a standalone application target from the development stage. It was not rebuilt or used in this verification. No LV2, Windows DLL, Wine, VST2 or AU plugin is enabled or installed.

Warnings/errors:

- **VERIFIED:** incremental build exited successfully with no warning/error diagnostics. It did not compile all sources, so this is not a clean-build warning audit.
- **NEEDS ATTENTION:** an additional `-Wall -Wextra -Wpedantic -fsyntax-only` check of `tests/DspTests.cpp` reported one `-Wmisleading-indentation` warning at `Source/Engine.h:50`, where two independent `if` statements share a line. Syntax check exited 0. This is a formatting/maintainability warning, not evidence that the second condition is nested.
- **VERIFIED:** shell syntax check passed for install/uninstall/rebuild scripts. Runtime `ldd` checks resolved the compiled plugin's and REAPER executable's direct dependencies; no missing linked dependencies were found. No packages were installed.

Evidence: `artifacts/post-build-verification/build.log`, `warnings.log` and `bundle-inspection.log`.

## 2. Compiled VST3 location

**VERIFIED** — absolute source bundle:

```text
/home/enos/AudioDevelopment/VocalisRider/build/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3
```

Binary inside it:

```text
Contents/x86_64-linux/Vocalis Rider Live.so
```

`file` and `readelf -h` identify an ELF64, little-endian, x86-64 shared object. `nm -D` exposes `GetPluginFactory`, `ModuleEntry`, `ModuleExit`. `Contents/Resources/moduleinfo.json` identifies Vocalis Rider Live 1.0.0, Vocalis Audio, VST 3.8.0 SDK, component and controller classes. The existing native VST3 smoke host successfully loaded and processed the binary.

Binary SHA-256:

```text
05e320ea3e145704b93f2715cbab79d310b10b81d87f9fa51818719e55605168
```

This build is traceable to the current project through its cache, source list, dependency freshness and outputs. It was not independently reproduced with a clean compiler invocation during this pass.

## 3. Installation status

**VERIFIED** — `~/.vst3/` was empty at inspection. Ran the existing `scripts/install.sh`, exit 0. Installed bundle:

```text
/home/enos/.vst3/Vocalis Rider Live.vst3
```

Installed binary:

```text
/home/enos/.vst3/Vocalis Rider Live.vst3/Contents/x86_64-linux/Vocalis Rider Live.so
```

Recursive `diff -qr` found no differences between build and installation bundles. Installed binary SHA-256 matches the value above. The existing VST3 smoke host also passed all 26 checks against this installed binary, independently of the CTest build-path run. No unrelated plugins were overwritten. No system installation or REAPER configuration edit was performed.

Script review: installer targets only this named bundle in the user VST3 directory, backs up an existing same-name installation, and prefers the build output with a packaged-bundle fallback. Uninstaller removes only this named bundle. Rebuild script currently builds all default targets, including standalone and tests; use the explicit VST3 target command above for the current scope. Existing-install backup/uninstall were reviewed, not executed against the real installation in this pass.

## 4. REAPER detection status

**VERIFIED** — executable found at:

```text
/home/enos/opt/REAPER/reaper
```

It is an executable native Linux x86-64 ELF binary, with direct linked dependencies resolved. The adjacent `whatsnew.txt` begins `v7.82 - October 4 2026`; this is the installed distribution's version indication, not a running-host version query. REAPER is not on PATH.

**NOT TESTED** — REAPER launch, plugin scanning/loading, embedded GUI, audio playback, sidechain routing, native automation recording/playback and project reopening. The current `DISPLAY=:0` is inaccessible (`xwininfo -root` exits 1: unable to open display). REAPER was not launched and its configuration was not modified. The small VST3 smoke executable is not REAPER.

Manual rescan instructions (to be performed by the user):

1. Launch `/home/enos/opt/REAPER/reaper` in the desktop session.
2. Open Options → Preferences → Plug-ins → VST. Check that `/home/enos/.vst3` is searched; add it if necessary, then use Re-scan. A full clear-cache rescan should only be needed if an ordinary rescan fails to find the installed plugin.
3. In a test project's FX browser, search `Vocalis Rider Live` and insert its **VST3** entry on a vocal track. Do not choose a Windows-bridged plugin or LV2 entry.
4. Play a short known vocal clip in Off mode and confirm real level changes and fader movement. Then verify the routing/automation/reopening steps listed in section 10.

**NEEDS ATTENTION** — older README/testing documentation says REAPER was not found. That was historical; this report supersedes it for current detection status. Older Ardour guides remain in the repository but are outside the current supported scope.

## 5. DSP implementation status

| Area | Classification | Evidence and limits |
|---|---|---|
| Main callback | VERIFIED | Real buffer processing; float audio is multiplied by computed gain. Native VST3 tests verify actual audio, not just meters. |
| RMS / peak | VERIFIED | Linked mean-square detector with 35 ms smoothing; peak tracking with decay. Peak is measured but not used to constrain riding. |
| Vocal detection / Spill | VERIFIED | Energy threshold adjusted by Vocal sensitivity, 3 dB entry / -2 dB exit hysteresis and 180 ms hold. Synthetic and broadband-noise tests pass. |
| Speech discrimination | NEEDS ATTENTION | No speech classifier or spectral spill discrimination. Loud stationary noise above threshold can trigger Ride. Real vocal/spill listening remains NOT TESTED. |
| Target / riding | VERIFIED | Target error, ±1 dB deadband, separate rise/fall constants, Fast/Slow choices and smoothed dB gain. |
| Music sidechain | VERIFIED | Optional mono/stereo bus, smoothed music RMS and bounded ±6 dB target influence. Missing-sidechain engine and stereo-sidechain module tests pass. REAPER routing remains NOT TESTED. |
| Range | VERIFIED | Goal is bounded to ordered endpoints; settled-range and crossed-parameter tests pass. Instantaneous enforcement after narrowing is FAILED; see section 9. |
| Idle | VERIFIED | Inactivity returns to range-bounded Idle over 650 ms; no unbounded silence boost. Idle outside range is deliberately clamped internally. |
| Stereo linking | VERIFIED | Identical gain on both main channels; detector averages their powers. Stereo balance test passes. |
| Output / bypass | VERIFIED | Smoothed trim and dry crossfade; processor test confirms dry main audio without sidechain leakage after bypass settles. |
| Rate / block handling | VERIFIED | Existing 6-rate × 9-block matrix passes, including 44.1–192 kHz and 1–4096 samples. Full live host stress remains NOT TESTED. |
| Zero latency | VERIFIED | No lookahead; VST3 reports zero samples. Loud initial transients can pass before the rider reacts; there is no limiter. |

**VERIFIED, scoped real-time inspection:** callback uses pre-cached atomic parameters, stack pointers, DSP math and a preallocated SPSC FIFO; no explicit heap allocations, filesystem/network operations, GUI calls, mutexes or host parameter notifications appear in `processAudio`. Lock-free float/int/bool atomics are compile-time checked. Denormal protection and nonfinite input/settings sanitisation are present. Existing allocation probe observed zero monitored static-linked malloc/calloc/realloc calls during 1000 callbacks. This does not prove absence of allocation inside every shared library/host or establish CPU deadline performance. Per-sample logarithm/exponential/power calculations and live-load behaviour have not been profiled.

## 6. GUI implementation status

**VERIFIED, offscreen only** — the integration test instantiates the real custom editor and writes `artifacts/editor.png`. Layout includes the dark upper panel, Target, Spill/Vocal/Music rotaries, Ride/Idle lights, silver Range/Rider/Output section, Idle marker, presets, automation buttons and LIVE nameplate. No simulated audio meters were found: they read processor atomics.

Source wiring:

- Target, Spill, Vocal, Music, Rider, Output and Idle use APVTS slider attachments.
- Range handles call the actual range parameters and use drag gestures.
- Fast/Slow, bypass and automation buttons change the real parameters.
- Preset menu calls `setCurrentProgram`, applying real parameter configurations.
- Export/Clear operate on the actual captured points.
- In Off/Write, editor timer displays engine Rider gain without notifying the parameter; Read enables the host-attached Rider control.
- Fixed-aspect resizing and mouse handlers exist in source.

**NOT TESTED** — interactive mouse/fine adjustment, every GUI control's end-to-end effect, desktop/REAPER embedding, resize behaviour and high-DPI operation. Source wiring and a snapshot do not establish these behaviours. `reference.png` is still absent, so reference-image fidelity is NOT TESTED.

**NEEDS ATTENTION** — some control gestures and displays have limitations listed below. No interface redesign was performed.

## 7. Automated test results

Executed existing CTest tests with verbose output:

```bash
ctest --test-dir build --output-on-failure -V
```

| Test | Classification | Actual result |
|---|---|---|
| DSP | VERIFIED | 82 PASS checks, 0 FAIL checks |
| Integration | VERIFIED | 36 PASS checks, 0 FAIL checks |
| VST3_Module | VERIFIED | 26 PASS checks, 0 FAIL checks |
| Overall existing suite | VERIFIED | 3/3 tests passed, 0 failed, 144 PASS checks; 1.97 seconds |
| Installed-binary smoke | VERIFIED | 26 PASS checks, exit 0; same existing smoke test, not counted as 26 new distinct checks |
| Existing ASan/UBSan DSP executable | VERIFIED | Re-executed with `ASAN_OPTIONS=detect_leaks=0`; 82 assertions passed, exit 0, no sanitizer diagnostics |
| REAPER host tests | NOT TESTED | No automated REAPER test exists; host not loaded |
| REAPER Lua importer | NOT TESTED | No Lua/luac on PATH; not executed in REAPER |

The sanitizer executable is newer than its DSP test/engine inputs and was not rebuilt. Leak detection was disabled. No ThreadSanitizer, full Steinberg validator or live deadline profiling was run.

Test coverage limits: the rate/block matrix is an engine test, not 54 REAPER configurations. Factory recall assertions check preset selection/automation reset, not every preset's entire value table. Persistence checks compare all 13 values between processor instances, but do not exercise REAPER project reopening. GUI capture does not simulate mouse interaction. VST3 smoke checks READ input automation and automatic audio; it does not implement a recording host's `IComponentHandler` to validate WRITE envelopes. Existing synthetic signals are harmonic speech-band tones, not recorded speech, despite some test labels using “speech”.

Evidence: `artifacts/post-build-verification/tests.log`, `LastTest.log`, `installed-vst3-smoke.log`, `sanitizers.log`. No existing test failed or was unavailable; the host/importer checks above are absent/unexecuted rather than passing tests.

## 8. Missing or incomplete functionality

**NEEDS ATTENTION:**

- REAPER acceptance testing is incomplete: scan, load, GUI, mono/stereo, sidechain, real audio, WRITE recording, READ playback and project restoration.
- WRITE uses a processor-owned 50 Hz message-thread timer with begin/end gestures and `setValueNotifyingHost`. JUCE's VST3 wrapper maps these to host edit notifications on that thread. Actual REAPER recording is unverified; GUI movement/parameter publication is insufficient proof.
- Offline or blocked-message-loop WRITE notifications are not reliable. FIFO holds 8192 points (about 164 seconds at 50 Hz); persistent capture caps at one million points (about 5.5 hours), then drops points. Captures are not saved with plugin state.
- Capture does not check transport-playing status or separate loop/seek passes. Host time resets can produce overlapping/duplicate points. READ adds 12 ms smoothing and Rider parameters have 0.1 dB resolution, so host replay need not be sample-identical to automatic gain or high-resolution CSV.
- The CSV/REAPER importer is supplied, but its REAPER execution and envelope value mapping remain unverified.
- The host-exposed `preset` parameter is a label only; automation of it does not apply a preset. Editor selection and host programs do apply configurations. This is documented but could confuse users.
- No limiter, sophisticated voice/noise classifier, CPU-load benchmark or recorded-vocal listening assessment. These are limitations, not newly requested features.
- Build/support documentation still includes Standalone/Ardour material outside the current scope.

## 9. Known bugs and inspection findings

1. **FAILED — strict immediate range enforcement when limits narrow.** `Engine.h:39` bounds the goal; `Engine.h:43` smooths gain without clamping the resulting current value. An isolated verification probe using existing engine code settled at +5.99199 dB, then changed Range Max to 0 dB. After 128 samples at 48 kHz the actual gain was still +5.9198 dB. Thus it respects settled limits, not an instantaneous ceiling during limit changes. Decide and document the intended smoothing-versus-hard-bound behaviour before a small fix. Probe: `artifacts/post-build-verification/inspection-probe.cpp` and `.log`. This is separate from the passing existing suites.

2. **NEEDS ATTENTION — first WRITE capture interval starts early.** `prepareToPlay` initializes `untilCapture=0`; `processAudio` records after its first processed chunk rather than waiting a full 20 ms. Existing integration CSV starts at 0.002667 seconds with 128-sample / 48 kHz callbacks, then 0.022667 and 0.042667. Subsequent cadence is 50 Hz. This is a timing inconsistency, not evidence of failed audio processing.

3. **NEEDS ATTENTION — incomplete parameter gesture coverage.** Fast/Slow, bypass, automation and preset buttons call the helper `setValueNotifyingHost` without explicit begin/end gestures. Range drag and attached slider gestures are present, as is the special WRITE Rider gesture. Some host automation modes may not record the non-slider edits as intended. Actual REAPER behaviour is NOT TESTED.

4. **NEEDS ATTENTION — GUI does not always show effective applied gain/bounds.** Rider meter publishes internal gain before Output and bypass; it can move while output is dry. Idle can display a value that is clamped by Range. Externally crossed range endpoints are sorted in DSP, but the GUI still draws the raw named endpoints and does not sort its highlighted segment. These are display/semantics issues, not fake DSP controls.

5. **NEEDS ATTENTION — recording boundary and concurrency coverage.** The timer samples automation mode once and then sends a Rider update; concurrent audio-thread mode changes are not transactionally coordinated. No stress test covers a Write-to-Read boundary or state restore while recording. A late update is a source-inspection risk, not a reproduced REAPER failure. Capture Clear likewise drains currently visible points but cannot stop a concurrent producer; clear while Off for a deterministic pass boundary.

6. **NEEDS ATTENTION — maintenance issues.** The warning at `Engine.h:50`, dense source formatting, labels that overstate speech/perceptual coverage, outdated host detection text and non-VST3 support material should be addressed with small scoped follow-ups.

No production fixes were applied during this verification. No crash, unresolved native dependency or failed existing automated test was observed.

## 10. Recommended next steps

1. **NOT TESTED → verify in REAPER first:** rescan the installed bundle, insert into a disposable project, test mono/stereo audio and every GUI control, and confirm smooth gain with real speech/singing and pauses.
2. **NOT TESTED → verify sidechain:** send music to vocal track channels 3/4; map main Vocal to 1/2, Music to 3/4 and output to 1/2. Check influence and no music leakage. Use REAPER's VST3 bus-count/pin options if required.
3. **NOT TESTED → verify automation:** arm only Rider, attempt WRITE, inspect actual envelope points, select plugin READ and host Read, then replay with editor open and closed. Verify Off emits no moving Rider edits. Test CSV import separately in a fresh envelope.
4. **NOT TESTED → verify restoration:** save/reopen the project and compare all controls, presets, sidechain pins and automation playback. Include mode changes and bypass transitions.
5. **NEEDS ATTENTION:** agree the intended range-limit semantics, then make minimal fixes/tests for that behaviour and the first capture interval; review button gestures and recording boundaries. No architecture or GUI redesign is needed to investigate these.
6. **NEEDS ATTENTION:** align future CMake/docs with native VST3 + REAPER only, clean the observed warning, then perform a warning-enabled build and live CPU/deadline/listening tests.

Current conclusion: **VERIFIED** native build, installation and existing automated processing checks; **NOT TESTED** inside REAPER; **NEEDS ATTENTION** for the identified limitations and focused defects. This is not a REAPER compatibility certification.
