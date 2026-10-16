# VOCALIS VOICE

Native Linux x86_64 VST3 vocal processor for evaluation in REAPER for Linux, built with C++20 and the existing JUCE checkout. Original DSP: intensity macro coordinates tone, RMS compression, presence/air and subtle harmonics, followed by parallel delay, stereo doubling and eight-line FDN reverb. No proprietary algorithms or Waves presets are copied or claimed equivalent.

Current audit: [final findings](docs/VOICE_FINAL_AUDIT.md), [exact Rider design system](docs/VOCALIS_DESIGN_SYSTEM.md), [visual comparison](docs/VOICE_DESIGN_COMPARISON.md), [audio regression](docs/VOICE_REGRESSION_TESTS.md), [remaining issues](docs/VOICE_KNOWN_ISSUES.md). Status is controlled testing; native REAPER acceptance is outstanding.

## Build and test

```bash
./scripts/build.sh Release
./scripts/test.sh Release
./scripts/build.sh Debug
./scripts/test.sh Debug
./scripts/sanitize.sh
./scripts/sanitize.sh --full  # instrument JUCE-linked processor and GUI
./scripts/thread-sanitize.sh # exits 77 when runtime is blocked
```

Requires CMake ≥3.22, Ninja, GCC with C++20 and JUCE's Linux dependencies. JUCE defaults to `~/AudioDevelopment/JUCE`; override `JUCE_PATH` if needed. `JOBS=3` controls build parallelism. Build targets only VST3; test executables are developer utilities, not additional plugin formats. The VST3 native GUI probe requires an accessible X11 display; the production-editor render and interaction test can run headlessly.

Package the tested Release with `./scripts/package.sh` (archive and SHA256 in `dist/`). Extracted packages include the VST3 at the top level and use the same install script.

Release bundle: `build/Release/VocalisVoice_artefacts/Release/VST3/Vocalis Voice.vst3`.

```bash
./scripts/install.sh
./scripts/uninstall.sh
```

Installation defaults to `~/.vst3/Vocalis Voice.vst3`. This sandbox mounts ~/.vst3 read-only; installation must be run in a normal terminal. Scripts accept an optional destination directory for staging tests. They touch only the specifically named Voice bundle, reject symlinks/unidentified replacements, recognize earlier unmarked Voice builds by both exact class IDs, and retain previous Voice versions as backups. Rider Live is never modified.

## Use

Set input for yellow sensitivity on loud passages, turn Intensity, add the three returns, and level-match with output. Defaults: intensity 50, delay off, doubler and reverb on. All returns can be switched fully off. There is no automatic makeup gain or output limiter. Ten factory presets preserve input/output trims and Bypass; REAPER state saves all parameters. See [controls](docs/CONTROLS.md), [DSP design](docs/DSP.md) and [preset values](docs/PRESETS.md).

The authoritative GUI is now adapted from actual Vocalis Rider Live source, with exact palette and rendering components locally reused under the preserved AGPL notices. The older concept PNG and previous Voice appearance are retained in the checkpoint. Actual GUI captures: [normal](artifacts/gui.png) and [80%](artifacts/gui-80-percent.png). The image is not used as an interactive overlay.

## Verification and limits

Read the current [audit](docs/VOICE_FINAL_AUDIT.md), [REAPER procedure](docs/REAPER_TESTING.md) and [known issues](docs/VOICE_KNOWN_ISSUES.md). Passing automated DSP and VST3 harness tests does not establish REAPER integration or subjective production quality. REAPER-specific and real-vocal listening acceptance checks remain explicitly unverified.

Identity: bundle `audio.vocalis.voice`, manufacturer `Vcls`, plugin `Vvce`; VST3 processor `ABCDEF019182FAEB56636C7356766365`, controller `ABCDEF011234ABCD56636C7356766365`. All ten stable parameter IDs begin `vv_`; no Rider identifiers or state are reused. Host adds a separate Program selector to the ten parameters.

Functional research: [Waves Greg Wells VoiceCentric user guide](https://www.waves.com/1lib/pdf/plugins/greg-wells-voicecentric.pdf). Vocalis is independent and not affiliated with Waves or Greg Wells. JUCE is dual-licensed AGPLv3/commercial; a commercial JUCE licence may be required depending on distribution. See `~/AudioDevelopment/JUCE/LICENSE.md` and its third-party inventory.

Shared Rider GUI adaptations are supplied under AGPL-3.0-or-later; see [LICENSE.md](LICENSE.md) and [NOTICE.md](NOTICE.md). No installed Rider component is required.

### Author & Development

**Created and maintained by Enos Takanaka**

Vocalis Audio is an independent open-source audio software project developed through AI-assisted programming using OpenAI Codex, C++ and JUCE.

The project focuses on creating accessible audio production tools for Linux content creators, podcasters, video producers, streamers and spoken-word professionals.

Vocalis Audio welcomes community contributions. Additional contributors will be acknowledged for their respective work.

Vocalis Audio is the software brand and manufacturer. OpenAI does not sponsor, endorse, publish or own this project.
