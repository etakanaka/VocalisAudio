# Build inputs

- Local JUCE source: `/home/enos/AudioDevelopment/JUCE`
- JUCE version: 9.0.4 (per source version macros)
- JUCE Git revision: `454fe891acc8204f99f4ef1e3a28c84b1a4da145`
- Compiler: GCC/G++ 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04)
- CMake: 3.28.3; generator: Ninja; C++20; Release / -O3 / NDEBUG
- Architecture: Linux x86_64, native build (not Wine)
- SDK: JUCE's bundled VST3 SDK, MIT license (2025 Steinberg)
- GUI: JUCE native graphics, Fontconfig/FreeType, dynamically loaded X11.
- Runtime dependency scan: `artifacts/dependencies.txt` (all libraries resolved).

No packages or dependencies were installed system-wide. A local-only attempt to download a virtual X server failed due to repository DNS resolution; GUI capture succeeded using JUCE offscreen rendering instead.

The project licence is AGPL-3.0-or-later, consistent with the installed JUCE 9 open-source licence option. Keep the exact JUCE source revision available with corresponding-source distributions. JUCE's SPDX dependency inventory and its root licence are included under `docs/licenses` alongside VST3 SDK notices. Other module-specific dependency notices remain in the JUCE source tree.
