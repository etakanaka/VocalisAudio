/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "VocalisLookAndFeel.h"
class EffectControls : public juce::Component
{
  public:
    juce::Slider level;
    juce::TextButton enable;
    juce::String title;
    EffectControls(juce::String name, juce::Colour colour) : title(name)
    {
        level.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        level.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 23);
        level.setTextValueSuffix(" dB");
        level.setNumDecimalPlacesToDisplay(1);
        level.setDoubleClickReturnValue(true, name == "DELAY" ? -30 : -11.3);
        level.setColour(juce::Slider::rotarySliderFillColourId, colour);
        level.setWantsKeyboardFocus(true);
        enable.setButtonText("ON");
        enable.setClickingTogglesState(true);
        enable.setWantsKeyboardFocus(true);
        addAndMakeVisible(level);
        addAndMakeVisible(enable);
        level.setTooltip(name + " parallel return level. Double-click to reset.");
        enable.setTooltip("Fade " + name.toLowerCase() + " return in or out");
    }
    void resized() override
    {
        level.setBounds(0, 17, getWidth() - 68, getHeight() - 17);
        enable.setBounds(getWidth() - 64, 58, 62, 37);
    }
    void paint(juce::Graphics &g) override
    {
        voiceui::label(g, title, {0, 0, getWidth() - 68, 20}, 13, voiceui::text);
    }
};
