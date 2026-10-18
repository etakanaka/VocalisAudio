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
#include "dsp/Smoothing.h"
#include "rider/RiderLevelMeter.h"
class LevelMeter : public juce::Component
{
    std::atomic<float> rmsDb[2]{{-120}, {-120}}, peakDb[2]{{-120}, {-120}};
    vocalis::design::RiderLevelMeter left{rmsDb[0], &peakDb[0], false},
        right{rmsDb[1], &peakDb[1], false};

    bool mono = false;

  public:
    bool clipping = false, output = false;
    LevelMeter()
    {
        addAndMakeVisible(left);
        addAndMakeVisible(right);
        setInterceptsMouseClicks(false, false);
    }
    void setLevels(int c, float peak, float rms)
    {
        peakDb[c].store(std::max(-120.0f, vocalis::gainDb(peak)));
        rmsDb[c].store(std::max(-120.0f, vocalis::gainDb(rms)));
    }
    void setMono(bool value)
    {
        if (mono != value)
        {
            mono = value;
            resized();
            repaint();
        }
    }
    bool isMono() const
    {
        return mono;
    }
    void resized() override
    {
        left.setBounds(mono ? 42 : 0, 16, 86, getHeight() - 16);
        right.setBounds(78, 16, 86, getHeight() - 16);
        right.setVisible(!mono);
    }
    void paint(juce::Graphics &g) override
    {
        if (mono)
            voiceui::label(g, output && clipping ? "CLIP" : "MONO", {42, 410, 86, 20}, 14,
                           output && clipping ? juce::Colours::red : voiceui::muted);
        else
        {
            voiceui::label(g, "L", {0, 410, 86, 20}, 14, voiceui::muted);
            voiceui::label(g, "R", {78, 410, 86, 20}, 14, voiceui::muted);
            for (int db : {0, -12, -24, -36, -48, -60})
            {
                float f = float(db + 60) / 60.f;
                float y = 16.f + 10.f + (1.f - f) * 386.f;
                voiceui::label(g, juce::String(db), juce::Rectangle<int>(0, int(y - 10), 25, 20), 15,
                               voiceui::muted, juce::Justification::centredRight);
                g.setColour(voiceui::muted.withAlpha(.75f));
                g.drawLine(27.f, y, 31.f, y, 1.f);
            }
        }
        if (output && clipping)
            voiceui::label(g, "CLIP", {43, 0, 78, 20}, 14, juce::Colours::red);
    }
};
