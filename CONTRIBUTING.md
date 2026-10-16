# Contributing to Vocalis Audio

Enos Takanaka is the creator and maintainer of Vocalis Audio, an independent open-source, AI-assisted suite of native Linux VST3 plugins for REAPER. Vocalis Audio is the software brand and manufacturer. Development uses OpenAI Codex, C++20 and JUCE; this does not imply OpenAI sponsorship, endorsement, publication or ownership.

## Report bugs

Open an issue at https://github.com/etakanaka/VocalisAudio/issues. Name the plugin and version, Linux distribution, REAPER version, sample rate, buffer size and channel/sidechain layout. Include reproducible steps, expected/actual behavior and relevant logs. Provide a minimal example where possible; remove private recordings and personal data before sharing.

## Submit changes

Discuss substantial changes in an issue, fork the repository, work on a focused branch and open a pull request. Describe the problem, resulting behavior, affected plugin, test evidence and known limitations. Preserve unrelated work and documentation. Do not include build trees, installed binaries, private recordings or unrelated generated artifacts.

Maintain C++20 compatibility and the supported native Linux x86_64 VST3 / REAPER environment. Preserve plugin class IDs, manufacturer/plugin codes, parameter IDs, state compatibility and automation behavior. Flag any intentional compatibility change for explicit maintainer review.

## Testing

Build the affected plugin with its documented CMake workflow and run its existing automated tests. For DSP changes, include meaningful audio regression evidence, supported sample rates/layouts, state/preset tests and real-time allocation/thread checks as appropriate. For interface changes, verify controls, scaling and About content; include screenshots where helpful. Report exact commands and results. Distinguish JUCE test-host checks from tests actually executed in REAPER and state what could not be tested. Never overwrite a working installed plugin just to run tests without its user's approval.

## Attribution and copyright

Contributors retain copyright in their respective work. No copyright assignment or contributor licence agreement is introduced here. Credit contributors for their actual contributions in notices, documentation or release notes as appropriate. Preserve original copyright, authorship, licence and adaptation notices, including those in imported code. Do not assert exclusive project ownership over external materials or add creator headers to generated or vendored files.

## Licence compatibility and third-party assets

Inspect the applicable licence in each directory before contributing. Both current plugin source trees document AGPL-3.0-or-later; the root LICENSE contains GPL version 3 and its intended scope relative to the plugin AGPL notices needs maintainer clarification before publication. Dependency-specific terms remain applicable to their respective materials. Do not silently relicense files or narrow an existing 'or later' grant. Submit only work you have permission to contribute under terms compatible with the affected component. Raise uncertain compatibility with the maintainer before importing material.

For external code, adapted graphics, fonts, audio examples or other assets, provide the upstream source URL, author/rightsholder, exact version or revision, licence text, redistribution permissions and required attribution. Document adaptations and retain notices. Keep independently created assets distinguishable from adaptations, references and generated resources. Do not assume a reference image, Calf asset, JUCE component or VST3 SDK file shares the project's copyright or licence. Preserve JUCE, Steinberg and other dependency notices and review corresponding-source obligations when distributing builds.

## AI-assisted contributions

Disclose material AI assistance in the pull request, including the tool and its role. Review and test all submitted output, check provenance and licensing of suggested material, and avoid including secrets or private third-party content. AI assistance does not replace responsibility for correctness, attribution or rights review. Do not imply that an AI provider owns, endorses or sponsors the project.
