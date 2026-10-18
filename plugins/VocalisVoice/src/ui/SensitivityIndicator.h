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
class SensitivityIndicator : public juce::Component
{
  public:
    int state = 0;
    void paint(juce::Graphics &g) override
    {
        juce::Colour colours[]{voiceui::muted, juce::Colour(0xff76ff32), vocalis::design::Suite::yellow,
                               juce::Colours::red};
        vocalis::design::Suite::led(g, {18, 18}, state > 0, colours[juce::jlimit(0, 3, state)]);
        voiceui::label(g, "SENSITIVITY", {44, 1, getWidth() - 44, 34}, 17, voiceui::text,
                       juce::Justification::centredLeft);
    }
};
