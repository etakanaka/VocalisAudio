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
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
extern thread_local bool watchAllocations;
extern thread_local int allocationCount;
int main(int argc,char** argv){juce::ScopedJuceInitialiser_GUI init;std::ofstream file(argc>1?argv[1]:"performance.csv");file<<"rate,block,mean_us,p99_us,max_us,audio_cpu_percent,worst_deadline_percent,alloc_calls,finite,latency\n";
 Vocalis p;auto l=p.getBusesLayout();l.inputBuses.set(1,juce::AudioChannelSet::stereo());if(!p.setBusesLayout(l))return 2;juce::MidiBuffer midi;bool all=true;
 for(double rate:{44100.,48000.,88200.,96000.,192000.})for(int size:{32,64,128,256,512,1024}){p.setCurrentProgram(0);p.prepareToPlay(rate,size);juce::AudioBuffer<float>b(4,size);std::vector<double> durations(2000);bool finite=true;allocationCount=0;
 for(int k=-100;k<2000;++k){for(int c=0;c<4;++c)for(int n=0;n<size;++n)b.setSample(c,n,float(std::sin(double(k*size+n)*.04))*(c<2?.06f:.2f));watchAllocations=true;auto begin=std::chrono::steady_clock::now();p.processBlock(b,midi);auto end=std::chrono::steady_clock::now();watchAllocations=false;if(k>=0)durations[size_t(k)]=std::chrono::duration<double,std::micro>(end-begin).count();for(int c=0;c<2;++c)for(int n=0;n<size;++n)finite &=std::isfinite(b.getSample(c,n));}
 auto mean=std::accumulate(durations.begin(),durations.end(),0.)/double(durations.size());std::sort(durations.begin(),durations.end());double deadline=1e6*size/rate;file<<rate<<','<<size<<','<<mean<<','<<durations[1980]<<','<<durations.back()<<','<<100*mean/deadline<<','<<100*durations.back()/deadline<<','<<allocationCount<<','<<finite<<','<<p.getLatencySamples()<<'\n';all &=finite&&allocationCount==0&&p.getLatencySamples()==0;}
 std::cout<<(all?"PASS":"FAIL")<<" 30 rate/block benchmark cases, allocation/numerical/latency checks\n";return all?0:1;}
