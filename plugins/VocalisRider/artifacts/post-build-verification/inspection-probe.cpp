#include "Engine.h"
#include <iostream>
int main(){vr::Engine e;e.prepare(48000);vr::Settings p;float data[128];float* ch[]={data};
for(int b=0;b<2000;++b){for(auto& x:data)x=.02f;e.process(ch,1,nullptr,0,128,p);}
std::cout<<"Before range narrowing: "<<e.appliedDb<<" dB\n";
p.hi=0;for(auto& x:data)x=.02f;e.process(ch,1,nullptr,0,128,p);
std::cout<<"After 128 samples with Range Max 0: "<<e.appliedDb<<" dB\n";
return 0;}
