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
    vocalis::design::RiderLevelMeter left{rmsDb[0], &peakDb[0]}, right{rmsDb[1], &peakDb[1]};

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
        left.setBounds(mono ? 50 : 0, 16, 94, getHeight() - 16);
        right.setBounds(100, 16, 94, getHeight() - 16);
        right.setVisible(!mono);
    }
    void paint(juce::Graphics &g) override
    {
        if (mono)
            voiceui::label(g, output && clipping ? "CLIP" : "MONO", {50, 0, 94, 16}, 10,
                           output && clipping ? juce::Colours::red : voiceui::muted);
        else
        {
            voiceui::label(g, "L", {0, 0, 94, 16}, 10, voiceui::muted);
            voiceui::label(g, "R", {100, 0, 94, 16}, 10, voiceui::muted);
        }
        if (!mono && output && clipping)
            voiceui::label(g, "CLIP", {55, 0, 84, 16}, 11, juce::Colours::red);
    }
};
