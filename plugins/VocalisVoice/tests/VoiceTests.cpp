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
#include <stdexcept>
void check(bool v, const char *m)
{
    if (!v)
        throw std::runtime_error(m);
}
void set(VoiceProcessor &p, int id, float v)
{
    auto *a = p.state.getParameter(voiceparams::ids[id]);
    a->setValueNotifyingHost(a->convertTo0to1(v));
}
int main()
{
    int passed = 0, failed = 0;
    auto test = [&](const char *n, auto fn)
    {
        try
        {
            fn();
            std::cout << "PASS " << n << '\n';
            ++passed;
        }
        catch (const std::exception &e)
        {
            std::cerr << "FAIL " << n << ": " << e.what() << '\n';
            ++failed;
        }
    };
    test("Stable parameters and defaults",
         []
         {
             VoiceProcessor p;
             check(p.getParameters().size() == 10, "parameter count");
             auto s = p.parameters.get();
             check(s.intensity == 50 && s.input == 0 && s.output == 0 && !s.delayOn &&
                       s.doublerOn && s.reverbOn,
                   "defaults");
             check(p.getBypassParameter() == p.state.getParameter("vv_bypass"), "host bypass");
             for (auto id : voiceparams::ids)
                 check(p.state.getParameter(id) != nullptr, "parameter identity");
         });
    test("Factory presets / trim preservation",
         []
         {
             VoiceProcessor p;
             set(p, 0, 7.3f);
             set(p, 8, -5.2f);
             for (int i = 0; i < p.getNumPrograms(); ++i)
             {
                 p.setCurrentProgram(i);
                 auto s = p.parameters.get();
                 const auto &preset = voicePresets[i];
                 check(p.matchesCurrentPreset(), "preset mismatch");
                 check(std::abs(s.intensity - preset.intensity) < .06f, "intensity");
                 check(std::abs(s.input - 7.3f) < .06f && std::abs(s.output + 5.2f) < .06f,
                       "trim changed");
             }
             p.setCurrentProgram(-1);
             p.setCurrentProgram(999);
             check(p.getCurrentProgram() == 9, "invalid program changed state");
         });
    test("Plugin state roundtrip / all ten parameters",
         []
         {
             VoiceProcessor p;
             p.setCurrentProgram(5);
             set(p, 0, 3.7f);
             set(p, 8, -2.1f);
             set(p, 9, 1);
             juce::MemoryBlock state;
             p.getStateInformation(state);
             VoiceProcessor q;
             q.setStateInformation(state.getData(), int(state.getSize()));
             for (int i = 0; i < 10; ++i)
                 check(std::abs(p.state.getRawParameterValue(voiceparams::ids[i])->load() -
                                q.state.getRawParameterValue(voiceparams::ids[i])->load()) < .0001f,
                       "roundtrip mismatch");
             check(q.getCurrentProgram() == 5, "preset metadata");
             q.setStateInformation(nullptr, 0);
             const char bad[] = "not a state";
             q.setStateInformation(bad, sizeof(bad));
             check(q.getCurrentProgram() == 5, "invalid state changed state");
         });
    test("Real processor bus layouts and closed editor processing",
         []
         {
             for (auto channels : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
             {
                 VoiceProcessor p;
                 auto l = p.getBusesLayout();
                 l.inputBuses.set(0, channels.first == 1 ? juce::AudioChannelSet::mono()
                                                         : juce::AudioChannelSet::stereo());
                 l.outputBuses.set(0, channels.second == 1 ? juce::AudioChannelSet::mono()
                                                           : juce::AudioChannelSet::stereo());
                 check(p.setBusesLayout(l), "layout rejected");
                 for (double sr : {44100, 48000, 96000})
                     for (int n : {1, 64, 257, 2048})
                     {
                         p.setRateAndBufferSizeDetails(sr, n);
                         p.prepareToPlay(sr, n);
                         juce::AudioBuffer<float> b(channels.second, n);
                         juce::MidiBuffer midi;
                         for (int k = 0; k < 40; ++k)
                         {
                             for (int c = 0; c < channels.second; ++c)
                                 for (int i = 0; i < n; ++i)
                                     b.setSample(c, i, .1f * std::sin(float(k * n + i) * .04f));
                             p.processBlock(b, midi);
                             for (int c = 0; c < channels.second; ++c)
                                 for (int i = 0; i < n; ++i)
                                     check(std::isfinite(b.getSample(c, i)), "nonfinite");
                         }
                         p.releaseResources();
                     }
             }
         });
    test("Host bypass / resume",
         []
         {
             VoiceProcessor p;
             p.prepareToPlay(48000, 256);
             juce::AudioBuffer<float> b(2, 256);
             juce::MidiBuffer midi;
             for (int k = 0; k < 10; ++k)
             {
                 for (int c = 0; c < 2; ++c)
                     for (int i = 0; i < 256; ++i)
                         b.setSample(c, i, .1f);
                 p.processBlockBypassed(b, midi);
             }
             check(std::abs(b.getSample(0, 255) - .1f) < 1.e-5, "host bypass wrong");
             for (int k = 0; k < 10; ++k)
             {
                 b.clear();
                 p.processBlock(b, midi);
             }
             check(std::isfinite(b.getSample(0, 255)), "resume invalid");
         });
    test("Repeated plugin creation and state restore",
         []
         {
             juce::MemoryBlock state;
             {
                 VoiceProcessor p;
                 p.setCurrentProgram(7);
                 p.getStateInformation(state);
             }
             for (int k = 0; k < 20; ++k)
             {
                 VoiceProcessor p;
                 p.setStateInformation(state.getData(), int(state.getSize()));
                 check(p.matchesCurrentPreset(), "restore mismatch");
                 p.prepareToPlay(48000, 64);
                 juce::AudioBuffer<float> b(2, 64);
                 b.clear();
                 juce::MidiBuffer m;
                 p.processBlock(b, m);
             }
         });
    test("Corrupt state rejection is atomic",
         []
         {
             VoiceProcessor source;
             set(source, 0, 7.3f);
             source.setCurrentProgram(5);
             auto base = source.state.copyState();
             for (int defect = 0; defect < 4; ++defect)
             {
                 auto tree = base.createCopy();
                 if (defect == 0)
                     tree.removeChild(tree.getNumChildren() - 1, nullptr);
                 if (defect == 1)
                     tree.getChild(1).setProperty("id", tree.getChild(0).getProperty("id"),
                                                  nullptr);
                 if (defect == 1)
                     tree.getChild(1).setProperty("value", 7.3f, nullptr);
                 if (defect == 2)
                     tree.getChild(0).removeProperty("value", nullptr);
                 if (defect == 3)
                     tree.getChild(0).setProperty("value", "7.3garbage", nullptr);
                 auto xml = tree.createXml();
                 juce::MemoryBlock bytes;
                 juce::AudioProcessor::copyXmlToBinary(*xml, bytes);
                 VoiceProcessor target;
                 std::array<float, 10> before{};
                 for (size_t n = 0; n < before.size(); ++n)
                     before[n] = target.state.getRawParameterValue(voiceparams::ids[n])->load();
                 target.setStateInformation(bytes.getData(), int(bytes.getSize()));
                 for (size_t n = 0; n < before.size(); ++n)
                     check(target.state.getRawParameterValue(voiceparams::ids[n])->load() ==
                               before[n],
                           "corrupt state partially applied");
                 check(target.getCurrentProgram() == 0, "corrupt state changed preset");
             }
         });
    test("Sixteen independent live processor instances",
         []
         {
             std::vector<std::unique_ptr<VoiceProcessor>> instances;
             for (int i = 0; i < 16; ++i)
             {
                 auto p = std::make_unique<VoiceProcessor>();
                 p->setCurrentProgram(i % 10);
                 p->prepareToPlay(i % 2 ? 44100 : 192000, 256);
                 instances.push_back(std::move(p));
             }
             juce::AudioBuffer<float> b(2, 256);
             juce::MidiBuffer midi;
             for (int block = 0; block < 80; ++block)
                 for (auto &p : instances)
                 {
                     for (int c = 0; c < 2; ++c)
                         for (int n = 0; n < 256; ++n)
                             b.setSample(c, n, .2f * std::sin(float(block * 256 + n) * .04f));
                     p->processBlock(b, midi);
                     for (int c = 0; c < 2; ++c)
                         for (int n = 0; n < 256; ++n)
                             check(std::isfinite(b.getSample(c, n)), "instance instability");
                 }
         });
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
