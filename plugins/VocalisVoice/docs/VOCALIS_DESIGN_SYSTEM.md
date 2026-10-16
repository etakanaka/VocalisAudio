# Vocalis shared design system — 2026-10-16

Status: VERIFIED PASS for source extraction and 30 executed pixel-parity tests. Reference is the actual `/home/enos/AudioDevelopment/VocalisRider/Source/Editor.cpp`, not the older Voice concept PNG. Its SHA256 and every ARGB literal are recorded in `artifacts/audit/rider-design-source.json`. Rider was inspected read-only, never built or reinstalled.

## Exact palette

All values below are ARGB, with FF opacity. Helpers retain fractional alpha where specified.

| Role in Rider | Exact value |
|---|---|
| Suite background / preset field | `FF0A111E` |
| Canvas gradient end | `FF080E19` |
| Panel gradient top-right / bottom-left | `FF1B273D` / `FF101925` |
| Border | `FF465B82` |
| Main text / muted text | `FFEDF1FF` / `FFA9BCE5` |
| Violet / purple | `FFA378FF` / `FF7130DE` |
| Yellow indicator | `FFFFD24B` |
| Brand waveform | `FFBCA7FA` |
| Plugin title blue | `FF9BB8FF` |
| Readout inset / slider label inset | `FF090F1B` / `FF0A101D` |
| Combo configured background | `FF0B1220` (custom draw uses `FF0A111E`) |
| Popup background / selection | `FF121D30` / `FF7130DE` |
| Meter track / separators | `FF070D18` / `FF354766` |
| Meter bottom / 68% stop / top | `FF00DBA0` / `FF74ED79` / `FFFFD24B` |
| Knob ticks minor / major | `FF455984` / `FF8496BD` |
| Knob track / face outline | `FF070B18` / `FF030813` |
| Outer face gradient | `FF6A779B` / `FF080E19` |
| Inner face gradient | `FF293247` / `FF0C1420` |
| Knob pointer | `FFD4CAFF` |
| Inactive button top / bottom | `FF1B2940` / `FF0D1523` |
| Active button top / bottom | `FF6130AF` / `FF38117F` |
| LED socket / inactive gradient | `FF040914` / `FF71809D` → `FF111A2A` |
| Fader socket / outline | `FF030610` / `FF7285B6` |
| Fader cap gradient / outline | `FF9DA6C2` → `FF111727` / `FF070C17` |
| Fader highlight / ridges | `FFBFCBE9` at .65 alpha; `FF090E1C`; `FF8995B4` at .35 |
| Fader glow core | `FFF5EAFF` |
| Scale ticks major / minor | `FF8195C1` / `FF394E73` |

The full literal inventory also includes Rider's horizontal detector track `FF030812`, `FF1B2539`, `FFF3BA34` → `FFFFE46A`, and inactive activity text `FFA9BDE7`. Those detector/range functions are retained in the shared source but are not connected to Voice's unrelated DSP.

Voice uses no alternate purple/blue knob palette. Every rotary uses Rider's same violet→purple arc. Additional Voice-only semantic red uses JUCE `Colours::red` (`FFFF0000`) for excessive sensitivity and clipping, because Rider has no corresponding clip widget. Green sensitivity uses the exact Rider meter green, and yellow uses its existing yellow. This exception adds a required meaning, not an alternative control style.

## Geometry and rendering

`src/ui/rider/RiderDesign.h` contains Rider's Suite helpers and SuiteLook. Free functions were made inline and placed inside `vocalis::design`; rendering code is otherwise retained. `RiderLevelMeter.h` is the original meter with only a class rename. Full AGPL licence and adaptation notice accompany the code. No runtime dependency on Rider exists.

* Panels: radius 9, black .35 shadow displaced down 3; gradient top-right to bottom-left; border .85; inner white .035 highlight at .7 width, inset 1.
* Readouts: radius 5, border .9; slider Label variant border .85, inset .5 and text 15.
* Rotary: area inset 9; radius = min(width,height)/2 − 7. Twenty-five ticks (every fourth major), stroke 1, radial offsets +5 to +9/+12. Background arc width 9; purple .2 glow width 14; value gradient width 6. Face radius = arc radius − 7, black .6 shadow displaced (2,5), outer border 1.5; inner face inset 3. Pointer is a 3-pixel line from .25 to .76 radius. Keyboard focus outline is violet, expanded 2. Angles/dragging remain JUCE Slider defaults, matching Rider's Slider setup. Larger Voice Intensity changes dimensions, not the renderer.
* Fader: 20-wide socket, radius 9, violet/purple core; output-style cap 58×47, textured ridges and glow. Rider's special automatic-rider cap is 62×65; Voice uses the ordinary manual trim cap. Voice range stays ±24 dB despite Rider's ±12 range.
* Level meter: exact 94-wide component, 18-wide track at width×.53; top 16, bottom height−60; −60…0 display, separators every 3 dB, labels 0/−6/−12/−24/−48 at size 12. Readout is RMS dBFS at size 13; white peak line width 1.2. Two instances make each stereo pair; mono shows only one centred component. Voice retains its own measured 150 ms RMS/450 ms peak decay and clip hold: importing Rider's audio-side detector would change behaviour and is deliberately excluded.
* GR: same track, gradient, border, segment-line and readout materials; adapted to Voice's 0…12 dB attenuation and top-down direction. It is not an audio-level meter and therefore cannot copy the level scale verbatim.
* Buttons: rect inset 1, radius 6, shadow .4 displaced down 2; hover top brighter .15; pressed bottom darker .2; selected border 1.4, otherwise .8; focus violet; selected inset glow .3. Label size 15 (11 if height<30). Effect enables and Bypass use this exact TextButton rendering. Text reflects ON/OFF.
* Combo: radius 5, border 1, arrow coordinates w−26/w−20/w−14 with stroke 1.5; font 15; Voice uses the same 166×37 selector dimensions.
* LED: 26-diameter socket, 18-diameter gradient lamp; five active glow rings. Voice retains the exact lamp geometry and supplies measured sensitivity state.

Rider audio-side meter timing was also inspected read-only in `Source/Engine.h`: input mean-square envelope 35 ms, music envelope 300 ms, and peak exponential decay 60 ms, with no separate peak-hold latch. `Plugin.cpp` publishes those dB values after processing, and its editor refreshes at 30 Hz. Voice preserves its original 150 ms RMS / 450 ms peak decay / 1.5 s clip hold. These deliberately differ in live ballistics because importing Rider detection would alter existing Voice metering; shared rendering is identical. This is a documented animation difference, not an unverified claim of identical detector timing.

Rider has no explicit alternate disabled-face branch in the copied rotary/button rendering. Rotary highlighting is keyboard-focus based; button hover/pressed effects are the branches above. No new hover or disabled palette was guessed.

## Typography and branding

Rider uses JUCE `FontOptions` with the platform default sans-serif; it does not embed or hard-code a named font. Bold is an explicit `Font::bold` flag. No explicit letter-spacing override is present. Numeric text uses one decimal and the existing parameter unit suffix.

The top bar uses the exact five waveform bars (heights 10/25/45/25/10, width 6, spacing 11), violet .12 halo, text at (108,30), size 51 bold; divider x417, y33…88. Title is size 26 bold, blue; subtitle size 10 muted. Voice changes only title/subtitle and the bar's width. Rider has no settings widget or footer counterpart in its current source: Voice's settings uses the same standard button renderer, and its informational footer uses the same text helper/palette. No invented settings icon is attributed to Rider.

## Reuse

Include `VocalisLookAndFeel.h`, set one LookAndFeel for the editor, and use `VocalisTopBar.h`, Suite::panel/readout/led and RiderLevelMeter. Keep processor-specific attachments, meter atomics and ranges in each plugin. Do not copy processor/state/IDs into the theme. The design parity oracle in `tests/RiderReferenceDesign.h` retains the independent source excerpt so future styling changes can be checked against it. Keep the AGPL notices and licence when adapting this source again.
