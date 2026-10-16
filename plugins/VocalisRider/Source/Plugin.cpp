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
namespace {const char* ids[]={"target","spill","vocal","music","rangeMin","rangeMax","idle","rider","output","attack","automation","bypass","preset"};
const char* names[]={"Default Vocal Rider","Podcast Dialogue","Interview Speech","Voice-over Narration","Singing Vocal","Fast Speech","Live Microphone","Gentle Leveling"};}
juce::AudioProcessorValueTreeState::ParameterLayout Vocalis::parameters(){
 juce::AudioProcessorValueTreeState::ParameterLayout l;
 auto f=[&](const char* id,const char* name,float lo,float hi,float d){l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(lo,hi,.1f),d));};
 f("target","Target",-42,0,-21);f("spill","Spill",-48,-12,-48);f("vocal","Vocal sensitivity",-12,12,0);f("music","Music sensitivity",-12,12,0);
 f("rangeMin","Range minimum",-12,11,-6);f("rangeMax","Range maximum",-11,12,6);f("idle","Idle",-12,12,0);f("rider","Rider",-12,12,0);f("output","Output",-12,12,0);
 l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"attack",1},"Attack",juce::StringArray{"Slow","Fast"},0));
 l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"automation",1},"Automation",juce::StringArray{"Off","Write","Read"},0));
 l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"bypass",1},"Bypass",false));
 l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"preset",1},"Preset label",juce::StringArray(names,8),0));return l;
}
Vocalis::Vocalis():AudioProcessor(BusesProperties().withInput("Vocal",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true).withInput("Music",juce::AudioChannelSet::stereo(),false)),state(*this,nullptr,"Vocalis",parameters()) {for(size_t i=0;i<13;++i)raw[i]=state.getRawParameterValue(ids[i]);recorded.reserve(180000);startTimerHz(50);}
Vocalis::~Vocalis(){stopTimer();if(gesture)state.getParameter("rider")->endChangeGesture();}
void Vocalis::prepareToPlay(double sr,int){
 sampleRate=std::clamp(std::isfinite(sr) && sr > 0 ? sr : 48000.,8000.,384000.);
 engine.prepare(sampleRate);timeline=0;untilCapture=std::max(1,int(sampleRate/50));setLatencySamples(0);
}
void Vocalis::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){processAudio(b,m,false);}
void Vocalis::processAudio(juce::AudioBuffer<float>& b,juce::MidiBuffer&,bool forceBypass){
 juce::ScopedNoDenormals nd;
 // Never obtain bus pointers from a malformed/undersized host buffer.
 if(b.getNumChannels()<getTotalNumInputChannels() || b.getNumChannels()<getTotalNumOutputChannels()){
  b.clear();return;
 }
 const float automationMode=raw[10]->load();
 vr::Settings p; p.target=raw[0]->load();p.spill=raw[1]->load();p.vocal=raw[2]->load();p.music=raw[3]->load();p.lo=raw[4]->load();p.hi=raw[5]->load();p.idle=raw[6]->load();p.rider=raw[7]->load();p.output=raw[8]->load();p.fast=raw[9]->load()>.5f;p.mode=std::isfinite(automationMode)?int(std::clamp(automationMode,0.f,2.f)):0;p.bypass=forceBypass||raw[11]->load()>.5f;
 auto main=getBusBuffer(b,false,0);auto music=getBusBuffer(b,true,1);
 if(auto* ph=getPlayHead())if(auto pos=ph->getPosition())if(auto t=pos->getTimeInSeconds())timeline=*t;
 const int step=std::max(1,int(sampleRate/50));
 for(int off=0;off<b.getNumSamples();){int n=std::min(b.getNumSamples()-off,untilCapture>0?untilCapture:step);float* out[2]={};const float* sc[2]={};for(int c=0;c<main.getNumChannels();++c)out[c]=main.getWritePointer(c,off);for(int c=0;c<music.getNumChannels();++c)sc[c]=music.getReadPointer(c,off);
 engine.process(out,main.getNumChannels(),sc,music.getNumChannels(),n,p);off+=n;timeline+=n/sampleRate;untilCapture-=n;
 if(untilCapture<=0){untilCapture=step;if(p.mode==1){int a,as,z,zs;fifo.prepareToWrite(1,a,as,z,zs);if(as){queue[size_t(a)]={timeline,engine.appliedDb};fifo.finishedWrite(1);}else dropped.fetch_add(1,std::memory_order_relaxed);}}
 }
 meterGain.store(engine.appliedDb);meterInput.store(engine.inputDb);meterPeak.store(engine.peakDb);meterMusic.store(engine.musicDb);activity.store(engine.riding);
}
void Vocalis::processBlockBypassed(juce::AudioBuffer<float>& b,juce::MidiBuffer& m) {processAudio(b,m,true);}
bool Vocalis::isBusesLayoutSupported(const BusesLayout& l) const {
 if(l.inputBuses.size()!=2 || l.outputBuses.size()!=1)return false;
 auto m=l.getMainInputChannelSet();auto sc=l.getChannelSet(true,1);return (m==juce::AudioChannelSet::mono()||m==juce::AudioChannelSet::stereo())&&m==l.getMainOutputChannelSet()&&(sc.isDisabled()||sc==juce::AudioChannelSet::mono()||sc==juce::AudioChannelSet::stereo());}
void Vocalis::drain(){int a,n,b,m;fifo.prepareToRead(fifo.getNumReady(),a,n,b,m);for(int i=0;i<n;++i)if(recorded.size()<1000000)recorded.push_back(queue[size_t(a+i)]);else ++dropped;for(int i=0;i<m;++i)if(recorded.size()<1000000)recorded.push_back(queue[size_t(b+i)]);else ++dropped;fifo.finishedRead(n+m);}
void Vocalis::timerCallback(){
 drain();
 auto* rider=state.getParameter("rider");
 if(value("automation")==1 && !gesture){
  gesture=true;
  rider->beginChangeGesture();
 }
 // Host gesture callbacks may synchronously switch to Read and set Rider.
 // Re-read mode after the callback rather than publishing a stale Write decision.
 if(value("automation")!=1){
  if(gesture){gesture=false;rider->endChangeGesture();}
  return;
 }
 rider->setValueNotifyingHost(rider->convertTo0to1(meterGain.load()));
}
void Vocalis::clearCapture(){drain();recorded.clear();dropped=0;}
bool Vocalis::exportCsv(const juce::File& f){drain();juce::String text="seconds,rider_db\n";for(auto& p:recorded)text+=juce::String(p.time,6)+","+juce::String(p.db,4)+"\n";return f.replaceWithText(text);}
int Vocalis::getCurrentProgram(){const float v=value("preset");return std::isfinite(v)?int(std::clamp(v,0.f,7.f)):0;}
const juce::String Vocalis::getProgramName(int n){return names[juce::jlimit(0,7,n)];}
void Vocalis::setCurrentProgram(int n){n=juce::jlimit(0,7,n); // target, spill, vocal, music, min, max, idle, rider, output, fast
 static constexpr float presets[8][10]={{-21,-48,0,0,-6,6,0,0,0,0},{-21,-42,3,-6,-5,8,0,0,0,0},{-23,-40,4,-6,-6,9,0,0,0,0},{-20,-44,2,-6,-5,7,0,0,0,0},{-22,-48,-1,3,-5,7,0,0,0,0},{-21,-40,3,-6,-7,7,0,0,0,1},{-21,-32,0,0,-6,4,0,0,0,1},{-23,-46,0,-6,-3,3,0,0,0,0}};
 for(int i=0;i<10;++i){set(ids[i],presets[n][i]);}
 set("automation",0);set("bypass",0);set("preset",float(n));}
void Vocalis::getStateInformation(juce::MemoryBlock& b){auto x=state.copyState().createXml();copyXmlToBinary(*x,b);}
void Vocalis::setStateInformation(const void* d,int n){auto x=getXmlFromBinary(d,n);if(x&&x->hasTagName(state.state.getType()))state.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new Vocalis;}
