# Vocalis Voice final technical audit — 2026-10-16

**Controlled-testing status. Not declared production-ready.** Native Linux VST3 builds and automated checks pass. Actual REAPER loading, native window interaction and recorded-vocal listening are unverified. Local installation is blocked by the read-only mount.

Related reports: [design system](VOCALIS_DESIGN_SYSTEM.md), [visual comparison](VOICE_DESIGN_COMPARISON.md), [regression tests](VOICE_REGRESSION_TESTS.md), [known issues](VOICE_KNOWN_ISSUES.md).

## Protection and baseline

Git status was attempted and failed: this session exposes a restricted .git directory that is not a usable repository. Before source edits, all accessible files—including untracked work, builds, archives and GUI captures—were archived before edits and copied durably to `checkpoints/VocalisVoice-checkpoint-20261016-073330/workspace.tar` (about 1.4 GB). The adjacent manifest records source hashes. Existing build/tests were run and logged in `artifacts/audit/baseline-*`; previous GUI images are `before-gui.png` and `before-gui-80-percent.png`.

Vocalis Rider was inspected read-only. No Rider build, installation, configuration or source edits were performed. A before/after hash comparison of 163 reference source/assets/artifact files found zero changes. Voice parameter and preset headers are byte-identical to the checkpoint, and both VST3 class IDs match the original. See `protection-and-identity.json`. The only DSP header changed is `VocalDelay.h` for the proven buffer boundary defect.

## Findings and corrections

| Severity | Finding | Status | Correction and executed evidence |
|---|---|---|---|
| High | Fractional delay wrap can round a tiny negative position to exactly buffer size, reading one sample beyond allocation | FIXED AND RETESTED | 11,528-element reproducer with pos=5000 and delay=5000.00048828125 generated ASan heap-buffer-overflow. Rounded endpoint now wraps to zero. Same ASan reproducer exits 0; audit boundary suite passes |
| Medium | Version-1 state validation accepted incomplete/duplicate/missing-value or partially numeric parameter payloads and could change live trim before corruption was noticed | FIXED AND RETESTED | Require exactly ten unique known PARAM entries with id/value, strict complete numeric parsing, finite/range checks before replaceState. Original corrupt-state test failed; corrected test passes; valid old-checkpoint VST3 state still restores all values and metadata |
| Medium, test tooling | GUI test hosts deleted active editors without notifying JUCE; repeated creation then returned no editor and the harness dereferenced it | FIXED AND RETESTED | Shared RAII EditorOwner calls editorBeingDeleted before delete. Twenty repeated editor cycles pass normally and under ASan/UBSan, including simultaneous audio processing. This was a test-owner defect, not evidence that JUCE's real VST3 wrapper had the same defect |
| Low, UI feedback | Newly opened editor initially showed default meters/preset text until first timer tick | FIXED AND RETESTED | Initialize timer-derived display immediately from real atomics/parameters; rendered snapshots and feedback checks pass |
| Low, UI channel feedback | Mono instances displayed a stereo-labelled meter pair | FIXED AND RETESTED | Mono uses one centred Rider meter; mono→stereo uses one input/two output meters. All three GUI configurations are tested |
| Functional installer limitation | Legitimate manually installed Voice bundles had no ownership marker and were refused | FIXED AND RETESTED | Recognize both exact Voice class IDs plus expected native binary, then retain full backup before replacement. Unidentified bundles/symlinks stay protected; staging tests exercise marked and unmarked upgrades |
| Design inconsistency | Original Voice palette, knobs, faders, meters, buttons and branding differed from actual Rider | FIXED AND RETESTED | Locally adapted Rider rendering with notices/licence; 30 equal-size component pixel tests pass; actual compiled Voice render inspected at 100%/80% |

No DSP redesign was performed. No threshold, ratio, time constant, effect tuning, gain map, parameter identity, default, range, plugin identity or valid state schema was changed. The prior sample-to-sample arithmetic is retained for every valid circular-buffer index. Undefined original memory reads have no sensible sound-preservation contract.

Two additional test assumptions were corrected rather than relaxing tolerances: a VST3 host queues parameter changes until audio processing, so state tests must flush those writes; normalized test values must align to the declared 0.1 dB grid (−6 dB uses .375, while .37 maps between steps). The strict legacy comparison remains at 1e−6 normalized units. An attempted native TextEditor test could not run without X11; its Label commit parsing callback and reset handler are tested, while physical typing is explicitly unverified.

## Technical review

Reviewed all processor/editor sources, each DSP module, state/parameter/preset headers, custom UI classes, CMake, VST3 manifest, scripts and tests. The chain remains input trim → tone → linked RMS dynamics → presence/clarity → harmonic enrichment → processed dry with independent delay/doubler/reverb returns → output trim/meters. Reverb and doubler have no zero-delay dry copy in their returns. Enabled effects change measured output; disabled returns reach exactly zero after their ramps.

The mono path uses two internal effect paths and averages them once; mono→stereo expands one dry channel without a level doubling. Only supported mono/mono, mono/stereo and stereo/stereo layouts are accepted. Audio continues with no editor. Stereo dynamics share one gain envelope, and actual compressor attenuation drives GR. Trims and return enables are smoothed; the compressor has no automatic makeup or lookahead.

Callbacks cache APVTS raw parameter pointers and load atomics once per block. DSP state and circular buffers belong to the audio thread; buffers allocate in prepareToPlay. No filesystem, network, GUI work, mutex, blocking host query or explicit allocation occurs in processBlock/render/engine processing. The read-wrap loop is bounded by the internal delay range (less than preallocated buffer length); its only callers use prepared, positive bounded delays. ScopedNoDenormals is active in normal and bypassed callbacks. Bad host NaN/inf samples are replaced with zero. Intensity coefficient refresh uses unsigned ticks, avoiding signed overflow. GUI is message-thread-only and consumes atomic meter values at 30 Hz. State copy/restore uses APVTS synchronization outside processBlock; it is not advertised as an RT serialization API. Preset selection performs host gestures outside the processing callback.

The allocation probe covers 1,800 actual callbacks, normal and bypassed, with new/new[]/aligned-new and linked-code malloc/calloc/realloc wrappers: all counts zero. This is executed evidence plus source review, not proof that arbitrary future external calls cannot allocate. Multi-parameter changes are individual atomics, not a transactional preset snapshot; existing smoothing semantics are retained.

## Executed validation

| Area | Classification | Evidence |
|---|---|---|
| Baseline Release build and existing tests | VERIFIED PASS | baseline-build.txt / baseline-tests.txt |
| Final Release VST3 and native ELF | VERIFIED PASS | release-build.txt, binary metadata; exact bundle name Vocalis Voice.vst3 |
| Final Debug build/tests | VERIFIED PASS | debug-build.txt / debug-tests.txt, all seven CTest entries pass |
| Original DSP coverage | VERIFIED PASS | 28 checks, dsp-tests-and-benchmark.txt |
| Added DSP audit | VERIFIED PASS | Six suites: boundary, measured dynamics timing, meter calibration, harmonics, block partition equality, 44.1–192 kHz vocal-like/noisy/transient matrix |
| Processor/state/multi-instance | VERIFIED PASS | Eight suites including 16 simultaneously retained processors |
| GUI bindings/lifecycle | VERIFIED PASS offscreen | Six sliders, all four enable/Bypass toggles, preset callback/trim preservation, keyboard, numeric parsing, reset, host feedback, 20 editor cycles with concurrent audio |
| Rider renderer parity | VERIFIED PASS | 30 pixel comparisons, zero differing pixels |
| Audio before/after | VERIFIED PASS | 5,255,600 samples; zero differing samples for both styling-only and final builds, tolerance 0 |
| Exported VST3 | VERIFIED PASS in JUCE harness | Discovery, all layouts, measured +6 dB automation, old/new state compatibility, repeated load/unload |
| ASan + UBSan | VERIFIED PASS for executed targets | Fully instrumented JUCE-linked processor/state, GUI cycles, audit DSP and concurrent processor tests; no reports. Instrumented native VST3 also builds |
| LeakSanitizer | BLOCKED | Minimal threaded probe fails with LeakSanitizer fatal ptrace error; leak-free operation is not claimed |
| ThreadSanitizer | BLOCKED | Minimal atomic/thread probe exits 66 with unexpected memory mapping; no race-free claim |
| Concurrent automation/meters | VERIFIED PASS as stress test | Actual processor callbacks with concurrent APVTS writes and atomic meter reads; zero numerical errors |
| GCC static analyzer | VERIFIED PASS for callback entry analysis; diagnostic reviewed in test setup | Engine entry analysis exits 0 without warnings. Full audit-test analysis emits one possible-null vector-access warning after successful assign; standard vector allocation throws on failure, size is 11528, and ASan confirms valid setup. Warning retained, not suppressed |
| Installer / uninstaller | VERIFIED PASS in staging | Hash comparison, two upgrade modes/backups, unknown/symlink rejection, Rider sentinel preserved |
| ~/.vst3 installation | BLOCKED | mktemp fails: read-only file system; no installed bundle changed |
| Native editor embedding / physical input | UNVERIFIED | XOpenDisplay fails; native editor and TextEditor attachment require a real desktop |
| REAPER-specific integration | UNVERIFIED | No user session opened; manual temporary-session procedure supplied |
| Recorded speech/singing and listening approval | UNVERIFIED | Synthetic fixtures measured; no human recording corpus or listening acceptance |

Release synthetic CPU: about 0.071 s for 10 s of 48 kHz stereo audio at intensity 100 and all effects on, roughly 0.71% of one realtime core on this Ryzen 9 5900XT. This is a repeatable throughput benchmark, not a REAPER dropout guarantee. Dry latency remains 0 samples, no lookahead/buffering. Effects intentionally delay wet signals; six-second tail declaration is unchanged.

## Release identity and delivery

Native Linux x86_64 VST3 only. Name `Vocalis Voice`, bundle `audio.vocalis.voice`, manufacturer `Vcls`, product `Vvce`. Processor CID `ABCDEF019182FAEB56636C7356766365`; controller CID `ABCDEF011234ABCD56636C7356766365`. All ten `vv_` IDs, parameter definitions and factory preset values are unchanged. XML root remains VocalisVoiceState, schema 1. No Rider processing/state is imported.

Release bundle: `build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3`. Audited archive: `dist/VocalisVoice-1.0.0-audited-linux-x86_64.tar.gz`, with SHA256 and sources/notices/reports. Old packages/checkpoint are retained.

Run from a normal terminal:

```bash
cd ~/AudioDevelopment/VocalisVoice
./scripts/install.sh
```

Close Voice instances before replacing the bundle. In REAPER for Linux, open Preferences → Plug-ins → VST, ensure `~/.vst3` is in paths, then use Re-scan (clear cache/re-scan if a stale build remains). Insert Vocalis Voice into a new temporary project. Execute `docs/REAPER_TESTING.md`; check discovery, UI, automation, layouts, bypass, all presets, saving/reopening, offline render and multiple instances. Existing identifiable Voice builds are backed up; Rider Live is untouched.

Remaining acceptance is tracked in VOICE_KNOWN_ISSUES.md. No production-ready declaration is made while host, native interaction, leak/race instrumentation and real-vocal checks remain outstanding.
