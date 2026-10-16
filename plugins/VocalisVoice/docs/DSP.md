# Independent DSP design — 1.0.0

The Waves manual documents controls and workflow, not the DSP below. No algorithm, tuning or preset equivalence is claimed. Functional source: https://www.waves.com/1lib/pdf/plugins/greg-wells-voicecentric.pdf (reviewed 2026-10-16, including page 9 routing diagram). The original diagram is only a functional reference; Vocalis uses the explicit routing below.

## Routing and macro

Input trim → tone → linked dynamics → presence/air → subtle saturation → dry plus three separate returns → output trim → meters. All returns receive the processed dry vocal, independently. No return feeds another. Mono input is copied mathematically to two internal paths without summing two dry copies; mono output averages both paths. Stereo channels share one detector and macro, retaining their dry channel independence.

Intensity x = value / 100, smoothed over 40 ms. C(x) = x²(3−2x), clamped to [0,1]. Most stages use C(x). Coefficients update every 32 samples using the smoothed value. This is continuous control interpolation with small coefficient updates, not switched processing bands. At zero intensity and with returns disabled, the chain is unity (tested); factory defaults include wet effects even at zero intensity.

| Stage | Design |
|---|---|
| Rumble | First-order 65 Hz high-pass, crossfaded from unfiltered to filtered with C(x) |
| Low mid | RBJ peaking biquad, 290 Hz, Q 0.8, 0 to −2.8 dB |
| Dynamics | Stereo mean-square detector, 10 ms exponential integration; threshold −12 to −26 dBFS; ratio 1:1 to 4:1; 6 dB quadratic soft knee |
| Gain envelope | Attack 24 to 10 ms, release 180 to 120 ms; coefficients exp(−1/(time×rate)); applies measured attenuation equally to L/R |
| Presence | RBJ bell at 3.2 kHz, Q .75, up to +2.6 dB |
| Air | Adds up to .65 times residual from a first-order 5.5 kHz low-pass |
| Brightness guard | 80 ms energy estimate compares high-pass residual to total energy; presence and air taper between ratios .15 and .50. Already bright material gets less enhancement |
| Harmonics | Only above intensity 60: mix .08 C((x−.6)/.4) of tanh(1.2s)/1.2 with s. Unity small-signal slope; mild peak softening, no boost |

Compressor has no makeup gain and no lookahead. RMS detection naturally releases toward unity in silence; there is no gate. Deliberate harmonic coloration is slight and band-unlimited, without oversampling; alias performance at extreme/high-frequency input needs listening assessment. It is an independent optional-in-the-macro contribution, not a separate user parameter. Tonal frequencies are fixed, but contributions and brightness guard vary continuously. This does not identify vocal pitch or classify speakers.

## Parallel effects

* Delay: independent 285 ms left / 380 ms right circular buffers, 27% feedback, first-order 4.2 kHz damping and 180 Hz low cut in feedback/return. No tempo sync. The repeat ratio is bounded below unity. Stereo channel input is retained. Mono output averages both times.
* Doubler: 18/26 ms variable delays; ±1.3 ms sinusoidal motion at .19/.27 Hz plus .35 ms slower motion; linear fractional interpolation, 6.5 kHz low-pass. Input crossfeed 15% retains stereo while giving both returns useful excitation. No feedback or duplicate zero-delay signal. Mono averages decorrelated wet paths; some frequency-dependent coloration remains inherent to doubling.
* Reverb: 24 ms pre-delay, controlled pre-delayed early tap and eight delay lines at 31.1–67.1 ms. Normalised orthogonal Walsh-Hadamard feedback, per-line decay coefficients 10^(−3 × lineSeconds / 1.45), 5.2 kHz damping. Nominal undamped RT60 1.45 s; damping shortens high-frequency decay. Distinct left/right output sign patterns. A conservative excitation level prevents gain buildup. No dry component in this return.

Buffers allocate only in prepare. Effects run while returns are disabled so automation does not expose stale buffers. Return gains ramp over 30 ms, and disabled returns become exactly zero. Disabling fades out the return including its tail. Enabling exposes the continuously running current tail. Input/output linear-amplitude gains ramp over 20 ms. Bypass crossfades to original untrimmed input over 15 ms, with the DSP kept running. Host hard bypass may be implemented outside the plugin by REAPER; the exposed Bypass parameter is smooth.

## Meters and numerical behaviour

Input is metered after trim; output after sum and trim. Each channel has 150 ms exponentially averaged RMS and 450 ms peak decay. The GR meter reads the actual smoothed compressor attenuation in dB (display capped at 12 dB; actual attenuation may exceed this). UI reads lock-free atomic floats at 30 Hz. GUI work never runs in processBlock. Metered GR is the end-of-block attenuation; effects and trim do not change it.

Sensitivity uses 150 ms stereo RMS: off below −42, green −42 to −24, yellow −24 to −10, red above −10 dBFS, with 1 dB hysteresis around each boundary. Input peaks above −1 dBFS also force red, using the 450 ms peak decay. This is guidance, not a calibrated hardware input indicator. Output samples at or above 0 dBFS light CLIP for 1.5 seconds. There is no output limiter or guarantee against clipping.

Host NaN/infinity input is replaced with zero; pathological finite input is bounded at ±32 before trim. Ordinary audio is not clipped by this guard. Final nonfinite output is replaced with zero. Scoped denormal suppression is used by the JUCE processor. Tested sample rates 22.05–192 kHz and buffers 1–4096 samples; supported normal processing is float32. No audio-thread disk I/O, blocking, locks, GUI, or allocation occurs in the engine. Parameter pointers are cached. State serialization runs outside processBlock using APVTS.

## Latency and performance

Reported processing latency: 0 samples. All filters and dynamics are causal with no block buffering/lookahead; parallel effect delay is intentional and is not dry-path latency. Host tail declaration: 6 seconds. Reverb and delay may leave mathematically smaller residuals beyond that point.

Repeat benchmark: `build/Release/DspTests --benchmark`. It processes 10 seconds of 48 kHz stereo at each of 64, 256 and 1024 sample blocks, intensity 100 and all returns enabled. Reports wall time / audio duration, not REAPER's CPU meter. See artifacts/dsp-results.txt and docs/VALIDATION.md. Silence is not used for the performance measurement.
