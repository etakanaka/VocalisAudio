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
    bool drawScale = true;

  public:
    RiderLevelMeter(const std::atomic<float> &value, const std::atomic<float> *peakValue = nullptr,
                    bool showScale = true)
        : rms(value), peak(peakValue), drawScale(showScale)
    {
        setInterceptsMouseClicks(false, false);
    }
    void paint(Graphics &g) override
    {
        if (!drawScale)
        {
            constexpr int segments = 26;
            constexpr float segmentHeight = 12.f, gap = 3.f;
            auto cx = float(getWidth()) * .53f;
            auto bar = Rectangle<float>(cx - 17.f, 10.f, 34.f,
                                        segments * segmentHeight + (segments - 1) * gap);
            g.setColour(Colour(0xff020810));
            g.fillRect(bar.expanded(3, 2));
            g.setGradientFill(ColourGradient(Colour(0xffadb8bf), bar.getTopLeft(),
                                             Colour(0xff36444d), bar.getTopRight(), false));
            g.drawRect(bar.expanded(1, 1), 1.f);
            auto active = jlimit(0, segments,
                                 int(std::round((jlimit(-60.f, 0.f, rms.load()) + 60.f) / 60.f * segments)));
            for (int i = 0; i < segments; ++i)
            {
                Colour led = i >= 24 ? Colour(0xffff501b)
                             : i >= 21 ? Colour(0xffffc51e)
                             : i >= 17 ? Colour(0xffc4ff24)
                                       : Colour(0xff60f522);
                auto y = bar.getBottom() - segmentHeight - float(i) * (segmentHeight + gap);
                auto r = Rectangle<float>(bar.getX() + 3, y, bar.getWidth() - 6, segmentHeight);
                g.setColour(i < active ? led : led.withMultipliedBrightness(.12f).withAlpha(.55f));
                g.fillRect(r);
                if (i < active)
                {
                    g.setColour(Colours::white.withAlpha(.18f));
                    g.drawHorizontalLine(int(y + 1), r.getX() + 1, r.getRight() - 1);
                }
            }
            if (peak)
            {
                auto p = jlimit(0.f, 1.f, (peak->load() + 60.f) / 60.f);
                auto y = bar.getBottom() - p * bar.getHeight();
                g.setColour(Colours::white);
                g.drawLine(bar.getX() - 2, y, bar.getRight() + 2, y, 1.5f);
            }
            return;
        }
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
