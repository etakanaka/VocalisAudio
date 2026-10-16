/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
// Branding geometry adapted directly from Rider Editor::paint, 2026-10-16.
#pragma once
#include "VocalisLookAndFeel.h"
namespace vocalis::design
{
inline void topBar(juce::Graphics &g, float width, const juce::String &title,
                   const juce::String &subtitle)
{
    Suite::panel(g, {18, 18, width - 36, 84});
    for (int i = 0; i < 5; ++i)
    {
        float heights[]{10, 25, 45, 25, 10};
        auto h = heights[i];
        g.setColour(Suite::violet.withAlpha(.12f));
        g.fillRoundedRectangle(39 + float(i * 11) - 2, 60 - h / 2 - 2, 10, h + 4, 4);
        g.setColour(juce::Colour(0xffbca7fa));
        g.fillRoundedRectangle(39 + float(i * 11), 60 - h / 2, 6, h, 3);
    }
    Suite::label(g, "VOCALIS", {108, 30, 292, 54}, 51, Suite::text, true,
                 juce::Justification::centredLeft);
    g.setColour(Suite::border.brighter(.3f));
    g.drawLine(417, 33, 417, 88, 1);
    Suite::label(g, title, {448, 31, 300, 34}, 26, juce::Colour(0xff9bb8ff), true,
                 juce::Justification::centredLeft);
    Suite::label(g, subtitle, {450, 69, 350, 17}, 10, Suite::muted, false,
                 juce::Justification::centredLeft);
}
} // namespace vocalis::design
