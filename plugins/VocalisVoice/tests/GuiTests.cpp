/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "EditorOwner.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>
#include <thread>
void require(bool v, const char *m)
{
    if (!v)
        throw std::runtime_error(m);
}
void collect(juce::Component &c, std::vector<juce::Slider *> &sliders,
             std::vector<juce::Button *> &toggles)
{
    if (auto *s = dynamic_cast<juce::Slider *>(&c))
        sliders.push_back(s);
    if (auto *b = dynamic_cast<juce::Button *>(&c))
        if (b->getClickingTogglesState())
            toggles.push_back(b);
    for (auto *child : c.getChildren())
        collect(*child, sliders, toggles);
}
void save(juce::AudioProcessorEditor &editor, const juce::File &target)
{
    target.getParentDirectory().createDirectory();
    auto image = editor.createComponentSnapshot(editor.getLocalBounds(), true, 1.0f);
    require(image.isValid(), "invalid snapshot");
    bool varied = false;
    auto base = image.getPixelAt(0, 0);
    for (int y = 0; y < image.getHeight(); y += 10)
        for (int x = 0; x < image.getWidth(); x += 10)
            varied |= image.getPixelAt(x, y) != base;
    require(varied, "blank snapshot");
    target.deleteFile();
    auto stream = target.createOutputStream();
    juce::PNGImageFormat png;
    require(stream && png.writeImageToStream(image, *stream), "PNG failed");
}
int main(int argc, char **argv)
{
    std::cout << std::unitbuf;
    juce::ScopedJuceInitialiser_GUI init;
    try
    {
        require(argc == 2, "usage: GuiTests /artifact/directory");
        VoiceProcessor p;
        p.prepareToPlay(48000, 256);
        juce::AudioBuffer<float> b(2, 256);
        juce::MidiBuffer m;
        for (int k = 0; k < 150; ++k)
        {
            for (int c = 0; c < 2; ++c)
                for (int i = 0; i < 256; ++i)
                    b.setSample(c, i, .08f * std::sin(float(k * 256 + i) * .04f));
            p.processBlock(b, m);
        }
        auto editor = ownEditor(p);
        require(editor != nullptr, "editor");
        require(editor->getWidth() == 960 && editor->getHeight() == 617,
                "host-facing default editor size");
        editor->setVisible(true);
        std::vector<juce::Slider *> sliders;
        std::vector<juce::Button *> toggles;
        collect(*editor, sliders, toggles);
        require(sliders.size() == 6 && toggles.size() == 4, "interactive controls missing");
        // The child traversal order is stable: intensity, input/output, then effect levels.
        sliders[0]->setValue(63.4, juce::sendNotificationSync);
        require(std::abs(p.parameters.get().intensity - 63.4f) < .06f, "intensity attachment");
        sliders[0]->keyPressed(juce::KeyPress(juce::KeyPress::upKey));
        require(p.parameters.get().intensity > 63.4f, "keyboard adjustment");
        sliders[1]->setValue(4.2, juce::sendNotificationSync);
        sliders[2]->setValue(-3.1, juce::sendNotificationSync);
        sliders[3]->setValue(-16, juce::sendNotificationSync);
        sliders[4]->setValue(-14, juce::sendNotificationSync);
        sliders[5]->setValue(-18, juce::sendNotificationSync);
        auto s = p.parameters.get();
        require(std::abs(s.input - 4.2f) < .06f && std::abs(s.output + 3.1f) < .06f &&
                    std::abs(s.delay + 16) < .06f && std::abs(s.doubler + 14) < .06f &&
                    std::abs(s.reverb + 18) < .06f,
                "slider attachments");
        const int readoutWidths[]{270, 96, 96, 146, 146, 146};
        const int readoutHeights[]{59, 31, 31, 39, 39, 39};
        for (size_t index = 0; index < sliders.size(); ++index)
        {
            auto *slider = sliders[index];
            require(slider->isDoubleClickReturnEnabled(), "double-click reset missing");
            require(slider->getTextBoxWidth() > 0, "numeric field missing");
            for (auto *child : slider->getChildren())
                if (auto *label = dynamic_cast<juce::Label *>(child))
                    require(label->getWidth() == readoutWidths[index] &&
                                label->getHeight() == readoutHeights[index],
                            "reference-styled numeric field geometry");
        }
        std::cout << "PASS compiled GUI slider attachments, keyboard, numeric fields, reset "
                     "configuration\n";
        const int toggleIds[]{2, 4, 6, 9};
        for (size_t i = 0; i < toggles.size(); ++i)
        {
            require(toggles[i]->getName() == voiceparams::ids[toggleIds[i]], "toggle identity");
            for (bool enabled : {false, true})
            {
                toggles[i]->setToggleState(enabled, juce::sendNotificationSync);
                require((p.state.getRawParameterValue(voiceparams::ids[toggleIds[i]])->load() >
                         .5f) == enabled,
                        "toggle attachment");
            }
        }
        toggles.back()->setToggleState(false, juce::sendNotificationSync);
        std::cout << "PASS all three effect enables and Bypass attachments\n";
        juce::ComboBox *preset = nullptr;
        for (auto *child : editor->getChildren())
            for (auto *nested : child->getChildren())
                if (auto *combo = dynamic_cast<juce::ComboBox *>(nested))
                    preset = combo;
        require(preset != nullptr, "preset selector");
        juce::Label *presetLabel = nullptr;
        for (auto *child : preset->getChildren())
            if (auto *label = dynamic_cast<juce::Label *>(child))
                presetLabel = label;
        require(presetLabel != nullptr, "preset display label");
        auto presetFont = preset->getLookAndFeel().getLabelFont(*presetLabel);
        const float displayedHeight = presetFont.getHeight() * editor->getHeight() / 1000.f;
        require(displayedHeight >= 13.f && displayedHeight <= 14.f,
                "preset font at default displayed size");
        require(preset->getLookAndFeel().getPopupMenuFont().getHeight() == presetFont.getHeight(),
                "preset menu and display font consistency");
        for (int i = 0; i < p.getNumPrograms(); ++i)
            require(juce::GlyphArrangement::getStringWidth(presetFont, p.getProgramName(i)) <=
                        presetLabel->getWidth() - presetLabel->getBorderSize().getLeftAndRight(),
                    "factory preset name fits without compression or clipping");
        require(preset->getRootMenu()->getNumItems() == p.getNumPrograms(),
                "preset dropdown menu entries");
        std::cout << "PASS preset font size, factory-name fit and menu font/entries\n";
        preset->setSelectedId(2, juce::sendNotificationSync);
        require(p.getCurrentProgram() == 1 && p.matchesCurrentPreset(), "preset selector callback");
        require(std::abs(p.parameters.get().input - 4.2f) < .06f &&
                    std::abs(p.parameters.get().output + 3.1f) < .06f,
                "GUI preset changed trim");
        std::cout << "PASS GUI preset selection and trim preservation\n";
        auto *param = p.state.getParameter(voiceparams::ids[1]);
        param->setValueNotifyingHost(.75f);
        require(std::abs(sliders[0]->getValue() - 75) < .06, "host parameter feedback");
        std::cout << "PASS compiled GUI host feedback\n";
        // Exercise JUCE's real editable readout and double-click handler, offscreen.
        juce::Label *number = nullptr;
        for (auto *child : sliders[0]->getChildren())
            if (auto *l = dynamic_cast<juce::Label *>(child))
                number = l;
        require(number != nullptr, "numeric label");
        // Deliver the same Label text-change callback used by an edit commit.
        // Opening TextEditor itself needs an accessible native desktop; this session
        // cannot connect to X11, so physical typing/focus stays a manual check.
        number->setText("42.7", juce::sendNotificationSync);
        require(std::abs(p.parameters.get().intensity - 42.7f) < .06f, "numeric edit not applied");
        auto now = juce::Time::getCurrentTime();
        juce::MouseEvent reset(juce::Desktop::getInstance().getMainMouseSource(), {100, 100},
                               juce::ModifierKeys{}, 1, 0, 0, 0, 0, sliders[0], sliders[0], now,
                               {100, 100}, now, 2, false);
        sliders[0]->mouseDoubleClick(reset);
        require(std::abs(p.parameters.get().intensity - 50) < .06f,
                "double-click reset not applied");
        param->setValueNotifyingHost(.75f);
        std::cout << "PASS numeric readout parse callback and double-click reset handler (native "
                     "typing UNVERIFIED)\n";
        sliders[3]->setValue(-16, juce::sendNotificationSync);
        sliders[4]->setValue(-14, juce::sendNotificationSync);
        sliders[5]->setValue(-18, juce::sendNotificationSync);
        toggles[0]->setToggleState(false, juce::sendNotificationSync);
        toggles[1]->setToggleState(true, juce::sendNotificationSync);
        toggles[2]->setToggleState(true, juce::sendNotificationSync);
        for (int k = 0; k < 150; ++k)
        {
            for (int c = 0; c < 2; ++c)
                for (int i = 0; i < 256; ++i)
                    b.setSample(c, i, .23f * std::sin(float(k * 256 + i) * .04f));
            p.processBlock(b, m);
        }
        juce::Thread::sleep(60);
        juce::Timer::callPendingTimersSynchronously();
        auto dir = juce::File(argv[1]);
        dir.createDirectory();
        {
            VoiceProcessor defaultView;
            auto defaultEditor = ownEditor(defaultView);
            defaultEditor->setVisible(true);
            save(*defaultEditor, dir.getChildFile("gui-preset-default-960x617.png"));
        }
        // Capture a reference-matched factory-preset view with live, non-clipping meter data.
        VoiceProcessor referenceView;
        referenceView.setCurrentProgram(9); // Vocal Polish, as shown in newdesign.png
        referenceView.prepareToPlay(48000, 256);
        for (int k = 0; k < 150; ++k)
        {
            for (int c = 0; c < 2; ++c)
                for (int i = 0; i < 256; ++i)
                    b.setSample(c, i, .08f * std::sin(float(k * 256 + i) * .04f));
            referenceView.processBlock(b, m);
        }
        auto referenceEditor = ownEditor(referenceView);
        referenceEditor->setVisible(true);
        require(referenceEditor->getWidth() == 960 && referenceEditor->getHeight() == 617,
                "reference editor default size");
        juce::Timer::callPendingTimersSynchronously();
        save(*referenceEditor, dir.getChildFile("gui-960x617.png"));
        referenceEditor->setSize(840, 540);
        require(referenceEditor->getWidth() == 840 && referenceEditor->getHeight() == 540,
                "compact editor size");
        save(*referenceEditor, dir.getChildFile("gui-840x540.png"));
        referenceEditor->setSize(1580, 1000);
        save(*referenceEditor, dir.getChildFile("gui-reference-layout.png"));
        referenceEditor.reset();
        save(*editor, dir.getChildFile("gui.png"));
        editor->setSize(1200, 771);
        save(*editor, dir.getChildFile("gui-large-1200x771.png"));
        std::cout << "PASS compiled production editor snapshots at default, compact, and large sizes\n";
        editor.reset();
        std::atomic<int> callbackCount{0}, callbackErrors{0};
        std::jthread playing(
            [&](std::stop_token stop)
            {
                juce::AudioBuffer<float> audio(2, 128);
                juce::MidiBuffer midi;
                while (!stop.stop_requested())
                {
                    int block = callbackCount.load();
                    for (int c = 0; c < 2; ++c)
                        for (int n = 0; n < 128; ++n)
                            audio.setSample(c, n, .12f * std::sin(float(block * 128 + n) * .03f));
                    p.processBlock(audio, midi);
                    for (int c = 0; c < 2; ++c)
                        for (int n = 0; n < 128; ++n)
                            if (!std::isfinite(audio.getSample(c, n)))
                                ++callbackErrors;
                    ++callbackCount;
                }
            });
        for (int repeat = 0; repeat < 20; ++repeat)
        {
            auto next = ownEditor(p);
            next->setSize(repeat % 2 ? 840 : 1200, repeat % 2 ? 540 : 771);
            next->setVisible(true);
            auto image = next->createComponentSnapshot(next->getLocalBounds(), true, .25f);
            require(image.isValid(), "repeated editor snapshot");
        }
        playing.request_stop();
        playing.join();
        require(callbackCount > 0 && callbackErrors == 0, "concurrent GUI/audio stability");
        std::cout << "PASS concurrent audio during editor cycles: " << callbackCount
                  << " callbacks, zero numerical errors\n";
        std::cout << "PASS 20 repeated editor create/resize/paint/destroy cycles\n";
        for (auto io : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
        {
            VoiceProcessor channels;
            auto layout = channels.getBusesLayout();
            layout.inputBuses.set(0, io.first == 1 ? juce::AudioChannelSet::mono()
                                                   : juce::AudioChannelSet::stereo());
            layout.outputBuses.set(0, io.second == 1 ? juce::AudioChannelSet::mono()
                                                     : juce::AudioChannelSet::stereo());
            require(channels.setBusesLayout(layout), "GUI bus configuration");
            channels.state.getParameter(voiceparams::ids[8])->setValueNotifyingHost(1);
            channels.prepareToPlay(48000, 64);
            juce::AudioBuffer<float> loud(io.second, 64);
            juce::MidiBuffer empty;
            for (int block = 0; block < 100; ++block)
            {
                for (int c = 0; c < io.second; ++c)
                    for (int n = 0; n < 64; ++n)
                        loud.setSample(c, n, .3f * std::sin(float(block * 64 + n) * .13f));
                channels.processBlock(loud, empty);
            }
            auto view = ownEditor(channels);
            int meters = 0;
            for (auto *child : view->getChildren())
                for (auto *nested : child->getChildren())
                    if (auto *meter = dynamic_cast<LevelMeter *>(nested))
                    {
                        bool expected =
                            meter->getName() == "inputMeter" ? io.first == 1 : io.second == 1;
                        require(meter->isMono() == expected,
                                "mono GUI meter incorrectly shows stereo");
                        ++meters;
                    }
            require(meters == 2, "meter widgets missing");
            require(channels.meters.clip.load(), "real clipping fixture did not trigger indicator");
            save(*view, dir.getChildFile("gui-layout-" + juce::String(io.first) + "-" +
                                         juce::String(io.second) + ".png"));
        }
        std::cout << "PASS mono / mono-to-stereo / stereo GUI meter configuration\n";
        editor=ownEditor(p);
        auto* voiceEditor=dynamic_cast<VoiceEditor*>(editor.get());
        require(voiceEditor != nullptr, "Voice editor type");
        voiceEditor->showAbout();
        juce::Component* about=nullptr;
        for(auto* child:editor->getChildren())
            for(auto* panel:child->getChildren())
                if(panel->getName()=="aboutPanel")about=panel;
        require(about && about->isVisible(), "About opens");
        bool credits=false;
        for(auto* child:about->getChildren())
            if(auto* label=dynamic_cast<juce::Label*>(child))
                credits=label->getText().contains("Created by Enos Takanaka")
                    && label->getText().contains("AGPL-3.0-or-later")
                    && label->getText().contains(JucePlugin_VersionString);
        require(credits,"About creator, verified licence and actual version");
        save(*editor, dir.getChildFile("author-attribution/about.png"));
        for(auto* child:about->getChildren())
            if(auto* button=dynamic_cast<juce::Button*>(child))button->onClick();
        require(!about->isVisible(), "About closes");
        std::cout << "PASS About opens, credits match and Close works\n";
        std::cout << "PASS compiled editor destruction\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FAIL " << e.what() << '\n';
        return 1;
    }
}
