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
struct VocalDynamics
{
    float energy = 0, gr = 0, rmsCoeff = 0, attack = 0, release = 0, threshold = -12, ratio = 1;
    double sr = 48000;
    void prepare(double rate)
    {
        sr = rate;
        energy = gr = 0;
        rmsCoeff = std::exp(-1.0f / float(.01 * sr));
        update(0);
    }
    void update(float t)
    {
        float c = curve(t);
        threshold = -12 - 14 * c;
        ratio = 1 + 3 * c;
        attack = std::exp(-1.0f / float((.024 - .014 * c) * sr));
        release = std::exp(-1.0f / float((.18 - .06 * c) * sr));
    }
    float gain(float l, float r)
    {
        float power = .5f * (l * l + r * r);
        energy = rmsCoeff * energy + (1 - rmsCoeff) * power;
        float over = 10 * std::log10(std::max(energy, 1.0e-18f)) - threshold;
        float knee = 6, desired = 0, slope = 1 - 1 / ratio;
        if (over > knee / 2)
            desired = slope * over;
        else if (over > -knee / 2)
            desired = slope * (over + knee / 2) * (over + knee / 2) / (2 * knee);
        float a = desired > gr ? attack : release;
        gr = a * gr + (1 - a) * desired;
        return dbGain(-gr);
    }
};
} // namespace vocalis
