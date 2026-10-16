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
#include <vector>
namespace vocalis
{
struct DelayLine
{
    std::vector<float> data;
    int pos = 0;
    void prepare(int n)
    {
        data.assign(size_t(n), 0);
        pos = 0;
    }
    float read(float delay) const
    {
        float p = float(pos) - delay;
        while (p < 0)
            p += float(data.size());
        // Adding the buffer length to a tiny negative position can round to
        // exactly size() in float32 (e.g. 192 kHz doubler buffers). Wrap that
        // representable endpoint before indexing; ordinary reads are unchanged.
        if (p >= float(data.size()))
            p = 0;
        int a = int(p);
        float f = p - float(a);
        return data[size_t(a)] * (1 - f) + data[(size_t(a) + 1) % data.size()] * f;
    }
    void push(float x)
    {
        data[size_t(pos)] = x;
        if (++pos == int(data.size()))
            pos = 0;
    }
};
struct VocalDelay
{
    DelayLine lines[2];
    float samples[2]{}, lp[2]{}, hpState[2]{}, damp = 0, highpass = 0;
    void prepare(double sr)
    {
        for (int c = 0; c < 2; ++c)
        {
            lines[c].prepare(int(sr * .6) + 8);
            samples[c] = float(sr * (c == 0 ? .285 : .380));
            lp[c] = hpState[c] = 0;
        }
        damp = std::exp(-2 * pi * 4200 / float(sr));
        highpass = std::exp(-2 * pi * 180 / float(sr));
    }
    void process(float l, float r, float &wl, float &wr)
    {
        float x[2]{l, r}, out[2];
        for (int c = 0; c < 2; ++c)
        {
            float d = lines[c].read(samples[c]);
            lp[c] = damp * lp[c] + (1 - damp) * d;
            hpState[c] = highpass * hpState[c] + (1 - highpass) * lp[c];
            out[c] = lp[c] - hpState[c];
            lines[c].push(x[c] + .27f * out[c]);
        }
        wl = out[0];
        wr = out[1];
    }
};
} // namespace vocalis
