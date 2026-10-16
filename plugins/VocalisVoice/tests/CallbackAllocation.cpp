/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "PluginProcessor.h"
#include <cstdlib>
#include <iostream>
#include <new>
static thread_local bool guard = false;
static thread_local size_t newCalls = 0, cCalls = 0;
extern "C" void *__real_malloc(size_t);
extern "C" void *__real_calloc(size_t, size_t);
extern "C" void *__real_realloc(void *, size_t);
extern "C" void *__wrap_malloc(size_t n)
{
    if (guard)
        ++cCalls;
    return __real_malloc(n);
}
extern "C" void *__wrap_calloc(size_t a, size_t b)
{
    if (guard)
        ++cCalls;
    return __real_calloc(a, b);
}
extern "C" void *__wrap_realloc(void *p, size_t n)
{
    if (guard)
        ++cCalls;
    return __real_realloc(p, n);
}
void *operator new(size_t n)
{
    if (guard)
        ++newCalls;
    if (void *p = __real_malloc(n ? n : 1))
        return p;
    throw std::bad_alloc();
}
void *operator new[](size_t n)
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
void operator delete(void *p, size_t) noexcept
{
    std::free(p);
}
void operator delete[](void *p, size_t) noexcept
{
    std::free(p);
}
void *operator new(size_t n, std::align_val_t align)
{
    if (guard)
        ++newCalls;
    void *p = nullptr;
    if (posix_memalign(&p, size_t(align), n ? n : 1) != 0)
        throw std::bad_alloc();
    return p;
}
void *operator new[](size_t n, std::align_val_t align)
{
    return ::operator new(n, align);
}
void operator delete(void *p, std::align_val_t) noexcept
{
    std::free(p);
}
void operator delete[](void *p, std::align_val_t) noexcept
{
    std::free(p);
}
void operator delete(void *p, size_t, std::align_val_t) noexcept
{
    std::free(p);
}
void operator delete[](void *p, size_t, std::align_val_t) noexcept
{
    std::free(p);
}
int main()
{
    int calls = 0;
    for (double sr : {44100, 48000, 88200, 96000, 176400, 192000})
        for (auto io : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
        {
            VoiceProcessor p;
            auto l = p.getBusesLayout();
            l.inputBuses.set(0, io.first == 1 ? juce::AudioChannelSet::mono()
                                              : juce::AudioChannelSet::stereo());
            l.outputBuses.set(0, io.second == 1 ? juce::AudioChannelSet::mono()
                                                : juce::AudioChannelSet::stereo());
            if (!p.setBusesLayout(l))
                return 2;
            p.prepareToPlay(sr, 1024);
            juce::AudioBuffer<float> b(io.second, 1024);
            juce::MidiBuffer m;
            for (int k = 0; k < 100; ++k)
            {
                int size = std::array{32, 64, 128, 256, 512, 1024, 33, 513}[size_t(k % 8)];
                b.setSize(io.second, size, false, false, true);
                for (int c = 0; c < io.second; ++c)
                    for (int n = 0; n < size; ++n)
                        b.setSample(c, n, .1f * std::sin(float(n + k) * .07f));
                auto *intensity = p.state.getParameter(voiceparams::ids[1]);
                intensity->setValueNotifyingHost(float(k % 101) * .01f);
                guard = true;
                if (k % 5 == 0)
                    p.processBlockBypassed(b, m);
                else
                    p.processBlock(b, m);
                guard = false;
                ++calls;
            }
        }
    std::cout << calls << " actual processor callbacks: new/new[]/aligned=" << newCalls
              << ", wrapped malloc/calloc/realloc=" << cCalls << '\n';
    return newCalls || cCalls ? 1 : 0;
}
