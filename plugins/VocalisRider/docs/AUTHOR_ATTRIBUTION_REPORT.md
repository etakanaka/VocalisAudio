# Author attribution report — Vocalis Rider Live

Date: 2026-10-16. Creator and Maintainer: **Enos Takanaka**. Software brand: **Vocalis Audio**. Development: OpenAI Codex assistance, C++20 and JUCE.

## Checkpoint and original attribution

VERIFIED PASS: this working directory is not a Git repository. The central clone at `../VocalisAudio` has origin `https://github.com/etakanaka/VocalisAudio.git`; initial status was `?? plugins/`. Full tracked/untracked project contents, build products and central `.git` were archived before editing to `/tmp/vocalis-attribution-checkpoint/projects.tar` (12 GiB). Initial expanded Git status is alongside it. No reset, commit, push, installation or publishing occurred. Preserve this archive for recovery; extract selected files into a separate directory before restoring.

Originally source files carried AGPL-3.0-or-later SPDX notices, the manufacturer was Vocalis Audio, and there was no About or Settings interface or named creator section.

## Changes

VERIFIED PASS: original project-owned `Source/*.h`, `Source/*.cpp` and `tests/*.cpp` now identify Enos Takanaka as creator/maintainer, copyright 2026, and OpenAI Codex assistance. Existing SPDX notices remain; no AGPL-3.0-only identifier was added because the actual licence differs. No third-party or generated source was headed.

Files modified: all four files in `Source/`; all nine C++ implementation files in `tests/`; `README.md`; this report. GUI test extended to open/close About and check creator/licence/build-version text. Evidence is in `artifacts/author-attribution/`.

A compact ABOUT button occupies unused footer space. Its child panel uses existing Suite panel colours, inherited button styling, existing scaled canvas and actual `JucePlugin_VersionString`; no existing control moved. Copyright credits include applicable contributors. The repository URL is shown as readable text. README gained the requested Author & Development section without removing existing content.

## Metadata and compatibility

VERIFIED PASS: CMake unchanged. Version 1.0.0; COMPANY_NAME Vocalis Audio; PRODUCT_NAME Vocalis Rider Live; PLUGIN_MANUFACTURER_CODE Vcls; PLUGIN_CODE VrLv; BUNDLE_ID audio.vocalis.riderlive; native Linux VST3 only.

VST3 audio class: `ABCDEF019182FAEB56636C7356724C76`; controller: `ABCDEF011234ABCD56636C7356724C76`. Generated manifest unchanged from checkpoint. No unsupported metadata fields added.

VERIFIED PASS: processor, parameter layout/ranges/defaults, Engine and state code differ only by comments. Existing Audit legacy-state test and Integration state checks pass. Audio callbacks, sidechain routing, presets and automation implementation preserved.

## Licensing and third-party review

VERIFIED PASS: README, source SPDX and docs/DEPENDENCIES.md explicitly establish AGPL-3.0-or-later. Full LICENSE.md preserved; no relicensing. Requested AGPL-3.0-only header omitted due to the differing existing licence.

JUCE checkout LICENSE.md records AGPLv3/commercial options. Existing JUCE notice and SPDX inventory in docs/licenses/ and Steinberg VST3 MIT copyright remain untouched; module-specific dependencies stay under JUCE. Current official legal index: https://juce.com/legal/. No exclusive copyright asserted over dependencies.

Original Vocalis code and geometric GUI drawing are separate from JUCE/SDK source. Existing GUI report states geometry rather than embedded reference pixels; default system fonts are used. No Calf attribution or embedded Calf assets were found in active source or project notices. Historical/reference artwork provenance and a complete dependency redistribution audit remain UNVERIFIED; no ownership is asserted over reference images. Generated resources and build code were excluded from headers.

## Verification

VERIFIED PASS: baseline 9/9 CTest tests; Release build and final 9/9 CTest tests, including DSP, Integration, VST3 module/reload, legacy Audit, GUI, thread stress and install safety. Logs saved in artifacts/author-attribution/. VST3 binary generated in build-release, without copying to installed plugin directories.

FIXED AND RETESTED: an initial native AlertWindow implementation crashed in this environment when opened. Replaced with an editor child panel; the expanded GUI test now opens, inspects, snapshots and closes it successfully. An initial test compile error from an unavailable AlertWindow getter was also corrected.

VERIFIED PASS: About snapshot inspected for readable creator, copyright, licence, version and correct repository URL. The author name displays correctly.

UNVERIFIED: actual REAPER interaction, reopening saved REAPER sessions, and native host window behavior were not exercised. GUI evidence comes from JUCE test rendering, not REAPER. No installed plugin binaries changed.

VERIFIED PASS: deterministic Engine comparison used the archived pre-edit Engine.h and current Engine.h compiled with the same C++20/-O3 fixture, across four sample rates, all automation modes, bypass, sidechain and gain changes. All 2,457,600 float32 samples are bit-identical (zero absolute/RMS error); see audio-comparison.json and audio-fixture.cpp. This covers the fixture, not all possible input signals.

Exact modified-file/header inventories and per-file integrity hashes are in `artifacts/author-attribution/changed-files.json` and `integrity.json`. Thirteen preexisting C++ files received headers: four Source files and nine test files. Existing test-generated screenshots/logs were restored from the checkpoint to avoid incidental documentation artifact changes.

## Licence scope requiring review

UNVERIFIED: the central VocalisAudio root LICENSE contains GNU GPL version 3, whereas both plugin trees explicitly declare AGPL-3.0-or-later. This preexisting difference was preserved, not resolved by relicensing. Maintainer review is needed to clarify the central licence's intended scope before publication. Plugin About text reflects the explicit plugin notices. Full asset/dependency provenance remains subject to the uncertainties above.

## Source header inventory

- `Source/Editor.cpp`
- `Source/Engine.h`
- `Source/Plugin.cpp`
- `Source/Plugin.h`
- `tests/Allocation.cpp`
- `tests/Audit.cpp`
- `tests/DspAudit.cpp`
- `tests/DspTests.cpp`
- `tests/GuiTests.cpp`
- `tests/Integration.cpp`
- `tests/Performance.cpp`
- `tests/ThreadStress.cpp`
- `tests/Vst3Smoke.cpp`
