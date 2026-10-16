/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include "Plugin.h"
#include <iostream>
#include <cstdlib>
namespace juce::detail { bool dispatchNextMessageOnSystemQueue(bool); }
void pump(int ms){auto end=juce::Time::getMillisecondCounter()+juce::uint32(ms);while(juce::Time::getMillisecondCounter()<end){juce::detail::dispatchNextMessageOnSystemQueue(true);juce::Thread::sleep(1);}}
int checks=0;void check(bool b,const char* s){++checks;std::cout<<(b?"PASS ":"FAIL ")<<s<<std::endl;if(!b)std::exit(1);}
juce::Component* find(juce::Component& c,const char* name){if(c.getName()==name)return &c;for(auto* child:c.getChildren())if(auto* r=find(*child,name))return r;return nullptr;}
juce::MouseEvent event(juce::Component& c,juce::Point<float> at,juce::Point<float> origin,bool shift=false){return {juce::Desktop::getInstance().getMainMouseSource(),at,juce::ModifierKeys(juce::ModifierKeys::leftButtonModifier|(shift?juce::ModifierKeys::shiftModifier:0)),1,0,0,0,0,&c,&c,juce::Time::getCurrentTime(),origin,juce::Time::getCurrentTime(),1,at!=origin};}
void snapshot(juce::Component& c,const char* name){auto f=juce::File::getCurrentWorkingDirectory().getChildFile(name);f.getParentDirectory().createDirectory();f.deleteFile();juce::FileOutputStream stream(f);juce::PNGImageFormat png;check(png.writeImageToStream(c.createComponentSnapshot(c.getLocalBounds()),stream),"actual editor snapshot written");}
int main(){juce::ScopedJuceInitialiser_GUI init;Vocalis p;auto buses=p.getBusesLayout();buses.inputBuses.set(1,juce::AudioChannelSet::stereo());check(p.setBusesLayout(buses),"test enables optional music bus");p.prepareToPlay(48000,128);auto e=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());e->setVisible(true);pump(70);p.set("automation",2);pump(50);
 const char* ids[]={"target","spill","vocal","music","rider","output","idle"};const float values[]={-28,-36,3,-4,2,1,-2};
 for(int i=0;i<7;++i){auto* s=dynamic_cast<juce::Slider*>(find(*e,ids[i]));check(s,"attached slider exists");p.set(ids[i],values[i]);pump(40);check(std::abs(float(s->getValue())-values[i])<.01,"programmatic parameter change reflected in GUI");s->setValue(values[i]+1,juce::sendNotificationSync);check(std::abs(p.value(ids[i])-(values[i]+1))<.01,"GUI slider changes correct parameter");double before=s->getValue();check(s->keyPressed(juce::KeyPress(juce::KeyPress::upKey)),"slider handles keyboard arrow");check(s->getValue()>before,"keyboard arrow updates value");check(std::abs(float(s->getValue())-p.value(ids[i]))<.01,"keyboard edit reaches parameter");
 double beforeMouse=s->getValue();auto centre=s->getLocalBounds().toFloat().getCentre();s->mouseDown(event(*s,centre,centre));s->mouseDrag(event(*s,centre.translated(20,-20),centre));s->mouseUp(event(*s,centre.translated(20,-20),centre));check(s->getValue()!=beforeMouse,"mouse drag changes slider value");check(std::abs(float(s->getValue())-p.value(ids[i]))<.01,"mouse drag remains bound to parameter");}
 auto* r=find(*e,"range");check(r,"dual range component exists");
 auto oldRange=r->createComponentSnapshot(r->getLocalBounds());p.set("rangeMin",-9);p.set("rangeMax",3);pump(40);auto newRange=r->createComponentSnapshot(r->getLocalBounds());bool rangeChanged=false;for(int y=0;y<oldRange.getHeight();++y)for(int x=0;x<oldRange.getWidth();++x)rangeChanged|=oldRange.getPixelAt(x,y)!=newRange.getPixelAt(x,y);check(rangeChanged,"programmatic range changes redraw real handles");p.set("rangeMin",-6);p.set("rangeMax",6);r->keyPressed(juce::KeyPress(juce::KeyPress::leftKey));r->keyPressed(juce::KeyPress(juce::KeyPress::upKey));check(std::abs(p.value("rangeMin")+5)<.01,"keyboard selects and adjusts minimum range");r->keyPressed(juce::KeyPress(juce::KeyPress::rightKey));r->keyPressed(juce::KeyPress(juce::KeyPress::downKey));check(std::abs(p.value("rangeMax")-5)<.01,"keyboard selects and adjusts maximum range");
 juce::Point<float> origin(50,16+7.f/24*274);r->mouseDown(event(*r,origin,origin));r->mouseDrag(event(*r,origin.translated(0,-20),origin));r->mouseUp(event(*r,origin.translated(0,-20),origin));check(p.value("rangeMax")>6,"mouse drags real upper range handle");r->mouseDoubleClick(event(*r,origin,origin));check(p.value("rangeMax")==6&&p.value("rangeMin")==-6,"range double click restores existing defaults");
 auto click=[&](const char* name,const char* id,float value){auto* b=dynamic_cast<juce::Button*>(find(*e,name));check(b,"functional button exists");b->triggerClick();pump(100);check(p.value(id)==value,"button changes existing parameter");};
 click("attackFast","attack",1);click("attackSlow","attack",0);click("automationOff","automation",0);click("automationWrite","automation",1);click("automationRead","automation",2);click("bypass","bypass",1);click("bypass","bypass",0);
 for(int mode=0;mode<3;++mode){p.set("automation",float(mode));pump(45);const char* names[]={"automationOff","automationWrite","automationRead"};for(int i=0;i<3;++i)check(dynamic_cast<juce::Button*>(find(*e,names[i]))->getToggleState()==(i==mode),"external automation mode reflected by all mode buttons");}
 p.set("attack",1);pump(40);check(dynamic_cast<juce::Button*>(find(*e,"attackFast"))->getToggleState(),"external attack update reflected by switch");p.set("bypass",1);pump(40);check(dynamic_cast<juce::Button*>(find(*e,"bypass"))->getToggleState(),"external bypass update reflected by button");p.set("bypass",0);
 p.set("automation",0);pump(50);auto* rider=dynamic_cast<juce::Slider*>(find(*e,"rider"));check(!rider->isEnabled(),"Rider read-only while auto riding");p.set("automation",2);pump(50);check(rider->isEnabled(),"Rider editable in Read");
 auto* menu=dynamic_cast<juce::ComboBox*>(find(*e,"preset"));check(menu,"factory menu exists");menu->setSelectedId(2,juce::sendNotificationSync);pump(50);check(p.getCurrentProgram()==1&&p.value("spill")==-42&&p.value("rangeMax")==8,"menu applies real preset values");p.setCurrentProgram(5);pump(50);check(menu->getSelectedId()==6,"external program change reflected in preset menu");
 p.setCurrentProgram(0);juce::AudioBuffer<float> b(4,128);juce::MidiBuffer midi;
 for(int k=0;k<750;++k){for(int c=0;c<4;++c)for(int n=0;n<128;++n)b.setSample(c,n,(c<2?.08f:.12f)*std::sin(float(k*128+n)*.04f));p.processBlock(b,midi);if(k%30==0)pump(5);}pump(40);
 check(p.activity.load()&&p.meterInput.load()>-40&&p.meterMusic.load()>-40,"running audio drives actual input, sidechain and activity meters");check(std::abs(float(rider->getValue())-p.meterGain.load())<.051,"automatic fader reflects real engine gain");
 snapshot(*e,"artifacts/gui-redesign/redesigned-running-editor.png");
 for(int size:{750,1400}){e->setSize(size,size);pump(30);check(e->getWidth()==size&&e->getHeight()==size,"editor resizes at fixed square aspect ratio");snapshot(*e,size==750?"artifacts/gui-redesign/editor-750.png":"artifacts/gui-redesign/editor-1400.png");}
 juce::MemoryBlock state;p.getStateInformation(state);e.reset();Vocalis q;q.setStateInformation(state.getData(),int(state.getSize()));auto restored=std::unique_ptr<juce::AudioProcessorEditor>(q.createEditor());pump(50);check(std::abs(dynamic_cast<juce::Slider*>(find(*restored,"target"))->getValue()-q.value("target"))<.01,"recreated editor reflects restored state");restored.reset();for(int i=0;i<5;++i){auto repeat=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());pump(5);}check(true,"repeated editor creation and destruction completed");
 auto aboutEditor=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());
 auto* about=dynamic_cast<juce::Button*>(find(*aboutEditor,"about"));
 check(about,"About button exists"); about->triggerClick();pump(100);
 auto* dialog=find(*aboutEditor,"aboutPanel");
 check(dialog&&dialog->isVisible(),"About opens inside the editor");
 bool credits=false;
 for(auto* child:dialog->getChildren())if(auto* label=dynamic_cast<juce::Label*>(child))credits=label->getText().contains("Enos Takanaka")&&label->getText().contains("AGPL-3.0-or-later")&&label->getText().contains(JucePlugin_VersionString);
 check(credits,"About contains creator, verified licence and actual build version");
 snapshot(*dialog,"artifacts/author-attribution/about.png");
 for(auto* child:dialog->getChildren())if(auto* button=dynamic_cast<juce::Button*>(child))button->triggerClick();pump(30);
 check(!dialog->isVisible(),"About closes correctly");
 std::cout<<checks<<" GUI checks passed\n";
}
