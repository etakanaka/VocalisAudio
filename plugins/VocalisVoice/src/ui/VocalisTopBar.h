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
    auto bar = juce::Rectangle<float>(70, 14, width - 140, 110);
    g.setColour(juce::Colour(0xff020812));
    g.fillRect(bar.translated(0, 4));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0c3150), bar.getTopLeft(),
                                           juce::Colour(0xff06172a), bar.getBottomLeft(), false));
    g.fillRect(bar);
    g.setColour(juce::Colour(0xff8197a5));
    g.drawRect(bar, 2.f);
    g.setColour(juce::Colour(0xffc9d4db).withAlpha(.55f));
    g.drawLine(bar.getX() + 3, bar.getY() + 3, bar.getRight() - 3, bar.getY() + 3, 1.3f);
    g.setColour(juce::Colour(0xff010812));
    g.drawLine(bar.getX() + 3, bar.getBottom() - 2, bar.getRight() - 3, bar.getBottom() - 2, 2.f);
    for (int y = 18; y < 120; y += 3)
    {
        g.setColour(juce::Colour(0xff82c8f2).withAlpha(y % 9 == 0 ? .045f : .018f));
        g.drawHorizontalLine(y, 75, width - 75);
    }
    for (int i = 0; i < 5; ++i)
    {
        float heights[]{20, 44, 72, 44, 20};
        auto h = heights[i];
        g.setColour(Suite::violet.withAlpha(.12f));
        g.fillRoundedRectangle(104 + float(i * 16) - 3, 68 - h / 2 - 3, 14, h + 6, 5);
        g.setColour(juce::Colour(0xffbca7fa));
        g.fillRoundedRectangle(104 + float(i * 16), 68 - h / 2, 8, h, 4);
    }
    g.setColour(Suite::text);
    g.setFont(FontOptions(57, Font::plain).withName(Font::getDefaultSerifFontName()));
    g.drawText("VOCALIS", Rectangle<float>(207, 40, 278, 67), Justification::centredLeft);
    g.setColour(juce::Colour(0xffc4d6e2));
    g.drawLine(500, 34, 500, 103, 1.5f);
    Suite::label(g, title, {567, 28, 230, 53}, 41, Suite::text, true,
                 juce::Justification::centredLeft);
    Suite::label(g, subtitle, {567, 80, 260, 25}, 18, Suite::muted, false,
                 juce::Justification::centredLeft);
}
} // namespace vocalis::design
