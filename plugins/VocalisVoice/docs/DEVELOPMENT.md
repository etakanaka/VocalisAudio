# Incremental build record

1. Environment: inspected workspace, existing JUCE README/licence/version and Linux tools; identified supplied design PNG; read official VoiceCentric manual and routing diagram. New project only.
2. Skeleton: configured JUCE CMake VST3-only target with new IDs; built the native bundle and manifest. Build log: artifacts/build-skeleton.log.
3. Core processing: implemented independent tone, dynamics, presence, harmonics, smooth trims and meters as testable header modules. Core tests cover zero through maximum intensity and accurate trim/GR.
4. Parallel effects: implemented and numerically tested circular delay, modulated doubler and orthogonal FDN reverb independently and through the parallel engine.
5. Interface: connected custom JUCE controls to APVTS; rendered the actual production editor; fixed input label/sensitivity spacing and checked normal/minimum scale.
6. Presets/state: added ten independent presets and schema-versioned APVTS state, preserved trims, exposed host bypass and Program selector.
7. Validation: tested numerical DSP, processor state/layouts, exported VST3 loading/audio/state, programmatic GUI control feedback, installer safeguards and CPU throughput; added Debug and sanitizer runs. Native GUI attachment is inaccessible without an X11 connection, and REAPER-specific checks are explicitly pending.
8. Package: Release bundle plus source, documentation, install/uninstall/build/test tools, actual editor renders and test records, with archive checksum. Local ~/.vst3 installation is blocked by the read-only execution mount; staged installation passed.

Two test fixture corrections were made without changing DSP tolerances: bypass initially reused already processed samples while expecting original input; VST3 parameter count initially omitted JUCE's additional Program selector. A host-harness native GUI crash was traced to unavailable X11 display and guarded with XOpenDisplay preflight, rather than treating a blank image as successful GUI validation.
