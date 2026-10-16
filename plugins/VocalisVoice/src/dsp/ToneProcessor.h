/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "Smoothing.h"
namespace vocalis
{
struct ToneProcessor
{
    HighPass hp[2];
    Biquad lowmid[2];
    double sr = 48000;
    void prepare(double rate)
    {
        sr = rate;
        for (int c = 0; c < 2; ++c)
        {
            hp[c].prepare(sr, 65);
            lowmid[c].reset();
        }
        update(0);
    }
    void update(float t)
    {
        for (auto &f : lowmid)
            f.peak(sr, 290, 0.8f, -2.8f * curve(t));
    }
    float process(int c, float x, float t)
    {
        float clean = hp[c].process(x);
        return lowmid[c].process(x + curve(t) * (clean - x));
    }
};
} // namespace vocalis
