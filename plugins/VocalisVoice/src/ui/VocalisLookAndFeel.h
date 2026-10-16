/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "rider/RiderDesign.h"
namespace voiceui
{
using LookAndFeel = vocalis::design::SuiteLook;
namespace theme = vocalis::design::Suite;
inline const auto navy = theme::background, panel = juce::Colour(0xff1b273d), line = theme::border,
                  text = theme::text, muted = theme::muted, purple = theme::violet,
                  teal = juce::Colour(0xff00dba0), blue = juce::Colour(0xff9bb8ff);
inline void label(juce::Graphics &g, const juce::String &s, juce::Rectangle<int> r, float size,
                  juce::Colour c = text, juce::Justification j = juce::Justification::centred,
                  bool bold = false)
{
    theme::label(g, s, r.toFloat(), size, c, bold, j);
}
inline void plate(juce::Graphics &g, juce::Rectangle<float> r)
{
    theme::panel(g, r);
}
} // namespace voiceui
