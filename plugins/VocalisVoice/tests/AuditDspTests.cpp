/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "dsp/VoiceEngine.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace vocalis;
void require(bool condition, const char *message)
{
    if (!condition)
        throw std::runtime_error(message);
}
int main()
{
    int count = 0;
    auto test = [&](const char *name, auto fn)
    {
        fn();
        ++count;
        std::cout << "PASS " << name << '\n';
    };
    try
    {
        test("Fractional wrap rounding boundary",
             []
             {
                 DelayLine d;
                 d.prepare(11528);
                 d.pos = 5000;
                 d.data[0] = .5f;
                 d.data.back() = .25f;
                 float y = d.read(5000.00048828125f);
                 require(std::isfinite(y) && std::abs(y - .5f) < .001f,
                         "wrap rounded to one past end");
             });
        test("Compressor detector / attack / release time constants",
             []
             {
                 VocalDynamics d;
                 d.prepare(48000);
                 d.update(1);
                 for (int i = 0; i < 480; ++i)
                     d.gain(.3f, .3f);
                 require(std::abs(d.energy - .09f * (1 - std::exp(-1.0f))) < 1.e-5f, "RMS 10ms");
                 d.energy = .09f;
                 d.gr = 0;
                 float target = (gainDb(.3f) + 26) * .75f;
                 for (int i = 0; i < 480; ++i)
                     d.gain(.3f, .3f);
                 require(std::abs(d.gr - target * (1 - std::exp(-1.0f))) < .003f, "10ms attack");
                 d.energy = 0;
                 d.gr = target;
                 for (int i = 0; i < 5760; ++i)
                     d.gain(0, 0);
                 require(std::abs(d.gr - target / std::exp(1.0f)) < .003f, "120ms release");
             });
        test("Meter RMS, peak decay and clip hold calibration",
             []
             {
                 Metering m;
                 MeterSnapshot snapshot;
                 m.prepare(48000);
                 for (int i = 0; i < 96000; ++i)
                     m.process(.1f, .1f, .2f, .2f);
                 m.publish(snapshot, 3.25f);
                 require(std::abs(snapshot.inputRms[0] - .1f) < .0001f &&
                             std::abs(snapshot.outputRms[0] - .2f) < .0001f,
                         "RMS calibration");
                 require(std::abs(snapshot.inputPeak[0] - .1f) < 1.e-7f && snapshot.gr == 3.25f,
                         "peak/GR calibration");
                 for (int i = 0; i < 21600; ++i)
                     m.process(0, 0, 0, 0);
                 m.publish(snapshot, 0);
                 require(std::abs(snapshot.inputPeak[0] - .1f / std::exp(1.0f)) < .0001f,
                         "450ms peak decay");
                 m.process(0, 0, 1.1f, 0);
                 for (int i = 0; i < 48000; ++i)
                     m.process(0, 0, 0, 0);
                 m.publish(snapshot, 0);
                 require(snapshot.clip, "clip hold too short");
                 for (int i = 0; i < 25000; ++i)
                     m.process(0, 0, 0, 0);
                 m.publish(snapshot, 0);
                 require(!snapshot.clip, "clip latch never clears");
             });
        test("Harmonic late engagement, symmetry and third harmonic",
             []
             {
                 require(HarmonicProcessor::process(.5f, .6f) == .5f,
                         "harmonics before engagement");
                 require(std::abs(HarmonicProcessor::process(.5f, 1) +
                                  HarmonicProcessor::process(-.5f, 1)) < 1.e-7f,
                         "not odd symmetric");
                 double harmonic = 0;
                 for (int i = 0; i < 48000; ++i)
                 {
                     float phase = 2 * pi * 1000 * float(i) / 48000;
                     float x = .7f * std::sin(phase);
                     harmonic += (HarmonicProcessor::process(x, 1) - x) * std::sin(3 * phase);
                 }
                 require(std::abs(harmonic) > 1, "nonlinearity absent");
                 require(std::abs(HarmonicProcessor::process(.0001f, 1) - .0001f) < 1.e-8f,
                         "small signal gain");
             });
        test("Block-size independence and denormal decay",
             []
             {
                 Settings s;
                 s.intensity = 87;
                 s.delayOn = true;
                 VoiceEngine a, b;
                 a.prepare(48000, s);
                 b.prepare(48000, s);
                 std::vector<float> l(96000), r(96000);
                 for (size_t i = 0; i < l.size(); ++i)
                     l[i] = r[i] = i < 24000 ? .2f * std::sin(float(i) * .03f) : 1.e-30f;
                 auto ll = l, rr = r;
                 for (int n = 0; n < 96000; n += 32)
                     a.process(l.data() + n, r.data() + n, std::min(32, 96000 - n), 2, 2);
                 int n = 0, k = 0;
                 while (n < 96000)
                 {
                     int block =
                         std::min(std::array{32, 127, 256, 513, 1024}[size_t(k++ % 5)], 96000 - n);
                     b.process(ll.data() + n, rr.data() + n, block, 2, 2);
                     n += block;
                 }
                 require(l == ll && r == rr, "block partition changed audio");
                 for (float v : l)
                     require(std::isfinite(v), "denormal decay instability");
             });
        test("Syllabic / singing-like / noisy / transient fixtures at rates and variable blocks",
             []
             {
                 for (double sr : {44100, 48000, 88200, 96000, 176400, 192000})
                     for (auto io : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
                     {
                         Settings s;
                         VoiceEngine e;
                         e.prepare(sr, s);
                         std::array<float, 1024> l{}, r{};
                         uint32_t random = 17;
                         int offset = 0;
                         for (int block = 0; block < 160; ++block)
                         {
                             int size = std::array{
                                 32, 64, 128, 256, 512, 1024, 33, 1000}[size_t(block % 8)];
                             s.intensity = float(block % 101);
                             s.input = float(block % 5) * 6 - 12;
                             s.output = -6;
                             s.delayOn = block % 2;
                             s.reverbOn = block % 3;
                             s.doublerOn = block % 4;
                             e.set(s);
                             for (int i = 0; i < size; ++i)
                             {
                                 float t = float(offset + i) / float(sr);
                                 random = random * 1664525u + 1013904223u;
                                 float noise = (float(random >> 8) / 16777216.0f - .5f) * .04f;
                                 float env = std::max(0.0f, std::sin(2 * pi * 4 * t));
                                 float pitch = 120 + 7 * std::sin(2 * pi * 5 * t);
                                 l[size_t(i)] = env * .25f *
                                                    (std::sin(2 * pi * pitch * t) +
                                                     .2f * std::sin(2 * pi * 3 * pitch * t)) +
                                                noise;
                                 r[size_t(i)] = l[size_t(i)] * .8f;
                                 if (block % 17 == 0 && i == 0)
                                     l[size_t(i)] = .98f;
                                 if (block > 140)
                                     l[size_t(i)] = r[size_t(i)] = 0;
                             }
                             e.process(l.data(), io.second == 2 ? r.data() : nullptr, size,
                                       io.first, io.second);
                             for (int i = 0; i < size; ++i)
                                 require(std::isfinite(l[size_t(i)]) && std::abs(l[size_t(i)]) < 8,
                                         "unstable vocal-like fixture");
                             offset += size;
                         }
                     }
             });
        std::cout << count << " audit DSP suites passed\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FAIL " << e.what() << '\n';
        return 1;
    }
}
