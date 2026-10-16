/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "dsp/VoiceEngine.h"
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <new>
#include <random>
static thread_local bool audioGuard = false;
static thread_local size_t allocations = 0;
void *operator new(std::size_t n)
{
    if (audioGuard)
        ++allocations;
    if (auto *p = std::malloc(n ? n : 1))
        return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n)
{
    return ::operator new(n);
}
void operator delete(void *p) noexcept
{
    std::free(p);
}
void operator delete[](void *p) noexcept
{
    std::free(p);
}
void operator delete(void *p, std::size_t) noexcept
{
    std::free(p);
}
void operator delete[](void *p, std::size_t) noexcept
{
    std::free(p);
}
using namespace vocalis;
void require(bool b, const char *s)
{
    if (!b)
        throw std::runtime_error(s);
}
struct Audio
{
    std::vector<float> l, r;
    Audio(int n) : l(size_t(n)), r(size_t(n)) {}
};
Settings dry(float t = 0)
{
    Settings s;
    s.intensity = t;
    s.delayOn = s.doublerOn = s.reverbOn = false;
    return s;
}
Audio signal(double sr, float seconds, float hz, float amplitude)
{
    Audio a(int(sr * seconds));
    for (size_t i = 0; i < a.l.size(); ++i)
        a.l[i] = a.r[i] = amplitude * std::sin(2 * pi * hz * float(i) / float(sr));
    return a;
}
void run(VoiceEngine &e, Audio &a, int inputs = 2, int outputs = 2, int block = 256)
{
    for (int n = 0; n < int(a.l.size()); n += block)
    {
        audioGuard = true;
        e.process(a.l.data() + n, outputs == 2 ? a.r.data() + n : nullptr,
                  std::min(block, int(a.l.size()) - n), inputs, outputs);
        audioGuard = false;
    }
}
double energy(const std::vector<float> &a, size_t start = 0)
{
    double v = 0;
    for (size_t i = start; i < a.size(); ++i)
        v += double(a[i]) * a[i];
    return v;
}
double difference(const Audio &a, const Audio &b)
{
    double v = 0;
    for (size_t i = 0; i < a.l.size(); ++i)
    {
        double d = a.l[i] - b.l[i];
        v += d * d;
    }
    return v;
}
int main(int argc, char **argv)
{
    int passed = 0, failed = 0;
    auto test = [&](const char *n, auto fn)
    {
        try
        {
            fn();
            ++passed;
            std::cout << "PASS " << n << '\n';
        }
        catch (const std::exception &e)
        {
            audioGuard = false;
            ++failed;
            std::cerr << "FAIL " << n << ": " << e.what() << '\n';
        }
    };
    test("Silence / all paths",
         []
         {
             VoiceEngine e;
             e.prepare(48000, {});
             Audio a(96000);
             run(e, a);
             require(energy(a.l) == 0, "nonzero silence");
         });
    test("Low-level input",
         []
         {
             VoiceEngine e;
             e.prepare(48000, dry(100));
             auto a = signal(48000, 1, 220, .000001f);
             run(e, a);
             require(energy(a.l) > 0 && e.dynamics.gr < .01, "quiet compression");
         });
    test("High-level / no uncontrolled boost",
         []
         {
             VoiceEngine e;
             e.prepare(48000, dry(100));
             auto a = signal(48000, 2, 220, .95f);
             run(e, a);
             require(e.dynamics.gr > 10, "no compression");
             for (float x : a.l)
                 require(std::isfinite(x) && std::abs(x) < 1.1f, "unstable output");
         });
    for (bool input : {true, false})
        test(input ? "Input gain accuracy" : "Output gain accuracy",
             [=]
             {
                 auto s = dry();
                 (input ? s.input : s.output) = 6;
                 VoiceEngine e;
                 e.prepare(48000, s);
                 auto a = signal(48000, 1, 1000, .05f), b = a;
                 run(e, a);
                 double ratio = std::sqrt(energy(a.l) / energy(b.l));
                 require(std::abs(ratio - dbGain(6)) < 1.e-5, "trim mismatch");
             });
    for (float t : {0, 25, 50, 75, 100})
        test(("Intensity " + std::to_string(int(t))).c_str(),
             [t]
             {
                 VoiceEngine e;
                 e.prepare(48000, dry(t));
                 auto a = signal(48000, 2, 220, .3f), b = a;
                 run(e, a);
                 if (t == 0)
                     require(difference(a, b) < 1.e-12, "zero not transparent");
                 else
                     require(difference(a, b) > .01, "macro no effect");
                 for (float v : a.l)
                     require(std::isfinite(v), "nonfinite");
             });
    test("Compressor static curve / soft knee / stereo linking",
         []
         {
             VocalDynamics d;
             d.prepare(48000);
             d.update(1);
             float gain = 0;
             for (int i = 0; i < 96000; ++i)
                 gain = d.gain(.3f, .3f);
             float expected = (gainDb(.3f) + 26) * .75f;
             require(std::abs(d.gr - expected) < .03, "wrong static curve");
             require(std::abs(gain - dbGain(-expected)) < .001, "wrong gain");
             VocalDynamics linked;
             linked.prepare(48000);
             linked.update(1);
             for (int i = 0; i < 96000; ++i)
                 linked.gain(.3f, 0);
             require(linked.gr < d.gr && linked.gr > 0, "stereo detector");
             for (int i = 0; i < 192000; ++i)
                 d.gain(0, 0);
             require(d.gr < .001, "silence pumping");
         });
    test("Gain reduction meter equals actual dynamics",
         []
         {
             VoiceEngine e;
             e.prepare(48000, dry(75));
             auto a = signal(48000, 2, 180, .4f);
             run(e, a);
             MeterSnapshot m;
             e.process(a.l.data(), a.r.data(), 0, 2, 2, &m);
             require(std::abs(m.gr.load() - e.dynamics.gr) < 1.e-6 && m.gr.load() > 3,
                     "meter mismatch");
         });
    test("Low-frequency cleanup",
         []
         {
             ToneProcessor t;
             t.prepare(48000);
             t.update(1);
             double lo = 0, hi = 0;
             for (int i = 0; i < 96000; ++i)
             {
                 float x = .1f * std::sin(2 * pi * 25 * float(i) / 48000);
                 float y = t.process(0, x, 1);
                 if (i > 48000)
                 {
                     lo += x * x;
                     hi += y * y;
                 }
             }
             require(hi / lo < .2, "no rumble attenuation");
         });
    test("Presence enhancement / bright guard",
         []
         {
             PresenceProcessor p;
             p.prepare(48000);
             p.update(1);
             double in = 0, out = 0;
             for (int i = 0; i < 96000; ++i)
             {
                 float x = .01f * std::sin(2 * pi * 3200 * float(i) / 48000), l = x, r = x;
                 p.process(l, r);
                 if (i > 48000)
                 {
                     in += x * x;
                     out += l * l;
                 }
             }
             require(out / in > 1.1 && out / in < 3, "presence excessive or absent");
         });
    test("Delay timing and feedback",
         []
         {
             VocalDelay d;
             d.prepare(48000);
             int firstL = -1, firstR = -1;
             double repeat = 0;
             for (int i = 0; i < 48000; ++i)
             {
                 float l, r;
                 d.process(i == 0 ? 1 : 0, i == 0 ? 1 : 0, l, r);
                 if (std::abs(l) > 1.e-5 && firstL < 0)
                     firstL = i;
                 if (std::abs(r) > 1.e-5 && firstR < 0)
                     firstR = i;
                 if (i > 27000 && i < 28000)
                     repeat += l * l;
             }
             require(firstL == 13680 && firstR == 18240, "wrong delay time");
             require(repeat > 1.e-6, "no feedback");
         });
    for (int effect = 0; effect < 3; ++effect)
        test((std::string("Parallel activation / isolation ") + std::to_string(effect)).c_str(),
             [effect]
             {
                 Settings s = dry();
                 VoiceEngine e0, e1;
                 e0.prepare(48000, s);
                 if (effect == 0)
                 {
                     s.delayOn = true;
                     s.delay = -6;
                 }
                 if (effect == 1)
                 {
                     s.doublerOn = true;
                     s.doubler = -6;
                 }
                 if (effect == 2)
                 {
                     s.reverbOn = true;
                     s.reverb = -6;
                 }
                 e1.prepare(48000, s);
                 auto a = signal(48000, 2, 317, .1f), b = a;
                 run(e0, a);
                 run(e1, b);
                 require(difference(a, b) > .001, "effect absent");
                 s.delayOn = s.doublerOn = s.reverbOn = false;
                 e1.set(s);
                 Audio silence(192000);
                 run(e1, silence);
                 require(energy(silence.l, 96000) == 0, "disabled wet contribution");
             });
    test("Doubler stereo decorrelation / mono compatibility",
         []
         {
             VocalDoubler d;
             d.prepare(48000);
             double ll = 0, rr = 0, lr = 0, mono = 0;
             std::mt19937 rng(7);
             std::uniform_real_distribution<float> random(-.1f, .1f);
             for (int i = 0; i < 96000; ++i)
             {
                 float x = random(rng), l, r;
                 d.process(x, x, l, r);
                 if (i > 4800)
                 {
                     ll += l * l;
                     rr += r * r;
                     lr += l * r;
                     mono += (l + r) * (l + r) * .25;
                 }
             }
             require(std::abs(lr / std::sqrt(ll * rr)) < .3, "correlated doubler");
             require(mono > .2 * (ll + rr), "mono cancellation");
         });
    test("Reverb pre-delay / decaying diffuse tail",
         []
         {
             VocalReverb r;
             r.prepare(48000);
             double early = 0, late = 0, veryLate = 0;
             int first = -1;
             for (int i = 0; i < 240000; ++i)
             {
                 float l, rr;
                 r.process(i == 0 ? 1 : 0, 0, l, rr);
                 if (std::abs(l) > 1.e-7 && first < 0)
                     first = i;
                 if (i > 2400 && i < 24000)
                     early += l * l + rr * rr;
                 if (i > 48000 && i < 96000)
                     late += l * l + rr * rr;
                 if (i > 192000)
                     veryLate += l * l + rr * rr;
                 require(std::isfinite(l) && std::abs(l) < 1, "reverb unstable");
             }
             require(first == 1152, "predelay");
             require(early > late && late > 0 && veryLate < late * .001, "tail not decaying");
         });
    test("Smooth effect / bypass transitions",
         []
         {
             VoiceEngine e;
             auto s = dry();
             e.prepare(48000, s);
             Audio a(6000);
             for (size_t i = 0; i < a.l.size(); ++i)
                 a.l[i] = a.r[i] = .1f;
             run(e, a);
             s.doublerOn = s.reverbOn = s.delayOn = true;
             s.doubler = s.reverb = s.delay = 0;
             e.set(s);
             Audio b(6000);
             for (size_t i = 0; i < b.l.size(); ++i)
                 b.l[i] = b.r[i] = .1f;
             run(e, b);
             float delta = 0;
             for (size_t i = 1; i < b.l.size(); ++i)
                 delta = std::max(delta, std::abs(b.l[i] - b.l[i - 1]));
             require(delta < .02, "abrupt return");
             s.bypass = true;
             e.set(s);
             for (size_t i = 0; i < b.l.size(); ++i)
                 b.l[i] = b.r[i] = .1f;
             run(e, b);
             require(std::abs(b.l.back() - .1f) < .001, "bypass not unity");
         });
    for (auto layout : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
        test(("Layout " + std::to_string(layout.first) + " to " + std::to_string(layout.second))
                 .c_str(),
             [=]
             {
                 VoiceEngine e;
                 e.prepare(48000, dry());
                 auto a = signal(48000, .2f, 1000, .1f), b = a;
                 run(e, a, layout.first, layout.second);
                 require(difference(a, b) < 1.e-12, "dry level changed");
                 if (layout.second == 2)
                     require(energy(a.l) == energy(a.r), "mono expansion imbalance");
             });
    test("Rapid automation / rate / block matrix / finite guard",
         []
         {
             for (double sr : {22050, 44100, 48000, 88200, 96000, 192000})
                 for (int block : {1, 17, 64, 256, 1024, 4096})
                 {
                     auto s = dry();
                     VoiceEngine e;
                     e.prepare(sr, s);
                     Audio a(block);
                     for (int b = 0; b < 12; ++b)
                     {
                         s.intensity = float((b * 37) % 101);
                         s.input = float(b % 3) * 12 - 12;
                         s.output = -6;
                         s.delayOn = b % 2;
                         s.doublerOn = b % 3;
                         s.reverbOn = b % 2;
                         e.set(s);
                         for (int n = 0; n < block; ++n)
                             a.l[size_t(n)] = a.r[size_t(n)] =
                                 .2f * std::sin(float(n + b * block) * .04f);
                         if (b == 3)
                             a.l[0] = std::numeric_limits<float>::quiet_NaN();
                         run(e, a, 2, 2, block);
                         for (float v : a.l)
                             require(std::isfinite(v) && std::abs(v) < 8, "matrix instability");
                     }
                 }
         });
    test("Repeated prepare / instantiation",
         []
         {
             for (int i = 0; i < 30; ++i)
             {
                 VoiceEngine e;
                 e.prepare(44100, {});
                 e.prepare(96000, {});
                 Audio a(1024);
                 run(e, a);
                 require(energy(a.l) == 0, "stale buffer");
             }
         });
    test("Audio-thread allocation guard",
         [] { require(allocations == 0, "allocation in process"); });
    test("Sensitivity hysteresis / clipping hold",
         []
         {
             Metering m;
             MeterSnapshot s;
             m.prepare(48000);
             for (float amp : {0.0f, .02f, .12f, .5f})
             {
                 for (int i = 0; i < 96000; ++i)
                     m.process(amp, amp, amp, amp);
                 m.publish(s, 0);
                 int expected = amp == 0 ? 0 : amp == .02f ? 1 : amp == .12f ? 2 : 3;
                 require(s.sensitivity == expected, "sensitivity threshold");
             }
             m.process(0, 0, 1.1f, 0);
             m.publish(s, 0);
             require(s.clip, "missing clipping indicator");
         });
    if (argc > 1 && std::string(argv[1]) == "--benchmark")
    {
        for (int block : {64, 256, 1024})
        {
            VoiceEngine e;
            Settings s;
            s.intensity = 100;
            s.delayOn = true;
            e.prepare(48000, s);
            auto a = signal(48000, 10, 220, .2f);
            auto start = std::chrono::steady_clock::now();
            run(e, a, 2, 2, block);
            double sec =
                std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
            std::cout << "BENCH all effects 48k stereo block=" << block << " 10s audio / " << sec
                      << "s CPU wall (" << sec * 10 << "% realtime)\n";
        }
    }
    std::cout << passed << " passed, " << failed << " failed; allocations in audio=" << allocations
              << '\n';
    return failed ? 1 : 0;
}
