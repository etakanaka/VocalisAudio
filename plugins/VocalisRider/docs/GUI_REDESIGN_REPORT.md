# Vocalis Rider Live — safe GUI redesign

Completed 2026-10-16. Native Linux x86_64 VST3, REAPER target. The editor was replaced using `Vocalis Leveler Plugin Interface.png` as the visual reference. **Only `Source/Editor.cpp` changed in production code.** DSP, processor, parameter definitions, callbacks, state format, automation code, factory tables, plugin metadata, CMake and existing tests/scripts are byte-for-byte unchanged.

## Baseline and restoration checkpoint

There was no Git repository (`git status` and `git rev-parse` reported that fact); there is no baseline commit. A full backup was created before any code edit, preserving all existing files, build products, reference artwork and uncommitted work without replacing history:

```text
/home/enos/AudioDevelopment/VocalisRider-checkpoints/gui-baseline-20261016-060436/project.tar.gz
```

Archive SHA-256: `228c545e7e7ffee1b47ec077d7342cbfe1d0a9f843dfebb59be758807f5fc641`. Archive listing and `project-manifest.json` preserve the exact starting file inventory/hashes; `gzip -t` passed. The same checkpoint directory contains `installed-Vocalis Rider Live.vst3`, the original installed plugin. Extract the archive to a separate directory to recover/inspect the original; do not overwrite later work accidentally. Original installation is also backed up at:

```text
/home/enos/.vst3/Vocalis Rider Live.vst3.backup-20261016061531
```

Inspection and baseline mapping are documented in `docs/GUI_REDESIGN_BASELINE.md`. Baseline Release build succeeded; all three existing CTest suites passed (144 checks, zero failures). Original editor capture: `artifacts/gui-redesign/baseline-editor.png`.

## Files changed and generated

- **Production change:** `Source/Editor.cpp` only — rendering, layout, reusable LookAndFeel/helpers, real meter components, Range presentation and keyboard interaction, existing GUI parameter bindings.
- **New documentation:** `docs/GUI_REDESIGN_BASELINE.md`, this report.
- **New verification material:** `artifacts/gui-redesign/AudioRegression.cpp`, `GuiVerification.cpp`, their executables, baseline/final audio-and-state captures, logs, checksums and screenshots; checkpoint-path record at `artifacts/gui-checkpoint-path.txt`.
- **Generated outputs:** rebuilt shared-code library, existing integration executable and native VST3 bundle/manifest. The redesigned VST3 was installed into the user directory with the original backed up. Existing release archive remains intact; a separately named GUI-redesign archive accompanies this delivery.
- No non-GUI implementation change required approval or was made. `protected-files.log` records unchanged source, existing tests, scripts, README, licence and CMake hashes, plus identical VST3 manifest bytes.

## Visual implementation and reference comparison

Reference: 1254×1254 square suite interface. New editor: 1000×1000 logical pixels; fixed square scaling from 750 to 1400 pixels. Original procedural JUCE rendering replaces the old portrait silver/hardware theme:

- Navy/black gradient panels, thin blue borders, rounded corners and restrained shadows.
- White VOCALIS branding, blue **RIDER LIVE** title, original geometric waveform mark, header preset selector and working bypass.
- Yellow Target strip displays the actual measured input; violet marker sets the existing Target parameter. The readout and scale retain the real -42…0 dBFS range.
- Violet rotary arcs, dark shaded knob faces, fine radial ticks and pale pointers. Existing Spill, Vocal and Music values stay in dB rather than using the reference's percentages.
- Real Ride/Idle indicators and stacked Fast/Slow response buttons.
- Tall dark fader tracks, violet light strips, mechanical caps, calibrated decibel ticks and rounded readouts. Illuminated cap lines align with the actual parameter position.
- Both Range handles and values retained. Idle has its own yellow draggable marker and numeric field adjacent to Rider.
- Segmented input RMS/peak and music RMS meters use existing atomics. No output meter is fabricated: the reference's OUT column becomes **MUSIC / SIDECHAIN**, since the unchanged processor does not publish measured output level.
- Violet selected automation buttons, with all existing CSV export and Clear functions accessible in the lower panel. Bypass replaces the reference's decorative settings icon with a real existing control.

Reusable GUI elements include `SuiteLook` for knobs/sliders/readouts/buttons/combos, panel/label/LED/track/cap/tick helpers, `LevelMeter` and `RangeControl`. No reference PNG is embedded or used as the background. No image-generation assets or external artwork dependencies were added.

Visual captures were inspected and refined: knob proportions, subtitle clearance, numeric box rounding, Idle readout width and illuminated fader-line alignment were corrected. The reference's general panel arrangement, hierarchy, palette, violet highlights, knobs and faders are retained; exact fonts, illustrative lighting, decorative detail and dimensions differ. Input/music meters show genuine measured values rather than the reference's illustrated levels.

## Preserved functionality and bindings

All IDs and ParameterID versions remain defined in unchanged `Plugin.cpp`. Sliders retain APVTS attachments; custom Range and existing buttons use the same processor setters. No duplicate or renamed parameters exist.

| Control | Existing ID | Range / choices | Default | GUI and external-update verification |
|---|---|---|---|---|
| Target | target | -42…0 dBFS | -21 | Attached; mouse, arrows, GUI writes and external updates passed |
| Spill | spill | -48…-12 dBFS | -48 | Attached; same checks passed |
| Vocal | vocal | -12…+12 dB | 0 | Attached; same checks passed |
| Music | music | -12…+12 dB | 0 | Attached; same checks passed |
| Range Min | rangeMin | -12…+11 dB | -6 | Existing setter/drag gestures retained; external redraw and keyboard selection/adjustment passed |
| Range Max | rangeMax | -11…+12 dB | +6 | Same; upper-handle mouse drag and default reset passed |
| Idle | idle | -12…+12 dB | 0 | Attached; mouse, arrows, GUI writes and external updates passed |
| Rider | rider | -12…+12 dB | 0 | Attached in Read; same checks passed; Off/Write engine-driven display and enabled states passed |
| Output | output | -12…+12 dB | 0 | Attached; mouse, arrows, GUI writes and external updates passed |
| Response | attack | 0 Slow / 1 Fast | Slow | Existing setters preserved; click and external selected-state checks passed |
| Automation | automation | 0 Off / 1 Write / 2 Read | Off | Existing setters preserved; all buttons and all external mode indicators passed |
| Bypass | bypass | Off / On | Off | Existing setter preserved; click and external selected-state checks passed |
| Preset | preset | Eight persisted labels/programs | Default | Existing menu calls setCurrentProgram; actual preset values and external program/menu reflection passed |

The existing preset-label parameter still stores a label; changing it alone does not apply a factory program. GUI selection and host programs retain their existing behaviour. No new user-preset save dialog was introduced: the pre-existing host/APVTS state save/restore mechanism is unchanged.

Automatic riding, detection, Spill, Target, sidechain/music influence, attack, range semantics, Idle, linked mono/stereo processing, output, bypass, Off/Write/Read, factory presets and host state all remain in unchanged processor/DSP files. Existing differences between internal gain, Output trim, Idle range bounding and bypass remain as documented; Rider tooltip explicitly describes gain before Output/Bypass.

Metering stays on atomics; editor's 30 Hz message-thread timer reads existing processor state. No GUI work was introduced into the callback. The processor-owned 50 Hz WRITE timer, gestures, FIFO, host notifications and capture logic were not modified. Slider gestures remain JUCE attachment gestures; original button notification behaviour is retained. Range additionally supports Left/Right to choose a handle and Up/Down to edit, with Shift fine adjustment, using the same parameter boundaries.

## Executed validation

### Existing tests: baseline versus final

| Suite | Baseline | Final |
|---|---:|---:|
| DSP | 82 PASS | 82 PASS |
| Integration | 36 PASS | 36 PASS |
| Native VST3 module | 26 PASS | 26 PASS |
| CTest total | 3/3, 0 failed | 3/3, 0 failed |

Both builds used the existing Release configuration and local JUCE. Final build compiled the editor, relinked the shared library, VST3 and existing integration executable; no build warnings/errors appeared in its log. Existing DSP sanitizer executable was also rerun: 82 assertions passed, no AddressSanitizer/UndefinedBehaviorSanitizer diagnostics, with leak detection disabled as before.

Logs: `baseline-build.log`, `baseline-tests.log`, `redesign-build.log`, `redesign-tests.log`, `sanitizers.log` under `artifacts/gui-redesign/`. No full warning-enabled or fresh sanitizer build was performed; protected DSP/test sources did not change.

### Audio and state identity

A verification-only harness was compiled against the baseline shared library before editing, then against the final shared library. It drives real `Vocalis::processBlock` callbacks, not a second DSP implementation, using identical harmonic input/music signals and states:

- 44.1, 48, 96 and 192 kHz × 32, 128 and 1024 samples × mono and stereo: **24 configurations**.
- 600 callbacks per configuration, all eight presets, Off/Write/Read, Rider values, bypass, changing input levels, pauses and enabled stereo music sidechain.
- Captures all processed main-channel float samples and APVTS serialized state at each configuration end.
- **34,110,984 bytes compared with `cmp`: identical.** Both files have SHA-256:

```text
b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647
```

Evidence: `AudioRegression.cpp`, `baseline-audio-state.bin`, `redesigned-audio-state.bin`, `audio-regression-sha256.txt`. This is a deterministic offline regression, not a live-host CPU or perceptual test. An intermediate renderer build also matched the same golden output. Protected DSP/processor files match the checkpoint exactly.

### Additional GUI verification

**102 checks passed**, 0 failed in the final `GuiVerification.cpp` run:

- Every attached slider: programmatic parameter updates, GUI value writes, keyboard arrows and synthetic mouse drags reach the correct unchanged parameter.
- Range: externally changed values alter the rendered handles; keyboard selects/adjusts both bounds; upper handle drag and double-click default reset work.
- Existing buttons: Fast/Slow, Off/Write/Read, Bypass; external mode/attack/bypass changes update selected visuals.
- Rider is read-only in automatic modes, editable in Read, and reflects actual automatic gain within the preserved 0.1 dB display resolution.
- Preset menu applies real settings and follows external program changes.
- Actual processed audio drives input, sidechain and activity; screenshots capture these real states.
- Snapshots at 750, 1000 and 1400 square pixels, recreated editor reflects restored state, and repeated create/destroy cycles complete successfully.

These are direct component event/API tests under JUCE's message loop, not native desktop or REAPER interaction tests. Two initial harness assumptions were corrected: enabling the optional bus before expecting a music reading, and using half the existing 0.1 dB display step as the Rider tolerance. A later expanded run passed all 102 checks. No production DSP fix was used to satisfy them.

### Bundle and identity

Compiled bundle:

```text
/home/enos/AudioDevelopment/VocalisRider/build/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3
```

Installed bundle:

```text
/home/enos/.vst3/Vocalis Rider Live.vst3
```

Native ELF64 Linux x86-64 `.so`; original product name, vendor, version 1.0.0 and complete manifest are unchanged. Component CID `ABCDEF019182FAEB56636C7356724C76`, controller CID `ABCDEF011234ABCD56636C7356724C76`. Manifest comparison to checkpoint is byte-identical. Installed/build bundle recursive comparison passed. Binary SHA-256:

```text
c5d7b3dfa19f8c953b607b1fe9a2a82d735435d2735f0cf757dca3d081a1a67f
```

Only native VST3 was rebuilt/installed. Existing CMake's unused standalone target remains unchanged to respect the protected non-GUI boundary; no LV2, Windows or other plugin format was added.

## REAPER compatibility and unverified items

REAPER exists at `/home/enos/opt/REAPER/reaper`. This session cannot access `DISPLAY=:0` (`xwininfo -root` fails), so **REAPER embedding, desktop mouse/keyboard behaviour, high-DPI host scaling, actual WRITE recording, project reopening and CSV importer execution remain NOT TESTED**. No REAPER configuration was changed. The native module test verifies load, buses, audio processing, READ host parameter input and zero latency; it is not evidence that REAPER has recorded WRITE envelopes.

State round-trip, GUI restoration, control event tests, identical manifest/IDs and exact audio/state regression provide automated compatibility evidence, but do not replace the pending manual REAPER checks. Native file-chooser interaction and real desktop focus/tab behaviour also remain unverified.

Actual captures:

- `artifacts/gui-redesign/redesigned-running-editor.png` — 1000×1000 real editor driven by processed audio.
- `artifacts/gui-redesign/editor-750.png`, `editor-1400.png` — scale checks.
- `artifacts/gui-redesign/redesigned-editor.png` — final existing integration-test capture.

These are offscreen JUCE component captures, not a DAW screenshot or static image mockup.

## Known issues and next manual checks

Pre-existing issues from POST_BUILD_VERIFICATION.md remain intentionally unchanged: gain can temporarily exceed newly narrowed Range limits while smoothing; initial capture point arrives before the first full 20 ms interval; non-slider buttons lack explicit parameter gestures; WRITE capture depends on the message loop, has finite storage and does not split loops/seeks; loud non-vocal noise can trigger energy detection. Capture is volatile and READ remains smoothed. The GUI redesign does not claim to fix these or validate REAPER WRITE recording.

Open/reopen the installed VST3 in REAPER after closing any already loaded old instance. Check all controls, numeric editing, fine drags, keyboard/focus, all three automation modes, sidechain pins and 75–140% scaling. Save/reopen a copy of an existing project and compare parameter/automation restoration. REAPER should retain plugin identity without a new instance; rescan only if needed. Keep the checkpoint and installed backup until these host checks are accepted.
