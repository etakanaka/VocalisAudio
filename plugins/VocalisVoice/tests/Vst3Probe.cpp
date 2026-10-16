/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "EditorOwner.h"
#include <iostream>
#include <juce_audio_utils/juce_audio_utils.h>
#include <stdexcept>
juce::Image captureX11(void *, int, int);
bool nativeDisplayAvailable();
void require(bool v, const char *m)
{
    if (!v)
        throw std::runtime_error(m);
}
int main(int argc, char **argv)
{
    std::cout << std::unitbuf;
    juce::ScopedJuceInitialiser_GUI init;
    try
    {
        require(argc >= 3, "usage: Vst3Probe /path/to/bundle /output/screenshot.png");
        juce::VST3PluginFormat format;
        juce::OwnedArray<juce::PluginDescription> descriptions;
        format.findAllTypesForFile(descriptions, argv[1]);
        require(descriptions.size() == 1, "VST3 discovery count");
        std::cout << "PASS VST3 discovery: " << descriptions[0]->name
                  << " id=" << descriptions[0]->createIdentifierString() << '\n';
        juce::String error;
        auto instance = format.createInstanceFromDescription(*descriptions[0], 48000, 256, error);
        require(instance != nullptr, error.toRawUTF8());
        for (auto *p : instance->getParameters())
            std::cout << "HOST PARAM " << p->getName(80) << " value=" << p->getValue() << '\n';
        require(instance->getParameters().size() == 11,
                "ten audio controls plus wrapper program parameter");
        const char *names[]{"Input Gain",     "Intensity",     "Delay Enable",  "Delay Level",
                            "Doubler Enable", "Doubler Level", "Reverb Enable", "Reverb Level",
                            "Output Gain",    "Bypass",        "Program"};
        for (int i = 0; i < 11; ++i)
            require(instance->getParameters()[i]->getName(80) == names[i],
                    "VST3 host parameter name/order");
        std::cout << "PASS VST3 host parameters: " << instance->getParameters().size() << '\n';
        for (auto layout : {std::pair{1, 1}, std::pair{1, 2}, std::pair{2, 2}})
        {
            auto buses = instance->getBusesLayout();
            buses.inputBuses.set(0, layout.first == 1 ? juce::AudioChannelSet::mono()
                                                      : juce::AudioChannelSet::stereo());
            buses.outputBuses.set(0, layout.second == 1 ? juce::AudioChannelSet::mono()
                                                        : juce::AudioChannelSet::stereo());
            require(instance->setBusesLayout(buses), "VST3 host layout rejected");
            instance->prepareToPlay(48000, 256);
            juce::AudioBuffer<float> b(layout.second, 256);
            juce::MidiBuffer m;
            double energy = 0;
            for (int block = 0; block < 200; ++block)
            {
                for (int c = 0; c < layout.second; ++c)
                    for (int i = 0; i < 256; ++i)
                        b.setSample(c, i, .15f * std::sin(float(block * 256 + i) * .033f));
                instance->processBlock(b, m);
                for (int c = 0; c < layout.second; ++c)
                    for (int i = 0; i < 256; ++i)
                    {
                        float v = b.getSample(c, i);
                        require(std::isfinite(v), "nonfinite host output");
                        energy += v * v;
                    }
            }
            require(energy > 1, "silent VST3");
            instance->releaseResources();
            std::cout << "PASS VST3 processing layout " << layout.first << " -> " << layout.second
                      << " (editor closed)\n";
        }
        // Host parameter writes are queued by the VST3 host. Process them before
        // measuring audio or serializing, and use values on the declared step grid.
        const float dryTrim[]{.5f, 0, 0, 0, 0, 0, 0, 0, .625f, 0};
        for (int i = 0; i < 10; ++i)
            instance->getParameters()[i]->setValueNotifyingHost(dryTrim[i]);
        instance->prepareToPlay(48000, 64);
        juce::AudioBuffer<float> trimAudio(2, 64);
        juce::MidiBuffer trimMidi;
        for (int block = 0; block < 100; ++block)
        {
            for (int c = 0; c < 2; ++c)
                for (int i = 0; i < 64; ++i)
                    trimAudio.setSample(c, i, .1f);
            instance->processBlock(trimAudio, trimMidi);
        }
        require(std::abs(trimAudio.getSample(0, 63) - .1f * std::pow(10.0f, 6.0f / 20)) < 1.e-5f,
                "VST3 automated trim did not change measured audio");
        instance->releaseResources();
        std::cout << "PASS measured VST3 automation: intensity 0, all returns off, output +6 dB\n";
        if (argc >= 4)
        {
            juce::OwnedArray<juce::PluginDescription> oldTypes;
            format.findAllTypesForFile(oldTypes, argv[3]);
            require(oldTypes.size() == 1, "checkpoint VST3 discovery");
            auto old = format.createInstanceFromDescription(*oldTypes[0], 48000, 256, error);
            require(old != nullptr, "checkpoint VST3 load");
            auto flush = [](juce::AudioPluginInstance &processor)
            {
                processor.prepareToPlay(48000, 64);
                juce::AudioBuffer<float> b(2, 64);
                b.clear();
                juce::MidiBuffer midi;
                processor.processBlock(b, midi);
                processor.releaseResources();
            };
            old->setCurrentProgram(5);
            flush(*old);
            const float values[]{.6f, .73f, 1, .4f, 1, .65f, 1, .5f, .375f, 0};
            std::array<float, 10> expected{};
            for (int i = 0; i < 10; ++i)
            {
                old->getParameters()[i]->setValueNotifyingHost(values[i]);
                expected[size_t(i)] = old->getParameters()[i]->getValue();
            }
            flush(*old);
            juce::MemoryBlock legacy;
            old->getStateInformation(legacy);
            old.reset();
            instance->setStateInformation(legacy.getData(), int(legacy.getSize()));
            for (int i = 0; i < 10; ++i)
            {
                std::cout << "LEGACY " << i << " expected=" << expected[size_t(i)]
                          << " actual=" << instance->getParameters()[i]->getValue() << '\n';
                require(std::abs(instance->getParameters()[i]->getValue() - expected[size_t(i)]) <
                            1.e-6f,
                        "legacy parameter state mismatch");
            }
            require(instance->getCurrentProgram() == 5, "legacy preset metadata");
            std::cout << "PASS checkpoint 1.0.0 VST3 state restored by audited VST3, all ten "
                         "parameters and preset metadata\n";
        }
        auto *param = instance->getParameters()[1];
        param->beginChangeGesture();
        param->setValueNotifyingHost(.75f);
        param->endChangeGesture();
        instance->prepareToPlay(48000, 64);
        trimAudio.clear();
        instance->processBlock(trimAudio, trimMidi);
        juce::MemoryBlock state;
        instance->getStateInformation(state);
        param->setValueNotifyingHost(.25f);
        trimAudio.clear();
        instance->processBlock(trimAudio, trimMidi);
        instance->setStateInformation(state.getData(), int(state.getSize()));
        require(std::abs(param->getValue() - .75f) < .001, "VST3 state restoration");
        std::cout << "PASS VST3 automation and state roundtrip\n";
        if (nativeDisplayAvailable())
        {
            auto editor = ownEditor(*instance);
            require(editor != nullptr, "no editor");
            juce::Component window;
            window.setOpaque(true);
            window.setBounds(40, 40, editor->getWidth(), editor->getHeight());
            window.addAndMakeVisible(*editor);
            editor->setBounds(window.getLocalBounds());
            window.addToDesktop(0);
            window.setVisible(true);
            juce::MessageManager::getInstance()->runDispatchLoopUntil(500);
            auto image = captureX11(window.getPeer()->getNativeHandle(), window.getWidth(),
                                    window.getHeight());
            require(image.isValid(), "native GUI capture");
            juce::File target(argv[2]);
            target.deleteFile();
            auto stream = target.createOutputStream();
            require(stream != nullptr, "screenshot file");
            juce::PNGImageFormat png;
            require(png.writeImageToStream(image, *stream), "PNG encode");
            stream.reset();
            std::cout << "PASS Actual loaded VST3 editor screenshot " << target.getFullPathName()
                      << '\n';
            // Resize the real editor and repaint to exercise component layout at minimum scale.
            window.setSize(1120, 720);
            editor->setBounds(window.getLocalBounds());
            juce::MessageManager::getInstance()->runDispatchLoopUntil(300);
            auto small = captureX11(window.getPeer()->getNativeHandle(), window.getWidth(),
                                    window.getHeight());
            auto smallFile = target.getSiblingFile("gui-80-percent.png");
            smallFile.deleteFile();
            auto smallStream = smallFile.createOutputStream();
            require(smallStream && png.writeImageToStream(small, *smallStream),
                    "scaled screenshot");
            window.setVisible(false);
            editor.reset();
            window.removeFromDesktop();
            std::cout << "PASS VST3 editor resize and destruction\n";
        }
        else
            std::cout << "UNVERIFIED VST3 native editor attachment: XOpenDisplay failed; use "
                         "GuiTests for the compiled editor render\n";
        instance.reset();
        for (int i = 0; i < 4; ++i)
        {
            auto next = format.createInstanceFromDescription(*descriptions[0], 44100, 64, error);
            require(next != nullptr, "reload failed");
            next->setStateInformation(state.getData(), int(state.getSize()));
            next->prepareToPlay(44100, 64);
            next->releaseResources();
        }
        std::cout << "PASS VST3 repeated load/unload\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FAIL " << e.what() << '\n';
        return 1;
    }
}
