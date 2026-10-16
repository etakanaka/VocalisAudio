/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "Plugin.h"

namespace {
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

class RangeControl final : public Component, public SettableTooltipClient {
    Vocalis& processor;
    int handle = 1;
    float origin = 0, start = 0;
    bool dragging = false;
    float position(float db) const { return 16+(12-db)/24*float(getHeight()-76); }
    const char* id() const { return handle ? "rangeMax" : "rangeMin"; }
public:
    explicit RangeControl(Vocalis& p) : processor(p)
    {
        setName("range"); setWantsKeyboardFocus(true);
        setTooltip("Range Min / Max: drag either handle. Shift for fine adjustment. Arrow Left/Right selects a handle; Up/Down adjusts. Double-click resets -6 / +6 dB.");
    }
    ~RangeControl() override
    {
        if (dragging) processor.state.getParameter(id())->endChangeGesture();
    }
    void paint(Graphics& g) override
    {
        auto cx = float(getWidth())/2;
        Suite::track(g, cx, 16, float(getHeight()-60));
        for (int i : {0, 1}) {
            auto y = position(processor.value(i ? "rangeMax" : "rangeMin"));
            Suite::cap(g, cx, y, 62, 35);
            if (hasKeyboardFocus(true) && handle == i) { g.setColour(Suite::violet); g.drawRoundedRectangle(cx-34, y-20, 68, 40, 4, 1); }
        }
    }
    void change(float v)
    {
        if (handle) v = jlimit(jmax(-11.f, processor.value("rangeMin")), 12.f, v);
        else v = jlimit(-12.f, jmin(11.f, processor.value("rangeMax")), v);
        processor.set(id(), v); repaint();
    }
    void mouseDown(const MouseEvent& e) override
    {
        handle = std::abs(e.position.y-position(processor.value("rangeMax"))) < std::abs(e.position.y-position(processor.value("rangeMin"))) ? 1 : 0;
        start = e.position.y; origin = processor.value(id()); dragging = true;
        processor.state.getParameter(id())->beginChangeGesture();
    }
    void mouseDrag(const MouseEvent& e) override
    { change(origin+(start-e.position.y)*24/float(getHeight()-76)*(e.mods.isShiftDown() ? .1f : 1.f)); }
    void mouseUp(const MouseEvent&) override
    { if (dragging) processor.state.getParameter(id())->endChangeGesture(); dragging = false; }
    void mouseDoubleClick(const MouseEvent&) override
    {
        for (const auto& item : {std::pair{"rangeMin", -6.f}, std::pair{"rangeMax", 6.f}}) {
            auto* parameter = processor.state.getParameter(item.first);
            const bool alreadyDragging = dragging && String(id()) == item.first;
            if (!alreadyDragging) parameter->beginChangeGesture();
            processor.set(item.first, item.second);
            if (!alreadyDragging) parameter->endChangeGesture();
        }
        repaint();
    }
    bool keyPressed(const KeyPress& k) override
    {
        if (k == KeyPress::leftKey || k == KeyPress::rightKey) { handle = k == KeyPress::rightKey ? 1 : 0; repaint(); return true; }
        if (k == KeyPress::upKey || k == KeyPress::downKey) {
            processor.state.getParameter(id())->beginChangeGesture();
            change(processor.value(id())+(k == KeyPress::upKey ? 1.f : -1.f)*(k.getModifiers().isShiftDown() ? .1f : 1.f));
            processor.state.getParameter(id())->endChangeGesture(); return true;
        }
        return false;
    }
};

class AboutPanel final : public Component {
    Label content;
    TextButton close{"CLOSE"};
public:
    AboutPanel()
    {
        setName("aboutPanel");
        content.setText(String::fromUTF8("VOCALIS AUDIO\nVocalis Rider Live\nVersion ") + JucePlugin_VersionString
            + String::fromUTF8("\n\nCreated by Enos Takanaka\nDeveloped with AI assistance using OpenAI Codex, C++ and JUCE.\nNative Linux VST3 | REAPER\n\nCopyright © 2026 Enos Takanaka and applicable contributors.\nOpen-source licence: AGPL-3.0-or-later\n\nGitHub:\nhttps://github.com/etakanaka/VocalisAudio"), dontSendNotification);
        content.setFont(FontOptions(17.f));
        content.setColour(Label::textColourId, Suite::text);
        content.setJustificationType(Justification::centred);
        addAndMakeVisible(content); addAndMakeVisible(close);
        close.onClick = [this]{ setVisible(false); };
    }
    void paint(Graphics& g) override { Suite::panel(g, getLocalBounds().toFloat()); }
    void resized() override
    {
        content.setBounds(16, 12, getWidth()-32, getHeight()-70);
        close.setBounds(getWidth()/2-50, getHeight()-48, 100, 32);
    }
};

class Editor final : public AudioProcessorEditor, private Timer {
    Vocalis& p;
    SuiteLook look;
    Component canvas;
    Slider target, spill, vocal, music, rider, output, idle;
    RangeControl range;
    LevelMeter inputMeter, musicMeter;
    TextButton slow{"Slow"}, fast{"Fast"}, off{"OFF"}, write{"WRITE"}, read{"READ"};
    TextButton bypass{"BYPASS"}, exportButton{"EXPORT CSV"}, clear{"CLEAR"}, about{"ABOUT"};
    ComboBox presets;
    TooltipWindow tooltip{this, 650};
    std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::unique_ptr<FileChooser> chooser;
    AboutPanel aboutPanel;
    void addSlider(Slider& s, const char* id, float lo, float hi, float def, Slider::SliderStyle style)
    {
        canvas.addAndMakeVisible(s); s.setName(id); s.setTitle(id);
        s.setSliderStyle(style); s.setRange(lo, hi, .1); s.setDoubleClickReturnValue(true, def);
        s.setTextBoxStyle(Slider::TextBoxBelow, false, 96, 28);
        s.setNumDecimalPlacesToDisplay(1); s.setTextValueSuffix(" dB");
        s.setWantsKeyboardFocus(true);
        s.setTooltip(String(id)+": drag to adjust; Shift for fine adjustment, arrows for steps; double-click restores the default. Values in dB.");
        attachments.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.state, id, s));
    }
    void setControlValue(const char* id, float value)
    {
        auto* parameter = p.state.getParameter(id);
        parameter->beginChangeGesture();
        p.set(id, value);
        parameter->endChangeGesture();
    }
    void addButton(TextButton& b, const char* name, std::function<void()> f)
    { canvas.addAndMakeVisible(b); b.setName(name); b.setWantsKeyboardFocus(true); b.onClick = std::move(f); }
    void timerCallback() override
    {
        bool reading = p.value("automation") == 2;
        rider.setEnabled(reading);
        if (!reading) rider.setValue(p.meterGain.load(), dontSendNotification);
        off.setToggleState(p.value("automation") == 0, dontSendNotification);
        write.setToggleState(p.value("automation") == 1, dontSendNotification);
        read.setToggleState(reading, dontSendNotification);
        slow.setToggleState(p.value("attack") == 0, dontSendNotification);
        fast.setToggleState(p.value("attack") == 1, dontSendNotification);
        bypass.setToggleState(p.value("bypass") > .5f, dontSendNotification);
        presets.setSelectedId(p.getCurrentProgram()+1, dontSendNotification);
        look.inputDb = p.meterInput.load();
        canvas.repaint(); repaint();
    }
public:
    explicit Editor(Vocalis& processor)
        : AudioProcessorEditor(processor), p(processor), range(p), inputMeter(p.meterInput, &p.meterPeak), musicMeter(p.meterMusic)
    {
        setLookAndFeel(&look); setOpaque(true); addAndMakeVisible(canvas);
        canvas.setInterceptsMouseClicks(false, true);
        addSlider(target, "target", -42, 0, -21, Slider::LinearHorizontal);
        addSlider(spill, "spill", -48, -12, -48, Slider::RotaryHorizontalVerticalDrag);
        addSlider(vocal, "vocal", -12, 12, 0, Slider::RotaryHorizontalVerticalDrag);
        addSlider(music, "music", -12, 12, 0, Slider::RotaryHorizontalVerticalDrag);
        addSlider(rider, "rider", -12, 12, 0, Slider::LinearVertical);
        addSlider(output, "output", -12, 12, 0, Slider::LinearVertical);
        addSlider(idle, "idle", -12, 12, 0, Slider::LinearVertical);
        rider.setTooltip("Automatic Rider gain before Output/Bypass in Off and Write. Host-automated/manual Rider in Read.");
        idle.setTooltip("Idle return position: drag the yellow marker; arrows adjust, double-click resets to 0 dB. Engine bounds Idle to Range.");
        canvas.addAndMakeVisible(range); canvas.addAndMakeVisible(inputMeter); canvas.addAndMakeVisible(musicMeter);
        addButton(slow, "attackSlow", [this]{ setControlValue("attack", 0); });
        addButton(fast, "attackFast", [this]{ setControlValue("attack", 1); });
        addButton(off, "automationOff", [this]{ setControlValue("automation", 0); });
        addButton(write, "automationWrite", [this]{ setControlValue("automation", 1); });
        addButton(read, "automationRead", [this]{ setControlValue("automation", 2); });
        addButton(bypass, "bypass", [this]{ setControlValue("bypass", p.value("bypass") > .5f ? 0 : 1); });
        addButton(clear, "clearCapture", [this]{ p.clearCapture(); });
        addButton(exportButton, "exportCapture", [this]{
            chooser = std::make_unique<FileChooser>("Export captured Rider automation", File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Vocalis-rider.csv"), "*.csv");
            chooser->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting,
                [safe = Component::SafePointer<Editor>(this)](const FileChooser& c){
                    if (safe && c.getResult() != File() && !safe->p.exportCsv(c.getResult()))
                        AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Export failed", "Could not write the selected CSV file.");
                });
        });
        exportButton.setTooltip("Export actual Write-mode Rider points as seconds,dB CSV. Host recording support depends on REAPER settings.");
        addButton(about, "about", [this]{
            aboutPanel.setVisible(!aboutPanel.isVisible());
            aboutPanel.toFront(false);
        });
        clear.setTooltip("Clear volatile automation capture. Stop Write before clearing a pass.");
        canvas.addAndMakeVisible(presets); presets.setName("preset");
        for (int i = 0; i < 8; ++i) presets.addItem(p.getProgramName(i), i+1);
        presets.onChange = [this]{ p.setCurrentProgram(presets.getSelectedId()-1); };
        canvas.addChildComponent(aboutPanel);
        setResizable(true, true); setResizeLimits(750, 750, 1400, 1400);
        getConstrainer()->setFixedAspectRatio(1); setSize(1000, 1000);
        timerCallback(); startTimerHz(30);
    }
    ~Editor() override { stopTimer(); setLookAndFeel(nullptr); }
    void resized() override
    {
        canvas.setTransform(AffineTransform()); canvas.setBounds(0, 0, 1000, 1000);
        canvas.setTransform(AffineTransform::scale(getWidth()/1000.f));
        aboutPanel.setBounds(175, 260, 650, 440);
        presets.setBounds(680, 42, 166, 37); bypass.setBounds(859, 42, 105, 37);
        target.setBounds(250, 137, 635, 48);
        spill.setBounds(325, 224, 144, 168);
        vocal.setBounds(469, 204, 178, 188);
        music.setBounds(651, 224, 144, 168);
        fast.setBounds(860, 284, 100, 40); slow.setBounds(860, 330, 100, 40);
        range.setBounds(230, 478, 100, 350);
        rider.setBounds(450, 478, 100, 350); output.setBounds(670, 478, 100, 350);
        idle.setBounds(550, 478, 78, 350);
        inputMeter.setBounds(42, 478, 94, 350); musicMeter.setBounds(868, 478, 94, 350);
        off.setBounds(504, 888, 132, 46); write.setBounds(644, 888, 132, 46); read.setBounds(784, 888, 132, 46);
        about.setBounds(270, 935, 90, 23);
        exportButton.setBounds(62, 935, 107, 23); clear.setBounds(177, 935, 66, 23);
    }
    void paint(Graphics& g) override
    {
        Graphics::ScopedSaveState saved(g);
        g.addTransform(AffineTransform::scale(getWidth()/1000.f));
        g.setGradientFill(ColourGradient(Suite::background.brighter(.1f), 0, 0, Colour(0xff080e19), 1000, 1000, false)); g.fillAll();
        Suite::panel(g, {18, 18, 964, 84});
        Suite::panel(g, {18, 114, 964, 294});
        Suite::panel(g, {18, 420, 964, 554});
        Suite::panel(g, {26, 860, 948, 100});
        // Original suite waveform mark, drawn from geometry rather than reference pixels.
        for (int i = 0; i < 5; ++i) {
            float heights[] = {10, 25, 45, 25, 10};
            auto h = heights[i]; g.setColour(Suite::violet.withAlpha(.12f)); g.fillRoundedRectangle(39+float(i*11)-2, 60-h/2-2, 10, h+4, 4);
            g.setColour(Colour(0xffbca7fa)); g.fillRoundedRectangle(39+float(i*11), 60-h/2, 6, h, 3);
        }
        Suite::label(g, "VOCALIS", {108, 30, 292, 54}, 51, Suite::text, true, Justification::centredLeft);
        g.setColour(Suite::border.brighter(.3f)); g.drawLine(417, 33, 417, 88, 1);
        Suite::label(g, "RIDER LIVE", {448, 31, 217, 34}, 26, Colour(0xff9bb8ff), true, Justification::centredLeft);
        Suite::label(g, "AUTOMATIC VOCAL GAIN RIDER", {450, 69, 220, 17}, 10, Suite::muted, false, Justification::centredLeft);
        Suite::label(g, "Target", {50, 126, 165, 28}, 23, Suite::text, true, Justification::centredLeft);
        Suite::label(g, "DESIRED VOCAL LEVEL", {50, 159, 184, 19}, 12, Suite::muted, false, Justification::centredLeft);
        for (int db = -42; db <= 0; db += 6) {
            float x = 258+(float(db+42)/42)*507;
            g.setColour(Suite::border); g.drawLine(x, 178, x, 183, .8f);
            Suite::label(g, String(db), {x-18, 184, 36, 16}, 11, Suite::muted);
        }
        Suite::label(g, "dBFS", {899, 147, 50, 23}, 12, Suite::muted);
        g.setColour(Suite::border); g.drawLine(242, 225, 242, 385, .8f); g.drawLine(830, 225, 830, 385, .8f);
        Suite::label(g, "Sensitivity", {262, 218, 240, 28}, 22, Suite::text, true, Justification::centredLeft);
        Suite::label(g, "DETECTOR", {263, 247, 245, 17}, 11, Suite::muted, false, Justification::centredLeft);
        Suite::led(g, {64, 270}, p.activity.load(), Suite::yellow);
        Suite::label(g, "Ride", {94, 255, 120, 28}, 20, Suite::text, true, Justification::centredLeft);
        Suite::label(g, "LEVELING ACTIVE", {94, 284, 138, 18}, 10, Suite::muted, false, Justification::centredLeft);
        Suite::led(g, {64, 342}, !p.activity.load(), Colour(0xffa9bde7));
        Suite::label(g, "Idle", {94, 326, 120, 28}, 20, Suite::text, true, Justification::centredLeft);
        Suite::label(g, "WAITING FOR SIGNAL", {94, 355, 143, 18}, 10, Suite::muted, false, Justification::centredLeft);
        Suite::label(g, "Spill", {340, 391, 112, 17}, 15, Suite::text, true);
        Suite::label(g, "Vocal", {500, 391, 115, 17}, 15, Suite::text, true);
        Suite::label(g, "Music", {665, 391, 115, 17}, 15, Suite::text, true);
        Suite::label(g, "Response", {845, 217, 130, 28}, 20, Suite::text, true);
        Suite::label(g, "RIDER SPEED", {846, 249, 128, 17}, 11, Suite::muted);
        for (float x : {146.f, 392.f, 628.f, 853.f}) { g.setColour(Suite::border.withAlpha(.55f)); g.drawLine(x, 437, x, 844, .8f); }
        Suite::label(g, "IN", {47, 441, 85, 24}, 16);
        Suite::label(g, "dBFS", {49, 466, 82, 16}, 10, Suite::muted);
        Suite::label(g, "Range", {197, 434, 166, 29}, 22, Suite::muted, true);
        Suite::label(g, "LEVEL RANGE", {201, 465, 160, 17}, 11, Suite::muted);
        Suite::label(g, "Rider", {434, 434, 135, 29}, 22, Suite::muted, true);
        Suite::label(g, "TARGET GAIN", {434, 465, 135, 17}, 11, Suite::muted);
        Suite::label(g, "Output", {649, 434, 145, 29}, 22, Suite::muted, true);
        Suite::label(g, "MAKEUP GAIN", {648, 465, 147, 17}, 11, Suite::muted);
        Suite::label(g, "MUSIC", {866, 441, 100, 24}, 15);
        Suite::label(g, "SIDECHAIN", {866, 466, 100, 16}, 10, Suite::muted);
        for (float x : {306.f, 526.f, 746.f}) Suite::ticks(g, x, 494, 768);
        for (float x : {237.f, 457.f, 677.f}) Suite::ticks(g, x-19, 494, 768, false);
        Suite::readout(g, {195, 798, 170, 23}, "MIN  " + String(p.value("rangeMin"), 1) + " dB", 13);
        Suite::readout(g, {195, 825, 170, 23}, "MAX  " + String(p.value("rangeMax"), 1) + " dB", 13);
        Suite::label(g, "IDLE", {550, 830, 78, 17}, 10, Suite::yellow);
        Suite::label(g, "Automation", {61, 877, 238, 29}, 22, Suite::muted, true, Justification::centredLeft);
        Suite::label(g, "WRITE GAIN CHANGES TO HOST", {62, 909, 280, 18}, 11, Suite::muted, false, Justification::centredLeft);
        g.setColour(Suite::border); g.drawRoundedRectangle(496, 880, 428, 62, 8, .8f);
        if (p.dropped.load() > 0) Suite::label(g, "CAPTURE FULL / DROPPED POINTS", {255, 936, 235, 20}, 10, Suite::yellow);
    }
};
}

juce::AudioProcessorEditor* Vocalis::createEditor() { return new Editor(*this); }
