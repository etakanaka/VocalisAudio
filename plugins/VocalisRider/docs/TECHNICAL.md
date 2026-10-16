# Technical design

## Signal path

A zero-lookahead scalar gain engine runs at the host sample rate. Main bus is mono or stereo with identical input/output layout. The auxiliary **Music** input is optional, mono or stereo, disabled by default. Auxiliary samples never enter the output mix. Stereo detection uses mean channel power; gain is shared between channels, preserving balance. No filtering, waveshaping, saturation, limiting, or lookahead is applied to audio.

Per-sample processing:

1. Sanitize nonfinite input, collect mean-square power and peak.
2. Smooth vocal power with a 35 ms one-pole RMS detector; peak meter has a 60 ms decay.
3. Detection threshold is Spill minus Vocal Sensitivity, bounded to -72…-6 dBFS. Entry requires threshold +3 dB; exit requires threshold -2 dB followed by a 180 ms hold. This avoids boosting into long silence; it does not classify speech acoustically.
4. Smooth music power over 300 ms. Active music above -70 dBFS changes Target by `(music RMS dBFS + 24) × 0.25 × 10^(MusicSensitivity/20)`, bounded ±6 dB. Missing/silent sidechain has no effect.
5. Subtract vocal RMS from adjusted Target; apply a ±1 dB deadband and effective Range bounds.
6. Smooth gain in dB. Fast gain reduction: 65 ms; Fast gain increase: 240 ms. Slow reduction: 220 ms; Slow increase: 700 ms. During idle, return to the bounded Idle value over 650 ms. These are exponential time constants, not settling times.
7. Read mode uses the host Rider value with 12 ms smoothing, bypassing automatic gain decisions and automatic range limits.
8. Smooth Output over 25 ms; convert Rider + Output to a scalar gain. Bypass crossfades to dry over 5 ms. Host bypass uses the same safe processing path. No sidechain signal is mixed into output.

Audio/output invalid values are replaced with zero. Detector subnormals are flushed; JUCE ScopedNoDenormals protects callbacks. Typical 44.1–192 kHz and arbitrary nonnegative block lengths are supported. Chunking at the capture interval is bounded and does not allocate scratch buffers. DSP coefficients are derived from sample rate, not callback size.

The Rider readout reflects the smoothed riding gain before Output and the bypass crossfade. It continues to show the internal ride while bypassed; Bypass delivers dry audio after its short transition. Input is an RMS meter; peak is measured and exposed to the test/processor interface but is not used as a compressor threshold.

## Parameters

| ID | Range | Default |
|---|---|---|
| target | -42…0 dBFS | -21 |
| spill | -48…-12 dBFS | -48 |
| vocal | -12…12 dB | 0 |
| music | -12…12 dB | 0 |
| rangeMin | -12…11 dB | -6 |
| rangeMax | -11…12 dB | 6 |
| idle | -12…12 dB | 0 |
| rider | -12…12 dB | 0 |
| output | -12…12 dB | 0 |
| attack | Slow, Fast | Slow |
| automation | Off, Write, Read | Off |
| bypass | Off, On | Off |
| preset | eight labels | Default Vocal Rider |

IDs are stable JUCE ParameterIDs, version 1. APVTS XML state contains all values. Range dragging prevents crossing. If external automation crosses limits, the engine uses the ordered pair (min,max), a deliberate deterministic fallback. Idle is bounded to the effective automatic range. Target, sensitivities, limits and automation changes feed the smoothing stages rather than directly stepping output gain. Preset selection from the editor or host programs applies values; the separate preset-label parameter persists the label and does not trigger preset loading when automated.

## Real-time and automation design

The callback reads pre-cached lock-free atomic parameter pointers and publishes lock-free meter atomics. Compile-time assertions require lock-free float/int/bool atomics. There are no mutexes, disk/network operations, host parameter setters or GUI calls in the callback. A preallocated 8192-slot SPSC FIFO (8191 usable records) carries 50 Hz Write-mode `{timeline_seconds,gain_db}` records. Message-thread timer drains it into a bounded 1,000,000-point session capture (about 5.5 hours). Overflow drops points and is reported in the GUI. Capture is volatile, so export before closing the session/plugin. A host that stops servicing its message loop for over about 163.82 seconds may overflow the FIFO, particularly during offline rendering.

The processor owns a 50 Hz JUCE Timer, independent of the editor. Entering Write opens a Rider parameter change gesture, then calls `setValueNotifyingHost` with actual smoothed gain. Leaving Write closes the gesture. Read never receives timer-driven Rider updates. No automation is written in Off. Mode detection on the timer can lag a host change by up to one tick; there is no sample-accurate Write notification claim. Read/Off/Write audio decisions are taken each callback.

Examined JUCE 9 `juce_audio_plugin_client_VST3.cpp`, `beginGesture`, `endGesture`, `paramChanged` (around lines 1458–1485): message-thread notifications invoke VST3 `beginEdit`, `setParamNormalized`, `performEdit`, `endEdit`; non-message-thread changes do not take that same host notification path. This is why host calls run on the timer, not in audio processing. Host recording depends on track/envelope arming and host policy. Offline/batch renders without a message loop may capture queued points but cannot reliably notify a host. Real-time Write capture/export is the supported fallback workflow. DAW recording and importer execution are unverified here.

Timestamps use host position seconds when supplied, else time since prepareToPlay. Captured points represent interval ends. Stop, CLEAR, then play one uninterrupted pass from the desired timeline location. Loops/seeks can produce duplicate/nonmonotonic timestamps. Export uses CSV seconds,dB; REAPER script maps Rider to normalized `(dB+12)/24`. Parameter playback retains the plugin's 12 ms Read smoothing, so replay is close, not a sample-identical reconstruction of automatic riding.

## Independent choices and references

The operation and ranges were specified by the project brief. The [Waves manual](https://assets.wavescdn.com/pdf/plugins/vocal-rider.pdf) corroborates the workflow for Spill, sidechain music adaptation and Off/Write/Read. Exact proprietary detector, time constants, noise discrimination and music transfer curve are not established; all equations above are independent design assumptions. No claim of sonic equivalence or sample-exact reproduction is made.

GUI is procedural JUCE Graphics: navy gradient panels, tick marks, violet rotary arcs, mechanical illuminated faders, rounded readouts and automation buttons. It follows the supplied Vocalis Leveler suite reference without loading it as a background; no external artwork is loaded. Current suite editor is 1000×1000 logical pixels, resizable from 750 to 1400 square pixels. Its layout and appearance were preserved in the final debugging audit.

Final audit updates: begin-gesture callbacks are followed by a fresh automation-mode check, user-edited buttons send balanced gestures, capture begins at a full nominal 20 ms interval, invalid host rates/layouts/buffers are guarded, and fixed gain/music coefficients are computed per process call rather than per sample. Formulas, time constants, IDs and state formats are unchanged. See FINAL_DEBUG_REPORT.md for current evidence and accepted limitations.
