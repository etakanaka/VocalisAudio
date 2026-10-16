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
        float top = 28, bottom = float(getHeight() - 60), cx = float(getWidth()) * .5f;
        auto r = Rectangle<float>(cx - 9, top, 18, bottom - top);
        g.setColour(Colour(0xff070d18));
        g.fillRect(r);
        ColourGradient gradient(Colour(0xff00dba0), cx, bottom, Suite::yellow, cx, top, false);
        gradient.addColour(.68, Colour(0xff74ed79));
        g.setGradientFill(gradient);
        g.fillRect(r.withHeight(r.getHeight() * jlimit(0.0f, 1.0f, reduction / 12)));
        g.setColour(Suite::border);
        g.drawRect(r, .75f);
        for (int db = 0; db <= 12; ++db)
        {
            float y = top + float(db) / 12 * r.getHeight();
            g.setColour(Colour(0xff354766));
            g.drawLine(cx - 9, y, cx + 9, y, .6f);
        }
        Suite::label(g, "GR", {0, 0, float(getWidth()), 18}, 12, Suite::muted);
        Suite::readout(g, {1, float(getHeight() - 30), float(getWidth() - 2), 28},
                       String(-reduction, 1), 11);
        Suite::label(g, "dB", {0, float(getHeight() - 53), float(getWidth()), 18}, 10,
                     Suite::muted);
    }
};
