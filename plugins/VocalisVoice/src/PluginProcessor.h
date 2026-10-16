/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "state/PresetManager.h"
#include <juce_audio_utils/juce_audio_utils.h>
class VoiceProcessor : public juce::AudioProcessor
{
  public:
    VoiceProcessor();
    const juce::String getName() const override
    {
        return "Vocalis Voice";
    }
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout &) const override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    void processBlockBypassed(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    juce::AudioProcessorParameter *getBypassParameter() const override
    {
        return state.getParameter(voiceparams::ids[9]);
    }
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override
    {
        return true;
    }
    double getTailLengthSeconds() const override
    {
        return 6;
    }
    bool acceptsMidi() const override
    {
        return false;
    }
    bool producesMidi() const override
    {
        return false;
    }
    int getNumPrograms() override
    {
        return int(std::size(voicePresets));
    }
    int getCurrentProgram() override
    {
        return presetIndex.load();
    }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int i) override
    {
        return juce::isPositiveAndBelow(i, getNumPrograms()) ? voicePresets[i].name : "";
    }
    void changeProgramName(int, const juce::String &) override {}
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;
    juce::AudioProcessorValueTreeState state;
    voiceparams::Reader parameters;
    vocalis::MeterSnapshot meters;
    bool matchesCurrentPreset() const;

  private:
    vocalis::VoiceEngine engine;
    std::atomic<int> presetIndex{0};
    void render(juce::AudioBuffer<float> &, bool);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceProcessor)
};
