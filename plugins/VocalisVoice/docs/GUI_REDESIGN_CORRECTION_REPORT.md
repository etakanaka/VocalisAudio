# Vocalis Voice GUI correction report

Date: 2026-10-18

## Subsequent laptop-friendly editor sizing

The approved interface now opens at 960 x 617 and retains the complete reference layout through the same logical canvas and proportional JUCE scaling. Settings offers 840 x 540 compact and 1200 x 771 large choices; window resizing remains enabled at a fixed aspect ratio up to 2054 x 1321. The default, compact, and large compiled-editor screenshots are `artifacts/gui-960x617.png`, `artifacts/gui-840x540.png`, and `artifacts/gui-large-1200x771.png`. Release build and all seven CTest suites pass. The VST3 probe verifies identity, parameters, processing, automation, and state; native editor embedding and manual REAPER checks remain unverified because the probe cannot connect to X11.

## Files changed

- `src/PluginEditor.cpp`
- `src/ui/VocalisTopBar.h`
- `src/ui/rider/RiderDesign.h`
- `src/ui/EffectControls.h`
- `src/ui/SensitivityIndicator.h`
- `src/ui/LevelMeter.h`
- `src/ui/rider/RiderLevelMeter.h`
- `src/ui/GainReductionMeter.h`
- `tests/GuiTests.cpp`
- `tests/Vst3Probe.cpp` (minimum-size editor verification)
- `artifacts/gui-reference-layout.png`
- `docs/GUI_REDESIGN_CORRECTION_REPORT.md`

## Changes from the previous redesign

The editor now uses a 1580 x 1000 logical canvas matching `newdesign.png`'s aspect and proportions. The previous smaller, simplified layout was replaced with the reference's exact three-panel geometry, full-width brushed header, metallic side rails, bottom knurling, and rack fasteners. Screw heads use recessed cross slots; panel edges have brighter inset strokes.

Control placement and styling were tightened against the reference: the Input and Output meters and gain dials now sit at the reference positions; the central Intensity dial has a 0–100 numbered scale, denser ticks, a smaller silver face inside the wide range ring, and the large percentage readout; Delay, Doubler, and Reverb use silver dials with rectangular value boxes and separate square lamp switches. Header typography, preset selector, gear mark, bypass lamp, channel labels, sensitivity lamp, and footer were resized and aligned. Meter displays use segmented LEDs and continue to read real RMS, peak, sensitivity, clipping, and gain-reduction values.

A dedicated reference-state render uses the existing `Vocal Polish` factory preset and real processed audio for the meter display. It is saved at `artifacts/gui-reference-layout.png`.

## Functionality preserved

Only editor and GUI drawing files plus GUI verification fixtures changed. No DSP, processor, parameter layout, preset definitions, serialization, host/automation, or plugin-target code changed. Existing parameter IDs, ranges, defaults, and attachments remain intact:

- Input Gain — `vv_input_db`; Intensity — `vv_intensity`
- Delay — `vv_delay_on` / `vv_delay_db`
- Doubler — `vv_doubler_on` / `vv_doubler_db`
- Reverb — `vv_reverb_on` / `vv_reverb_db`
- Output Gain — `vv_output_db`; Bypass — `vv_bypass`

Preset selection still uses the processor's existing program API. GUI tests verify slider and button attachments, host feedback, preset selection, value-field geometry, live mono/stereo meters, and editor resize/render behavior.

## Verification

- Release `VocalisVoice_VST3` build: passed.
- Release CTest: 7/7 passed.
- `GuiTests`: passed, including the new reference-state screenshot and readout geometry checks.
- `Vst3Probe`: passed VST3 discovery, parameter identity/order, mono/stereo bus processing, measured automation, state roundtrip, and repeated load/unload.
- Processor, DSP, parameters, presets, CMake plugin identity, and state files were not edited.
- A pre-correction source/docs/tests checkpoint was saved at `/tmp/vocalis-gui-correction-checkpoint-20261018/` because this checkout has no usable Git metadata.

## Remaining differences and unverified items

The reference uses photorealistic metal and texture. This implementation reproduces its shapes, proportions, hardware, and materials with JUCE drawing, so the surface grain and reflections remain cleaner and more regular. The green sensitivity lamp changes color with the existing live sensitivity state; it can be amber or red at higher levels. Gain-reduction LEDs only light when the real processor reports reduction, so their appearance depends on the audio being processed.

The native editor could not be opened through the VST3 probe because this environment has no accessible X11 display. JUCE offscreen editor snapshots passed. REAPER-specific scanning and in-host interaction remain unverified here.
