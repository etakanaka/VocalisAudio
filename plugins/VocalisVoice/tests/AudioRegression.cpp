/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "PluginProcessor.h"
#include <fstream>
#include <iostream>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    std::ofstream file(argv[1], std::ios::binary);
    uint64_t samples = 0;
    for (double sr : {44100, 48000, 96000, 192000})
        for (auto io : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
            for (int preset = 0; preset < 10; ++preset)
            {
                VoiceProcessor p;
                auto l = p.getBusesLayout();
                l.inputBuses.set(0, io.first == 1 ? juce::AudioChannelSet::mono()
                                                  : juce::AudioChannelSet::stereo());
                l.outputBuses.set(0, io.second == 1 ? juce::AudioChannelSet::mono()
                                                    : juce::AudioChannelSet::stereo());
                if (!p.setBusesLayout(l))
                    return 3;
                p.setCurrentProgram(preset);
                p.prepareToPlay(sr, 1024);
                juce::AudioBuffer<float> b(io.second, 1024);
                juce::MidiBuffer m;
                uint32_t rng = 1234567;
                int offset = 0;
                for (int k = 0; k < 80; ++k)
                {
                    int n = std::array{32, 64, 127, 256, 511, 1024}[size_t(k % 6)];
                    b.setSize(io.second, n, false, false, true);
                    for (int c = 0; c < io.second; ++c)
                        for (int i = 0; i < n; ++i)
                        {
                            rng = rng * 1664525u + 1013904223u;
                            float noise = (float(rng >> 8) / 16777216.0f - .5f) * .03f;
                            float t = float(offset + i) / float(sr);
                            float envelope = .5f + .5f * std::sin(2 * vocalis::pi * 3.7f * t);
                            float voice = .2f * envelope *
                                          (std::sin(2 * vocalis::pi * (c ? 227 : 123) * t) +
                                           .25f * std::sin(2 * vocalis::pi * 1300 * t));
                            float x = k < 3               ? 0
                                      : k < 10            ? voice * .001f
                                      : k == 10 && i == 0 ? .95f
                                      : k > 65            ? 0
                                                          : voice + noise;
                            b.setSample(c, i, x);
                        }
                    if (k % 13 == 0)
                    {
                        auto *a = p.state.getParameter(voiceparams::ids[1]);
                        a->setValueNotifyingHost(float(k % 5) * .25f);
                    }
                    if (k == 40)
                        p.state.getParameter(voiceparams::ids[9])->setValueNotifyingHost(1);
                    if (k == 44)
                        p.state.getParameter(voiceparams::ids[9])->setValueNotifyingHost(0);
                    p.processBlock(b, m);
                    for (int c = 0; c < io.second; ++c)
                    {
                        file.write(reinterpret_cast<const char *>(b.getReadPointer(c)),
                                   std::streamsize(n * sizeof(float)));
                        samples += uint64_t(n);
                    }
                    offset += n;
                }
            }
    std::cout << samples << " float32 output samples\n";
    return file ? 0 : 4;
}
