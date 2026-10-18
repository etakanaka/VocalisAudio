# GUI redesign from newdesign.png

Date: 2026-10-18. Reference: project-root `newdesign.png` (1122 × 1402). Production changes are confined to `Source/Editor.cpp`.

## Inspection and checkpoint

The existing GUI was a square 1000 × 1000 logical canvas, uniformly scaled with a fixed square resize constraint (750–1400). The editor contains its LookAndFeel, meters, dual Range control, About overlay, attachments, and button callbacks. `Source/Plugin.cpp` owns parameter definitions, presets, host/state/automation and processing integration; `Source/Plugin.h` defines the processor; `Source/Engine.h` contains DSP. None of these processor/DSP files were edited. CMake, identity definitions, scripts and parameter architecture were retained.

Git metadata was unusable (`git status`: not a git repository). Before editing, a filesystem archive preserved Source, tests, docs, scripts, CMake, README and reference image:

`artifacts/newdesign/checkpoint-20261018-1108/source-before.tar.gz`

The checkpoint also includes source/test checksums and the pre-change VST3 manifest. Extract into a separate directory to recover the original files without overwriting subsequent work. Existing build and test logs remain in their original locations; this task's evidence is under `artifacts/newdesign/`.

## Files changed

- `Source/Editor.cpp`: rendering, styling, layout and portrait resize constraints only.
- `tests/GuiTests.cpp`: replace old fixed Range drag coordinates/distances with proportional coordinates; check portrait resize dimensions; save fresh screenshots under the new evidence directory. Assertions for bindings and behaviour are retained.
- This report; generated builds, test logs, checkpoints, regression captures and GUI PNGs.

## Appearance

The reference establishes a tall navy metal chassis, brushed silver side rails and fasteners, a top Target strip, Ride/Idle lamps, three silver sensitivity knobs, three large framed fader panels, lower automation controls and footer branding. These are recreated procedurally with JUCE gradients, geometry and fine horizontal material lines. The reference PNG is not embedded as a flat background.

The editor now opens at 800 × 1000, using a 1000 × 1250 logical canvas and fixed 4:5 aspect ratio. Resize limits are 640 × 800 through 1120 × 1400. Uniform scaling preserves the layout. Silver knobs have dark pointers, radial graduations and editable dB boxes. Metal fader caps have ridges; Rider has a red position line. Target uses measured green/yellow segments and a metal/red target marker. Navy framed panels, black readouts, green Ride lamp and silver/red/green automation buttons follow the reference. Footer branding retains VOCALIS RIDER LIVE.

Extra existing functionality is integrated: presets, bypass, export, clear and About are in the slim top utility strip. Input and music meters remain real measurements beside the fader areas. Idle remains an attached draggable marker and editable value beside Range. Range continues to show both actual bounds rather than a fabricated single value.

## Binding verification

| Visible control | Existing binding / source |
| --- | --- |
| Target | APVTS SliderAttachment `target`; strip level reads `meterInput` |
| Spill, Vocal, Music | APVTS attachments `spill`, `vocal`, `music` |
| Range handles and readouts | `rangeMin`, `rangeMax`; existing gestures and setters |
| Idle marker and value | APVTS attachment `idle` |
| Rider | APVTS attachment `rider`; existing Off/Write engine-gain display and Read editing retained |
| Output | APVTS attachment `output` |
| Fast / Slow | Existing `attack` setters, values 1 / 0 |
| Off / Write / Read | Existing `automation` setters, values 0 / 1 / 2 |
| Bypass | Existing `bypass` setter and selected-state polling |
| Presets | Existing `setCurrentProgram` and program selection polling |
| Input meter / peak | Existing `meterInput`, `meterPeak` atomics |
| Music meter | Existing `meterMusic` atomic |
| Ride / Idle lights | Existing `activity` atomic and its inverse |
| CSV export / clear / About | Existing callbacks and About content retained |

Attachment creation, parameter IDs, ranges, defaults, control value formatting, callbacks, gesture logic, timer logic and host notifications are unchanged. No duplicate parameters were introduced. Existing automated GUI tests exercise each slider in both directions (GUI writes and external updates), mouse/keyboard edits, both Range handles, buttons, modes, Rider enabled states, actual processing-driven metering, preset synchronization, state restoration and repeated editor lifetime. Utility/About coverage is retained.

## Validation

- Baseline Release build succeeded; all 9 existing CTest suites passed.
- Final Release native Linux VST3 build succeeded. Logs: `artifacts/newdesign/build.log`.
- Final CTest suites: DSP, Integration, VST3_Module, Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety, VST3_Reload. Results in `artifacts/newdesign/tests.log`.
- Before/after actual processor audio and serialized state compared byte-for-byte: **34,110,984 bytes identical**, SHA-256 `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`. Existing AudioRegression harness covers 24 combinations of sample rate, block size and channel layout, all presets, all automation modes, Rider, bypass and music sidechain. The same checksum also matches the pre-existing regression captures. See `audio-comparison.txt` and `audio-before.bin` / `audio-after.bin`.
- Protected-file checks confirm only Editor.cpp and the layout-dependent GuiTests.cpp differ from the checkpoint among hashed sources/tests/CMake. Plugin.cpp, Plugin.h and Engine.h remain byte-identical.
- VST3 moduleinfo.json compares byte-identically with the checkpoint, preserving product/vendor/version/class IDs. Native module load/reload and automation/state checks run in the existing tests.
- JUCE GUI creation, rendering, events and resizing exercised by the GUI executable. These are component snapshots and direct test events, not DAW screenshots.

Built bundle: `build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`. No installed plugin or REAPER configuration was changed.

## Review images

- `artifacts/newdesign/running-editor.png` — actual default 800 × 1000 editor with processing-driven meter/activity/gain values.
- `artifacts/newdesign/editor-640x800.png` — minimum size.
- `artifacts/newdesign/editor-1120x1400.png` — maximum size.

## Remaining differences and unverified items

The reference is illustrative hardware: exact typography, photorealistic reflections, seven-segment digits and machining details are approximated with JUCE rendering. Fast/Slow retains two functional buttons rather than a single mechanical switch. Additional editable values, meters and utility controls require space absent from the reference. Target retains its real -42…0 dBFS values, and Range shows its two real limits. Rendered readings depend on actual plugin state and deliberately do not copy the image's example numbers.

REAPER was **not launched**. REAPER embedding, real envelope recording, project reopening in REAPER, native file chooser interaction, desktop focus and host-specific/high-DPI scaling remain unverified. Automated state, parameter, native module and exact processing regression checks provide evidence but do not replace these host checks. No DSP, automation, preset or compatibility change is claimed.
