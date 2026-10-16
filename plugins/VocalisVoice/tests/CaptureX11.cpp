/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <juce_graphics/juce_graphics.h>
juce::Image captureX11(void *handle, int width, int height)
{
    auto *d = XOpenDisplay(nullptr);
    if (!d)
        return {};
    auto *raw = XGetImage(d, static_cast<Window>(reinterpret_cast<uintptr_t>(handle)), 0, 0,
                          unsigned(width), unsigned(height), AllPlanes, ZPixmap);
    if (!raw)
    {
        XCloseDisplay(d);
        return {};
    }
    juce::Image image(juce::Image::RGB, width, height, false);
    juce::Image::BitmapData pixels(image, juce::Image::BitmapData::writeOnly);
    auto component = [](unsigned long v, unsigned long mask)
    {
        if (!mask)
            return juce::uint8(0);
        while ((mask & 1) == 0)
        {
            mask >>= 1;
            v >>= 1;
        }
        return juce::uint8((v & mask) * 255 / mask);
    };
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            auto v = XGetPixel(raw, x, y);
            pixels.setPixelColour(x, y,
                                  juce::Colour(component(v, raw->red_mask),
                                               component(v, raw->green_mask),
                                               component(v, raw->blue_mask)));
        }
    XDestroyImage(raw);
    XCloseDisplay(d);
    return image;
}

bool nativeDisplayAvailable()
{
    auto *d = XOpenDisplay(nullptr);
    if (!d)
        return false;
    XCloseDisplay(d);
    return true;
}
