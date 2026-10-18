/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "PluginEditor.h"
#include "ui/VocalisTopBar.h"
VoiceEditor::VoiceEditor(VoiceProcessor &p) : AudioProcessorEditor(p), processor(p)
{
    setLookAndFeel(&look);
    setResizable(true, true);
    // Keep the reference artwork's logical 1580 x 1000 canvas and report the
    // compact default directly to the host. The requested editor ratio differs
    // from the artwork by less than two percent; the existing whole-surface
    // transform applies that very small correction consistently to every item.
    setResizeLimits(840, 540, 2054, 1321);
    getConstrainer()->setFixedAspectRatio(960.0 / 617.0);
    setSize(960, 617);
    setOpaque(true);
    addAndMakeVisible(surface);
    surface.setInterceptsMouseClicks(false, true);
    surface.addChildComponent(aboutPanel);
    for (auto *c : std::initializer_list<juce::Component *>{
             &intensity, &input, &output, &delay, &doubler, &reverb, &inMeter, &outMeter,
             &reduction, &sensitivity, &presets, &settings, &bypass})
        surface.addAndMakeVisible(c);
    for (auto *s : {&input, &output})
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 86, 25);
        s->setTextValueSuffix(" dB");
        s->setNumDecimalPlacesToDisplay(1);
        s->setDoubleClickReturnValue(true, 0);
        s->setColour(juce::Slider::rotarySliderFillColourId, voiceui::purple);
        s->setColour(juce::Slider::thumbColourId, voiceui::text);
        s->setWantsKeyboardFocus(true);
    }
    input.setTooltip(
        "Input trim before vocal processing. Aim for yellow sensitivity on louder passages.");
    output.setTooltip(
        "Final output trim. Match processed and bypassed loudness. No limiter is applied.");
    auto attachSlider = [this](juce::Slider &s, int id)
    {
        s.setName(voiceparams::ids[id]);
        s.setTitle(processor.state.getParameter(voiceparams::ids[id])->getName(80));
        sliderAttachments.push_back(
            std::make_unique<SliderAttachment>(processor.state, voiceparams::ids[id], s));
    };
    attachSlider(input, 0);
    attachSlider(intensity, 1);
    attachSlider(delay.level, 3);
    attachSlider(doubler.level, 5);
    attachSlider(reverb.level, 7);
    attachSlider(output, 8);
    input.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 96, 32);
    output.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 96, 32);
    intensity.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 270, 59);
    for (auto *effectLevel : {&delay.level, &doubler.level, &reverb.level})
        effectLevel->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 146, 39);
    auto attachButton = [this](juce::Button &b, int id)
    {
        b.setName(voiceparams::ids[id]);
        b.setWantsKeyboardFocus(true);
        buttonAttachments.push_back(
            std::make_unique<ButtonAttachment>(processor.state, voiceparams::ids[id], b));
    };
    attachButton(delay.enable, 2);
    attachButton(doubler.enable, 4);
    attachButton(reverb.enable, 6);
    bypass.setClickingTogglesState(true);
    attachButton(bypass, 9);
    presets.setLookAndFeel(&presetLook);
    for (int i = 0; i < processor.getNumPrograms(); ++i)
        presets.addItem(processor.getProgramName(i), i + 1);
    presets.setSelectedId(processor.getCurrentProgram() + 1, juce::dontSendNotification);
    presets.onChange = [this]
    {
        int id = presets.getSelectedId();
        if (id > 0)
            processor.setCurrentProgram(id - 1);
    };
    presets.setTooltip("Factory presets preserve Input and Output gain. REAPER saves all current "
                       "settings with the project.");
    settings.setName("settings");
    settings.setTooltip("Interface scale and Vocalis Voice information");
    presets.setName("preset");
    settings.onClick = [this] { showSettings(); };
    inMeter.setName("inputMeter");
    outMeter.setName("outputMeter");
    outMeter.output = true;
    timerCallback();
    startTimerHz(30);
}
VoiceEditor::~VoiceEditor()
{
    stopTimer();
    presets.setLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
}
void VoiceEditor::resized()
{
    surface.setTransform(juce::AffineTransform());
    surface.setBounds(0, 0, 1580, 1000);
    surface.setTransform(
        juce::AffineTransform::scale(float(getWidth()) / 1580, float(getHeight()) / 1000));
    presets.setBounds(853, 43, 337, 52);
    aboutPanel.setBounds(440, 180, 700, 600);
    settings.setBounds(1208, 43, 145, 52);
    bypass.setBounds(1365, 43, 128, 52);
    input.setBounds(140, 697, 142, 170);
    output.setBounds(1314, 697, 142, 170);
    inMeter.setBounds(132, 274, 170, 430);
    outMeter.setBounds(1311, 274, 170, 430);
    intensity.setBounds(394, 333, 455, 480);
    delay.setBounds(908, 225, 264, 220);
    doubler.setBounds(908, 438, 264, 220);
    reverb.setBounds(908, 655, 264, 220);
    reduction.setBounds(1170, 238, 68, 590);
    sensitivity.setBounds(124, 249, 190, 38);
}
void VoiceEditor::paint(juce::Graphics &g)
{
    juce::Graphics::ScopedSaveState saved(g);
    g.addTransform(
        juce::AffineTransform::scale(float(getWidth()) / 1580, float(getHeight()) / 1000));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff07182a), 70, 0,
                                           juce::Colour(0xff020914), 1510, 1000, false));
    g.fillAll();

    // Continuous silver rails and chassis reproduce the reference's rack housing.
    auto drawRail = [&g](juce::Rectangle<float> r)
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xfff0f1ed), r.getTopLeft(),
                                               juce::Colour(0xff66727d), r.getTopRight(), false));
        g.fillRect(r);
        g.setColour(juce::Colour(0xffffffff).withAlpha(.42f));
        g.drawLine(r.getX() + 2, r.getY(), r.getX() + 2, r.getBottom(), 1.2f);
        g.setColour(juce::Colour(0xff28333d).withAlpha(.7f));
        g.drawLine(r.getRight() - 2, r.getY(), r.getRight() - 2, r.getBottom(), 1.2f);
        for (int y = int(r.getY()) + 5; y < int(r.getBottom()); y += 8)
        {
            g.setColour(juce::Colour(0xffffffff).withAlpha(y % 16 == 0 ? .13f : .045f));
            g.drawLine(r.getX() + 4, float(y), r.getRight() - 4, float(y - 3), .65f);
        }
    };
    drawRail({8, 14, 64, 972});
    drawRail({1508, 14, 64, 972});
    g.setColour(juce::Colour(0xff0b1520));
    g.fillRect(72, 14, 1436, 972);
    g.setColour(juce::Colour(0xffabb5bb));
    g.drawRect(8.f, 14.f, 1564.f, 972.f, 1.2f);
    // The reference has a short diagonal knurl at the lower inside corners.
    for (int i = 0; i < 8; ++i)
    {
        g.setColour(juce::Colour(0xffffffff).withAlpha(.22f));
        g.drawLine(74 + i * 7.f, 981, 86 + i * 7.f, 969, 1);
        g.drawLine(1494 - i * 7.f, 981, 1506 - i * 7.f, 969, 1);
    }
    vocalis::design::topBar(g, 1580, "VOICE", "VOCAL ENHANCEMENT");
    voiceui::plate(g, {82, 143, 240, 804});
    voiceui::plate(g, {334, 153, 918, 768});
    voiceui::plate(g, {1272, 145, 228, 802});
    for (auto panel : {juce::Rectangle<float>{82, 143, 240, 804},
                       juce::Rectangle<float>{334, 153, 918, 768},
                       juce::Rectangle<float>{1272, 145, 228, 802}})
    {
        juce::Path mask;
        mask.addRoundedRectangle(panel, 9.f);
        g.saveState();
        g.reduceClipRegion(mask);
        for (int y = int(panel.getY()) + 2; y < int(panel.getBottom()); y += 2)
        {
            g.setColour(juce::Colour(0xff8fc9ea).withAlpha(y % 8 == 0 ? .052f : .018f));
            g.drawHorizontalLine(y, panel.getX() + 2, panel.getRight() - 2);
        }
        g.restoreState();
    }
    voiceui::label(g, "INPUT", {103, 161, 198, 39}, 31, voiceui::text,
                   juce::Justification::centred, true);
    voiceui::label(
        g, processor.getTotalNumInputChannels() == 1 ? "MONO IN" : "STEREO IN",
        {103, 202, 198, 24}, 18, voiceui::muted);
    voiceui::label(g, "OUTPUT", {1287, 161, 198, 39}, 31, voiceui::text,
                   juce::Justification::centred, true);
    voiceui::label(
        g, processor.getTotalNumOutputChannels() == 1 ? "MONO OUT" : "STEREO OUT",
        {1287, 202, 198, 24}, 18, voiceui::muted);
    voiceui::label(g, "INTENSITY", {377, 173, 455, 43}, 37, voiceui::text,
                   juce::Justification::centred, true);
    voiceui::label(g, "TONE  /  DYNAMICS  /  CLARITY", {377, 220, 455, 24}, 18, voiceui::muted);
    voiceui::label(g, "PARALLEL RETURNS", {915, 174, 255, 31}, 21, voiceui::muted,
                   juce::Justification::centred, true);
    g.setColour(voiceui::line.withAlpha(.8f));
    g.drawLine(872, 227, 872, 872, 1.4f);
    g.drawLine(1157, 227, 1157, 870, 1.2f);
    voiceui::label(g, "NATURAL", {407, 830, 150, 26}, 19, voiceui::muted,
                   juce::Justification::centredLeft);
    voiceui::label(g, "ASSERTIVE", {685, 830, 150, 26}, 19, voiceui::muted,
                   juce::Justification::centredRight);
    voiceui::label(g, "INPUT GAIN", {149, 866, 125, 24}, 18, voiceui::text);
    voiceui::label(g, "OUTPUT GAIN", {1322, 866, 125, 24}, 18, voiceui::text);
    voiceui::label(g, "PEAK / RMS", {148, 911, 130, 22}, 17, voiceui::muted);
    voiceui::label(g, "PEAK / RMS", {1323, 911, 130, 22}, 17, voiceui::muted);
    voiceui::label(g, "VOCALIS VOICE 1.0.0   |   NATIVE LINUX VST3", {490, 931, 600, 24}, 20,
                   voiceui::muted);
    // Metal screw heads echo the rack plate corners in the reference.
    auto screw = [&g](juce::Point<float> p, float radius)
    {
        g.setColour(juce::Colour(0xff050b12));
        g.fillEllipse(p.x-radius-2, p.y-radius-2, 2*radius+4, 2*radius+4);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffdbe2e5), p.x-radius, p.y-radius,
                                               juce::Colour(0xff596670), p.x+radius, p.y+radius, false));
        g.fillEllipse(p.x-radius, p.y-radius, 2*radius, 2*radius);
        g.setColour(juce::Colour(0xff35434e));
        g.drawEllipse(p.x-radius, p.y-radius, 2*radius, 2*radius, 1.2f);
        g.setColour(juce::Colour(0xff1b2832));
        g.drawLine(p.x-radius*.42f, p.y, p.x+radius*.42f, p.y, radius*.22f);
        g.drawLine(p.x, p.y-radius*.42f, p.x, p.y+radius*.42f, radius*.22f);
        g.setColour(juce::Colour(0xffffffff).withAlpha(.45f));
        g.drawLine(p.x-radius*.35f, p.y-radius*.55f, p.x+radius*.25f, p.y-radius*.78f, .8f);
    };
    for (auto p : {juce::Point<float>{39, 55}, {1540, 55}, {39, 925}, {1540, 925}})
        screw(p, 20);
    for (auto p : {juce::Point<float>{100, 159}, {307, 159}, {100, 929}, {307, 929},
                   {359, 170}, {1226, 170}, {359, 900}, {1226, 900},
                   {1290, 160}, {1483, 160}, {1290, 929}, {1483, 929}})
        screw(p, 10);
}
void VoiceEditor::timerCallback()
{
    auto &m = processor.meters;
    inMeter.setMono(processor.getTotalNumInputChannels() == 1);
    outMeter.setMono(processor.getTotalNumOutputChannels() == 1);
    for (int c = 0; c < 2; ++c)
    {
        inMeter.setLevels(c, m.inputPeak[c].load(), m.inputRms[c].load());
        outMeter.setLevels(c, m.outputPeak[c].load(), m.outputRms[c].load());
    }
    const auto values = processor.parameters.get();
    delay.enable.setButtonText(values.delayOn ? "ON" : "OFF");
    doubler.enable.setButtonText(values.doublerOn ? "ON" : "OFF");
    reverb.enable.setButtonText(values.reverbOn ? "ON" : "OFF");
    outMeter.clipping = m.clip.load();
    reduction.reduction = juce::jlimit(0.0f, 12.0f, m.gr.load());
    sensitivity.state = m.sensitivity.load();
    inMeter.repaint();
    outMeter.repaint();
    reduction.repaint();
    sensitivity.repaint();
    int id = processor.getCurrentProgram() + 1;
    if (processor.matchesCurrentPreset())
    {
        if (presets.getSelectedId() != id)
            presets.setSelectedId(id, juce::dontSendNotification);
    }
    else if (presets.getSelectedId() != 0)
        presets.setText("Custom", juce::dontSendNotification);
}
void VoiceEditor::showAbout()
{
    aboutPanel.setVisible(!aboutPanel.isVisible());
    aboutPanel.toFront(false);
}

void VoiceEditor::showSettings()
{
    juce::PopupMenu menu;
    menu.addSectionHeader(juce::String("Vocalis Voice ") + JucePlugin_VersionString);
    menu.addItem(1, "UI size: Compact (840 x 540)");
    menu.addItem(2, "UI size: Default (960 x 617)");
    menu.addItem(3, "UI size: Large (1200 x 771)");
    menu.addSeparator();
    menu.addItem(4, "About / signal flow");
    juce::Component::SafePointer<VoiceEditor> safe(this);
    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetComponent(&settings),
        [safe](int id)
        {
            if (!safe)
                return;
            if (id >= 1 && id <= 3)
            {
                const int widths[]{840, 960, 1200};
                const int heights[]{540, 617, 771};
                safe->setSize(widths[id - 1], heights[id - 1]);
            }
            if (id == 4)
                safe->showAbout();
        });
}
juce::AudioProcessorEditor *VoiceProcessor::createEditor()
{
    return new VoiceEditor(*this);
}
