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
#include <iostream>
#include <cstdlib>
namespace juce::detail { bool dispatchNextMessageOnSystemQueue(bool); }
void pump(int ms){auto end=juce::Time::getMillisecondCounter()+juce::uint32(ms);while(juce::Time::getMillisecondCounter()<end){juce::detail::dispatchNextMessageOnSystemQueue(true);juce::Thread::sleep(1);}}
extern thread_local bool watchAllocations;
extern thread_local int allocationCount;
void require(bool b,const char* s){std::cout<<(b?"PASS ":"FAIL ")<<s<<std::endl;if(!b)std::exit(1);}
int main(int argc,char** argv){juce::ScopedJuceInitialiser_GUI init;juce::File::getCurrentWorkingDirectory().getChildFile("artifacts").createDirectory();Vocalis p;
 p.setCurrentProgram(5);p.set("target",-25);p.set("rider",4);p.set("automation",2);p.set("output",-2);
 juce::MemoryBlock state;p.getStateInformation(state);Vocalis q;q.setStateInformation(state.getData(),int(state.getSize()));
 for(const char* id:{"target","spill","vocal","music","rangeMin","rangeMax","idle","rider","output","attack","automation","bypass","preset"})require(q.value(id)==p.value(id),"all persisted parameter values round trip");
 require(q.value("target")==-25&&q.value("rider")==4&&q.value("automation")==2&&q.getCurrentProgram()==5&&q.value("output")==-2,"parameter and preset persistence");
 for(int preset=0;preset<8;++preset){p.setCurrentProgram(preset);require(p.getCurrentProgram()==preset&&p.value("automation")==0,"factory preset recall");}
 for(int channels:{1,2}){auto layout=p.getBusesLayout();layout.inputBuses.set(0,channels==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo());layout.outputBuses.set(0,layout.inputBuses[0]);require(p.setBusesLayout(layout),"mono/stereo layout accepted");p.prepareToPlay(48000,128);juce::AudioBuffer<float> audio(channels,128);juce::MidiBuffer midi;audio.clear();p.processBlock(audio,midi);require(audio.getMagnitude(0,128)==0,"processor silence path");}
 {auto l=p.getBusesLayout();l.inputBuses.set(1,juce::AudioChannelSet::stereo());require(p.setBusesLayout(l),"optional music bus enabled");juce::AudioBuffer<float>b(4,128);b.clear();juce::MidiBuffer m;p.prepareToPlay(48000,128);p.processBlock(b,m);require(std::isfinite(p.meterGain.load()),"sidechain processor path");}
 p.setCurrentProgram(0);auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());editor->setVisible(true);pump(150);
 auto image=editor->createComponentSnapshot(editor->getLocalBounds());juce::File f=juce::File::getCurrentWorkingDirectory().getChildFile(argc>1?argv[1]:"artifacts/editor.png");f.deleteFile();juce::FileOutputStream stream(f);juce::PNGImageFormat png;require(png.writeImageToStream(image,stream),"actual editor screenshot saved");editor->removeFromDesktop();
 // Processor timer must publish host notifications and drain capture with no editor alive.
 editor.reset();p.set("automation",1);p.prepareToPlay(48000,128);juce::AudioBuffer<float>b(4,128);juce::MidiBuffer m;
 for(int k=0;k<500;++k){for(int c=0;c<4;++c)for(int i=0;i<128;++i)b.setSample(c,i,.02f*std::sin(float(k*128+i)*.04f));p.processBlock(b,m);if(k%20==0)pump(25);}
 pump(100);require(!p.capture().empty(),"automation captured with editor closed");require(std::abs(p.value("rider")-p.meterGain.load())<.11,"Write publishes real Rider parameter");require(p.exportCsv(juce::File::getCurrentWorkingDirectory().getChildFile("artifacts/automation-test.csv")),"automation CSV export succeeds");p.set("automation",2);p.set("rider",-3);for(int i=0;i<500;++i){b.clear();p.processBlock(b,m);}require(std::abs(p.meterGain.load()+3)<.01,"Read follows host parameter through processor");
 p.set("automation",1);allocationCount=0;watchAllocations=true;
 for(int i=0;i<1000;++i){b.clear();p.processBlock(b,m);}watchAllocations=false;require(allocationCount==0,"no static-linked malloc/calloc/realloc calls in 1000 audio callbacks");
 p.clearCapture();require(p.capture().empty(),"capture clear");
 p.set("output",12);for(int i=0;i<500;++i){for(int c=0;c<4;++c)for(int n=0;n<128;++n)b.setSample(c,n,c<2?.1f:.8f);p.processBlockBypassed(b,m);}require(std::abs(b.getSample(0,127)-.1f)<1e-6,"host bypass returns dry vocal with no sidechain leakage");
 std::cout<<"Integration checks completed\n";
}
