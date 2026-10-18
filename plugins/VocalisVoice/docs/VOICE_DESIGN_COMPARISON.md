# Voice / Rider design comparison

## Current approved interface

The approved GUI follows `newdesign.png` and opens at 960 x 617. All existing panels, controls, indicators, preset selection, and branding remain present; the complete JUCE interface scales proportionally at a fixed aspect ratio. Settings offers 840 x 540 compact, 960 x 617 default, and 1200 x 771 large sizes. Current compiled-editor captures are `artifacts/gui-960x617.png`, `artifacts/gui-840x540.png`, and `artifacts/gui-large-1200x771.png`. The historical audit below describes the component parity work and earlier screenshot names; it does not describe the current default dimensions.

Reference: actual Rider `Source/Editor.cpp`; source hash is recorded. Existing Rider artifact `artifacts/final-debug/final-editor.png` was inspected and copied read-only into Voice's `artifacts/audit/rider-existing-editor.png`. It is a pre-existing compiled-editor render, not a new desktop capture made during this task.

| Item | Outcome | Evidence |
|---|---|---|
| Palette, surfaces, borders | VERIFIED PASS | Exact Rider constants and Suite::panel implementation imported locally |
| Rotary renderer | VERIFIED PASS | Same SuiteLook method; 12 executed pixel comparisons at three diameters/four positions |
| Button states | VERIFIED PASS | Eight pixel comparisons: selected/unselected × hover × pressed |
| Panel / readout / fader | VERIFIED PASS | Three pixel comparisons, zero differing pixels |
| LED | VERIFIED PASS | Active/inactive pixel comparisons |
| Preset selector | VERIFIED PASS | Pixel comparison and same 166×37 geometry/font |
| Level meter | VERIFIED PASS | Four pixel comparisons at −60/−24/−6/0 dB; same 94-wide component |
| Typography | VERIFIED PASS for code/rendered controls | Exact font sizes/weights and default sans-serif selection; no alternate embedded font |
| Branding | VERIFIED PASS for source geometry | Same waveform bars, halo, 51 bold wordmark, divider, title/subtitle styling; Voice-specific wording |
| Full Voice layout and 80% scale | VERIFIED PASS for compiled offscreen render | `artifacts/gui.png`, `artifacts/gui-80-percent.png`; inspected for overlap and clipping |
| Native desktop screenshots of both running plugins | UNVERIFIED | No accessible X11 display; Rider was not launched or rebuilt |
| REAPER focus, drag, hover, popup and physical typing | UNVERIFIED | Manual host verification remains required |

Thirty component pixel comparisons report zero differing pixels in `artifacts/audit/design-parity.txt`. They compare separately compiled, verbatim Rider rendering excerpts against Voice's adapted namespace on equal geometry, data and default focus state. They do not claim the entire, functionally different plugin layouts are pixel-identical. The oracle is stored locally; tests need no installed Rider plugin and do not build Rider.

Voice captures are actual production editor code rendered by JUCE at one pixel per logical unit, matching the scale of the existing 1000×1000 Rider artifact. The old Voice capture is preserved under `artifacts/audit/before-gui.png`. Component-level comparisons provide the stricter equal-size/equal-state evidence; the old Rider full-editor artifact has different audio/control state and cannot support a full-image pixel difference claim.

Voice keeps a dominant Intensity knob, grouped parallel effects, input/output meters adapting between mono and stereo pairs, trims, sensitivity, actual GR, presets, Settings and Bypass. Its 1400×900 canvas (80% minimum 1120×720) differs from Rider's 1000×1000 functional layout. That placement difference is intentional.

Necessary differences are documented: GR uses the same meter materials with an attenuation scale/direction; stereo meters are paired Rider components; red indicates Voice clipping/excessive input because Rider has no corresponding red clip widget; Settings uses Rider's standard button rendering because its actual source contains no settings icon. Meter timing remains Voice's existing 150/450 ms RMS/peak response, versus Rider's inspected 35/60 ms input RMS/peak response (music RMS 300 ms). This retained functional difference means live ballistics are not claimed identical. Panel gradient pixels depend on each panel's dimensions even though the algorithm and colours are identical. No pixel-fidelity claim is made for unrelated control placement or native host scaling.

The three `artifacts/gui-layout-1-1.png`, `gui-layout-1-2.png`, and `gui-layout-2-2.png` renders exercise actual measured input and output at +24 dB output trim to trigger the clip indicator. Mono uses one centred meter, with CLIP replacing its channel header rather than overlapping it. They are test states, not recommended gain settings.
