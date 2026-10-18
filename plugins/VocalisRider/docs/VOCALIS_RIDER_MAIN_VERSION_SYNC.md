# Vocalis Rider Live main version synchronisation

Recorded 2026-10-18. **DESIGN LOCKED**, approved by Enos Takanaka. Default GUI: **480 × 600**, existing portrait 4:5 responsive scaling retained.

## Source and recoverable checkpoint

Authoritative source: `/home/enos/AudioDevelopment/VocalisRider`. Central mirror: `/home/enos/AudioDevelopment/VocalisAudio/plugins/VocalisRider`. Source has unusable Git metadata; central Git started clean at `0fe2413`. No existing uncommitted/untracked central changes were found.

Checkpoint: `/home/enos/AudioDevelopment/VocalisRider-checkpoints/main-sync-20261018T102107Z`. Complete previous destination and central docs are in `destination.tar.gz` and `central-docs.tar.gz`; the selected approved source is in `approved-source.tar.gz`. Archives were opened successfully and their hashes saved in `archive-sha256.txt`. Extract into a separate location for recovery. No history reset or destination deletion occurred.

## Files and exclusions

59 selected files copied; 19 added/updated relative to the destination. Complete inventory and SHA-256 manifest: `artifacts/main-version-sync/copied-files.txt` and `source-sha256.json`. Changed/added files before new lock/report documents:

- `newdesign.png`
- `Source/Editor.cpp`
- `tests/GuiTests.cpp`
- `docs/GUI_REDESIGN_CORRECTION_REPORT.md`
- `docs/GUI_REDESIGN_FROM_NEWDESIGN_REPORT.md`
- `docs/RIDER_COMPACT_GUI_REPORT.md`
- `docs/RIDER_SILVER_AND_SCREW_CORRECTION_REPORT.md`
- `artifacts/silver-screws/editor-1120x1400.png`
- `artifacts/silver-screws/editor-480x600.png`
- `artifacts/silver-screws/editor-560x700.png`
- `artifacts/silver-screws/editor-800x1000.png`
- `artifacts/silver-screws/silver-reference-comparison.png`
- `artifacts/compact-gui/editor-1120x1400.png`
- `artifacts/compact-gui/editor-480x600.png`
- `artifacts/compact-gui/editor-560x700.png`
- `artifacts/compact-gui/editor-640x800.png`
- `artifacts/compact-gui/editor-800x1000.png`
- `artifacts/compact-gui/running-editor.png`
- `artifacts/compact-gui/utilities.png`

The two new documents are mirrored in both Rider `docs/` directories and central `docs/`. Verification evidence contains new logs, manifests, comparison results and central captures.

Included all Source, CMake, tests and legacy fixtures, scripts, project documentation, licensing/attribution, approved screenshots and the existing regression harness source. `newdesign.png` is retained as historical reference only; implemented GUI and silver-screws capture are authoritative.

Excluded source build/build-release/build-sanitize, dist, caches, compiled plugins/objects/executables, temporary/debug/sanitizer logs and output, checkpoint archives, local Git metadata, credentials and private configuration. Local central build outputs are verification products, not synchronised assets. Existing destination-only historical artifacts (including pre-existing compiled evidence) remain preserved; see `destination-only-preserved.txt`. No destination-only Source/tests/scripts implementation files were found; nothing was identified for deletion. The old `Vocalis Leveler Plugin Interface.png` is retained as historical material, not a baseline.

## Verification

- Fresh CMake/Ninja Release build from the central project: passed, native Linux VST3; C++20, existing JUCE. Build log contains no compiler warnings/errors. Bundle: `build-main-sync/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`.
- All nine existing suites passed after build completion: DSP, Integration, VST3_Module, Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety, VST3_Reload. 0 failures; 9.78 seconds. An initial run started before the final VST3 link and failed VST3_Module because the module did not yet exist; its log is retained as `initial-ctest-build-race.log`. The full completed-build rerun passed.
- Source GUI harness: 274 checks passed. Central GUI suite also passed, covering actual 480 × 600 creation/restoration, proportional sizes through 1120 × 1400, parameter attachments, mouse/keyboard edits, scaled hit areas, factory presets, automation modes, utilities and metering.
- Latest central 480 × 600, 800 × 1000 and 1120 × 1400 captures are pixel-identical to approved review captures. At 560 × 700 a small informational-icon region differs at bounding box (402,591)-(413,602); source's fresh harness capture has the same small difference against central. Rails and all screw regions are pixel-identical at every compared size. No aesthetic changes were made to address dynamic capture variation. See `visual-comparison.txt` and `central-editor-*.png`.
- Existing AudioRegression harness linked separately to source and central shared libraries: all 34,110,984 output/state bytes identical across 24 sample-rate/block-size/channel combinations, presets, automation, Rider, bypass and sidechain. SHA-256: `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`.
- Source and central VST3 `moduleinfo.json` are byte-identical, including component/controller class IDs. CMake remains byte-identical, preserving version 1.0.0, vendor Vocalis Audio, manufacturer Vcls, plugin code VrLv and bundle ID audio.vocalis.riderlive. Native load and 20-cycle reload passed.
- Integration/Audit cover parameter/default schema, all eight factory presets, preset selection and serialized parameter/program state restoration including the pre-debug legacy fixture, automation notification/gesture behaviour, sidechain layouts and CSV export. No separate user-preset file-save UI is implemented; host state serialization is the available save/restore path tested.
- Final selected-source comparison: 59/59 files byte-identical; new documents also identical. Source production files were not edited during this task.

## Git and controls

Central changes consist of the approved Editor.cpp and GuiTests.cpp updates, new review screenshots/reports/design-lock documents and verification evidence. Full status: `artifacts/main-version-sync/git-status-after.txt`. Existing central README, LICENSE, CONTRIBUTING.md and unrelated docs remain unchanged. Vocalis Voice was not modified. No commit, push, tag, release or publication occurred. No installed VST3 bundle was overwritten. Install_Safety uses temporary isolated HOME directories. Temporary build-release alias used for installer tests was removed after tests.

`git diff --check` reports one inherited trailing-whitespace line in approved Editor.cpp (line 360); preserved to keep the approved source exact.

## Unverified items and remaining issues

REAPER was not launched. Manual host embedding, real project reopening, host envelope recording, host preset-file saving and display/DPI behaviour remain unverified. Automated compatibility evidence does not substitute for manual REAPER validation. No functional test failures remain. The small 560 × 700 informational-icon capture variation and inherited whitespace are recorded above.

The implemented silver/navy design is the baseline. See `VOCALIS_RIDER_APPROVED_DESIGN.md` for change control. Functional bug fixes, accessibility and security maintenance remain permitted; necessary visual changes must be separately reported for creator approval.
