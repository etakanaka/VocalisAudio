# Vocalis Rider Live — compact GUI and screw restoration

Completed 2026-10-18. This is a focused update to the approved interface, not a redesign. The only production file changed is `Source/Editor.cpp`.

## Inspection and recoverable checkpoint

Inspected the current editor, LookAndFeel, parameter attachments, layout/scaling, screw renderer and existing GUI tests, plus both available references:

- `newdesign.png`
- `/home/enos/AudioDevelopment/VocalisVoice/artifacts/gui-layout-2-2.png`

The current source implements the approved correction captured in `artifacts/gui-correction/running-editor.png`; the older `artifacts/newdesign/` images were also preserved. No earlier design was substituted for the current source.

Git metadata is still unusable. Before editing, Source, tests, docs, scripts, CMake, README, the design reference and existing GUI review images were archived to:

`artifacts/compact-gui/checkpoint-20261018/before.tar.gz`

The checkpoint also contains source/test checksums, the original VST3 manifest and archive checksum. `gzip -t` passed. Extract into a separate directory to inspect or recover the prior work without overwriting newer changes.

Baseline Release build succeeded; all nine baseline CTest suites passed. Logs: `baseline-build.log` and `baseline-tests.log` under `artifacts/compact-gui/`.

## Dimensions and proportional scaling

Previous default: **800 × 1000**. Previous minimum: 640 × 800.

New default and minimum: **480 × 600**. The existing maximum of 1120 × 1400 remains available. The resize constrainer still enforces **4:5**.

The complete existing **1000 × 1250 logical canvas** and every individual control bound are unchanged. Both editor painting and the interactive canvas retain the existing uniform transform:

`scale = min(editorWidth / 1000, editorHeight / 1250)`

| Opening/display size | Canvas scale | Relative to old default |
| --- | ---: | ---: |
| 480 × 600 — default | 0.48 | 60% |
| 560 × 700 — medium | 0.56 | 70% |
| 800 × 1000 — original | 0.80 | 100% |

The actual JUCE AudioProcessorEditor is constructed at 480 × 600; this is not merely a smaller drawing inside an 800 × 1000 editor. Tests verify that the scaled canvas exactly fills the editor at every tested proportional size, with no cropping or empty margins. No scrollbars or stretching were introduced. Text, textures, readouts, screws, borders, knobs, faders, meters and mouse coordinates all use the same existing transform.

No individual control was repositioned. The full editor `resized()` implementation is byte-identical to the checkpoint. The complete LookAndFeel is also unchanged. See `binding-and-layout-verification.txt` and `editor.diff`.

## Screws restored and standardised

Restored **four missing screws**:

- Range: top-right and bottom-right.
- Rider: top-right and bottom-right.

These use the existing inset, radius and renderer already used by Output. All three lower panels now have four small corner fasteners. Their logical centers are:

| Panel | Left / right x | Top / bottom y |
| --- | --- | --- |
| Range | 117 / 340 | 338 / 1017 |
| Rider | 382 / 621 | 338 / 1017 |
| Output | 662 / 885 | 338 / 1017 |

Retained the four side-rail screws and all four outer-frame screws, including the two framing the footer. No duplicate footer screws or new unrelated mounting points were added. **20 unique screws total**: 12 panel, 4 rail, 4 outer-frame/footer.

The existing silver shading, head sizes, outlines, highlights and shadows remain. The shared central recess is now a consistent dark cross, following the Vocalis Voice hardware language and retaining contrast at compact scale. Panel heads remain radius 7 logical pixels (about 6.7 pixels across at the default size); outer frame and rail heads retain their previous proportions. No screws were enlarged or functional controls moved.

`VerifyRender.py` checks all 20 positions at each requested size for a dark recess and bright metallic rim. All 60 size/position checks passed. The images were also inspected visually.

The original-size image matches the checkpoint pixel-for-pixel outside screw regions and transient automation-button hover/press faces. Those faces can differ after synthetic clicks/resizing; their drawing code is unchanged. See `render-verification.txt`.

## Files modified/generated

- **Production:** `Source/Editor.cpp` — screw recess, four restored screws, default/minimum editor dimensions only.
- **Tests:** `tests/GuiTests.cpp` — default-size and constraint assertions, canvas coverage, requested-size snapshots, scaled hit/event checks and state-restored editor dimensions. Existing functional assertions are retained.
- **Documentation:** this report.
- **Evidence:** `artifacts/compact-gui/` — checkpoint, logs, source diffs/checks, regression captures, render verification script/results and screenshots.

No processor/DSP/header/CMake or other existing test source changed. No commit, push, publishing, user-plugin installation or REAPER configuration change was performed. The existing Install_Safety suite uses its temporary test environment.

## Preserved functionality

All existing APVTS slider setup and attachment code, control setters, parameter gestures, button callbacks, preset selection, state polling and timer logic are unchanged. Source comparisons verify this directly.

Bindings remain `target`, `spill`, `vocal`, `music`, `rider`, `output`, `idle`, `rangeMin`, `rangeMax`, `attack`, `automation` and `bypass`. No IDs, parameter ranges/defaults or preset/state semantics changed. The existing preset selector, Export CSV, Clear and About remain in the unchanged expandable utility panel. Meters still read the original input RMS/peak, music RMS, activity and gain atomics.

DSP, vocal riding, sidechain, Off/Write/Read, host automation, saved state and host-facing identity remain in unchanged processor code.

## Build and regression results

- **Release native Linux VST3 build succeeded**, using CMake/Ninja. Final build log contains no compiler warnings/errors.
- **9/9 CTest suites passed, 0 failures:** DSP, Integration, VST3_Module, Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety and VST3_Reload.
- **274 GUI checks passed.** Existing binding, parameter update, keyboard, preset, state restore, real metering, mode and lifetime checks are retained. Added checks verify the actual 480 × 600 default, minimum dimensions, 4:5 constraint, recreated-editor default size, exact canvas coverage and multi-size interaction.
- At **480 × 600, 560 × 700 and 800 × 1000**, transformed editor hit tests reach every attached slider, both Range handles, speed/mode buttons, utility toggle and utility controls. Fixed **10 physical-pixel synthetic drags** change the correct slider/Range parameters. Button callbacks and Bypass continue to update their original parameters. Utility controls remain inside the editor bounds.
- Existing 640 × 800 and 1120 × 1400 resize coverage remains.
- **Audio and serialized state are byte-identical before/after:** 34,110,984 bytes, SHA-256 `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`. The existing AudioRegression harness was compiled against the pre-edit and final shared libraries. It covers 24 sample-rate/block-size/channel configurations, all presets and automation modes, Rider, bypass and music sidechain. Evidence: `audio-comparison.txt`, `before-audio-state.bin`, `after-audio-state.bin`.
- Protected checksums confirm `Source/Engine.h`, `Source/Plugin.cpp`, `Source/Plugin.h`, CMake and all other existing test sources are unchanged.
- The complete VST3 `moduleinfo.json` is byte-identical to the checkpoint, including product/vendor/version and component/controller class IDs. Native module loading and repeated reloading pass.

Built bundle:

`build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`

## Screenshots

Actual JUCE component captures, using processing-driven values:

- `artifacts/compact-gui/editor-480x600.png`
- `artifacts/compact-gui/editor-560x700.png`
- `artifacts/compact-gui/editor-800x1000.png`
- `artifacts/compact-gui/running-editor.png` — initial compact default.
- `artifacts/compact-gui/utilities.png` — compact expanded tools with real meters.

Additional preserved resize coverage: `editor-640x800.png`, `editor-1120x1400.png`.

## Legibility, interaction limits and unverified REAPER behavior

At 480 × 600, the main section names, mode labels and large digital readouts remain distinguishable in the native-size capture. Knobs and fader caps retain useful mouse areas. Fine scale text, the small Range endpoint line, hover value fields and utility captions are small; some fine print has approximately 4–6-pixel glyph height. The information/utility button is only about 12.5 pixels across. This can be difficult on a dense laptop display or with a trackpad. These items were not enlarged, moved or removed because only proportional scaling was authorised. Medium/original sizes remain available when more readable labels or larger targets are needed.

Scaled synthetic hit tests, component drags and callback checks passed. These are not a manual usability study or native desktop mouse/trackpad validation. Native file chooser interaction was not tested.

**REAPER was not launched.** Its actual plugin window framing, host DPI/desktop scaling, resize negotiation, live envelope recording and saved-project reopening remain unverified manually. JUCE reports the requested 480 × 600 editor dimensions; physical display size can also depend on host/desktop DPI settings. No claim of manual REAPER validation is made.
