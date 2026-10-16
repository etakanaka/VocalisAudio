/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "dsp/VoiceEngine.h"
#include <juce_audio_processors/juce_audio_processors.h>
namespace voiceparams
{
inline constexpr const char *ids[]{"vv_input_db",   "vv_intensity",  "vv_delay_on",  "vv_delay_db",
                                   "vv_doubler_on", "vv_doubler_db", "vv_reverb_on", "vv_reverb_db",
                                   "vv_output_db",  "vv_bypass"};
inline juce::AudioProcessorValueTreeState::ParameterLayout layout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto number = [&](int i, const char *name, float min, float max, float def, const char *unit)
    {
        l.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ids[i], 1}, name, juce::NormalisableRange<float>(min, max, .1f), def,
            juce::AudioParameterFloatAttributes().withLabel(unit)));
    };
    auto boolean = [&](int i, const char *name, bool def)
    { l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ids[i], 1}, name, def)); };
    number(0, "Input Gain", -24, 24, 0, "dB");
    number(1, "Intensity", 0, 100, 50, "%");
    boolean(2, "Delay Enable", false);
    number(3, "Delay Level", -30, 0, -30, "dB");
    boolean(4, "Doubler Enable", true);
    number(5, "Doubler Level", -30, 0, -11.3f, "dB");
    boolean(6, "Reverb Enable", true);
    number(7, "Reverb Level", -30, 0, -11.3f, "dB");
    number(8, "Output Gain", -24, 24, 0, "dB");
    boolean(9, "Bypass", false);
    return l;
}
struct Reader
{
    std::array<std::atomic<float> *, 10> p{};
    explicit Reader(juce::AudioProcessorValueTreeState &s)
    {
        for (size_t i = 0; i < p.size(); ++i)
            p[i] = s.getRawParameterValue(ids[i]);
    }
    vocalis::Settings get() const
    {
        vocalis::Settings s;
        s.input = p[0]->load();
        s.intensity = p[1]->load();
        s.delayOn = p[2]->load() > .5f;
        s.delay = p[3]->load();
        s.doublerOn = p[4]->load() > .5f;
        s.doubler = p[5]->load();
        s.reverbOn = p[6]->load() > .5f;
        s.reverb = p[7]->load();
        s.output = p[8]->load();
        s.bypass = p[9]->load() > .5f;
        return s;
    }
};
} // namespace voiceparams
