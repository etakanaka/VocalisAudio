/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include <algorithm>
#include <cmath>
#include <numbers>
namespace vocalis
{
constexpr float pi = std::numbers::pi_v<float>;
inline float dbGain(float d)
{
    return std::pow(10.0f, d * 0.05f);
}
inline float gainDb(float g)
{
    return 20.0f * std::log10(std::max(g, 1.0e-9f));
}
inline float finite(float v)
{
    return std::isfinite(v) ? v : 0.0f;
}
inline float curve(float v)
{
    v = std::clamp(v, 0.0f, 1.0f);
    return v * v * (3 - 2 * v);
}
struct Smooth
{
    float value = 0, target = 0, step = 0;
    int left = 0, length = 1;
    void prepare(double sr, float ms, float start)
    {
        length = std::max(1, int(sr * ms / 1000));
        value = target = start;
        left = 0;
    }
    void set(float t)
    {
        if (t != target)
        {
            target = t;
            left = length;
            step = (t - value) / float(left);
        }
    }
    float next()
    {
        if (left > 0)
        {
            value += step;
            if (--left == 0)
                value = target;
        }
        return value;
    }
};
struct Biquad
{
    float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
    void reset()
    {
        z1 = z2 = 0;
    }
    float process(float x)
    {
        float y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }
    void peak(double sr, float hz, float q, float db)
    {
        float a = std::pow(10.0f, db / 40), w = 2 * pi * hz / float(sr), c = std::cos(w),
              s = std::sin(w) / (2 * q), n = 1 + s / a;
        b0 = (1 + s * a) / n;
        b1 = -2 * c / n;
        b2 = (1 - s * a) / n;
        a1 = -2 * c / n;
        a2 = (1 - s / a) / n;
    }
};
struct HighPass
{
    float prev = 0, y = 0, a = 0;
    void prepare(double sr, float hz)
    {
        a = std::exp(-2 * pi * hz / float(sr));
        prev = y = 0;
    }
    float process(float x)
    {
        y = a * (y + x - prev);
        prev = x;
        return y;
    }
};
} // namespace vocalis
