/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "PluginProcessor.h"
#include "ui/AboutPanel.h"
#include "ui/EffectControls.h"
#include "ui/GainReductionMeter.h"
#include "ui/IntensityKnob.h"
#include "ui/LevelMeter.h"
#include "ui/SensitivityIndicator.h"
class VoiceEditor : public juce::AudioProcessorEditor, private juce::Timer
{
  public:
    explicit VoiceEditor(VoiceProcessor &);
    ~VoiceEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;
    void showAbout();

  private:
    VoiceProcessor &processor;
    voiceui::LookAndFeel look;
    juce::Component surface;
    voiceui::AboutPanel aboutPanel;
    IntensityKnob intensity;
    juce::Slider input, output;
    EffectControls delay{"DELAY", voiceui::purple}, doubler{"DOUBLER", voiceui::blue},
        reverb{"REVERB", voiceui::teal};
    LevelMeter inMeter, outMeter;
    GainReductionMeter reduction;
    SensitivityIndicator sensitivity;
    juce::ComboBox presets;
    juce::TextButton settings{"SETTINGS"}, bypass{"BYPASS"};
    juce::TooltipWindow tooltip{this, 650};
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;
    void timerCallback() override;
    void showSettings();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceEditor)
};
