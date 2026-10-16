/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "Engine.h"
#include <array>
#include <iostream>
#include <limits>
#include <cstdlib>
static int checks=0;
void check(bool v,const char* s){++checks;std::cout<<(v?"PASS ":"FAIL ")<<s<<'\n';if(!v)std::exit(1);}
struct Rig {
 vr::Engine e;double sr,phase=0;int block;float ratio=1;double energy=0;float maxJump=0,last=0;bool linked=true,finite=true;
 Rig(double s=48000,int b=128):sr(s),block(b){e.prepare(sr);}
 void run(double seconds,float level,vr::Settings p={},float scLevel=-150,int channels=2){
 std::array<float,4096>a{},b{},sc{};int total=int(seconds*sr);energy=0;
 while(total>0){int n=std::min({total,block,4096});for(int i=0;i<n;++i){double t=phase++/sr; // Harmonic, speech-band signal with modest syllabic modulation.
 float x=level<=-140?0:vr::Engine::amplitude(level)*float(std::sqrt(2.)*(.87*std::sin(t*2*3.141592653589793*183)+.35*std::sin(t*2*3.141592653589793*367)+.2*std::sin(t*2*3.141592653589793*731)));
 a[i]=x;b[i]=x*.37f;sc[i]=scLevel<=-140?0:vr::Engine::amplitude(scLevel)*float(std::sqrt(2.)*std::sin(t*2*3.141592653589793*311));}
 auto dry=a;float* ptr[]={a.data(),b.data()};const float* side[]={sc.data()};e.process(ptr,channels,side,scLevel<=-140?0:1,n,p);
 for(int i=0;i<n;++i){finite&=std::isfinite(a[i]);energy+=a[i]*a[i];linked&=std::abs(b[i]-.37f*a[i])<1e-5f;if(std::abs(dry[i])>.001)ratio=a[i]/dry[i];}total-=n;}
 }
};
int main(){vr::Settings p;
 {Rig r;r.run(4,-150);check(!r.e.riding&&std::abs(r.e.appliedDb)<.01,"silence does not boost");}
 {Rig r;r.run(5,-34);check(r.e.riding&&r.e.appliedDb>5.8&&r.e.appliedDb<=6.001,"quiet speech reaches upper range");check(r.linked,"stereo channels share identical gain");check(std::abs(vr::Engine::db(r.ratio)-r.e.appliedDb)<.02,"display gain matches applied audio");}
 {Rig r;r.run(5,-5);check(r.e.appliedDb< -5.8&&r.e.appliedDb>=-6.001,"loud speech reaches lower range");r.run(6,-150);check(!r.e.riding&&std::abs(r.e.appliedDb)<.02,"long pause returns to idle");}
 {Rig r;r.run(5,-62);check(!r.e.riding&&std::abs(r.e.appliedDb)<.01,"low background noise rejected");p.spill=-32;r.run(3,-37,p);check(!r.e.riding,"spill threshold rejects stage noise");p.vocal=12;r.run(3,-37,p);check(r.e.riding,"vocal sensitivity changes detection");p={};}
 {Rig r;r.run(4,-28,p);float before=r.e.appliedDb;p.target=-32;r.run(4,-28,p);check(r.e.appliedDb<before-4,"target changes gain direction");p={};}
 {Rig fast,slow;auto f=p;f.fast=true;fast.run(.15,-4,f);slow.run(.15,-4,p);check(fast.e.appliedDb<slow.e.appliedDb-1,"Fast reacts earlier than Slow");}
 {Rig r;r.run(3,-34);r.run(2,-4);check(r.e.appliedDb< -5,"sudden quiet-to-loud transition recovers");}
 {Rig a,b,c;a.run(5,-23,p);b.run(5,-23,p,-12);c.run(5,-23,p,-36);check(b.e.appliedDb>a.e.appliedDb+1,"loud music raises vocal reference");check(c.e.appliedDb<a.e.appliedDb-1,"quiet music lowers vocal reference");auto f=p;f.music=-12;Rig d;d.run(5,-23,f,-36);check(d.e.appliedDb>c.e.appliedDb,"music sensitivity changes sidechain influence");check(a.finite,"missing sidechain operates normally");}
 {Rig r;p.mode=2;p.rider=3;p.output=-2;r.run(2,-24,p);check(std::abs(r.e.appliedDb-3)<.01,"Read obeys host Rider value");check(std::abs(vr::Engine::db(r.ratio)-1)<.02,"output trim affects actual signal");p.mode=0;r.run(4,-5,p);check(r.e.appliedDb< -5,"Read-to-Off resumes automatic riding");p.mode=1;r.run(4,-34,p);check(r.e.appliedDb>5,"Write continues automatic riding");p.bypass=true;r.run(1,-24,p);check(std::abs(r.ratio-1)<.0001,"bypass settles to exact unity");p={};}
 {Rig r;p.lo=-2;p.hi=2;r.run(5,-35,p);check(r.e.appliedDb<=2.001,"custom gain ceiling");r.run(4,-3,p);check(r.e.appliedDb>=-2.001,"custom gain floor");p.lo=4;p.hi=-4;r.run(2,-3,p);check(r.finite&&r.e.appliedDb>=-4.001,"crossed automated limits remain defined");p={};}
 {Rig r;p.idle=-3;r.run(6,-150,p);check(std::abs(r.e.appliedDb+3)<.01,"custom Idle gain");p={};}
 float reference=0;
 for(double sr:{44100.,48000.,88200.,96000.,176400.,192000.})for(int block:{1,32,64,128,256,512,1024,2048,4096}){Rig r(sr,block);r.run(1,-30,p,-20);if(!reference)reference=r.e.appliedDb;check(r.finite&&std::abs(r.e.appliedDb-reference)<.02,"sample-rate/block-size stability");}
 {vr::Engine e;e.prepare(48000);float a[]={std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity(),.2f};float* ptr[]={a};e.process(ptr,1,nullptr,0,3,p);check(std::isfinite(a[0])&&std::isfinite(a[1])&&std::isfinite(a[2]),"invalid audio sanitized");}
 {vr::Engine e;e.prepare(48000);std::array<float,256> noise{};unsigned seed=42;for(int k=0;k<1000;++k){for(auto& v:noise){seed=1664525u*seed+1013904223u;v=(float(seed>>8)/8388608.f-1)*.001f;}float* ptr[]={noise.data()};e.process(ptr,1,nullptr,0,256,p);}check(!e.riding&&std::abs(e.appliedDb)<.01,"deterministic broadband background noise rejected");}
 {vr::Engine e;e.prepare(192000);float a[]={.1f,.2f,-.3f};float* ptr[]={a};auto invalid=p;invalid.target=std::numeric_limits<float>::quiet_NaN();invalid.music=std::numeric_limits<float>::infinity();invalid.output=std::numeric_limits<float>::quiet_NaN();e.process(ptr,1,nullptr,0,3,invalid);check(std::isfinite(a[0])&&std::isfinite(e.appliedDb),"invalid settings sanitized");}
 std::cout<<checks<<" assertions passed\n";
}
