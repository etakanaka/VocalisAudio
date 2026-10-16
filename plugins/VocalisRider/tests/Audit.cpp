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
#include <fstream>
#include <thread>
#include <limits>
namespace juce::detail {bool dispatchNextMessageOnSystemQueue(bool);}
void pump(int ms){auto end=juce::Time::getMillisecondCounter()+juce::uint32(ms);while(juce::Time::getMillisecondCounter()<end){juce::detail::dispatchNextMessageOnSystemQueue(true);juce::Thread::sleep(1);}}
int failed=0,checks=0;void check(bool b,const char* s){++checks;failed+=!b;std::cout<<(b?"PASS ":"FAIL ")<<s<<std::endl;}
juce::Component* find(juce::Component& c,const char* n){if(c.getName()==n)return &c;for(auto* ch:c.getChildren())if(auto* r=find(*ch,n))return r;return nullptr;}
struct Listener:juce::AudioProcessorParameter::Listener{
 Vocalis& p;bool reenter=false;int starts=0,ends=0,changes=0;bool messageOnly=true;
 explicit Listener(Vocalis& x):p(x){}
 void parameterValueChanged(int,float) override{++changes;messageOnly &=juce::MessageManager::getInstance()->isThisTheMessageThread();}
 void parameterGestureChanged(int,bool b)override{b?++starts:++ends;if(b&&reenter){reenter=false;p.set("automation",2);p.set("rider",-3);}}
};
int main(int argc,char** argv){juce::ScopedJuceInitialiser_GUI init;bool baseline=argc>1&&juce::String(argv[1])=="--baseline";
 {Vocalis p;p.prepareToPlay(48000,128);p.set("automation",1);juce::AudioBuffer<float>b(2,128);b.clear();juce::MidiBuffer midi;p.processBlock(b,midi);pump(30);check(p.capture().empty(),"capture waits for a complete 20 ms interval");for(int i=0;i<7;++i)p.processBlock(b,midi);pump(30);check(!p.capture().empty()&&std::abs(p.capture().front().time-.02)<1e-9,"first capture point is exactly 20 ms");}
 {Vocalis p;Listener l(p);l.reenter=true;auto* rider=p.state.getParameter("rider");rider->addListener(&l);p.set("automation",1);pump(45);check(p.value("automation")==2&&p.value("rider")==-3,"reentrant host mode change cannot overwrite Read Rider");check(l.starts==l.ends,"Write gesture closes after reentrant mode change");p.set("automation",0);l.changes=0;p.meterGain=5;pump(45);check(l.changes==0,"Off sends no moving Rider notifications");p.set("automation",1);pump(45);check(l.messageOnly&&l.changes>0,"Write notifications occur on message thread");p.set("automation",0);pump(30);rider->removeListener(&l);}
 {Vocalis p;Listener l(p);auto* attack=p.state.getParameter("attack");attack->addListener(&l);auto e=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());auto* fast=dynamic_cast<juce::Button*>(find(*e,"attackFast"));check(fast!=nullptr,"response button exists");if(fast){fast->triggerClick();pump(100);}check(l.starts==1&&l.ends==1&&p.value("attack")==1,"response click emits a balanced host gesture");attack->removeListener(&l);
 auto* range=find(*e,"range");Listener rangeListener(p);auto* max=p.state.getParameter("rangeMax");max->addListener(&rangeListener);
 auto point=juce::Point<float>(50,84.5f);juce::MouseEvent event(juce::Desktop::getInstance().getMainMouseSource(),point,juce::ModifierKeys(juce::ModifierKeys::leftButtonModifier),1,0,0,0,0,range,range,juce::Time::getCurrentTime(),point,juce::Time::getCurrentTime(),2,false);
 range->mouseDown(event);range->mouseDoubleClick(event);range->mouseUp(event);check(rangeListener.starts==1&&rangeListener.ends==1,"range double-click avoids nested gestures");range->mouseDown(event);e.reset();check(rangeListener.starts==2&&rangeListener.ends==2,"closing editor during range drag closes gesture");max->removeListener(&rangeListener);
 }
 if(!baseline){Vocalis p;juce::MidiBuffer midi;for(double rate:{0.,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity()}){p.prepareToPlay(rate,128);juce::AudioBuffer<float>b(2,128);b.clear();p.processBlock(b,midi);check(std::isfinite(p.meterGain.load()),"invalid sample rate handled without undefined conversion");}
 p.state.getRawParameterValue("automation")->store(std::numeric_limits<float>::quiet_NaN());{juce::AudioBuffer<float>b(2,128);b.clear();p.processBlock(b,midi);check(std::isfinite(p.meterGain.load()),"invalid mode snapshot sanitized before integer conversion");}p.set("automation",0);
 auto bad=p.getBusesLayout();bad.inputBuses.clear();check(!p.isBusesLayoutSupported(bad),"missing main input bus rejected safely");bad=p.getBusesLayout();bad.inputBuses.remove(1);check(!p.isBusesLayoutSupported(bad),"missing music bus slot rejected safely");
 auto l=p.getBusesLayout();l.inputBuses.set(1,juce::AudioChannelSet::stereo());check(p.setBusesLayout(l),"stereo music enabled");p.prepareToPlay(48000,128);juce::AudioBuffer<float>shortBuffer(1,128);shortBuffer.clear();p.processBlock(shortBuffer,midi);check(shortBuffer.getMagnitude(0,128)==0,"undersized host buffer safely cleared");
 for(int musicChannels:{0,1,2,0,2,1}){auto layout=p.getBusesLayout();layout.inputBuses.set(1,musicChannels==0?juce::AudioChannelSet::disabled():musicChannels==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo());check(p.setBusesLayout(layout),"music routing configuration accepted");p.prepareToPlay(48000,128);juce::AudioBuffer<float>b(2+musicChannels,128);for(int k=0;k<200;++k){b.clear();for(int c=2;c<2+musicChannels;++c)for(int n=0;n<128;++n)b.setSample(c,n,.9f);p.processBlock(b,midi);}check(b.getMagnitude(0,0,128)==0&&b.getMagnitude(1,0,128)==0,"sidechain never leaks into vocal output");}
 Vocalis q;p.set("target",-31);q.set("target",-17);check(p.value("target")==-31&&q.value("target")==-17,"instances have independent state");
 const char* presetIds[]={"target","spill","vocal","music","rangeMin","rangeMax","idle","rider","output","attack"};
 const float expectedPresets[8][10]={{-21,-48,0,0,-6,6,0,0,0,0},{-21,-42,3,-6,-5,8,0,0,0,0},{-23,-40,4,-6,-6,9,0,0,0,0},{-20,-44,2,-6,-5,7,0,0,0,0},{-22,-48,-1,3,-5,7,0,0,0,0},{-21,-40,3,-6,-7,7,0,0,0,1},{-21,-32,0,0,-6,4,0,0,0,1},{-23,-46,0,-6,-3,3,0,0,0,0}};
 for(int program=0;program<8;++program){p.setCurrentProgram(program);bool same=p.getCurrentProgram()==program&&p.value("automation")==0&&p.value("bypass")==0;for(int i=0;i<10;++i)same &=p.value(presetIds[i])==expectedPresets[program][i];check(same,"factory preset applies its complete expected configuration");}
 p.prepareToPlay(48000,960);p.clearCapture();p.set("automation",1);{juce::AudioBuffer<float>b(p.getTotalNumInputChannels(),960);b.clear();for(int i=0;i<9000;++i)p.processBlock(b,midi);}check(p.dropped.load()>0,"offline queue overflow is counted without blocking");pump(30);std::cout<<"Captured "<<p.capture().size()<<", dropped "<<p.dropped.load()<<" points\n";check(p.capture().size()==8191,"overflow capture matches AbstractFifo usable capacity (one slot reserved)");p.set("automation",0);pump(30);p.clearCapture();
 if(argc>2){juce::File f=juce::File::getCurrentWorkingDirectory().getChildFile(argv[2]);juce::MemoryBlock state;check(f.loadFileAsData(state),"old state fixture readable");q.setStateInformation(state.getData(),int(state.getSize()));const char* ids[]={"target","spill","vocal","music","rangeMin","rangeMax","idle","rider","output","attack","automation","bypass","preset"};const float expected[]={-29,-44,2,-6,-5,7,0,4,0,0,2,0,3};for(int i=0;i<13;++i)check(q.value(ids[i])==expected[i],"pre-debugging parameter fixture restores exactly");}
 }
 std::cout<<checks<<" checks, "<<failed<<" failed\n";return failed?1:0;}
