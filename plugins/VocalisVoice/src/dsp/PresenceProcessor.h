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
struct PresenceProcessor
{
    Biquad presence[2];
    float low[2]{}, energy = 0, highEnergy = 0, lp = 0, ec = 0, amount = 0;
    double sr = 48000;
    void prepare(double s)
    {
        sr = s;
        lp = std::exp(-2 * pi * 5500 / float(sr));
        ec = std::exp(-1.0f / float(.08 * sr));
        energy = highEnergy = 0;
        for (int c = 0; c < 2; ++c)
        {
            low[c] = 0;
            presence[c].reset();
        }
        update(0);
    }
    void update(float t)
    {
        amount = curve(t);
        for (auto &p : presence)
            p.peak(sr, 3200, .75f, 2.6f * amount);
    }
    void process(float &l, float &r)
    {
        float x[2]{l, r}, bright = 0, power = 0;
        for (int c = 0; c < 2; ++c)
        {
            low[c] = lp * low[c] + (1 - lp) * x[c];
            float h = x[c] - low[c];
            bright += h * h;
            power += x[c] * x[c];
        }
        energy = ec * energy + (1 - ec) * power;
        highEnergy = ec * highEnergy + (1 - ec) * bright;
        float guard = 1 - curve((highEnergy / std::max(energy, 1.e-12f) - .15f) / .35f);
        for (int c = 0; c < 2; ++c)
        {
            float eq = presence[c].process(x[c]);
            x[c] += guard * ((eq - x[c]) + .65f * amount * (x[c] - low[c]));
        }
        l = x[0];
        r = x[1];
    }
};
} // namespace vocalis
