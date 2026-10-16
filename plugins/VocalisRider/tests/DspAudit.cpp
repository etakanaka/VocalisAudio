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
int fails=0,checks=0;void check(bool b,const char* s){++checks;fails+=!b;std::cout<<(b?"PASS ":"FAIL ")<<s<<'\n';}
int main(){for(double sr:{44100.,48000.,88200.,96000.,192000.}){vr::Engine e;e.prepare(sr);vr::Settings p;p.mode=2;std::array<float,1024>a{},b{},sc{};float* ch[]={a.data(),b.data()};const float* music[]={sc.data(),sc.data()};auto run=[&](int count,float level,int scCount){for(int k=0;k<count;++k){a.fill(level);b.fill(level*.5f);sc.fill(.2f);e.process(ch,2,music,scCount,1024,p);}};
 a.fill(0);b.fill(0);a[0]=.25f;e.process(ch,2,nullptr,0,1024,p);check(a[0]!=0&&a[1]==0,"impulse reaches output at sample zero without added buffering");
 e.prepare(sr);run(500,.1f,0);check(std::abs(e.inputDb-vr::Engine::db(std::sqrt((.01+.0025)/2)))<.002,"RMS measurement matches analytical linked power");check(std::abs(e.peakDb+20)<.002,"peak measurement matches analytical peak");
 p.rider=12;p.output=12;float last=1,maxJump=0;for(int k=0;k<500;++k){a.fill(.01f);b.fill(.005f);e.process(ch,2,nullptr,0,1024,p);for(float v:a){float ratio=v/.01f;maxJump=std::max(maxJump,std::abs(ratio-last));last=ratio;}}check(maxJump<.05,"abrupt Rider and Output changes remain smoothed");check(std::abs(vr::Engine::db(last)-24)<.03,"combined Rider and Output gain accuracy");
 p.bypass=true;run(300,.1f,2);check(std::abs(a.back()-.1f)<1e-6,"bypass settles to unity with music present");p={};run(int(std::ceil(sr*8/1024)),0,2);check(!e.riding&&std::abs(e.appliedDb)<.01&&a.back()==0,"long silence returns to Idle without gain creep or music leakage");
 for(int k=0;k<1000;++k){a.fill(k%2?.001f:.3f);b=a;p.target=float(k%43-42);p.fast=(k%2)!=0;p.output=float(k%25-12);e.process(ch,2,music,k%2,1024,p);for(float v:a)if(!std::isfinite(v))++fails;}check(fails==0,"rapid level and parameter changes remain numerically stable");}
 vr::Engine e;e.prepare(std::numeric_limits<double>::quiet_NaN());vr::Settings p;float a=.1f;float* ch[]={&a};e.process(ch,1,nullptr,0,1,p);check(std::isfinite(a),"invalid engine sample rate falls back safely");
 std::cout<<checks<<" checks, "<<fails<<" failed\n";return fails?1:0;}
