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
struct HarmonicProcessor
{
    static float process(float x, float t)
    {
        float amount = .08f * curve((t - .6f) / .4f);
        return x + amount * (std::tanh(1.2f * x) / 1.2f - x);
    }
};
} // namespace vocalis
