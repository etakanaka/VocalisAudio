# Vocalis Rider Live — silver rails and Phillips-head screws

Completed 2026-10-18. This is an appearance-only correction. **The only production file changed is `Source/Editor.cpp`.** The default remains 480 × 600, with the existing portrait layout and proportional scaling unchanged.

## References located and inspected

Silver reference:

`/home/enos/Downloads/ChatGPT Image Oct 18, 2026, 12_01_14 PM.png`

Screw reference (the requested root-level file was absent; the matching artifact was available):

`/home/enos/AudioDevelopment/VocalisVoice/artifacts/gui-reference-layout.png`

Both were inspected. No substitute silver material was invented. `artifacts/silver-screws/SampleReference.py` records actual median RGB samples from reference rows 200–800, away from screws. The source path, image dimensions, reference SHA-256 and samples are preserved in `reference-silver-samples.json`.

## Recoverable checkpoint

Git metadata is unusable. Before editing, Source, tests, docs, CMake and existing compact/editor/About review images were archived to:

`artifacts/silver-screws/checkpoint-20261018/before.tar.gz`

The checkpoint also contains source/test checksums, the original VST3 manifest and an archive checksum. `gzip -t` passed. Extract into a separate directory to inspect or restore the previous files without overwriting newer work.

## Silver finish correction

Replaced only the left/right outer rail material. The former near-white, vertically striped finish is now a cooler silver gradient following the actual reference's highlights and blue-grey shadow tones.

Representative sampled RGB colours:

| Reference location | RGB | Hex |
| --- | --- | --- |
| Bright edge reflection | 240, 240, 238 | `#F0F0EE` |
| Highlight | 230, 232, 229 | `#E6E8E5` |
| Upper silver | 213, 216, 215 | `#D5D8D7` |
| Mid silver | 187, 192, 194 | `#BBC0C2` |
| Mid/shadow transition | 170, 177, 180 | `#AAB1B4` |
| Shadow | 144, 153, 159 | `#90999F` |
| Deep silver shadow | 110, 121, 131 | `#6E7983` |
| Dark edge reflection | 78, 89, 100 | `#4E5964` |

One `silverRail()` helper draws both sides with the same left-lit gradient stops, fine low-contrast horizontal grain and edge treatment. Rail positions, dimensions and overall lighting direction are retained. The reference is not embedded as a background or image asset.

The navy control panels, knob/fader materials, text, meters, buttons and branding were not recoloured or redrawn differently.

## Phillips-head screw correction

Updated the shared screw renderer without changing any mounting coordinates or head radii:

- Retained all **20** existing fastener positions: 12 panel corners, 4 outer-frame/footer and 4 rail positions.
- Replaced the large dark central disc and short cross strokes with an open metallic face and a clearly defined cross-shaped recessed slot.
- Added a shaded dome, narrow outer bevel, dark seating ring, restrained shadow and a light lower slot edge to suggest depth.
- Used the sampled silver highlights/shadows for the screw metal, consistently lit from the upper left.
- Extended slot arms to 65% of the head radius. The smallest panel head stays about **6.7 pixels across** at default scale, with an approximately **4.4-pixel cross span**. No heads were enlarged.
- Consolidated the rail screws' former broad pale washer halos into the shared darker seating treatment, removing the rivet-like appearance without moving the screws.

No new fasteners, overlaps or control movements were introduced.

## Scope verification

`protected-files.txt` confirms the complete editor source outside the silver/screw helpers and the rail-painting block is byte-identical to the checkpoint. This includes the LookAndFeel, all control bounds, the 480 × 600 default, resize limits/aspect ratio, parameter attachments, callbacks, state polling, buttons, meters and branding.

`Source/Engine.h`, `Source/Plugin.cpp`, `Source/Plugin.h`, CMake and every existing test source also match their original checksums. **No existing tests were modified.**

A direct comparison of the original and updated 480 × 600 captures verifies that **all pixels outside the rail and screw regions are byte-identical**. No additional GUI regions were excluded from this comparison.

## Build and regression results

- Release native Linux VST3 build: **passed**, with no compiler warnings/errors in `artifacts/silver-screws/build.log`.
- Existing automated suites: **9/9 passed, 0 failures** — DSP, Integration, VST3_Module, Audit, GUI, DSP_Audit, Thread_Stress, Install_Safety and VST3_Reload.
- GUI verification: **274 checks passed**, including default dimensions, 4:5 scaling, bindings, state restore, presets, actual metering, automation modes, scaled mouse drags/hit areas and utility access.
- Before/after audio and serialized state: **34,110,984 bytes identical**, SHA-256 `b957b90dfe56eb591810cb78a1a6232b9fa51b516976b269f5a080cc8efdb647`. The existing AudioRegression harness was linked against the baseline and final shared libraries; it covers 24 sample-rate/block-size/channel configurations, presets, automation, Rider, bypass and music sidechain. See `audio-comparison.txt`.
- Identity: rebuilt `moduleinfo.json` is byte-identical to the checkpoint. Product/vendor/version, manufacturer codes and component/controller identities remain unchanged. Native VST3 load/reload tests pass.

Built bundle:

`build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`

No commit, synchronisation, push, publishing, user-plugin installation or REAPER configuration change was performed.

## Visual verification and review images

Fresh actual editor capture:

`artifacts/silver-screws/editor-480x600.png`

Additional captures:

- `editor-560x700.png`
- `editor-800x1000.png`
- `editor-1120x1400.png`
- `silver-reference-comparison.png` — enlarged material strips from the actual reference and updated left/right rails, for finish comparison only; this image is not used by the plugin.

`VerifyAppearance.py` and `appearance-verification.txt` document:

- Exact unchanged pixels outside rails/screws at 480 × 600.
- All 20 screws at 480 × 600, 560 × 700 and 800 × 1000 have darker horizontal/vertical slot arms than the diagonal silver quadrants, verifying cross contrast rather than just a dark dot.
- At six interior reference-profile locations, mean absolute RGB error is **1.43/255** for the left rail and **1.27/255** for the right rail.
- Corresponding left/right rail profiles differ by only **0.32/255** RGB units on average. They share identical material/lighting code; small differences arise from fractional pixel placement during scaling.

The native compact render and the material comparison were visually inspected. The rail tone now follows the supplied image, and the crosses are recognisable within the existing screw sizes.

## Remaining visual differences and unverified items

The sampled gradient is recreated with JUCE rather than copying reference pixels. Border reflections, fine grain and tiny specular details remain approximations, particularly after compact-size antialiasing. The numerical reference comparison concerns six interior profile locations, not an assertion that every material pixel matches. Both rails deliberately share one material and lighting direction.

The smallest screw heads have limited pixel resolution for bevel detail. The cross occupies more of their face and retains measured contrast, but fine machining details cannot exactly reproduce a larger reference screw at a 6.7-pixel diameter. No unrelated GUI element was enlarged or altered to compensate.

REAPER was **not launched**. Manual host embedding, DPI behaviour, envelope recording and existing-project reopening remain unverified. Existing native-module, state, parameter and processing tests passed; no manual REAPER validation is claimed.
