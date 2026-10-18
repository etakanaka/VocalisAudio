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
class EffectControls : public juce::Component, private juce::Button::Listener
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
        enable.setButtonText("");
        enable.setClickingTogglesState(true);
        enable.setWantsKeyboardFocus(true);
        enable.addListener(this);
        addAndMakeVisible(level);
        addAndMakeVisible(enable);
        level.setTooltip(name + " parallel return level. Double-click to reset.");
        enable.setTooltip("Fade " + name.toLowerCase() + " return in or out");
    }
    ~EffectControls() override { enable.removeListener(this); }
    void resized() override
    {
        level.setBounds(0, 50, 148, 168);
        enable.setBounds(164, 88, 44, 44);
    }
    void paint(juce::Graphics &g) override
    {
        voiceui::label(g, title, {0, 16, 148, 27}, 21, voiceui::text);
        voiceui::label(g, "ON", {156, 48, 69, 25}, 20,
                       enable.getToggleState() ? juce::Colour(0xffdcf6c7) : voiceui::muted);
    }
    void buttonClicked(juce::Button *) override { repaint(); }
    void buttonStateChanged(juce::Button *) override { repaint(); }
};
