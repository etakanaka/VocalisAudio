/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "HarmonicProcessor.h"
#include "Metering.h"
#include "PresenceProcessor.h"
#include "ToneProcessor.h"
#include "VocalDoubler.h"
#include "VocalDynamics.h"
#include "VocalReverb.h"
#include <cstdint>
namespace vocalis
{
struct Settings
{
    float input = 0, intensity = 50, delay = -30, doubler = -11.3f, reverb = -11.3f, output = 0;
    bool delayOn = false, doublerOn = true, reverbOn = true, bypass = false;
};
class VoiceEngine
{
  public:
    ToneProcessor tone;
    VocalDynamics dynamics;
    PresenceProcessor presence;
    VocalDelay delay;
    VocalDoubler doubler;
    VocalReverb reverb;
    Metering metering;
    Smooth input, output, intensity, returns[3], bypass;
    uint32_t tick = 0;
    void prepare(double sr, const Settings &s)
    {
        tone.prepare(sr);
        dynamics.prepare(sr);
        presence.prepare(sr);
        delay.prepare(sr);
        doubler.prepare(sr);
        reverb.prepare(sr);
        metering.prepare(sr);
        input.prepare(sr, 20, dbGain(s.input));
        output.prepare(sr, 20, dbGain(s.output));
        intensity.prepare(sr, 40, s.intensity * .01f);
        returns[0].prepare(sr, 30, s.delayOn ? dbGain(s.delay) : 0);
        returns[1].prepare(sr, 30, s.doublerOn ? dbGain(s.doubler) : 0);
        returns[2].prepare(sr, 30, s.reverbOn ? dbGain(s.reverb) : 0);
        bypass.prepare(sr, 15, s.bypass ? 1 : 0);
        tick = 0;
    }
    void set(const Settings &s)
    {
        input.set(dbGain(s.input));
        output.set(dbGain(s.output));
        intensity.set(s.intensity * .01f);
        returns[0].set(s.delayOn ? dbGain(s.delay) : 0);
        returns[1].set(s.doublerOn ? dbGain(s.doubler) : 0);
        returns[2].set(s.reverbOn ? dbGain(s.reverb) : 0);
        bypass.set(s.bypass ? 1 : 0);
    }
    void process(float *left, float *right, int count, int inputs, int outputs,
                 MeterSnapshot *meters = nullptr)
    {
        for (int n = 0; n < count; ++n)
        {
            float rawL = finite(left[n]),
                  rawR = inputs == 2 ? finite(right[n])
                                     : rawL; // Clamp pathological host data, not normal audio.
            rawL = std::clamp(rawL, -32.0f, 32.0f);
            rawR = std::clamp(rawR, -32.0f, 32.0f);
            float g = input.next(), t = intensity.next();
            if ((tick++ & 31) == 0)
            {
                tone.update(t);
                dynamics.update(t);
                presence.update(t);
            }
            float il = rawL * g, ir = rawR * g, l = tone.process(0, il, t),
                  r = tone.process(1, ir, t);
            float comp = dynamics.gain(l, r);
            l *= comp;
            r *= comp;
            presence.process(l, r);
            l = HarmonicProcessor::process(l, t);
            r = HarmonicProcessor::process(r, t);
            float dl, dr, bl, br, rl, rr;
            delay.process(l, r, dl, dr);
            doubler.process(l, r, bl, br);
            reverb.process(l, r, rl, rr);
            float dg = returns[0].next(), bg = returns[1].next(), rg = returns[2].next();
            l += dg * dl + bg * bl + rg * rl;
            r += dg * dr + bg * br + rg * rr;
            float og = output.next(), bp = bypass.next();
            l = l * og * (1 - bp) + rawL * bp;
            r = r * og * (1 - bp) + rawR * bp;
            if (outputs == 1)
                l = r = .5f * (l + r);
            left[n] = finite(l);
            if (outputs == 2)
                right[n] = finite(r);
            metering.process(il, ir, l, r);
        }
        if (meters)
            metering.publish(*meters, dynamics.gr);
    }
};
} // namespace vocalis
