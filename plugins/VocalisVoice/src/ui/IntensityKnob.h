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
class IntensityKnob : public juce::Slider
{
  public:
    IntensityKnob()
    {
        setSliderStyle(RotaryHorizontalVerticalDrag);
        setTextBoxStyle(TextBoxBelow, false, 120, 34);
        setTextValueSuffix(" %");
        setDoubleClickReturnValue(true, 50);
        setNumDecimalPlacesToDisplay(1);
        setColour(rotarySliderFillColourId, voiceui::purple);
        setWantsKeyboardFocus(true);
        setTooltip("Coordinated vocal tone, dynamics and clarity. Double-click to reset; edit the "
                   "number for precise values.");
    }
};
