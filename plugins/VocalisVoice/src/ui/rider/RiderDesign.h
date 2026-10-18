/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
// Adapted 2026-10-16 from VocalisRider/Source/Editor.cpp. Rendering retained;
// free functions made inline and names enclosed for independent reuse.
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
namespace vocalis::design
{
using namespace juce;
namespace Suite
{
const Colour background{0xff071424}, border{0xff50799a}, text{0xffedf3f7}, muted{0xffafc3d4};
const Colour violet{0xffa378ff}, purple{0xff7130de}, yellow{0xffffd24b};
inline void label(Graphics &g, const String &s, Rectangle<float> r, float size,
                  Colour colour = text, bool bold = false,
                  Justification align = Justification::centred)
{
    g.setColour(colour);
    g.setFont(FontOptions(size, bold ? Font::bold : Font::plain));
    g.drawText(s, r, align);
}
inline void panel(Graphics &g, Rectangle<float> r, float radius = 9.f)
{
    g.setColour(Colours::black.withAlpha(.35f));
    g.fillRoundedRectangle(r.translated(0, 3), radius);
    g.setGradientFill(ColourGradient(Colour(0xff10304b), r.getTopRight(), Colour(0xff0a2035),
                                     r.getBottomLeft(), false));
    g.fillRoundedRectangle(r, radius);
    g.setColour(border);
    g.drawRoundedRectangle(r, radius, .85f);
    g.setColour(Colours::white.withAlpha(.10f));
    g.drawRoundedRectangle(r.reduced(1), radius - 1, .7f);
}
inline void readout(Graphics &g, Rectangle<float> r, const String &value, float size = 16)
{
    g.setColour(Colour(0xff090f1b));
    g.fillRoundedRectangle(r, 5);
    g.setColour(border);
    g.drawRoundedRectangle(r, 5, .9f);
    label(g, value, r, size);
}
inline void glowLine(Graphics &g, Line<float> line, float width = 3.f)
{
    for (int i = 3; i > 0; --i)
    {
        g.setColour(violet.withAlpha(.06f * float(4 - i)));
        g.drawLine(line, width + float(i * 4));
    }
    g.setColour(violet);
    g.drawLine(line, width + 2);
    g.setColour(Colour(0xfff5eaff));
    g.drawLine(line, width);
}
inline void cap(Graphics &g, float x, float y, float width, float height)
{
    auto r = Rectangle<float>(x - width / 2, y - height * .65f, width, height);
    g.setColour(Colours::black.withAlpha(.6f));
    g.fillRoundedRectangle(r.translated(2, 5), 5);
    g.setGradientFill(ColourGradient(Colour(0xff9da6c2), r.getTopLeft(), Colour(0xff111727),
                                     r.getBottomLeft(), false));
    g.fillRoundedRectangle(r, 5);
    g.setColour(Colour(0xff070c17));
    g.drawRoundedRectangle(r, 5, 1.8f);
    g.setColour(Colour(0xffbfcbe9).withAlpha(.65f));
    g.drawHorizontalLine(int(r.getY() + 4), r.getX() + 5, r.getRight() - 5);
    for (int i = 1; i <= 3; ++i)
    {
        auto yy = r.getY() + height * float(i) / 5;
        g.setColour(Colour(0xff090e1c));
        g.drawLine(r.getX() + 4, yy, r.getRight() - 4, yy, 1.6f);
        g.setColour(Colour(0xff8995b4).withAlpha(.35f));
        g.drawLine(r.getX() + 4, yy + 2, r.getRight() - 4, yy + 2, .7f);
    }
    glowLine(g, {{r.getX() + 8, y}, {r.getRight() - 8, y}}, 2.2f);
}
inline void track(Graphics &g, float x, float top, float bottom)
{
    auto r = Rectangle<float>(x - 10, top - 10, 20, bottom - top + 20);
    g.setColour(Colour(0xff030610));
    g.fillRoundedRectangle(r, 9);
    g.setColour(Colour(0xff7285b6));
    g.drawRoundedRectangle(r, 9, .9f);
    g.setColour(purple.withAlpha(.35f));
    g.fillRoundedRectangle(x - 4, top, 8, bottom - top, 4);
    g.setGradientFill(ColourGradient(violet, x, top, purple, x, bottom, false));
    g.fillRoundedRectangle(x - 2, top, 4, bottom - top, 2);
}
inline void ticks(Graphics &g, float x, float top, float bottom, bool numbers = true)
{
    for (int db = -12; db <= 12; ++db)
    {
        auto y = top + (12 - float(db)) / 24 * (bottom - top);
        bool major = db % 6 == 0;
        g.setColour(Colour(major ? 0xff8195c1 : 0xff394e73));
        g.drawLine(x, y, x + (major ? 17.f : 5.f), y, major ? 1.4f : .8f);
        if (major && numbers)
            label(g, (db > 0 ? "+" : "") + String(db), {x + 23, y - 10, 38, 20}, 13, muted, false,
                  Justification::centredLeft);
    }
}
inline void led(Graphics &g, Point<float> c, bool active, Colour colour)
{
    if (active)
        for (int i = 5; i > 0; --i)
        {
            g.setColour(colour.withAlpha(.035f * float(6 - i)));
            g.fillEllipse(c.x - 10 - float(i * 2), c.y - 10 - float(i * 2), 20 + float(i * 4),
                          20 + float(i * 4));
        }
    g.setColour(Colour(0xff040914));
    g.fillEllipse(c.x - 16, c.y - 16, 32, 32);
    g.setColour(border);
    g.drawEllipse(c.x - 16, c.y - 16, 32, 32, 1);
    auto r = Rectangle<float>(c.x - 11, c.y - 11, 22, 22);
    g.setGradientFill(ColourGradient(active ? colour.brighter(.6f) : Colour(0xff71809d),
                                     r.getTopLeft(), active ? colour : Colour(0xff111a2a),
                                     r.getBottomRight(), false));
    g.fillEllipse(r);
}
} // namespace Suite

class SuiteLook final : public LookAndFeel_V4
{
  public:
    float inputDb = -120;
    explicit SuiteLook(bool usePresetFont = false) : presetFont(usePresetFont)
    {
        setColour(Slider::textBoxTextColourId, Suite::text);
        setColour(Slider::textBoxBackgroundColourId, Colour(0xff0a101d));
        setColour(Slider::textBoxOutlineColourId, Suite::border);
        setColour(TextEditor::highlightColourId, Suite::purple);
        setColour(ComboBox::backgroundColourId, Colour(0xff0b1220));
        setColour(ComboBox::outlineColourId, Suite::border);
        setColour(ComboBox::textColourId, Suite::text);
        setColour(PopupMenu::backgroundColourId, Colour(0xff121d30));
        setColour(PopupMenu::textColourId, Suite::text);
        setColour(PopupMenu::highlightedBackgroundColourId, Suite::purple);
    }
    Font getLabelFont(Label &label) override
    {
        if (presetFont && dynamic_cast<ComboBox *>(label.getParentComponent()) != nullptr)
            return Font(FontOptions(22)); // 13.6 px after the default 617 / 1000 scale.
        return Font(FontOptions(15));
    }
    Font getPopupMenuFont() override
    {
        return presetFont ? Font(FontOptions(22)) : LookAndFeel_V4::getPopupMenuFont();
    }
    void drawLabel(Graphics &g, Label &l) override
    {
        if (auto *slider = dynamic_cast<Slider *>(l.getParentComponent()))
        {
            auto r = l.getLocalBounds().toFloat().reduced(.5f);
            g.setColour(Colour(0xff050c17));
            g.fillRect(r);
            g.setColour(Colour(0xff8a9eac));
            g.drawRect(r, 1.1f);
            g.setColour(Colour(0xffffffff).withAlpha(.30f));
            g.drawLine(r.getX() + 1, r.getY() + 1, r.getRight() - 1, r.getY() + 1, .7f);
            if (!l.isBeingEdited())
            {
                bool effectLevel = slider->getName() == "vv_delay_db" ||
                                   slider->getName() == "vv_doubler_db" ||
                                   slider->getName() == "vv_reverb_db";
                auto size = slider->getName() == "vv_intensity" ? 34.f
                            : effectLevel ? 24.f : 20.f;
                Suite::label(g, l.getText(), r.reduced(3), size);
            }
            return;
        }
        LookAndFeel_V4::drawLabel(g, l);
    }
    Font getComboBoxFont(ComboBox &) override
    {
        return Font(FontOptions(presetFont ? 22 : 21));
    }
    Slider::SliderLayout getSliderLayout(Slider &s) override
    {
        if (s.isRotary())
        {
            Slider::SliderLayout l;
            auto intensity = s.getName() == "vv_intensity";
            auto effect = s.getName() == "vv_delay_db" || s.getName() == "vv_doubler_db" ||
                          s.getName() == "vv_reverb_db";
            auto trim = s.getName() == "vv_input_db" || s.getName() == "vv_output_db";
            auto boxHeight = s.getTextBoxPosition() == Slider::TextBoxBelow
                                 ? (intensity ? 62 : effect ? 42 : 34)
                                 : 0;
            auto side = jmin(s.getWidth(), s.getHeight() - boxHeight);
            l.sliderBounds = {s.getWidth() / 2 - side / 2, trim ? 5 : effect ? 2 : 0, side, side};
            auto boxWidth = intensity ? 270 : effect ? 146 : 96;
            auto effectOffset = s.getName() == "vv_reverb_db" ? 2 : 0;
            auto boxOffset = intensity ? 8 : trim ? 6 : effectOffset;
            auto boxY = s.getHeight() - boxHeight - boxOffset;
            l.textBoxBounds = {s.getWidth() / 2 - boxWidth / 2,
                               boxY, boxWidth, boxHeight - 3};
            return l;
        }
        if (s.isVertical())
        {
            Slider::SliderLayout l;
            l.sliderBounds = {0, 16, s.getWidth(), s.getHeight() - 76};
            auto width = s.getName() == "idle" ? 60 : 96;
            l.textBoxBounds = {s.getWidth() / 2 - width / 2, s.getHeight() - 30, width, 28};
            return l;
        }
        if (s.isHorizontal())
        {
            Slider::SliderLayout l;
            l.sliderBounds = {8, 8, s.getWidth() - 128, 28};
            l.textBoxBounds = {s.getWidth() - 100, 5, 96, 34};
            return l;
        }
        return LookAndFeel_V4::getSliderLayout(s);
    }
    void drawRotarySlider(Graphics &g, int x, int y, int w, int h, float pos, float start,
                          float end, Slider &s) override
    {
        auto area = Rectangle<float>(float(x), float(y), float(w), float(h)).reduced(10);
        auto c = area.getCentre();
        const bool intensity = s.getName() == "vv_intensity";
        const bool effect = s.getName() == "vv_delay_db" || s.getName() == "vv_doubler_db" ||
                            s.getName() == "vv_reverb_db";
        const bool trim = s.getName() == "vv_input_db" || s.getName() == "vv_output_db";
        auto radius = jmin(area.getWidth(), area.getHeight()) * .5f -
                      (intensity ? 9.f : trim ? 7.f : effect ? -3.f : 2.f);
        const int divisions = intensity ? 50 : 0;
        // Reference-style indexed arc with long marks at the numbered increments.
        for (int i = 0; intensity && i <= divisions; ++i)
        {
            auto angle = start + (end - start) * float(i) / float(divisions);
            auto v = Point<float>(std::sin(angle), -std::cos(angle));
            bool major = intensity ? (i % 5 == 0) : (i % 4 == 0);
            g.setColour(major ? Colour(0xffc6d1da) : Colour(0xff8397a6));
            g.drawLine(Line<float>(c + v * (radius + (intensity ? 9.f : 5.f)),
                                   c + v * (radius + (intensity ? (major ? 23.f : 15.f)
                                                                   : (major ? 12.f : 9.f)))),
                       major ? 1.7f : 1.f);
            if (intensity && i % 5 == 0)
            {
                auto labelPoint = c + v * (radius + 27.f);
                auto value = String(i * 2);
                g.setColour(Suite::text);
                g.setFont(FontOptions(19));
                g.drawText(value, Rectangle<float>(labelPoint.x - 22, labelPoint.y - 12, 44, 24),
                           Justification::centred);
            }
        }
        Path full, value;
        full.addCentredArc(c.x, c.y, radius, radius, 0, start, end, true);
        value.addCentredArc(c.x, c.y, radius, radius, 0, start, start + pos * (end - start), true);
        g.setColour(intensity ? Colour(0xff07101c) : Colour(0xff111b24));
        g.strokePath(full, PathStrokeType(intensity ? 10.f : 8.f));
        if (intensity)
        {
            g.setColour(Suite::violet.withAlpha(.2f));
            g.strokePath(value, PathStrokeType(15));
            g.setColour(Suite::violet);
            g.strokePath(value, PathStrokeType(7));
        }
        auto faceRadius = radius - (intensity ? 16.f : 8.f);
        auto face = Rectangle<float>(c.x - faceRadius, c.y - faceRadius,
                                     2 * faceRadius, 2 * faceRadius);
        g.setColour(Colours::black.withAlpha(.68f));
        g.fillEllipse(face.translated(1, 7));
        g.setGradientFill(ColourGradient(Colour(0xfff8fafb), c, Colour(0xff747f87),
                                         Point<float>{c.x + radius * .88f,
                                                      c.y + radius * .88f}, true));
        g.fillEllipse(face);
        g.setColour(Colour(0xff101a24));
        g.drawEllipse(face, 2.2f);
        auto metal = face.reduced(intensity ? 18.f : 5.f);
        g.setGradientFill(ColourGradient(Colour(0xfff2f4f5), c, Colour(0xff9aa5ad),
                                         Point<float>{c.x + radius * .78f,
                                                      c.y + radius * .78f}, true));
        g.fillEllipse(metal);
        g.setColour(Colour(0xfff7fbff).withAlpha(.45f));
        g.drawEllipse(metal.reduced(2), 1.f);
        // Radial hairlines suggest brushed aluminum while keeping the indicator readable.
        for (int i = 0; i < 80; ++i)
        {
            auto a = MathConstants<float>::twoPi * float(i) / 80.f;
            auto v = Point<float>(std::sin(a), -std::cos(a));
            g.setColour(Colour(0xfff4f7f8).withAlpha(i % 4 == 0 ? .17f : .065f));
            g.drawLine(Line<float>(c + v * (radius * .13f), c + v * (radius * .78f)), .7f);
        }
        auto angle = start + pos * (end - start);
        auto v = Point<float>(std::sin(angle), -std::cos(angle));
        g.setColour(Colour(0xff101923));
        g.drawLine(Line<float>(c + v * (radius * .17f), c + v * (radius * .72f)),
                   intensity ? 6.f : 4.f);
        g.setColour(Colour(0xfff0f3f4).withAlpha(.35f));
        g.drawLine(Line<float>(c + v * (radius * .18f) + Point<float>{1, -1},
                               c + v * (radius * .71f) + Point<float>{1, -1}),
                   intensity ? 1.5f : 1.f);
        g.setColour(Colour(0xff111c28));
        g.fillEllipse(c.x - 3, c.y - 3, 6, 6);
        if (s.hasKeyboardFocus(true))
        {
            g.setColour(Suite::violet);
            g.drawEllipse(face.expanded(2), 1);
        }
    }
    void drawLinearSlider(Graphics &g, int x, int y, int w, int h, float pos, float, float,
                          Slider::SliderStyle style, Slider &s) override
    {
        if (style == Slider::LinearHorizontal)
        {
            auto r = Rectangle<float>(float(x), float(y + h / 2 - 7), float(w), 14);
            g.setColour(Colour(0xff030812));
            g.fillRoundedRectangle(r.expanded(10, 10), 5);
            g.setColour(Suite::border);
            g.drawRoundedRectangle(r.expanded(10, 10), 5, .8f);
            g.setColour(Colour(0xff1b2539));
            g.fillRoundedRectangle(r, 2);
            auto fraction = jlimit(0.f, 1.f, (inputDb + 42) / 42);
            g.setGradientFill(ColourGradient(Colour(0xfff3ba34), r.getTopLeft(), Colour(0xffffe46a),
                                             r.getTopRight(), false));
            g.fillRect(r.withWidth(r.getWidth() * fraction));
            g.setColour(Suite::border);
            g.drawRoundedRectangle(r, 2, .7f);
            Suite::glowLine(g, {{pos, r.getY() - 9}, {pos, r.getBottom() + 9}}, 3);
            return;
        }
        auto cx = float(x + w / 2);
        if (s.getName() == "idle")
        {
            g.setColour(Suite::yellow.withAlpha(.12f));
            g.drawLine(cx, float(y), cx, float(y + h), 1);
            Path marker;
            marker.addTriangle(cx - 9, pos - 6, cx + 2, pos, cx - 9, pos + 6);
            g.setColour(Suite::yellow);
            g.fillPath(marker);
            return;
        }
        Suite::track(g, cx, float(y), float(y + h));
        Suite::cap(g, cx, pos, s.getName() == "rider" ? 62.f : 58.f,
                   s.getName() == "rider" ? 65.f : 47.f);
        if (s.hasKeyboardFocus(true))
        {
            g.setColour(Suite::violet);
            g.drawRoundedRectangle(cx - 36, pos - 37, 72, 74, 5, 1);
        }
    }
    void drawButtonBackground(Graphics &g, Button &b, const Colour &, bool over, bool down) override
    {
        auto r = b.getLocalBounds().toFloat().reduced(1);
        bool selected = b.getToggleState();
        if (b.getName().endsWith("_on"))
        {
            g.setColour(Colour(0xff050a10));
            g.fillRoundedRectangle(r.translated(0, 3), 4);
            g.setGradientFill(ColourGradient(Colour(0xff9aa8ae), r.getTopLeft(),
                                             Colour(0xff28343b), r.getBottomLeft(), false));
            g.fillRoundedRectangle(r, 4);
            g.setColour(Colour(0xffd8e0e4));
            g.drawRoundedRectangle(r, 4, 1.f);
            auto lamp = r.reduced(6);
            g.setColour(Colour(0xff111b21));
            g.fillRoundedRectangle(lamp, 2);
            if (selected)
            {
                g.setColour(Colour(0xff53ff13).withAlpha(.3f));
                g.fillRoundedRectangle(lamp.expanded(3), 4);
                g.setGradientFill(ColourGradient(Colour(0xffc6ff8e), lamp.getTopLeft(),
                                                 Colour(0xff29e800), lamp.getBottomLeft(), false));
                g.fillRoundedRectangle(lamp, 2);
                g.setColour(Colour(0xffe9ffd9).withAlpha(.8f));
                g.drawLine(lamp.getX() + 3, lamp.getY() + 2, lamp.getRight() - 3,
                           lamp.getY() + 2, .8f);
            }
            return;
        }
        if (b.getName() == "settings" || b.getName() == "vv_bypass")
        {
            auto top = Colour(0xff102b45);
            auto bottom = Colour(0xff061528);
            if (over)
                top = top.brighter(.14f);
            if (down)
                bottom = bottom.darker(.2f);
            g.setColour(Colours::black.withAlpha(.65f));
            g.fillRoundedRectangle(r.translated(0, 3), 6);
            g.setGradientFill(ColourGradient(top, r.getTopLeft(), bottom, r.getBottomLeft(), false));
            g.fillRoundedRectangle(r, 6);
            g.setColour(Colour(0xff9cb2c4));
            g.drawRoundedRectangle(r, 6, 1.4f);
            g.setColour(Colour(0xffffffff).withAlpha(.25f));
            g.drawRoundedRectangle(r.reduced(2), 5, .7f);
            return;
        }
        auto top = selected ? Colour(0xff57fa24) : Colour(0xff293d50);
        auto bottom = selected ? Colour(0xff087b23) : Colour(0xff0a1725);
        if (over)
            top = top.brighter(.15f);
        if (down)
            bottom = bottom.darker(.2f);
        g.setColour(Colours::black.withAlpha(.4f));
        g.fillRoundedRectangle(r.translated(0, 2), 6);
        g.setGradientFill(ColourGradient(top, r.getTopLeft(), bottom, r.getBottomLeft(), false));
        g.fillRoundedRectangle(r, 6);
        g.setColour(selected ? Colour(0xffa5ff78) : (b.hasKeyboardFocus(true) ? Suite::violet : Suite::border));
        g.drawRoundedRectangle(r, 6, selected ? 1.4f : .8f);
        if (selected)
        {
            g.setColour(Colour(0xff4cff27).withAlpha(.35f));
            g.drawRoundedRectangle(r.reduced(2), 5, 1);
        }
    }
    void drawButtonText(Graphics &g, TextButton &b, bool, bool) override
    {
        if (b.getName().endsWith("_on"))
            return;
        auto bounds = b.getLocalBounds().toFloat();
        if (b.getName() == "settings")
        {
            auto c = Point<float>(bounds.getX() + 20, bounds.getCentreY());
            g.setColour(Suite::text);
            for (int i = 0; i < 8; ++i)
            {
                auto a = MathConstants<float>::twoPi * float(i) / 8.f;
                auto v = Point<float>(std::sin(a), std::cos(a));
                g.drawLine({c + v * 6.f, c + v * 9.f}, 2.3f);
            }
            g.setColour(Suite::text);
            g.drawEllipse(c.x - 5, c.y - 5, 10, 10, 2.2f);
            bounds.removeFromLeft(35);
        }
        if (b.getName() == "vv_bypass")
        {
            auto c = Point<float>(bounds.getRight() - 15, bounds.getCentreY());
            auto colour = b.getToggleState() ? Colour(0xff44f24f) : Colour(0xfff32f43);
            g.setColour(colour.withAlpha(.2f));
            g.fillEllipse(c.x - 8, c.y - 8, 16, 16);
            g.setColour(Colour(0xff020913));
            g.fillEllipse(c.x - 5, c.y - 5, 10, 10);
            g.setGradientFill(ColourGradient(colour.brighter(.35f), c.x - 3, c.y - 3,
                                             colour.darker(.2f), c.x + 3, c.y + 3, false));
            g.fillEllipse(c.x - 3, c.y - 3, 6, 6);
            bounds.removeFromRight(24);
        }
        Suite::label(g, b.getButtonText(), bounds,
                     b.getHeight() >= 45 ? 20.f : b.getHeight() < 30 ? 11.f : 15.f);
    }
    void drawComboBox(Graphics &g, int w, int h, bool, int, int, int, int, ComboBox &) override
    {
        auto r = Rectangle<float>(.5f, .5f, float(w) - 1, float(h) - 1);
        g.setColour(Colour(0xff050d1a));
        g.fillRoundedRectangle(r, 5);
        g.setColour(Colour(0xffb4c5d3));
        g.drawRoundedRectangle(r, 5, 1.6f);
        g.setColour(Colour(0xffffffff).withAlpha(.27f));
        g.drawRoundedRectangle(r.reduced(2), 4, .7f);
        Path arrow;
        arrow.startNewSubPath(float(w - 26), float(h / 2 - 3));
        arrow.lineTo(float(w - 20), float(h / 2 + 3));
        arrow.lineTo(float(w - 14), float(h / 2 - 3));
        g.setColour(Suite::muted);
        g.strokePath(arrow, PathStrokeType(1.5f));
    }

  private:
    const bool presetFont;
};

} // namespace vocalis::design
