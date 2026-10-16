#include "Plugin.h"
#include <fstream>
#include <cmath>
int main(int argc,char** argv){if(argc!=2)return 2;juce::ScopedJuceInitialiser_GUI init;std::ofstream out(argv[1],std::ios::binary);Vocalis p;juce::MidiBuffer midi;
 for(double sr:{44100.,48000.,96000.,192000.})for(int size:{32,128,1024})for(int channels:{1,2}){
 auto l=p.getBusesLayout();l.inputBuses.set(0,channels==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo());l.outputBuses.set(0,l.inputBuses[0]);l.inputBuses.set(1,juce::AudioChannelSet::stereo());if(!p.setBusesLayout(l))return 3;
 p.prepareToPlay(sr,size);p.setCurrentProgram(0);juce::AudioBuffer<float> b(channels+2,size);
 for(int k=0;k<600;++k){if(k%75==0){p.setCurrentProgram(k/75);p.set("automation",float((k/75)%3));p.set("rider",float((k/75)-4));}if(k==510)p.set("bypass",1);
 for(int c=0;c<channels+2;++c)for(int n=0;n<size;++n){double t=(double(k)*size+n)/sr;float x=float(std::sin(2*juce::MathConstants<double>::pi*(c<channels?183:311)*t));b.setSample(c,n,x*(k%120<25?0.f:(k%120<75?.02f:.2f))*(c==1?.37f:1.f));}
 p.processBlock(b,midi);for(int c=0;c<channels;++c)out.write(reinterpret_cast<const char*>(b.getReadPointer(c)),size*sizeof(float));
 }
 juce::MemoryBlock state;p.getStateInformation(state);out.write(static_cast<const char*>(state.getData()),std::streamsize(state.getSize()));
 }
 return out?0:4;}
