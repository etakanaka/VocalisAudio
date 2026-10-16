/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
// Rendering from VocalisRider/Source/Editor.cpp; class renamed only, 2026-10-16.
#pragma once
#include "RiderDesign.h"
#include <atomic>
namespace vocalis::design
{
class RiderLevelMeter final : public Component
{
    const std::atomic<float> &rms;
    const std::atomic<float> *peak;

  public:
    RiderLevelMeter(const std::atomic<float> &value, const std::atomic<float> *peakValue = nullptr)
        : rms(value), peak(peakValue)
    {
        setInterceptsMouseClicks(false, false);
    }
    void paint(Graphics &g) override
    {
        auto top = 16.f, bottom = float(getHeight() - 60), cx = float(getWidth()) * .53f;
        auto r = Rectangle<float>(cx - 9, top, 18, bottom - top);
        g.setColour(Colour(0xff070d18));
        g.fillRect(r);
        auto f = jlimit(0.f, 1.f, (rms.load() + 60) / 60);
        ColourGradient gradient(Colour(0xff00dba0), cx, bottom, Suite::yellow, cx, top, false);
        gradient.addColour(.68, Colour(0xff74ed79));
        g.setGradientFill(gradient);
        g.fillRect(r.withTop(bottom - f * r.getHeight()));
        g.setColour(Suite::border);
        g.drawRect(r, .75f);
        for (int db = -60; db <= 0; db += 3)
        {
            auto y = bottom - float(db + 60) / 60 * r.getHeight();
            g.setColour(Colour(0xff354766));
            g.drawLine(cx - 9, y, cx + 9, y, .6f);
        }
        for (int db : {0, -6, -12, -24, -48})
        {
            auto y = bottom - float(db + 60) / 60 * r.getHeight();
            Suite::label(g, String(db), {0, y - 9, cx - 18, 18}, 12, Suite::muted, false,
                         Justification::centredRight);
            g.setColour(Suite::muted);
            g.drawLine(cx - 15, y, cx - 11, y, 1);
        }
        if (peak)
        {
            auto y = bottom - jlimit(0.f, 1.f, (peak->load() + 60) / 60) * r.getHeight();
            g.setColour(Suite::text);
            g.drawLine(cx - 9, y, cx + 9, y, 1.2f);
        }
        Suite::readout(g, {2, float(getHeight() - 30), float(getWidth() - 4), 28},
                       String(rms.load(), 1), 13);
    }
};

} // namespace vocalis::design
