/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "Parameters.h"
struct VoicePreset
{
    const char *name;
    float intensity;
    bool delayOn;
    float delay;
    bool doublerOn;
    float doubler;
    bool reverbOn;
    float reverb;
};
inline constexpr VoicePreset voicePresets[]{
    {"Default Vocal", 50, false, -30, true, -11.3f, true, -11.3f},
    {"Clean Podcast Voice", 28, false, -30, false, -30, false, -30},
    {"Warm Narration", 34, false, -30, false, -30, true, -24},
    {"Interview Presence", 43, false, -30, false, -30, false, -30},
    {"Natural Lead Vocal", 40, false, -30, true, -19, true, -18},
    {"Modern Pop Vocal", 76, true, -19, true, -15, true, -17},
    {"Smooth Ballad", 56, true, -23, true, -21, true, -14},
    {"Wide Vocal", 60, false, -30, true, -10, true, -19},
    {"Intimate Voice", 23, false, -30, false, -30, true, -25},
    {"Vocal Polish", 48, false, -30, true, -24, true, -23}};
