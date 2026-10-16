# Author attribution report — Vocalis Voice

Date: 2026-10-16. **Enos Takanaka — Creator and Maintainer**. **Vocalis Audio — Software Brand**. Development: OpenAI Codex assistance, C++20 and JUCE.

## Original attribution and checkpoint

VERIFIED PASS: Voice is not a Git repository. Central clone `../VocalisAudio` has the requested origin and initially reported `?? plugins/`; those untracked copies were preserved. Before editing either project a full 12 GiB archive, including existing builds, checkpoints and the central Git metadata, was saved to `/tmp/vocalis-attribution-checkpoint/projects.tar`. Initial expanded Git status is alongside it. To recover, extract selected files into a separate directory first. No reset, commit, push, publication or installation performed.

Original manufacturer was Vocalis Audio. Settings already provided About / signal flow with technical notes but no creator credit. NOTICE.md established AGPL-3.0-or-later for combined source and documented adaptations from Rider; those notices remain intact.

## New attribution and files

VERIFIED PASS: original project C++ source and tests now have creator/maintainer, copyright 2026 and Codex-assistance headers. Existing SPDX and adaptation notices remain verbatim. The requested AGPL-3.0-only line was omitted because this project's verified licence is AGPL-3.0-or-later. Generated, vendored JUCE, external SDK source and reference images received no attribution headers.

Exact modified files and per-file header inventory: `artifacts/author-attribution/changed-files.json`. All preexisting `src/**/*.h`, `src/**/*.cpp`, `tests/**/*.h` and `tests/**/*.cpp` received headers (38 files); the new `src/ui/AboutPanel.h` also has one. Functional edits are limited to `src/PluginEditor.h`, `src/PluginEditor.cpp`, new `src/ui/AboutPanel.h` and GUI verification in `tests/GuiTests.cpp`. README and this report are documentation changes. Build, audio, identity and test evidence is under `artifacts/author-attribution/`.

## About and README

FIXED AND RETESTED: existing Settings > About / signal flow now opens an editor child panel using the existing colours, panel borders, inherited buttons and scaled surface. Original signal-flow/help text retained. The creator, Codex/C++/JUCE acknowledgement, native Linux VST3 / REAPER target, contributor-inclusive copyright, verified licence and GitHub URL display with actual `JucePlugin_VersionString`. Settings' former literal version was replaced by that macro. Existing controls and window scaling unchanged. Repository URL is displayed as text.

Expanded GUI test opens the same panel through `showAbout()`, checks text, saves a rendered screenshot and closes it. Screenshot visually inspected. Settings popup selection itself is UNVERIFIED in a native host because native display attachment is unavailable. An initial test used an already-destroyed editor; recreating the editor fixed it and all tests passed again.

README gained the requested Author & Development section without removing instructions or limitations. No OpenAI logo or official affiliation claimed.

## Metadata and compatibility

VERIFIED PASS: CMake unchanged. COMPANY_NAME Vocalis Audio; PRODUCT_NAME Vocalis Voice; PLUGIN_MANUFACTURER_CODE Vcls; PLUGIN_CODE Vvce; BUNDLE_ID audio.vocalis.voice; version 1.0.0; VST3 only; categories Fx / Dynamics / Reverb. No unsupported author metadata fields introduced.

VST3 audio class `ABCDEF019182FAEB56636C7356766365` and controller `ABCDEF011234ABCD56636C7356766365` are preserved; generated moduleinfo.json is byte-identical to checkpoint (see integrity.json). Parameter IDs, ranges, defaults, presets, state serialization, callbacks, routing and all DSP source are unchanged except attribution comments. Existing processor tests and VST3 probe verify state roundtrip, parameter names/order, automation, layouts and load/unload.

## Licensing and third-party materials

VERIFIED PASS: NOTICE.md, README and preserved Rider adaptation SPDX notices explicitly use AGPL-3.0-or-later; full licence retained. No silent relicensing to AGPL-3.0-only. Third-party copyrights remain with their holders; header statements concern original Vocalis work and do not claim exclusive ownership.

JUCE checkout LICENSE.md supplies AGPLv3/commercial options and its own module dependency notices; official legal index: https://juce.com/legal/. Bundled VST3 SDK has Steinberg's MIT notice. JUCE, SDK and their generated resources were not modified. Future binary distributions still need corresponding-source and third-party notice review for the exact build inputs.

RiderDesign.h and RiderLevelMeter.h are adapted in-project graphical source with original SPDX and provenance comments retained; src/ui/rider/NOTICE.md and LICENSE.md remain unchanged. Other active graphical components use code drawing; no embedded third-party font found (default system font selection). Reference/concept PNGs are separate materials and received no ownership claim. No active Calf asset or Calf-derived source was identified in source/notices. Independent provenance of historical reference images and a full transitive dependency/asset rights audit remain UNVERIFIED.

## Builds and regression results

VERIFIED PASS: baseline 7/7 CTest tests; Release VST3 build; final 7/7 tests including processor/state, DSP, GUI, design parity, stress and callback allocations. Logs are in artifacts/author-attribution/. VST3 binary regenerated in build/Release without changing installed plugins.

VERIFIED PASS: existing AudioRegression fixture compared 5,255,600 float32 output samples before/after, with zero different samples and zero absolute/RMS error; identical SHA-256 hashes in audio-comparison.json. This covers the fixture's rates/layouts/parameter changes, not all possible input signals.

VERIFIED PASS: JUCE Vst3Probe loaded the new VST3 and passed parameter order, mono/mono-to-stereo/stereo processing, measured automation, state restoration and repeated load/unload. This is a JUCE host probe, not REAPER.

UNVERIFIED: XOpenDisplay failed in the VST3 probe; native editor attachment and actual REAPER saved-session/GUI integration were not tested. GUI screenshots are compiled editor renders. No claim of REAPER testing is made. No remaining automated failures.

VERIFIED PASS: central README and CONTRIBUTING.md were updated after both plugins were verified. The central repository's untracked plugin copies remain untouched. Existing test-generated screenshots/logs in this working project were restored from the checkpoint; new evidence is isolated in artifacts/author-attribution/.

## Licence scope requiring review

UNVERIFIED: the central VocalisAudio root LICENSE contains GNU GPL version 3, whereas both plugin trees explicitly declare AGPL-3.0-or-later. This preexisting difference was preserved, not resolved by relicensing. Maintainer review is needed to clarify the central licence's intended scope before publication. Plugin About text reflects the explicit plugin notices. Full asset/dependency provenance remains subject to the uncertainties above.

## Source header inventory

- `src/PluginEditor.cpp`
- `src/PluginEditor.h`
- `src/PluginProcessor.cpp`
- `src/PluginProcessor.h`
- `src/dsp/HarmonicProcessor.h`
- `src/dsp/Metering.h`
- `src/dsp/PresenceProcessor.h`
- `src/dsp/Smoothing.h`
- `src/dsp/ToneProcessor.h`
- `src/dsp/VocalDelay.h`
- `src/dsp/VocalDoubler.h`
- `src/dsp/VocalDynamics.h`
- `src/dsp/VocalReverb.h`
- `src/dsp/VoiceEngine.h`
- `src/state/Parameters.h`
- `src/state/PresetManager.h`
- `src/ui/EffectControls.h`
- `src/ui/GainReductionMeter.h`
- `src/ui/IntensityKnob.h`
- `src/ui/LevelMeter.h`
- `src/ui/SensitivityIndicator.h`
- `src/ui/VocalisLookAndFeel.h`
- `src/ui/VocalisTopBar.h`
- `src/ui/rider/RiderDesign.h`
- `src/ui/rider/RiderLevelMeter.h`
- `tests/AudioRegression.cpp`
- `tests/AuditDspTests.cpp`
- `tests/CallbackAllocation.cpp`
- `tests/CaptureX11.cpp`
- `tests/DesignParity.cpp`
- `tests/DspTests.cpp`
- `tests/EditorOwner.h`
- `tests/GuiTests.cpp`
- `tests/RiderReferenceDesign.h`
- `tests/SanitizerRuntimeProbe.cpp`
- `tests/ThreadStress.cpp`
- `tests/VoiceTests.cpp`
- `tests/Vst3Probe.cpp`
- `src/ui/AboutPanel.h`
