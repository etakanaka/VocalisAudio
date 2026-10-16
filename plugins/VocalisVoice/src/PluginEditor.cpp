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
    setResizeLimits(1120, 720, 1960, 1260);
    getConstrainer()->setFixedAspectRatio(1400.0 / 900.0);
    setSize(1400, 900);
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
        s->setSliderStyle(juce::Slider::LinearVertical);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 86, 26);
        s->setTextValueSuffix(" dB");
        s->setNumDecimalPlacesToDisplay(1);
        s->setDoubleClickReturnValue(true, 0);
        s->setColour(juce::Slider::trackColourId, voiceui::purple);
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
    setLookAndFeel(nullptr);
}
void VoiceEditor::resized()
{
    surface.setTransform(juce::AffineTransform());
    surface.setBounds(0, 0, 1400, 900);
    surface.setTransform(
        juce::AffineTransform::scale(float(getWidth()) / 1400, float(getHeight()) / 900));
    presets.setBounds(972, 42, 166, 37);
    aboutPanel.setBounds(350, 150, 700, 600);
    settings.setBounds(1147, 42, 103, 37);
    bypass.setBounds(1259, 42, 105, 37);
    input.setBounds(223, 280, 96, 500);
    output.setBounds(1283, 280, 96, 500);
    inMeter.setBounds(28, 264, 194, 516);
    outMeter.setBounds(1088, 264, 194, 516);
    intensity.setBounds(347, 234, 445, 470);
    delay.setBounds(810, 214, 206, 173);
    doubler.setBounds(810, 414, 206, 173);
    reverb.setBounds(810, 614, 206, 173);
    reduction.setBounds(1020, 260, 44, 520);
    sensitivity.setBounds(53, 208, 230, 36);
}
void VoiceEditor::paint(juce::Graphics &g)
{
    juce::Graphics::ScopedSaveState saved(g);
    g.addTransform(
        juce::AffineTransform::scale(float(getWidth()) / 1400, float(getHeight()) / 900));
    g.setGradientFill(juce::ColourGradient(voiceui::navy.brighter(.1f), 0, 0,
                                           juce::Colour(0xff080e19), 1400, 900, false));
    g.fillAll();
    vocalis::design::topBar(g, 1400, "VOICE", "ALL-IN-ONE VOCAL ENHANCER");
    voiceui::plate(g, {18, 114, 303, 760});
    voiceui::plate(g, {333, 114, 734, 760});
    voiceui::plate(g, {1079, 114, 303, 760});
    voiceui::label(g, "Input", {48, 145, 225, 29}, 22, voiceui::muted,
                   juce::Justification::centredLeft, true);
    voiceui::label(
        g, processor.getTotalNumInputChannels() == 1 ? "MONO IN / dBFS" : "STEREO IN / dBFS",
        {49, 180, 220, 17}, 11, voiceui::muted, juce::Justification::centredLeft);
    voiceui::label(g, "Output", {1133, 145, 225, 29}, 22, voiceui::muted,
                   juce::Justification::centredLeft, true);
    voiceui::label(
        g, processor.getTotalNumOutputChannels() == 1 ? "MONO OUT / dBFS" : "STEREO OUT / dBFS",
        {1134, 180, 220, 17}, 11, voiceui::muted, juce::Justification::centredLeft);
    voiceui::label(g, "INTENSITY", {370, 165, 404, 34}, 26, voiceui::text,
                   juce::Justification::centred, true);
    voiceui::label(g, "TONE / DYNAMICS / CLARITY", {370, 204, 404, 17}, 11, voiceui::muted);
    voiceui::label(g, "PARALLEL RETURNS", {810, 165, 206, 28}, 15, voiceui::muted,
                   juce::Justification::centred, true);
    g.setColour(voiceui::line.withAlpha(.55f));
    g.drawLine(803, 214, 803, 804, .8f);
    voiceui::label(g, "NATURAL", {367, 733, 110, 18}, 11, voiceui::muted);
    voiceui::label(g, "ASSERTIVE", {673, 733, 110, 18}, 11, voiceui::muted);
    voiceui::label(g, "Input Gain", {223, 242, 96, 24}, 13, voiceui::muted);
    voiceui::label(g, "Output Gain", {1283, 242, 96, 24}, 13, voiceui::muted);
    voiceui::label(g, "PEAK / RMS", {46, 815, 246, 18}, 11, voiceui::muted);
    voiceui::label(g, "PEAK / RMS", {1130, 815, 226, 18}, 11, voiceui::muted);
    voiceui::label(g, "VOCALIS VOICE 1.0.0 / NATIVE LINUX VST3", {365, 831, 450, 18}, 10,
                   voiceui::muted, juce::Justification::centredLeft);
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
    menu.addItem(1, "UI size: 80%");
    menu.addItem(2, "UI size: 100%");
    menu.addItem(3, "UI size: 125%");
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
                float scale = id == 1 ? .8f : id == 2 ? 1.0f : 1.25f;
                safe->setSize(int(1400 * scale), int(900 * scale));
            }
            if (id == 4)
                safe->showAbout();
        });
}
juce::AudioProcessorEditor *VoiceProcessor::createEditor()
{
    return new VoiceEditor(*this);
}
