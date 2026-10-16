/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/base/coreiids.cpp"
#include "public.sdk/source/vst/vstinitiids.cpp"
#include <dlfcn.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <algorithm>
using namespace Steinberg;using namespace Steinberg::Vst;
void check(bool v,const char* msg){std::cout<<(v?"PASS ":"FAIL ")<<msg<<std::endl;if(!v)std::exit(1);}
struct Host:IHostApplication {
 tresult PLUGIN_API queryInterface(const TUID id,void** obj)override{*obj=nullptr;if(FUnknownPrivate::iidEqual(id,IHostApplication::iid)||FUnknownPrivate::iidEqual(id,FUnknown::iid)){*obj=this;return kResultOk;}return kNoInterface;}
 uint32 PLUGIN_API addRef()override{return 1;}uint32 PLUGIN_API release()override{return 1;}
 tresult PLUGIN_API getName(String128 s)override {const char* n="Vocalis Smoke Host";int i=0;while(*n)s[i++]=*n++;s[i]=0;return kResultOk;}
 tresult PLUGIN_API createInstance(TUID,TUID,void** o)override{*o=nullptr;return kNoInterface;}
};
struct Queue:IParamValueQueue {
 ParamID id=0;ParamValue val=0;
 tresult PLUGIN_API queryInterface(const TUID,void** o)override{*o=nullptr;return kNoInterface;}
 uint32 PLUGIN_API addRef()override{return 1;}uint32 PLUGIN_API release()override{return 1;}
 ParamID PLUGIN_API getParameterId()override{return id;}int32 PLUGIN_API getPointCount()override{return 1;}
 tresult PLUGIN_API getPoint(int32 i,int32& offset,ParamValue& v)override{offset=0;v=val;return i==0?kResultOk:kResultFalse;}
 tresult PLUGIN_API addPoint(int32,ParamValue,int32&)override{return kNotImplemented;}
};
struct Changes:IParameterChanges {
 Queue q[2];
 tresult PLUGIN_API queryInterface(const TUID,void** o)override{*o=nullptr;return kNoInterface;}
 uint32 PLUGIN_API addRef()override{return 1;}uint32 PLUGIN_API release()override{return 1;}
 int32 PLUGIN_API getParameterCount()override{return 2;}IParamValueQueue* PLUGIN_API getParameterData(int32 i)override{return i>=0&&i<2?&q[i]:nullptr;}
 IParamValueQueue* PLUGIN_API addParameterData(const ParamID&,int32&)override{return nullptr;}
};
int exercise(const char* path){void* lib=dlopen(path,RTLD_NOW|RTLD_LOCAL);if(!lib)std::cerr<<dlerror()<<'\n';check(lib,"native VST3 module loads");
 auto entry=reinterpret_cast<bool(*)(void*)>(dlsym(lib,"ModuleEntry"));auto exitModule=reinterpret_cast<bool(*)()>(dlsym(lib,"ModuleExit"));auto getFactory=reinterpret_cast<IPluginFactory*(*)()>(dlsym(lib,"GetPluginFactory"));check(entry&&exitModule&&getFactory,"Linux VST3 entry points present");check(entry(lib),"ModuleEntry");IPluginFactory* factory=getFactory();check(factory,"factory returned");
 IComponent* component=nullptr;for(int i=0;i<factory->countClasses();++i){PClassInfo info{};factory->getClassInfo(i,&info);if(std::strcmp(info.category,kVstAudioEffectClass)==0){std::cout<<"Class: "<<info.name<<'\n';factory->createInstance(info.cid,IComponent::iid,reinterpret_cast<void**>(&component));}}
 check(component,"audio component instantiated");Host host;check(component->initialize(&host)==kResultOk,"component initializes");IAudioProcessor* processor=nullptr;component->queryInterface(IAudioProcessor::iid,reinterpret_cast<void**>(&processor));check(processor,"IAudioProcessor available");
 TUID controllerId{};component->getControllerClassId(controllerId);IEditController* controller=nullptr;factory->createInstance(controllerId,IEditController::iid,reinterpret_cast<void**>(&controller));check(controller,"edit controller instantiated");check(controller->initialize(&host)==kResultOk,"controller initializes");
 IConnectionPoint *componentConnection=nullptr,*controllerConnection=nullptr;component->queryInterface(IConnectionPoint::iid,reinterpret_cast<void**>(&componentConnection));controller->queryInterface(IConnectionPoint::iid,reinterpret_cast<void**>(&controllerConnection));check(componentConnection&&controllerConnection,"component/controller connection points");componentConnection->connect(controllerConnection);controllerConnection->connect(componentConnection);
 Changes changes;bool foundMode=false,foundRider=false;for(int i=0;i<controller->getParameterCount();++i){ParameterInfo info{};controller->getParameterInfo(i,info);std::string name;for(auto c:info.title){if(!c)break;name+=char(c);}if(name=="Automation"){changes.q[0].id=info.id;changes.q[0].val=1;foundMode=true;}if(name=="Rider"){changes.q[1].id=info.id;changes.q[1].val=.625;foundRider=true;}}
 check(foundMode&&foundRider,"stable Rider and Automation parameters exposed");
 for(int channels:{1,2}){SpeakerArrangement input[2]={channels==1?SpeakerArr::kMono:SpeakerArr::kStereo,SpeakerArr::kStereo};SpeakerArrangement output= input[0];check(processor->setBusArrangements(input,2,&output,1)==kResultOk,"mono/stereo plus music arrangements");
 component->activateBus(kAudio,kInput,0,true);component->activateBus(kAudio,kInput,1,true);component->activateBus(kAudio,kOutput,0,true);
 ProcessSetup setup{};setup.processMode=kRealtime;setup.symbolicSampleSize=kSample32;setup.maxSamplesPerBlock=128;setup.sampleRate=48000;check(processor->setupProcessing(setup)==kResultOk,"processing setup");check(component->setActive(true)==kResultOk,"component active");processor->setProcessing(true);
 float in[2][128]{},side[2][128]{},out[2][128]{};float* ip[]={in[0],in[1]};float* sp[]={side[0],side[1]};float* op[]={out[0],out[1]};AudioBusBuffers ins[2]{},outs{};ins[0].numChannels=channels;ins[0].channelBuffers32=ip;ins[1].numChannels=2;ins[1].channelBuffers32=sp;outs.numChannels=channels;outs.channelBuffers32=op;ProcessData data{};data.processMode=kRealtime;data.symbolicSampleSize=kSample32;data.numSamples=128;data.numInputs=2;data.numOutputs=1;data.inputs=ins;data.outputs=&outs;data.inputParameterChanges=&changes;
 double dryEnergy=0,wetEnergy=0;for(int block=0;block<1200;++block){for(int c=0;c<channels;++c)for(int i=0;i<128;++i)in[c][i]=.03f*std::sin(float(block*128+i)*.03f);if(block==0)check(processor->process(data)==kResultOk,"first audio block");else processor->process(data);if(block>1100)for(int i=0;i<128;++i){dryEnergy+=in[0][i]*in[0][i];wetEnergy+=out[0][i]*out[0][i];}}
 check(std::abs(10*std::log10(wetEnergy/dryEnergy)-3)<.03,"VST3 host automation Read applies +3 dB");changes.q[0].val=0;dryEnergy=wetEnergy=0;for(int block=0;block<1800;++block){for(int c=0;c<channels;++c)for(int i=0;i<128;++i)in[c][i]=.015f*std::sin(float(block*128+i)*.03f);processor->process(data);if(block>1700)for(int i=0;i<128;++i){dryEnergy+=in[0][i]*in[0][i];wetEnergy+=out[0][i]*out[0][i];}}check(10*std::log10(wetEnergy/dryEnergy)>5.5,"VST3 automatic riding changes actual audio");changes.q[0].val=1;
 check(processor->getLatencySamples()==0,"zero reported latency");processor->setProcessing(false);component->setActive(false);
 }
 componentConnection->disconnect(controllerConnection);controllerConnection->disconnect(componentConnection);componentConnection->release();controllerConnection->release();
 controller->terminate();controller->release();processor->release();component->terminate();component->release();factory->release();exitModule();dlclose(lib);std::cout<<"VST3 smoke checks completed\n";return 0;
}

int main(int argc,char** argv){check(argc>=2,"bundle path supplied");int cycles=argc>2?std::clamp(std::atoi(argv[2]),1,100):1;for(int i=0;i<cycles;++i)if(exercise(argv[1])!=0)return 1;std::cout<<cycles<<" complete native load/process/unload cycles\n";return 0;}
