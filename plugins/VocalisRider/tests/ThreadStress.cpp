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
#include <thread>
#include <iostream>
namespace juce::detail {bool dispatchNextMessageOnSystemQueue(bool);}
int main(){juce::ScopedJuceInitialiser_GUI init;Vocalis p,q;p.prepareToPlay(96000,1024);q.prepareToPlay(96000,1024);std::atomic<bool> stop{false},finite{true};std::atomic<int> blocks{0};
 std::thread audio([&]{juce::AudioBuffer<float>b(2,1024);juce::MidiBuffer midi;int k=0;while(!stop.load()){b.setSize(2,(k%2)?32:1024,false,false,true);for(int c=0;c<2;++c)for(int n=0;n<b.getNumSamples();++n)b.setSample(c,n,.05f*std::sin(float(k+n)*.04f));p.processBlock(b,midi);for(int c=0;c<2;++c)for(int n=0;n<b.getNumSamples();++n)if(!std::isfinite(b.getSample(c,n)))finite=false;++k;++blocks;}});
 std::unique_ptr<juce::AudioProcessorEditor> e;for(int i=0;i<1000;++i){p.set("target",-30+float(i%20));p.set("output",float(i%25-12));p.set("automation",float(i%3));if(i%3==2)p.set("rider",float(i%25-12));if(i%40==0)e.reset(p.createEditor());if(i%40==20)e.reset();if(i%50==0){juce::MemoryBlock state;p.getStateInformation(state);q.setStateInformation(state.getData(),int(state.getSize()));}juce::detail::dispatchNextMessageOnSystemQueue(true);juce::Thread::sleep(1);}
 stop=true;audio.join();e.reset();std::cout<<(finite&&blocks>100?"PASS ":"FAIL ")<<"concurrent audio / parameters / editor lifecycle / state, "<<blocks.load()<<" blocks\n";return finite&&blocks>100?0:1;}
