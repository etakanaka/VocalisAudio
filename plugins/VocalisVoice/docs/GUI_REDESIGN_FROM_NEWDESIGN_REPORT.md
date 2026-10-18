# Vocalis Voice GUI redesign report

Date: 2026-10-18

> Historical first-pass report. The final visual correction and 960 x 617 laptop-friendly default are documented in [GUI_REDESIGN_CORRECTION_REPORT.md](GUI_REDESIGN_CORRECTION_REPORT.md); the initial dimensions and screenshots below have been superseded.

## Files changed

- `src/PluginEditor.cpp`
- `src/ui/rider/RiderDesign.h` (shared UI drawing helpers and look and feel)
- `src/ui/VocalisTopBar.h`
- `tests/DesignParity.cpp` (updated its visual regression expectations for intentionally redesigned controls and materials)
- `docs/GUI_REDESIGN_FROM_NEWDESIGN_REPORT.md`

The working directory did not contain usable Git metadata: `.git` was an empty directory. Before edits, the original `src/`, `docs/`, and `CMakeLists.txt` were copied to `/tmp/vocalis-gui-checkpoint-20261018/`. No existing source or build output was discarded.

## Visual updates

The editor now follows the reference's rack-console arrangement: a full-width header, framed input and output bays, a wider central control panel, and a footer. The navy blue panels and header have procedural brushed lines, brighter metal borders, and corner fasteners. Controls use silver rotary faces with violet range arcs, green enabled effect buttons, a lit bypass indicator, dark bordered readouts, and a settings gear mark. Gain controls were enlarged to better match the reference proportions. The 1400 x 900 editor canvas and existing resize range remain in place.

The reference image has a photorealistic brushed-metal texture and detailed hardware. The JUCE version draws its materials procedurally, so the finish is cleaner and less photographic. The gear and screw details are also simplified.

## Preserved behavior and bindings

No processor, DSP, parameter-layout, CMake plugin-target, preset/state, or identity files were changed. Parameter IDs, ranges, defaults, ordering, and host semantics remain as before. The GUI continues to use the existing attachments in `PluginEditor.cpp`:

| Visible control | Existing parameter ID |
|---|---|
| Input Gain | `vv_input_db` |
| Intensity | `vv_intensity` |
| Delay enable / level | `vv_delay_on` / `vv_delay_db` |
| Doubler enable / level | `vv_doubler_on` / `vv_doubler_db` |
| Reverb enable / level | `vv_reverb_on` / `vv_reverb_db` |
| Output Gain | `vv_output_db` |
| Bypass | `vv_bypass` |

Preset selection still calls the existing program API. Input/output meters, sensitivity, clipping, and gain reduction are still painted from the processor's live meter atomics. The title remains Vocalis Voice, and the built VST3 reports the same Vocalis Voice identity and parameter names/order.

## Verification

- Release `VocalisVoice_VST3` build: passed.
- Release CTest suite: 7/7 passed (`voice_tests`, `dsp_tests`, `gui_tests`, `design_parity`, `audit_dsp_tests`, `thread_stress`, `callback_allocation`).
- `GuiTests`: passed, including control-count/binding checks, mono/stereo meter configurations, editor creation, resize, and offscreen snapshots.
- `Vst3Probe`: passed discovery, all 11 host parameter names/order, 1→1, 1→2, and 2→2 processing layouts, measured host automation, state roundtrip, and repeated load/unload.
- `DesignParity`: retains exact checks for still-shared drawing parts and now asserts the redesigned controls/materials differ from the previous Rider look.
- Processor/DSP/parameter files were not edited. A before/after audio binary comparison was therefore not needed to validate a DSP edit; no DSP edit occurred.
- A fresh 100% GUI render is available at `artifacts/gui-layout-2-2.png`.

## Unverified items

The environment has no accessible X11 display, so the VST3 probe could not open the exported plug-in editor natively. The editor was compiled, created, resized, and rendered by the JUCE GUI test. REAPER itself was not launched, so REAPER-specific scanning, live interaction, and session recall remain unverified in this environment. Automated VST3 state/automation and bus-layout checks passed.
