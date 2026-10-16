/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include "VocalisLookAndFeel.h"
namespace voiceui {
using namespace juce;
class AboutPanel final : public Component {
    Label content;
    TextButton close{"CLOSE"};
public:
    AboutPanel()
    {
        setName("aboutPanel");
        content.setText(String::fromUTF8("VOCALIS AUDIO\nVocalis Voice\nVersion ") + JucePlugin_VersionString
            + String::fromUTF8("\n\nCreated by Enos Takanaka\nDeveloped with AI assistance using OpenAI Codex, C++ and JUCE.\nNative Linux VST3 | REAPER\n\nCopyright © 2026 Enos Takanaka and applicable contributors.\nOpen-source licence: AGPL-3.0-or-later\n\nGitHub:\nhttps://github.com/etakanaka/VocalisAudio\n\nIndependent vocal DSP\nInput > Tone > Dynamics > Presence > Harmonics\nParallel Delay / Doubler / Reverb > Output\nNo lookahead, no automatic makeup, no limiter.\nDouble-click controls to reset; type values for precision.\nFactory presets preserve input and output trim."), dontSendNotification);
        content.setFont(FontOptions(17.f));
        content.setColour(Label::textColourId, voiceui::text);
        content.setJustificationType(Justification::centred);
        addAndMakeVisible(content); addAndMakeVisible(close);
        close.onClick = [this]{ setVisible(false); };
    }
    void paint(Graphics& g) override { voiceui::plate(g, getLocalBounds().toFloat()); }
    void resized() override
    {
        content.setBounds(16, 12, getWidth()-32, getHeight()-70);
        close.setBounds(getWidth()/2-50, getHeight()-48, 100, 32);
    }
};

}
