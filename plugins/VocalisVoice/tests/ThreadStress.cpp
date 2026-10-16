/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "PluginProcessor.h"
#include <iostream>
#include <thread>
int main()
{
    VoiceProcessor p;
    p.prepareToPlay(48000, 1024);
    auto *intensity = p.state.getParameter(voiceparams::ids[1]);
    auto *delay = p.state.getParameter(voiceparams::ids[2]);
    std::atomic<bool> done{false};
    std::atomic<int> errors{0};
    std::thread audio(
        [&]
        {
            juce::AudioBuffer<float> b(2, 1024);
            juce::MidiBuffer m;
            for (int k = 0; k < 3000; ++k)
            {
                int n = std::array{32, 64, 128, 256, 512, 1024}[size_t(k % 6)];
                b.setSize(2, n, false, false, true);
                for (int c = 0; c < 2; ++c)
                    for (int i = 0; i < n; ++i)
                        b.setSample(c, i, .1f * std::sin(float(i + k) * .03f));
                p.processBlock(b, m);
                for (int c = 0; c < 2; ++c)
                    for (int i = 0; i < n; ++i)
                        if (!std::isfinite(b.getSample(c, i)))
                            ++errors;
            }
            done = true;
        });
    int updates = 0;
    while (!done)
    {
        intensity->setValueNotifyingHost(float(updates % 101) * .01f);
        delay->setValueNotifyingHost(float(updates % 2));
        for (int c = 0; c < 2; ++c)
        {
            float v = p.meters.inputRms[c].load();
            if (!std::isfinite(v))
                ++errors;
        }
        p.matchesCurrentPreset();
        ++updates;
        std::this_thread::yield();
    }
    audio.join();
    std::cout << "Concurrent actual processor callbacks / APVTS automation / atomic meter reads: "
              << updates << " updates, " << errors << " errors\n";
    return errors ? 1 : 0;
}
