# Validation record — 2026-10-16

> Initial-build record. Superseded by [the current audit](VOICE_FINAL_AUDIT.md) and [current issues](VOICE_KNOWN_ISSUES.md).

## Environment and build

Linux Mint 22.3 (Zena), x86_64, GCC 13.3, CMake 3.28.3, Ninja 1.11.1, existing JUCE 9.0.4. AMD Ryzen 9 5900XT, 32 logical CPUs. No Rider source or installed bundle was modified. Project identity and parameter namespaces are separate. Read-only inspection of the installed Rider manifest confirmed its processor/controller IDs end in 56724C76, while Voice uses 56766365.

Release native ELF shared object and VST3 module manifest are generated under build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3. Debug build uses a separate build directory. The only plugin format target is VST3. JUCE internally compiles client dispatch translation units bearing other format names; no other format bundle or target is produced.

## Automated evidence

* `artifacts/dsp-results.txt`: 28 numerical DSP checks, including all effect paths, and repeatable all-effects CPU benchmark.
* `artifacts/processor-results.txt`: six processor suites covering parameters, factory presets, state, layouts, bypass and repeated instantiation.
* `artifacts/gui-results.txt`: compiled production editor attachments, keyboard adjustment, reset configuration, numeric field presence, effect enable, host feedback, screenshots and destruction.
* `artifacts/vst3-results.txt`: real exported-bundle discovery, all ten controls plus JUCE Program selector, audio in all three layouts with editor closed, host parameter/state roundtrip and repeated load/unload.
* `artifacts/install-test.txt`: staged binary hash comparison, replacement backup, uninstall, refusal to touch unmarked/symlink bundles, sentinel Rider preservation.
* `artifacts/sanitizer-results.txt`: DSP suite under AddressSanitizer and UndefinedBehaviorSanitizer. LeakSanitizer is disabled by default because ptrace is restricted. This is not a full GUI or wrapper sanitizer run.
* `artifacts/full-validation.txt` and `artifacts/debug-validation.txt`: Release and Debug test runs.

The allocation guard overrides C++ new/new[] during engine process calls and recorded zero allocations, including rate/block/rapid-control matrix runs. Direct C allocation is not intercepted; source inspection finds no allocation inside processing. The processor caches APVTS parameter atomics and uses no locks, GUI, filesystem or host queries inside processBlock.

Benchmark covers 10 s of non-silent 48 kHz stereo, intensity 100 and all returns on, at blocks 64/256/1024. Measured approximately 0.7–1% of one realtime core in the Release run; exact wall times are in dsp-results.txt. This is a synthetic throughput measurement, not a REAPER CPU reading or dropout guarantee. Scheduling and concurrent builds affect results.

## Requested test coverage

| Requested checks | Evidence / status |
|---|---|
| 1–5 silence, low/high input, input/output gain | PASS numerical DSP tests |
| 6–10 intensity 0/25/50/75/100 | PASS unity at zero with effects off; nonzero macro changes and finite output at each setting |
| 11–12 compressor / GR | PASS expected steady-state curve, stereo detector, silence release and meter equality |
| 13–14 LF / presence | PASS rumble attenuation and bounded presence enhancement |
| 15–16 delay activation/timing/feedback | PASS isolated return and exact 285/380 ms first arrival |
| 17–18 doubler activation/decorrelation | PASS isolated return, stereo noise correlation and mono energy |
| 19 reverb tail | PASS predelay, early/late energy and decay stability |
| 20 enable/disable | PASS ramps, no wet contribution after off ramp, bypass unity after ramp |
| 21–23 mono / mono→stereo / stereo | PASS engine, processor and actual VST3 harness; REAPER layout selection UNVERIFIED |
| 24 preset restoration | PASS all ten presets and preserved trims |
| 25 REAPER state restoration | Plugin and VST3-host state roundtrips PASS; actual REAPER project reopen UNVERIFIED |
| 26 rapid parameter changes | PASS per-block changes throughout sample-rate/block matrix |
| 27 repeated instantiation | PASS 30 engines, 20 processors, repeated exported VST3 loads |
| 28–29 multiple rates/buffers | PASS engine 22.05/44.1/48/88.2/96/192 kHz; buffers 1/17/64/256/1024/4096. Processor tests additionally use 257 samples |
| 30 numerical stability | PASS finite bounded output throughout matrix, pathological NaN input guard and sanitizers |

These grouped tests do not assert perceptual equivalence to any commercial processor. Coverage is synthetic and does not establish naturalness on a recorded vocal corpus.

## GUI comparison

Inspected the supplied VOCALIS Voice Enhancer Interface.png. Exact filename reference.png was absent; no claim of pixel-identical matching is made. Rendered the real compiled production editor through GuiTests (same sources linked into the VST3). Checked both artifacts/gui.png and gui-80-percent.png: title, waveform brand, navy/charcoal plates, lilac main knob, blue/teal secondary controls, side peak/RMS meters, input sensitivity, GR, numeric fields, unified preset/settings bar, spacing and scaling. No overlap observed at either size. Meter values come from processed samples, not animation.

Native GUI attachment in the exported VST3 is UNVERIFIED: the session's DISPLAY exists but XOpenDisplay fails. The host harness now checks display access before trying XEmbed. Its initial offscreen wrapper snapshot was blank and was rejected; production-editor captures replace it. Programmatic attachment and keyboard tests pass; actual pointer dragging, hover, numeric typing, popup interaction and host focus routing need manual REAPER validation.

## Release acceptance remaining

Actual installation into ~/.vst3 is blocked by its read-only session mount; run scripts/install.sh from a normal terminal. REAPER discovery, UI embedding, automation envelopes, project reopen, offline rendering, host hard bypass, multi-instance CPU and load/unload remain UNVERIFIED in REAPER. Real-vocal audition across speech/singing timbres is also pending. Procedures and limitations are in REAPER_TESTING.md and KNOWN_ISSUES.md. The artifact is a working DSP/VST3 evaluation build, not a declaration that subjective production and host acceptance are finished.
