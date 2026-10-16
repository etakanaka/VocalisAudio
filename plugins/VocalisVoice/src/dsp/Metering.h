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
#include <atomic>
namespace vocalis
{
static_assert(std::atomic<float>::is_always_lock_free);
struct MeterSnapshot
{
    std::atomic<float> inputPeak[2]{}, inputRms[2]{}, outputPeak[2]{}, outputRms[2]{}, gr{0};
    std::atomic<int> sensitivity{0};
    std::atomic<bool> clip{false};
};
struct Metering
{
    double sr = 48000;
    float rmsIn[2]{}, rmsOut[2]{}, peakIn[2]{}, peakOut[2]{}, ec = 0, pc = 0, sensePower = 0,
                                                              sensePeak = 0;
    int sensitivity = 0;
    float clipHold = 0;
    void prepare(double rate)
    {
        *this = Metering{};
        sr = rate;
        ec = std::exp(-1.0f / float(.15 * sr));
        pc = std::exp(-1.0f / float(.45 * sr));
    }
    void process(float il, float ir, float ol, float ore)
    {
        float in[2]{il, ir}, out[2]{ol, ore};
        for (int c = 0; c < 2; ++c)
        {
            rmsIn[c] = ec * rmsIn[c] + (1 - ec) * in[c] * in[c];
            rmsOut[c] = ec * rmsOut[c] + (1 - ec) * out[c] * out[c];
            peakIn[c] = std::max(std::abs(in[c]), peakIn[c] * pc);
            peakOut[c] = std::max(std::abs(out[c]), peakOut[c] * pc);
        }
        sensePower = ec * sensePower + (1 - ec) * .5f * (il * il + ir * ir);
        sensePeak = std::max(std::max(std::abs(il), std::abs(ir)), sensePeak * pc);
        if (std::abs(ol) >= 1 || std::abs(ore) >= 1)
            clipHold = float(sr) * 1.5f;
        else
            clipHold = std::max(0.0f, clipHold - 1);
    }
    void publish(MeterSnapshot &s, float gr)
    {
        float db = gainDb(std::sqrt(sensePower));
        constexpr float boundaries[3]{-42, -24, -10};
        while (sensitivity < 3 && db > boundaries[sensitivity] + 1)
            sensitivity++;
        while (sensitivity > 0 && db < boundaries[sensitivity - 1] - 1)
            sensitivity--;
        int display = sensePeak > dbGain(-1) ? 3 : sensitivity;
        for (int c = 0; c < 2; ++c)
        {
            s.inputPeak[c].store(peakIn[c], std::memory_order_relaxed);
            s.inputRms[c].store(std::sqrt(rmsIn[c]), std::memory_order_relaxed);
            s.outputPeak[c].store(peakOut[c], std::memory_order_relaxed);
            s.outputRms[c].store(std::sqrt(rmsOut[c]), std::memory_order_relaxed);
        }
        s.gr.store(gr, std::memory_order_relaxed);
        s.sensitivity.store(display, std::memory_order_relaxed);
        s.clip.store(clipHold > 0, std::memory_order_relaxed);
    }
};
} // namespace vocalis
