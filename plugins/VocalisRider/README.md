# Vocalis Rider Live

An independently developed C++20/JUCE native Linux VST3 automatic vocal gain rider, with a custom portrait metal-panel editor. Processes mono or linked stereo, with an optional mono/stereo music sidechain. No Waves code, artwork, or affiliation.

## Build and install

Tested build environment: Linux Mint/Ubuntu toolchain, GCC 13.3, CMake 3.28, Ninja, local JUCE 9. The project uses `~/AudioDevelopment/JUCE`; override with `-DJUCE_PATH=/path/to/JUCE`. Dependencies: ALSA, FreeType, Fontconfig and X11 development libraries (Xext, Xrandr, Xinerama, Xcursor); these were already present. No system packages were installed.

```bash
cd ~/AudioDevelopment/VocalisRider
./scripts/rebuild.sh
./scripts/install.sh
```

Release bundle: `build-release/VocalisRiderLive_artefacts/Release/VST3/Vocalis Rider Live.vst3`.
The install script stages a complete copy, preserves a unique backup and copies only to `~/.vst3/` and backs up an existing bundle. Restart/rescan your host after installation. Remove it with `./scripts/uninstall.sh`. Native Linux VST3 is the only enabled plugin format. REAPER for Linux is the supported host.

## Use

Insert on a vocal track, start with Default Vocal Rider, and set Target to the desired approximate RMS vocal level. Range sets the allowed automatic correction, not the final output ceiling. Adjust Spill above background noise but below quiet words. Increase Vocal sensitivity if quiet words fail to engage Ride. Slow gives gentle phrase movement; Fast follows changes sooner. Output sets the final trim. Leave headroom: this gain rider has no limiter and can exceed 0 dBFS.

Yellow Target strip, rotaries, faders, brass Idle marker and both Range handles are draggable. Shift-drag gives finer adjustment; double-click restores defaults. Black readouts can be edited. Rider is displayed from the engine in Off/Write and is editable/automatable in Read. The green Ride light and yellow Idle light show detector state; the Music light indicates sidechain activity. Resize by dragging the editor corner; proportions remain fixed (75–140% scale).

Use the preset menu for eight real configurations. Modified presets keep their starting preset name; the menu is not an indication that parameters are unchanged. Host state saves every parameter, including the preset label and the host-controlled Rider value. Audio detector history and captured automation are not session state.

## Automation

- **OFF:** automatic riding; no moving Rider notifications sent to the host.
- **WRITE:** automatic riding; processor-owned 50 Hz message-thread timer sends Rider gesture/value notifications and retains timestamped points for CSV export. Works with the editor closed while the host message loop runs. Arm only Rider in the host and use its Write/Touch recording mode. Confirm that actual envelope points are recorded before relying on it.
- **READ:** host/manual Rider value supplies the gain, independent of automatic range and activity. Output still applies; input/sidechain analysis continues.

WRITE host recording has not been validated in REAPER here. VST3 notifications are implemented using JUCE's supported parameter APIs; host recording policy remains host-dependent. CSV export plus the included REAPER importer provides an explicit alternative. See [current REAPER compatibility](docs/REAPER_COMPATIBILITY.md) and [technical details](docs/TECHNICAL.md).

## Current release status

**READY FOR CONTROLLED TESTING.** See [final debugging report](docs/FINAL_DEBUG_REPORT.md), [regression results](docs/REGRESSION_TEST_RESULTS.md), [REAPER compatibility](docs/REAPER_COMPATIBILITY.md) and [known issues](docs/KNOWN_ISSUES.md). These supersede historical validation statements below. The current interface follows `Vocalis Leveler Plugin Interface.png`; processor identity and parameter/state formats remain compatible. REAPER is installed, but this session cannot access the desktop to validate it inside the host.

## Historical verification and limitations

See [executed checks](docs/TESTING.md) and logs in `artifacts/`. The actual JUCE editor is rendered to `artifacts/editor.png` by the integration test. `reference.png` was absent, so direct visual matching could not be performed. REAPER and Ardour were not found; their scanning, embedded GUI, automation recording and project/session reopening remain unverified.

This is an energy/activity detector, not a trained speech classifier. Loud continuous noise above Spill can trigger riding. Exact proprietary detector behaviour is unknown. Music adaptation uses a documented independent reference and is deliberately bounded. Zero lookahead means the onset of a sudden loud word can pass before the fader reacts.

## Licence

Project code is offered under AGPL-3.0-or-later; see `LICENSE.md`. The installed JUCE 9 source is dual AGPLv3/commercial; this build uses its open-source option. VST3 SDK is MIT-licensed. Dependency notices are in `docs/licenses/`; JUCE source is required to rebuild. No proprietary artwork or dependencies were added. For distributing binaries, provide corresponding source including the exact JUCE revision and applicable third-party notices. This local build is not a commercially licensed JUCE product.

### Author & Development

**Created and maintained by Enos Takanaka**

Vocalis Audio is an independent open-source audio software project developed through AI-assisted programming using OpenAI Codex, C++ and JUCE.

The project focuses on creating accessible audio production tools for Linux content creators, podcasters, video producers, streamers and spoken-word professionals.

Vocalis Audio welcomes community contributions. Additional contributors will be acknowledged for their respective work.

Vocalis Audio is the software brand and manufacturer. OpenAI does not sponsor, endorse, publish or own this project.
