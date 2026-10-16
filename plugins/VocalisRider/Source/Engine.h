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
#include <algorithm>
#include <cmath>
namespace vr {
struct Settings {float target=-21,spill=-48,vocal=0,music=0,lo=-6,hi=6,idle=0,rider=0,output=0; int mode=0;bool fast=false,bypass=false;};
class Engine {
 double rate=48000, env=0,musicEnv=0,peak=0,hold=0; float gain=0,trim=0,wet=1; bool active=false;
 static float safe(float x){return std::isfinite(x)?x:0.f;}
 double coef(double seconds) const {return 1-std::exp(-1/(seconds*rate));}
public:
 float inputDb=-120,musicDb=-120,appliedDb=0,peakDb=-120; bool riding=false;
 void prepare(double sr){rate=std::clamp(std::isfinite(sr) && sr > 0 ? sr : 48000.,8000.,384000.);env=musicEnv=peak=hold=0;gain=trim=0;wet=1;active=false;}
 static float db(double x){return float(20*std::log10(std::max(1.0e-6,x)));}
 static float amplitude(float d){return std::pow(10.f,d*.05f);}
 void process(float* const* ch,int channels,const float* const* sc,int scChannels,int count,const Settings& input){
 Settings p=input;
 p.target=std::clamp(safe(p.target),-42.f,0.f);p.spill=std::clamp(safe(p.spill),-48.f,-12.f);
 p.vocal=std::clamp(safe(p.vocal),-12.f,12.f);p.music=std::clamp(safe(p.music),-12.f,12.f);
 p.lo=std::clamp(safe(p.lo),-12.f,11.f);p.hi=std::clamp(safe(p.hi),-11.f,12.f);
 p.idle=safe(p.idle);p.rider=safe(p.rider);p.output=safe(p.output);
 const float lo=std::min(p.lo,p.hi),hi=std::max(p.lo,p.hi);
 const double detect=coef(.035),musicC=coef(.3),peakC=coef(.06),trimC=coef(.025),wetC=coef(.005);
 const float rise = float(coef(p.fast ? .24 : .7));
 const float fall = float(coef(p.fast ? .065 : .22));
 const float idleStep = float(coef(.65)), readStep = float(coef(.012));
 const float musicInfluence = amplitude(p.music);
 for(int n=0;n<count;++n){
 double power=0,pk=0,mp=0;
 for(int c=0;c<channels;++c){float x=safe(ch[c][n]);power+=double(x)*x;pk=std::max(pk,double(std::abs(x)));}
 power/=std::max(1,channels);
 for(int c=0;c<scChannels;++c){float x=safe(sc[c][n]);mp+=double(x)*x;}mp/=std::max(1,scChannels);
 env+=detect*(power-env);musicEnv+=musicC*(mp-musicEnv);peak=std::max(pk,peak*(1-peakC));
 inputDb=db(std::sqrt(std::max(0.,env)));musicDb=db(std::sqrt(std::max(0.,musicEnv)));peakDb=db(peak);
 const float threshold=std::clamp(p.spill-p.vocal,-72.f,-6.f);
 if(inputDb>threshold+(active?0.f:3.f)){active=true;hold=.18*rate;}
 else if(inputDb<threshold-2){if(hold>0)--hold;else active=false;}
 float goal=std::clamp(p.idle,lo,hi);
 if(active){
 // No sidechain activity means exactly zero music influence. A -24 dBFS music reference is an independent design choice.
 const float musicOffset=scChannels&&musicDb>-70 ? std::clamp((musicDb+24)*.25f*musicInfluence,-6.f,6.f):0;
 const float error=p.target+musicOffset-inputDb;
 goal=std::clamp(error>1?error-1:(error< -1?error+1:0),lo,hi);
 }
 if(p.mode==2)goal=std::clamp(p.rider,-12.f,12.f);
 const float gainStep = p.mode == 2 ? readStep : (!active ? idleStep : (goal < gain ? fall : rise));
 gain+=gainStep*(goal-gain);trim+=float(trimC)*(std::clamp(p.output,-12.f,12.f)-trim);
 wet+=float(wetC)*((p.bypass?0.f:1.f)-wet);
 const float factor=1+wet*(amplitude(gain+trim)-1);
 for(int c=0;c<channels;++c)ch[c][n]=safe(safe(ch[c][n])*factor);
 }
 appliedDb=gain;riding=active;
 // Avoid persistent subnormal detector states after extended silence.
 if(env<1e-30)env=0;
 if(musicEnv<1e-30)musicEnv=0;
 }
};
}
