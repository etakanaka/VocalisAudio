# Vocalis Rider Live — GUI correction against newdesign.png

Completed 2026-10-18. Reference: `newdesign.png` (1122 × 1402). This pass corrects the previous GUI rather than changing the processor. **The only production file changed is `Source/Editor.cpp`.**

## Checkpoint and scope

Before editing, the prior Source, tests, documentation, CMake and three prior review renders were archived to:

`artifacts/gui-correction/checkpoint-20261018/before.tar.gz`

The dated checkpoint includes source/CMake SHA-256 checksums and the pre-correction VST3 manifest. Archive integrity was checked with `gzip -t`. Git metadata remains unusable. To recover or inspect the prior version, extract the archive into a separate directory; avoid overwriting subsequent work.

Other changed/new files:

- `tests/GuiTests.cpp`: new screenshot destinations, utility-panel access checks, and actual component hit-testing checks for Idle and Range. Existing parameter, mode, state and preset assertions remain. A pre-existing unbraced-loop indentation warning was also corrected.
- `docs/GUI_REDESIGN_CORRECTION_REPORT.md`: this report.
- `artifacts/gui-correction/`: build/test evidence, actual editor snapshots, comparison sheet, source/binding checks and deterministic audio/state regression capture.

No processor, DSP, parameter definition, CMake identity, preset table, serializer, automation implementation, installer or REAPER configuration was edited. No plugin was installed over the user's current installation.

## What was corrected

### Geometry follows the reference

The reference was mapped into the existing 1000 × 1250 logical canvas, rather than approximating a new arrangement. Representative positions, in logical pixels after normalizing the reference to a width of 1000:

| Feature | Reference, approximately | Corrected GUI |
| --- | ---: | ---: |
| Left silver rail width | 84 | 84 |
| Spill / Vocal / Music centers, x | 351 / 555 / 768 | 351 / 555 / 768 |
| Upper/lower section divider, y | 311 | 311 |
| Three panel left edges, x | 102 / 367 / 647 | 102 / 367 / 647 |
| Main panel top / bottom, y | 323 / 1030 | 323 / 1031 |
| Range / Rider / Output track centers, x | 216 / 499 / 766 | 216 / 500 / 765 |
| Fader scale +12 / -12, y | 417 / 875 | 417 / 875 |
| Footer divider, y | 1141 | 1141 |

The default size remains 800 × 1000; the existing 4:5 resize limits remain 640 × 800 through 1120 × 1400. Uniform scaling is retained. The decorative corner resize grip was removed; host resizing remains enabled.

### Top area

- Removed the top utility strip completely from the default view.
- Lengthened and reframed the Target meter, shifted Target labeling into the reference's position, and retained its actual dB value.
- Restored the reference's left Ride/Idle grouping and bracketed Sensitivity heading.
- Replaced separate visible Fast/Slow button plates with a single recessed mechanical-switch presentation. The existing two button hit areas and callbacks select the same original attack values. The lever position follows the real parameter.
- Positioned all three sensitivity knobs from the reference. Their editable value fields appear on hover/focus, keeping the default view clean without removing their attachments or numeric editing.

### Knobs, materials and lighting

- Multi-layer metallic knob rims, concentric lathe rings, subtle radial brushing, bevels, soft shadows and tapered black pointers.
- Larger, heavier radial graduations and outer arcs following the reference's control proportions.
- Deterministic cached horizontal grain, separate navy lighting for the top, lower panels and footer, stronger recessed framing and fine edge highlights.
- Silver rails with a multi-stop cross-sectional gradient and restrained vertical brushing; separate screw surrounds, dark sockets and highlights.
- Glass-like status and mode lamps with real selected/activity state, highlights and shaded lenses.

All finishes are procedural JUCE geometry/gradients and a small internally generated grain texture. The reference PNG is not embedded, sampled into controls or used as a flat background.

### Lower panels, faders and readouts

- Matched panel widths, gaps, margins, rounded metal borders, corner screw locations and information marks.
- Extended the wells to the reference's dimensions and drew them on the chassis so their tops/ends are not clipped by slider bounds.
- Enlarged both Range handles and Output caps; rebuilt caps with distinct upper plates, dark slots, brushed faces, lower bevels and shadows.
- Rebuilt Rider's larger cap with inset face, grooves, separate lower bevel and a red live-position stripe.
- Added half-dB subdivisions and reference-sized decibel labels, including Rider's left/right scales and fixed red zero reference.
- Replaced small text boxes with framed, illuminated seven-segment-style readouts drawn from real values. Numeric editing remains provided by the existing JUCE slider labels.
- Range's main readout shows the actual `rangeMax - rangeMin` span, with both actual endpoints printed below. This is a calculated display, not another parameter. Both original handles still edit their original parameters.
- Integrated the real Idle marker into the Range area and preserved its value field on hover/focus. A restricted Range hit region prevents it from intercepting the adjoining Idle marker; actual editor hit testing now verifies both remain reachable.

### Automation and branding

- Matched the bracket/title positions, button proportions, black housings, silver/red/green face treatments and real mode lamps.
- Replaced the previous loose footer spacing with individually fitted VOCALIS, RIDER and LIVE wordmarks using the reference's positions and dimensions. Retained the waveform mark, blue LIVE treatment, metallic divider and end screws.
- Adjusted typography to taller, slightly condensed forms with the reference's hierarchy and consistent alignment.

### Existing utilities and meters remain available

The circular **i** button beside Automation opens a navy tools panel containing the existing preset selector, Bypass, Export CSV, Clear and About controls, plus the actual input RMS/peak and music meters. Click it again to close. This removes the invented top strip and permanent extra meter columns while retaining all functionality. The panel's expanded/collapsed state is a GUI-only view state; no parameter or serialization field was added.

## Functional preservation and binding evidence

`artifacts/gui-correction/binding-verification.txt` compares the checkpoint and corrected source. It confirms exact unchanged implementations for attachment creation, control setters, button setup, state polling, Range changes/gestures, double-click and keyboard behavior. All existing button/preset/export/About callback source and seven slider setup calls are unchanged.

| Control | Existing binding/source, preserved |
| --- | --- |
| Target, Spill, Vocal, Music | APVTS SliderAttachments: `target`, `spill`, `vocal`, `music` |
| Rider, Output, Idle | APVTS SliderAttachments: `rider`, `output`, `idle` |
| Range handles | Existing `rangeMin` / `rangeMax` setters and gestures |
| Fast / Slow switch | Existing `attack` values 1 / 0 and callbacks |
| OFF / WRITE / READ | Existing `automation` values 0 / 1 / 2 and callbacks |
| Bypass | Existing `bypass` setter and state polling |
| Preset selector | Existing eight programs and `setCurrentProgram` callback |
| Target strip / input meter / peak | `meterInput`, `meterPeak` atomics |
| Music meter | `meterMusic` atomic |
| Ride / Idle lamps | Existing `activity` atomic and inverse |
| Rider in Off/Write | Existing live `meterGain` display; existing disabled manual editing |
| Rider in Read | Existing attached editable/automated Rider parameter |
| Export / Clear / About | Original callbacks and About content |

No fake levels, decorative simulated activity, duplicated parameters or renamed IDs were introduced. Different readings, handle positions and illuminated modes in the review images reflect actual plugin state rather than the illustration's example values.

## Executed verification

- **Release native Linux VST3:** build succeeded with CMake/Ninja. Final build log: `artifacts/gui-correction/build.log`.
- **Existing CTest suite: 9/9 passed, 0 failures.** DSP, Integration, VST3_Module, Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety and VST3_Reload. Log: `artifacts/gui-correction/tests.log`.
- **GUI: 118 checks passed.** Includes attached controls in both directions, synthetic mouse/keyboard edits, Range handles, all existing parameter buttons, external mode updates, presets, actual processing-driven meters, Rider display/editing behavior, state restore, repeated editor lifetime, About, portrait scaling, utility visibility, and actual Idle/Range component hit testing.
- **DSP/processor protection:** `Source/Engine.h`, `Source/Plugin.cpp`, `Source/Plugin.h` and CMake match the pre-correction checksums. Only Editor.cpp differs among those protected files. See `protected-verification.txt`.
- **Identity:** the rebuilt VST3 `moduleinfo.json` is byte-identical to the checkpoint, preserving product/vendor/version/component/controller IDs. Existing module load/reload tests pass.
- **Processing and state regression:** compiled the existing `artifacts/gui-redesign/AudioRegression.cpp` harness against the corrected shared library and compared with the preceding redesign's capture. Across 24 sample-rate/block-size/channel configurations, eight presets, automation modes, Rider changes, bypass and music sidechain, **34,110,984 bytes of actual processed audio and serialized state are identical**. SHA-256: `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`. Evidence: `audio-comparison.txt`, `corrected-audio-state.bin` and the prior `artifacts/newdesign/audio-after.bin`.

Built VST3: `build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`.

## Visual review artifacts

- `artifacts/gui-correction/running-editor.png`: actual 800 × 1000 JUCE editor, driven by processed audio.
- `artifacts/gui-correction/editor-640x800.png`: minimum size.
- `artifacts/gui-correction/editor-1120x1400.png`: maximum size, nearly the reference's native dimensions.
- `artifacts/gui-correction/utilities.png`: expanded tools panel with real meters.
- `artifacts/gui-correction/reference-comparison.png`: reference, preceding render and corrected render at a common height. The UI images in this sheet are actual component captures; only resizing and labels are added for comparison.

The fresh snapshots were visually inspected against the reference. The correction now follows its dominant geometry, proportions, typography hierarchy, mechanical switch, fader dimensions, framed readouts and branding placement much more directly than the preceding render.

## Remaining mismatches and unverified items

- The original is illustrative hardware. Procedural JUCE metal, screw machining, illumination and reflections still differ from its photorealistic finish. Fonts are an installed condensed rendering of DejaVu Sans rather than the unidentified reference typeface; the digit shapes are custom vector segments rather than an exact font match.
- Values and mode indicators differ because the GUI displays real state. The reference's 0 dB Target example, 15.8/7.4/3.8 readouts and pointer positions are not copied as fake measurements.
- Numeric knob and Idle fields reveal on hover/focus. Actual Range endpoints remain as an extra small line below its span readout. Tools/meters live in the added expandable panel rather than being removed or occupying the reference's main fader layout.
- Nonfunctional panel information circles are decorative, as in the previous editor. The Automation information button is functional and opens the tools panel.
- REAPER was **not launched**. Actual REAPER embedding, envelope recording, existing-project reopening, host-specific DPI behavior and native file-chooser interaction remain unverified. Tests create/render JUCE components and send component events; these are not DAW screenshots or a claim of manual desktop interaction.
- The corrected VST3 is built but not installed over the existing user plugin.
