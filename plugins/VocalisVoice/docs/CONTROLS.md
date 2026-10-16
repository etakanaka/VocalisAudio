# Controls and stable identities

| Control | Stable ID (version hint 1) | Range / default |
|---|---|---|
| Input Gain | vv_input_db | −24…+24 dB / 0, step .1 |
| Intensity | vv_intensity | 0…100 / 50, step .1 |
| Delay Enable | vv_delay_on | off |
| Delay Level | vv_delay_db | −30…0 dB / −30, step .1 |
| Doubler Enable | vv_doubler_on | on |
| Doubler Level | vv_doubler_db | −30…0 dB / −11.3, step .1 |
| Reverb Enable | vv_reverb_on | on |
| Reverb Level | vv_reverb_db | −30…0 dB / −11.3, step .1 |
| Output Gain | vv_output_db | −24…+24 dB / 0, step .1 |
| Bypass | vv_bypass | off |

Set input trim using the louder vocal passages, aiming for yellow sensitivity. Start intensity around 50, listen for consistency and clarity, add effects, then level-match using output trim and Bypass. High intensity does not guarantee a louder output: the compressor intentionally has no makeup gain. Effect levels are parallel return gains, not percentages of a dry/wet crossfade. −30 dB is quiet, not off; use each enable switch to remove the return.

Drag knobs or trims, use the mouse wheel, focus sliders and use arrow keys, or type a numerical value in the field. Double-click resets each value to its specified default. JUCE attachments send host parameter gestures and display automation changes. Tooltip help is available. The Settings menu changes UI size to 80/100/125% and shows signal-flow information; the window also resizes at a fixed aspect ratio. UI size is per editor, not a saved audio parameter. BYPASS is the real host-exposed bypass parameter.

Factory preset selection retains both trims and Bypass. Changed processing values show Custom. REAPER project state stores all ten parameter values and last factory preset metadata (XML schema version 1). State is configuration-only: delay/reverb histories reset on prepare/reload. No external preset file is required.
