/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "VocalDelay.h"
namespace vocalis
{
struct VocalDoubler
{
    DelayLine lines[2];
    float phase[2]{}, slowPhase[2]{}, step[2]{}, sr = 48000, lp[2]{}, coeff = 0;
    void prepare(double rate)
    {
        sr = float(rate);
        coeff = std::exp(-2 * pi * 6500 / sr);
        for (int c = 0; c < 2; ++c)
        {
            lines[c].prepare(int(sr * .06) + 8);
            phase[c] = float(c) * 1.7f;
            slowPhase[c] = float(c);
            step[c] = 2 * pi * (c == 0 ? .19f : .27f) / sr;
            lp[c] = 0;
        }
    }
    void process(float l, float r, float &wl, float &wr)
    {
        float x[2]{.85f * l + .15f * r, .85f * r + .15f * l}, out[2];
        for (int c = 0; c < 2; ++c)
        {
            float ms = (c == 0 ? 18.0f : 26.0f) + 1.3f * std::sin(phase[c]) +
                       .35f * std::sin(slowPhase[c]);
            float d = lines[c].read(sr * ms * .001f);
            lines[c].push(x[c]);
            lp[c] = coeff * lp[c] + (1 - coeff) * d;
            out[c] = lp[c];
            phase[c] += step[c];
            slowPhase[c] += .371f * step[c];
            if (phase[c] > 2 * pi)
                phase[c] -= 2 * pi;
            if (slowPhase[c] > 2 * pi)
                slowPhase[c] -= 2 * pi;
        }
        wl = out[0];
        wr = out[1];
    }
};
} // namespace vocalis
