/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine.h"
static_assert(std::atomic<float>::is_always_lock_free && std::atomic<int>::is_always_lock_free && std::atomic<bool>::is_always_lock_free);
class Vocalis final : public juce::AudioProcessor, private juce::Timer {
public:
 Vocalis(); ~Vocalis() override;
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 const juce::String getName() const override {return "Vocalis Rider Live";}
 bool acceptsMidi() const override {return false;} bool producesMidi() const override {return false;}
 double getTailLengthSeconds() const override {return 0;}
 int getNumPrograms() override {return 8;} int getCurrentProgram() override;
 void setCurrentProgram(int) override; const juce::String getProgramName(int) override;
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorParameter* getBypassParameter() const override {return state.getParameter("bypass");}
 juce::AudioProcessorValueTreeState state;
 float value(const char* id)const{return state.getRawParameterValue(id)->load();}
 void set(const char* id,float v){auto* p=state.getParameter(id);p->setValueNotifyingHost(p->convertTo0to1(v));}
 std::atomic<float> meterGain{0},meterInput{-120},meterPeak{-120},meterMusic{-120};
 std::atomic<bool> activity{false};std::atomic<int> dropped{0};
 struct Point {double time;float db;};
 bool exportCsv(const juce::File&);void clearCapture();
 const std::vector<Point>& capture()const{return recorded;}
private:
 void processAudio(juce::AudioBuffer<float>&,juce::MidiBuffer&,bool);
 static juce::AudioProcessorValueTreeState::ParameterLayout parameters();
 vr::Engine engine;
 std::array<std::atomic<float>*,14> raw{};
 juce::AbstractFifo fifo{8192};std::array<Point,8192> queue{};std::vector<Point> recorded;
 double sampleRate=48000,timeline=0;int untilCapture=0;bool gesture=false;
 void timerCallback() override;void drain();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Vocalis)
};
