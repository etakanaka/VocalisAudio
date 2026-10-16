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
#include <array>
namespace vocalis
{
// Eight-line orthogonal feedback delay network, with a 24 ms pre-delay.
struct VocalReverb
{
    DelayLine pre[2], lines[8];
    std::array<float, 8> lengths{}, state{}, feedback{};
    float damping = 0, sr = 48000;
    void prepare(double rate)
    {
        sr = float(rate);
        for (auto &p : pre)
            p.prepare(int(sr * .025) + 8);
        constexpr float times[8]{.0311f, .0367f, .0413f, .0439f, .0479f, .0533f, .0593f, .0671f};
        for (int i = 0; i < 8; ++i)
        {
            lengths[size_t(i)] = std::round(sr * times[i]);
            lines[i].prepare(int(lengths[size_t(i)]) + 2);
            feedback[size_t(i)] = std::pow(10.0f, -3 * times[i] / 1.45f);
            state[size_t(i)] = 0;
        }
        damping = std::exp(-2 * pi * 5200 / sr);
    }
    void process(float l, float r, float &wl, float &wr)
    {
        float pl = pre[0].read(sr * .024f), pr = pre[1].read(sr * .024f);
        pre[0].push(l);
        pre[1].push(r);
        std::array<float, 8> y;
        for (int i = 0; i < 8; ++i)
        {
            float d = lines[i].read(lengths[size_t(i)]);
            state[size_t(i)] = damping * state[size_t(i)] + (1 - damping) * d;
            y[size_t(i)] = state[size_t(i)];
        }
        wl = .18f * (y[0] + y[2] - y[4] + y[6]) + .12f * pl;
        wr = .18f * (y[1] + y[3] - y[5] + y[7]) + .12f * pr;
        // Normalised Walsh-Hadamard transform: orthogonal, energy preserving.
        for (int stride = 1; stride < 8; stride *= 2)
            for (int i = 0; i < 8; i += 2 * stride)
                for (int j = 0; j < stride; ++j)
                {
                    float a = y[size_t(i + j)], b = y[size_t(i + j + stride)];
                    y[size_t(i + j)] = a + b;
                    y[size_t(i + j + stride)] = a - b;
                }
        for (int i = 0; i < 8; ++i)
        {
            float excitation = ((i & 1) ? pr : pl) * ((i & 2) ? -.24f : .24f);
            lines[i].push(excitation + .35355339f * feedback[size_t(i)] * y[size_t(i)]);
        }
    }
};
} // namespace vocalis
