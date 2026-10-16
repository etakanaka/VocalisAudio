# Known issues and unverified requirements

> Initial-build record. Superseded by [the current audit](VOICE_FINAL_AUDIT.md) and [current issues](VOICE_KNOWN_ISSUES.md).

* REAPER discovery, UI interaction, envelope automation, project reopen, offline render, host hard bypass, multiple instances and live device switching: UNVERIFIED in REAPER. See REAPER_TESTING.md. Synthetic processor state roundtrips and a separate VST3 harness are verified, not REAPER session persistence itself.
* Installation into ~/.vst3 blocked by read-only session mount. Installer and uninstaller are tested against a writable staging directory. Run scripts/install.sh in your own terminal for actual installation. Rider Live is untouched.
* No recorded vocal listening corpus was supplied. Real-world speech/singing timbre coverage, perceptual transparency, metallic/ringing artefacts and mix decisions require listening validation. Mathematical and synthetic-signal tests cannot establish professional sound quality alone.
* No dedicated de-esser, pitch correction, noise removal, tempo sync or output limiter. High output trims/parallel sums can clip downstream. Brightness guard is not dynamic sibilance suppression.
* Harmonic stage uses a very small un-oversampled tanh contribution above intensity 60; no alias-free claim. Disabling effects fades their complete returns, including tails. Disabled effects continue processing and consume CPU.
* GUI follows the supplied `VOCALIS Voice Enhancer Interface.png` direction. Exact `reference.png` was absent. Actual compiled production-editor captures are in artifacts; this is an original JUCE implementation, not a pixel reproduction. Native editor window attachment is unverified because XOpenDisplay fails in this sandbox; an attempted host GUI attachment revealed a harness/framework null-display crash, now avoided by a preflight check. REAPER-specific scaling and keyboard routing remain unverified.
* Resize preference is per-editor and is not persisted. No separate user-preset file browser; use REAPER presets/projects for user states.
* Host reports zero dry latency and a six-second tail. Effects introduce intentional wet delays. Filter phase shift is causal and does not require plugin delay compensation.
* Linux Mint 22.3 x86_64 is the build/test environment; no other distribution, architecture or host is certified. Build/installer does not produce other plugin formats.
* JUCE dependencies are subject to JUCE's AGPLv3/commercial licence terms. Read the local JUCE LICENSE.md before distribution. No commercial licence entitlement is assumed.
