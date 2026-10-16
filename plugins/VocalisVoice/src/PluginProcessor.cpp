/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "PluginProcessor.h"
#include <charconv>
#include <cstring>
VoiceProcessor::VoiceProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      state(*this, nullptr, "VocalisVoiceState", voiceparams::layout()), parameters(state)
{
}
void VoiceProcessor::prepareToPlay(double sr, int)
{
    engine.prepare(sr, parameters.get());
    setLatencySamples(0);
}
bool VoiceProcessor::isBusesLayoutSupported(const BusesLayout &l) const
{
    auto i = l.getMainInputChannelSet(), o = l.getMainOutputChannelSet();
    return (i == juce::AudioChannelSet::mono() &&
            (o == juce::AudioChannelSet::mono() || o == juce::AudioChannelSet::stereo())) ||
           (i == juce::AudioChannelSet::stereo() && o == juce::AudioChannelSet::stereo());
}
void VoiceProcessor::render(juce::AudioBuffer<float> &b, bool hostBypass)
{
    juce::ScopedNoDenormals noDenormals;
    auto s = parameters.get();
    s.bypass |= hostBypass;
    engine.set(s);
    int outputs = getTotalNumOutputChannels();
    if (b.getNumChannels() < outputs || b.getNumSamples() == 0)
        return;
    engine.process(b.getWritePointer(0), outputs == 2 ? b.getWritePointer(1) : nullptr,
                   b.getNumSamples(), getTotalNumInputChannels(), outputs, &meters);
}
void VoiceProcessor::processBlock(juce::AudioBuffer<float> &b, juce::MidiBuffer &midi)
{
    midi.clear();
    render(b, false);
}
void VoiceProcessor::processBlockBypassed(juce::AudioBuffer<float> &b, juce::MidiBuffer &midi)
{
    midi.clear();
    render(b, true);
}
void VoiceProcessor::setCurrentProgram(int i)
{
    if (!juce::isPositiveAndBelow(i, getNumPrograms()))
        return;
    const auto &p = voicePresets[i];
    const float values[]{p.intensity, p.delayOn ? 1.0f : 0.0f,  p.delay, p.doublerOn ? 1.0f : 0.0f,
                         p.doubler,   p.reverbOn ? 1.0f : 0.0f, p.reverb};
    for (int n = 0; n < 7; ++n)
    {
        auto *param = state.getParameter(voiceparams::ids[n + 1]);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(values[n]));
        param->endChangeGesture();
    }
    presetIndex.store(i);
}
bool VoiceProcessor::matchesCurrentPreset() const
{
    auto s = parameters.get();
    const auto &p = voicePresets[presetIndex.load()];
    return std::abs(s.intensity - p.intensity) < .06f && s.delayOn == p.delayOn &&
           std::abs(s.delay - p.delay) < .06f && s.doublerOn == p.doublerOn &&
           std::abs(s.doubler - p.doubler) < .06f && s.reverbOn == p.reverbOn &&
           std::abs(s.reverb - p.reverb) < .06f;
}
void VoiceProcessor::getStateInformation(juce::MemoryBlock &data)
{
    auto tree = state.copyState();
    tree.setProperty("schemaVersion", 1, nullptr);
    tree.setProperty("factoryPreset", presetIndex.load(), nullptr);
    if (auto xml = tree.createXml())
        copyXmlToBinary(*xml, data);
}
void VoiceProcessor::setStateInformation(const void *data, int size)
{
    if (data == nullptr || size <= 0 || size > 1024 * 1024)
        return;
    auto xml = getXmlFromBinary(data, size);
    if (!xml || !xml->hasTagName("VocalisVoiceState"))
        return;
    auto tree = juce::ValueTree::fromXml(*xml);
    if (int(tree.getProperty("schemaVersion", 1)) != 1)
        return;
    // Validate the complete version-1 payload before mutating any live values.
    // APVTS otherwise fills missing children and can partially apply corrupt state.
    if (tree.getNumChildren() != int(std::size(voiceparams::ids)))
        return;
    std::array<bool, std::size(voiceparams::ids)> seen{};
    for (auto child : tree)
    {
        if (!child.hasType("PARAM") || !child.hasProperty("id") || !child.hasProperty("value"))
            return;
        const auto id = child.getProperty("id").toString();
        size_t index = 0;
        while (index < seen.size() && id != voiceparams::ids[index])
            ++index;
        if (index == seen.size() || seen[index])
            return;
        seen[index] = true;
        const auto text = child.getProperty("value").toString();
        const auto *first = text.toRawUTF8();
        const auto *last = first + std::strlen(first);
        double value = 0;
        const auto parsed = std::from_chars(first, last, value);
        if (parsed.ec != std::errc{} || parsed.ptr != last || !std::isfinite(value))
            return;
        const auto range = state.getParameter(voiceparams::ids[index])->getNormalisableRange();
        if (value < range.start || value > range.end)
            return;
    }
    presetIndex.store(
        juce::jlimit(0, getNumPrograms() - 1, int(tree.getProperty("factoryPreset", 0))));
    state.replaceState(tree);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new VoiceProcessor();
}
