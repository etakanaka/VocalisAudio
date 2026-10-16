/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
// Unmodified rendering excerpt retained as the pixel-comparison oracle, 2026-10-16.
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
namespace rider_reference {
using namespace juce;
namespace Suite {
const Colour background{0xff0a111e}, border{0xff465b82}, text{0xffedf1ff}, muted{0xffa9bce5};
const Colour violet{0xffa378ff}, purple{0xff7130de}, yellow{0xffffd24b};
void label(Graphics& g, const String& s, Rectangle<float> r, float size,
           Colour colour = text, bool bold = false, Justification align = Justification::centred)
{
    g.setColour(colour);
    g.setFont(FontOptions(size, bold ? Font::bold : Font::plain));
    g.drawText(s, r, align);
}
void panel(Graphics& g, Rectangle<float> r, float radius = 9.f)
{
    g.setColour(Colours::black.withAlpha(.35f));
    g.fillRoundedRectangle(r.translated(0, 3), radius);
    g.setGradientFill(ColourGradient(Colour(0xff1b273d), r.getTopRight(),
                                     Colour(0xff101925), r.getBottomLeft(), false));
    g.fillRoundedRectangle(r, radius);
    g.setColour(border); g.drawRoundedRectangle(r, radius, .85f);
    g.setColour(Colours::white.withAlpha(.035f));
    g.drawRoundedRectangle(r.reduced(1), radius - 1, .7f);
}
void readout(Graphics& g, Rectangle<float> r, const String& value, float size = 16)
{
    g.setColour(Colour(0xff090f1b)); g.fillRoundedRectangle(r, 5);
    g.setColour(border); g.drawRoundedRectangle(r, 5, .9f);
    label(g, value, r, size);
}
void glowLine(Graphics& g, Line<float> line, float width = 3.f)
{
    for (int i = 3; i > 0; --i) {
        g.setColour(violet.withAlpha(.06f * float(4-i)));
        g.drawLine(line, width + float(i * 4));
    }
    g.setColour(violet); g.drawLine(line, width + 2);
    g.setColour(Colour(0xfff5eaff)); g.drawLine(line, width);
}
void cap(Graphics& g, float x, float y, float width, float height)
{
    auto r = Rectangle<float>(x-width/2, y-height*.65f, width, height);
    g.setColour(Colours::black.withAlpha(.6f)); g.fillRoundedRectangle(r.translated(2, 5), 5);
    g.setGradientFill(ColourGradient(Colour(0xff9da6c2), r.getTopLeft(), Colour(0xff111727), r.getBottomLeft(), false));
    g.fillRoundedRectangle(r, 5);
    g.setColour(Colour(0xff070c17)); g.drawRoundedRectangle(r, 5, 1.8f);
    g.setColour(Colour(0xffbfcbe9).withAlpha(.65f));
    g.drawHorizontalLine(int(r.getY()+4), r.getX()+5, r.getRight()-5);
    for (int i = 1; i <= 3; ++i) {
        auto yy = r.getY() + height * float(i) / 5;
        g.setColour(Colour(0xff090e1c)); g.drawLine(r.getX()+4, yy, r.getRight()-4, yy, 1.6f);
        g.setColour(Colour(0xff8995b4).withAlpha(.35f)); g.drawLine(r.getX()+4, yy+2, r.getRight()-4, yy+2, .7f);
    }
    glowLine(g, {{r.getX()+8, y}, {r.getRight()-8, y}}, 2.2f);
}
void track(Graphics& g, float x, float top, float bottom)
{
    auto r = Rectangle<float>(x-10, top-10, 20, bottom-top+20);
    g.setColour(Colour(0xff030610)); g.fillRoundedRectangle(r, 9);
    g.setColour(Colour(0xff7285b6)); g.drawRoundedRectangle(r, 9, .9f);
    g.setColour(purple.withAlpha(.35f)); g.fillRoundedRectangle(x-4, top, 8, bottom-top, 4);
    g.setGradientFill(ColourGradient(violet, x, top, purple, x, bottom, false));
    g.fillRoundedRectangle(x-2, top, 4, bottom-top, 2);
}
void ticks(Graphics& g, float x, float top, float bottom, bool numbers = true)
{
    for (int db = -12; db <= 12; ++db) {
        auto y = top + (12-float(db))/24 * (bottom-top);
        bool major = db % 6 == 0;
        g.setColour(Colour(major ? 0xff8195c1 : 0xff394e73));
        g.drawLine(x, y, x+(major ? 17.f : 5.f), y, major ? 1.4f : .8f);
        if (major && numbers)
            label(g, (db > 0 ? "+" : "") + String(db), {x+23, y-10, 38, 20}, 13, muted, false, Justification::centredLeft);
    }
}
void led(Graphics& g, Point<float> c, bool active, Colour colour)
{
    if (active) for (int i = 5; i > 0; --i) {
        g.setColour(colour.withAlpha(.035f * float(6-i)));
        g.fillEllipse(c.x-10-float(i*2), c.y-10-float(i*2), 20+float(i*4), 20+float(i*4));
    }
    g.setColour(Colour(0xff040914)); g.fillEllipse(c.x-13, c.y-13, 26, 26);
    g.setColour(border); g.drawEllipse(c.x-13, c.y-13, 26, 26, 1);
    auto r = Rectangle<float>(c.x-9, c.y-9, 18, 18);
    g.setGradientFill(ColourGradient(active ? colour.brighter(.6f) : Colour(0xff71809d), r.getTopLeft(),
                                    active ? colour : Colour(0xff111a2a), r.getBottomRight(), false));
    g.fillEllipse(r);
}
}

class SuiteLook final : public LookAndFeel_V4 {
public:
    float inputDb = -120;
    SuiteLook()
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
    Font getLabelFont(Label&) override { return Font(FontOptions(15)); }
    void drawLabel(Graphics& g, Label& l) override
    {
        if (dynamic_cast<Slider*>(l.getParentComponent()) != nullptr) {
            auto r = l.getLocalBounds().toFloat().reduced(.5f);
            g.setColour(Colour(0xff0a101d)); g.fillRoundedRectangle(r, 5);
            g.setColour(Suite::border); g.drawRoundedRectangle(r, 5, .85f);
            if (!l.isBeingEdited()) Suite::label(g, l.getText(), r.reduced(2), 15);
            return;
        }
        LookAndFeel_V4::drawLabel(g, l);
    }
    Font getComboBoxFont(ComboBox&) override { return Font(FontOptions(15)); }
    Slider::SliderLayout getSliderLayout(Slider& s) override
    {
        if (s.isVertical()) {
            Slider::SliderLayout l;
            l.sliderBounds = {0, 16, s.getWidth(), s.getHeight()-76};
            auto width = s.getName() == "idle" ? 60 : 96;
            l.textBoxBounds = {s.getWidth()/2-width/2, s.getHeight()-30, width, 28};
            return l;
        }
        if (s.isHorizontal()) {
            Slider::SliderLayout l;
            l.sliderBounds = {8, 8, s.getWidth()-128, 28};
            l.textBoxBounds = {s.getWidth()-100, 5, 96, 34};
            return l;
        }
        return LookAndFeel_V4::getSliderLayout(s);
    }
    void drawRotarySlider(Graphics& g, int x, int y, int w, int h, float pos,
                          float start, float end, Slider& s) override
    {
        auto area = Rectangle<float>(float(x), float(y), float(w), float(h)).reduced(9);
        auto c = area.getCentre();
        auto radius = jmin(area.getWidth(), area.getHeight())*.5f-7;
        for (int i = 0; i <= 24; ++i) {
            auto angle = start+(end-start)*float(i)/24;
            auto v = Point<float>(std::sin(angle), -std::cos(angle));
            g.setColour(Colour(i%4 ? 0xff455984 : 0xff8496bd));
            g.drawLine({c+v*(radius+5), c+v*(radius+(i%4 ? 9 : 12))}, 1);
        }
        Path full, value;
        full.addCentredArc(c.x, c.y, radius, radius, 0, start, end, true);
        value.addCentredArc(c.x, c.y, radius, radius, 0, start, start+pos*(end-start), true);
        g.setColour(Colour(0xff070b18)); g.strokePath(full, PathStrokeType(9));
        g.setColour(Suite::purple.withAlpha(.2f)); g.strokePath(value, PathStrokeType(14));
        g.setGradientFill(ColourGradient(Suite::violet.brighter(.2f), c.x-radius, c.y-radius, Suite::purple, c.x+radius, c.y+radius, false));
        g.strokePath(value, PathStrokeType(6));
        auto face = Rectangle<float>(c.x-radius+7, c.y-radius+7, 2*(radius-7), 2*(radius-7));
        g.setColour(Colours::black.withAlpha(.6f)); g.fillEllipse(face.translated(2, 5));
        g.setGradientFill(ColourGradient(Colour(0xff6a779b), face.getTopLeft(), Colour(0xff080e19), face.getBottomRight(), false));
        g.fillEllipse(face); g.setColour(Colour(0xff030813)); g.drawEllipse(face, 1.5f);
        g.setGradientFill(ColourGradient(Colour(0xff293247), face.getTopLeft(), Colour(0xff0c1420), face.getBottomRight(), false));
        g.fillEllipse(face.reduced(3));
        auto angle = start+pos*(end-start);
        auto v = Point<float>(std::sin(angle), -std::cos(angle));
        g.setColour(Colour(0xffd4caff)); g.drawLine({c+v*(radius*.25f), c+v*(radius*.76f)}, 3);
        if (s.hasKeyboardFocus(true)) { g.setColour(Suite::violet); g.drawEllipse(face.expanded(2), 1); }
    }
    void drawLinearSlider(Graphics& g, int x, int y, int w, int h, float pos,
                          float, float, Slider::SliderStyle style, Slider& s) override
    {
        if (style == Slider::LinearHorizontal) {
            auto r = Rectangle<float>(float(x), float(y+h/2-7), float(w), 14);
            g.setColour(Colour(0xff030812)); g.fillRoundedRectangle(r.expanded(10, 10), 5);
            g.setColour(Suite::border); g.drawRoundedRectangle(r.expanded(10, 10), 5, .8f);
            g.setColour(Colour(0xff1b2539)); g.fillRoundedRectangle(r, 2);
            auto fraction = jlimit(0.f, 1.f, (inputDb+42)/42);
            g.setGradientFill(ColourGradient(Colour(0xfff3ba34), r.getTopLeft(), Colour(0xffffe46a), r.getTopRight(), false));
            g.fillRect(r.withWidth(r.getWidth()*fraction));
            g.setColour(Suite::border); g.drawRoundedRectangle(r, 2, .7f);
            Suite::glowLine(g, {{pos, r.getY()-9}, {pos, r.getBottom()+9}}, 3);
            return;
        }
        auto cx = float(x+w/2);
        if (s.getName() == "idle") {
            g.setColour(Suite::yellow.withAlpha(.12f)); g.drawLine(cx, float(y), cx, float(y+h), 1);
            Path marker; marker.addTriangle(cx-9, pos-6, cx+2, pos, cx-9, pos+6);
            g.setColour(Suite::yellow); g.fillPath(marker);
            return;
        }
        Suite::track(g, cx, float(y), float(y+h));
        Suite::cap(g, cx, pos, s.getName() == "rider" ? 62.f : 58.f, s.getName() == "rider" ? 65.f : 47.f);
        if (s.hasKeyboardFocus(true)) {
            g.setColour(Suite::violet); g.drawRoundedRectangle(cx-36, pos-37, 72, 74, 5, 1);
        }
    }
    void drawButtonBackground(Graphics& g, Button& b, const Colour&, bool over, bool down) override
    {
        auto r = b.getLocalBounds().toFloat().reduced(1);
        bool selected = b.getToggleState();
        auto top = selected ? Colour(0xff6130af) : Colour(0xff1b2940);
        auto bottom = selected ? Colour(0xff38117f) : Colour(0xff0d1523);
        if (over) top = top.brighter(.15f);
        if (down) bottom = bottom.darker(.2f);
        g.setColour(Colours::black.withAlpha(.4f)); g.fillRoundedRectangle(r.translated(0, 2), 6);
        g.setGradientFill(ColourGradient(top, r.getTopLeft(), bottom, r.getBottomLeft(), false));
        g.fillRoundedRectangle(r, 6);
        g.setColour(selected || b.hasKeyboardFocus(true) ? Suite::violet : Suite::border);
        g.drawRoundedRectangle(r, 6, selected ? 1.4f : .8f);
        if (selected) { g.setColour(Suite::violet.withAlpha(.3f)); g.drawRoundedRectangle(r.reduced(2), 5, 1); }
    }
    void drawButtonText(Graphics& g, TextButton& b, bool, bool) override
    { Suite::label(g, b.getButtonText(), b.getLocalBounds().toFloat(), b.getHeight()<30 ? 11.f : 15.f); }
    void drawComboBox(Graphics& g, int w, int h, bool, int, int, int, int, ComboBox&) override
    {
        auto r = Rectangle<float>(.5f, .5f, float(w)-1, float(h)-1);
        g.setColour(Colour(0xff0a111e)); g.fillRoundedRectangle(r, 5);
        g.setColour(Suite::border); g.drawRoundedRectangle(r, 5, 1);
        Path arrow; arrow.startNewSubPath(float(w-26), float(h/2-3));
        arrow.lineTo(float(w-20), float(h/2+3)); arrow.lineTo(float(w-14), float(h/2-3));
        g.setColour(Suite::muted); g.strokePath(arrow, PathStrokeType(1.5f));
    }
};

class LevelMeter final : public Component {
    const std::atomic<float>& rms;
    const std::atomic<float>* peak;
public:
    LevelMeter(const std::atomic<float>& value, const std::atomic<float>* peakValue = nullptr)
        : rms(value), peak(peakValue) { setInterceptsMouseClicks(false, false); }
    void paint(Graphics& g) override
    {
        auto top = 16.f, bottom = float(getHeight()-60), cx = float(getWidth())*.53f;
        auto r = Rectangle<float>(cx-9, top, 18, bottom-top);
        g.setColour(Colour(0xff070d18)); g.fillRect(r);
        auto f = jlimit(0.f, 1.f, (rms.load()+60)/60);
        ColourGradient gradient(Colour(0xff00dba0), cx, bottom, Suite::yellow, cx, top, false);
        gradient.addColour(.68, Colour(0xff74ed79));
        g.setGradientFill(gradient); g.fillRect(r.withTop(bottom-f*r.getHeight()));
        g.setColour(Suite::border); g.drawRect(r, .75f);
        for (int db = -60; db <= 0; db += 3) {
            auto y = bottom-float(db+60)/60*r.getHeight();
            g.setColour(Colour(0xff354766)); g.drawLine(cx-9, y, cx+9, y, .6f);
        }
        for (int db : {0, -6, -12, -24, -48}) {
            auto y = bottom-float(db+60)/60*r.getHeight();
            Suite::label(g, String(db), {0, y-9, cx-18, 18}, 12, Suite::muted, false, Justification::centredRight);
            g.setColour(Suite::muted); g.drawLine(cx-15, y, cx-11, y, 1);
        }
        if (peak) {
            auto y = bottom-jlimit(0.f, 1.f, (peak->load()+60)/60)*r.getHeight();
            g.setColour(Suite::text); g.drawLine(cx-9, y, cx+9, y, 1.2f);
        }
        Suite::readout(g, {2, float(getHeight()-30), float(getWidth()-4), 28}, String(rms.load(), 1), 13);
    }
};

}
