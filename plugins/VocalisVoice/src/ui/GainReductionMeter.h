/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
// Rider LevelMeter track, gradient, segmentation and readout; range/direction
// adapted for Voice's real 0..12 dB attenuation, 2026-10-16.
#pragma once
#include "VocalisLookAndFeel.h"
class GainReductionMeter : public juce::Component
{
  public:
    float reduction = 0;
    void paint(juce::Graphics &g) override
    {
        using namespace juce;
        namespace Suite = vocalis::design::Suite;
        constexpr int segments = 40;
        constexpr float segmentHeight = 9.f, gap = 3.f;
        float top = 54, bottom = float(getHeight() - 66), cx = float(getWidth()) * .62f;
        auto r = Rectangle<float>(cx - 12, top,
                                  24, segments * segmentHeight + (segments - 1) * gap);
        g.setColour(Colour(0xff020811));
        g.fillRect(r.expanded(3, 2));
        g.setGradientFill(ColourGradient(Colour(0xffb8c7d0), r.getTopLeft(),
                                         Colour(0xff394b55), r.getTopRight(), false));
        g.drawRect(r.expanded(1, 1), 1.f);
        int active = jlimit(0, segments, int(std::round(jlimit(0.f, 12.f, reduction) / 12.f * segments)));
        for (int i = 0; i < segments; ++i)
        {
            Colour led = i >= 36 ? Colour(0xffff3d22)
                         : i >= 32 ? Colour(0xffff9822)
                         : i >= 27 ? Colour(0xffffdc20)
                                   : Colour(0xff39ed31);
            float y = r.getBottom() - segmentHeight - float(i) * (segmentHeight + gap);
            auto segment = Rectangle<float>(r.getX() + 3, y, r.getWidth() - 6, segmentHeight);
            g.setColour(i < active ? led : led.withMultipliedBrightness(.10f).withAlpha(.55f));
            g.fillRect(segment);
        }
        for (int i = 0; i <= 4; ++i)
        {
            float y = r.getY() + float(i) * r.getHeight() / 4.f;
            Suite::label(g, i == 0 ? "0" : "-" + String(i * 3),
                         {0, y - 9, cx - 17, 18}, 15, Suite::muted, false,
                         Justification::centredRight);
            g.setColour(Suite::muted);
            g.drawLine(cx - 15, y, cx - 12, y, .8f);
        }
        Suite::label(g, "GR", {0, 1, float(getWidth()), 24}, 21, Suite::text);
        Suite::label(g, "dB", {0, float(getHeight() - 56), float(getWidth()), 18}, 15,
                     Suite::muted);
        Suite::readout(g, {1, float(getHeight() - 33), float(getWidth() - 2), 31},
                       String(-reduction, 1), 16);
    }
};
