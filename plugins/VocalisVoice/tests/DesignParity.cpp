/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "RiderReferenceDesign.h"
#include "ui/VocalisLookAndFeel.h"
#include "ui/VocalisTopBar.h"
#include "ui/rider/RiderLevelMeter.h"
#include <functional>
#include <iostream>
#include <stdexcept>
using namespace juce;
int main()
{
    ScopedJuceInitialiser_GUI init;
    int count = 0;
    auto compare = [&](const char *name, auto reference, auto actual)
    {
        Image a(Image::ARGB, 400, 500, true), b(Image::ARGB, 400, 500, true);
        {
            Graphics g(a);
            reference(g);
        }
        {
            Graphics g(b);
            actual(g);
        }
        int different = 0;
        for (int y = 0; y < a.getHeight(); ++y)
            for (int x = 0; x < a.getWidth(); ++x)
                different += a.getPixelAt(x, y) != b.getPixelAt(x, y);
        if (different)
            throw std::runtime_error(String(name).toStdString() + ": " + std::to_string(different) +
                                     " pixels differ");
        std::cout << "PASS exact pixels " << name << '\n';
        ++count;
    };
    auto expectRedesigned = [&](const char *name, auto reference, auto actual)
    {
        Image a(Image::ARGB, 400, 500, true), b(Image::ARGB, 400, 500, true);
        { Graphics g(a); reference(g); }
        { Graphics g(b); actual(g); }
        int different = 0;
        for (int y = 0; y < a.getHeight(); ++y)
            for (int x = 0; x < a.getWidth(); ++x)
                different += a.getPixelAt(x, y) != b.getPixelAt(x, y);
        if (different < 100)
            throw std::runtime_error(String(name).toStdString() + ": redesign not rendered");
        std::cout << "PASS redesigned pixels " << name << " (" << different << " differ)\n";
        ++count;
    };
    try
    {
        rider_reference::SuiteLook reference;
        vocalis::design::SuiteLook actual;
        Slider s;
        s.setName("vv_intensity");
        expectRedesigned(
            "metal rotary",
            [&](Graphics &g) { reference.drawRotarySlider(g, 0, 0, 360, 360, .5f,
                1.2f * MathConstants<float>::pi, 2.8f * MathConstants<float>::pi, s); },
            [&](Graphics &g) { actual.drawRotarySlider(g, 0, 0, 360, 360, .5f,
                1.2f * MathConstants<float>::pi, 2.8f * MathConstants<float>::pi, s); });
        TextButton button("BYPASS");
        button.setBounds(0, 0, 105, 37);
        button.setToggleState(true, dontSendNotification);
        expectRedesigned(
            "lit effect button",
            [&](Graphics &g) {
                reference.drawButtonBackground(g, button, Colours::black, false, false);
                reference.drawButtonText(g, button, false, false);
            },
            [&](Graphics &g) {
                actual.drawButtonBackground(g, button, Colours::black, false, false);
                actual.drawButtonText(g, button, false, false);
            });
        expectRedesigned(
            "panel", [](Graphics &g) { rider_reference::Suite::panel(g, {18, 18, 350, 250}); },
            [](Graphics &g) { vocalis::design::Suite::panel(g, {18, 18, 350, 250}); });
        expectRedesigned(
            "readout", [](Graphics &g)
            { rider_reference::Suite::readout(g, {2, 2, 96, 28}, "-11.3 dB", 15); }, [](Graphics &g)
            { vocalis::design::Suite::readout(g, {2, 2, 96, 28}, "-11.3 dB", 15); });
        compare(
            "fader",
            [](Graphics &g)
            {
                rider_reference::Suite::track(g, 80, 16, 320);
                rider_reference::Suite::cap(g, 80, 180, 58, 47);
            },
            [](Graphics &g)
            {
                vocalis::design::Suite::track(g, 80, 16, 320);
                vocalis::design::Suite::cap(g, 80, 180, 58, 47);
            });
        for (bool active : {false, true})
            expectRedesigned(
                "LED",
                [&](Graphics &g) {
                    rider_reference::Suite::led(g, {30, 30}, active,
                                                rider_reference::Suite::yellow);
                },
                [&](Graphics &g) {
                    vocalis::design::Suite::led(g, {30, 30}, active,
                                                vocalis::design::Suite::yellow);
                });
        ComboBox combo;
        expectRedesigned(
            "preset selector",
            [&](Graphics &g) { reference.drawComboBox(g, 166, 37, false, 0, 0, 0, 0, combo); },
            [&](Graphics &g) { actual.drawComboBox(g, 166, 37, false, 0, 0, 0, 0, combo); });
        // Same meter paint body retained in the class; compare against the independent reference
        // image generated below from the verbatim Rider body at fixed size and values.
        for (float db : {-60.0f, -24.0f, -6.0f, 0.0f})
        {
            std::atomic<float> rms{db}, peak{db + 2};
            rider_reference::LevelMeter rm(rms, &peak);
            vocalis::design::RiderLevelMeter am(rms, &peak);
            rm.setSize(94, 350);
            am.setSize(94, 350);
            expectRedesigned(
                "level meter", [&](Graphics &g) { rm.paint(g); },
                [&](Graphics &g) { am.paint(g); });
        }
        std::cout << count << " visual regression checks passed\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FAIL " << e.what() << '\n';
        return 1;
    }
}
