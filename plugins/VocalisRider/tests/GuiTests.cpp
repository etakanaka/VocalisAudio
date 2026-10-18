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
// Exercise mouse routes through the scaled editor, using a fixed physical
// pixel drag at each size rather than assuming unscaled component coordinates.
void verifyScaledInteraction(juce::AudioProcessorEditor& editor, Vocalis& processor)
{
 juce::MemoryBlock saved;
 processor.getStateInformation(saved);
 processor.set("automation",2);
 pump(40);
 const char* ids[]={"target","spill","vocal","music","rider","output","idle"};
 const float values[]={-28,-36,3,-4,2,1,-2};
 for(int i=0;i<7;++i){
  processor.set(ids[i],values[i]);pump(15);
  auto* slider=dynamic_cast<juce::Slider*>(find(editor,ids[i]));
  const auto bounds=slider->getLookAndFeel().getSliderLayout(*slider).sliderBounds.toFloat();
  const float proportion=float(slider->valueToProportionOfLength(slider->getValue()));
  auto local=bounds.getCentre();
  if(slider->isHorizontal())local.x=bounds.getX()+proportion*bounds.getWidth();
  else if(slider->isVertical())local.y=bounds.getBottom()-proportion*bounds.getHeight();
  if(slider->getName()=="idle")local.x-=5;
  auto physical=editor.getLocalPoint(slider,local);
  auto* hit=editor.getComponentAt(physical.roundToInt());
  check(hit==slider||slider->isParentOf(hit),"scaled mouse hit reaches attached slider");
  auto start=slider->getLocalPoint(&editor,physical);
  auto end=slider->getLocalPoint(&editor,physical.translated(10,-10));
  double before=slider->getValue();
  slider->mouseDown(event(*slider,start,start));
  slider->mouseDrag(event(*slider,end,start));
  slider->mouseUp(event(*slider,end,start));
  check(slider->getValue()!=before,"10-pixel drag changes slider at current editor scale");
  check(std::abs(float(slider->getValue())-processor.value(ids[i]))<.01,"scaled mouse edit keeps existing parameter binding");
 }
 auto* range=find(editor,"range");
 processor.set("rangeMin",-6);processor.set("rangeMax",6);pump(20);
 for(const char* id:{"rangeMin","rangeMax"}){
  float before=processor.value(id);
  auto physical=editor.getLocalPoint(range,juce::Point<float>(float(range->getWidth())/2,16+(12-before)/24*float(range->getHeight()-76)));
  check(editor.getComponentAt(physical.roundToInt())==range,"scaled mouse hit reaches Range handle");
  auto start=range->getLocalPoint(&editor,physical),end=range->getLocalPoint(&editor,physical.translated(0,-10));
  range->mouseDown(event(*range,start,start));range->mouseDrag(event(*range,end,start));range->mouseUp(event(*range,end,start));
  check(processor.value(id)>before,"scaled mouse drag adjusts correct Range endpoint");
 }
 struct ButtonCase {const char* name;const char* id;float value;};
 for(auto item:{ButtonCase{"attackFast","attack",1},ButtonCase{"attackSlow","attack",0},ButtonCase{"automationOff","automation",0},ButtonCase{"automationWrite","automation",1},ButtonCase{"automationRead","automation",2}}){
  auto* button=dynamic_cast<juce::Button*>(find(editor,item.name));
  auto point=editor.getLocalPoint(button,button->getLocalBounds().getCentre());
  check(editor.getComponentAt(point)==button,"scaled mode/speed button remains reachable");
  button->triggerClick();pump(35);
  check(processor.value(item.id)==item.value,"scaled button retains original callback");
 }
 auto* tools=dynamic_cast<juce::Button*>(find(editor,"utilities"));
 check(editor.getComponentAt(editor.getLocalPoint(tools,tools->getLocalBounds().getCentre()))==tools,"scaled utilities button remains reachable");
 tools->triggerClick();pump(40);
 for(const char* name:{"preset","bypass","exportCapture","clearCapture","about"}){
  auto* control=find(editor,name);
  auto* hit=editor.getComponentAt(editor.getLocalPoint(control,control->getLocalBounds().getCentre()));
  check(hit==control||control->isParentOf(hit),"expanded utility control has a reachable scaled hit area");
  check(editor.getLocalBounds().contains(editor.getLocalArea(control,control->getLocalBounds())),"scaled utility bounds stay inside actual editor");
 }
 auto* bypass=dynamic_cast<juce::Button*>(find(editor,"bypass"));
 processor.set("bypass",0);pump(20);bypass->triggerClick();pump(35);
 check(processor.value("bypass")==1,"scaled utility bypass remains functional");
 tools->triggerClick();pump(30);
 processor.setStateInformation(saved.getData(),int(saved.getSize()));pump(50);
}
int main(){juce::ScopedJuceInitialiser_GUI init;Vocalis p;auto buses=p.getBusesLayout();buses.inputBuses.set(1,juce::AudioChannelSet::stereo());check(p.setBusesLayout(buses),"test enables optional music bus");p.prepareToPlay(48000,128);auto e=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());check(e->getWidth()==480&&e->getHeight()==600,"default editor reports actual 480 x 600 dimensions");
 check(e->getConstrainer()->getMinimumWidth()==480&&e->getConstrainer()->getMinimumHeight()==600,"compact dimensions are accepted by resize constraints");
 check(std::abs(e->getConstrainer()->getFixedAspectRatio()-.8)<.000001,"editor enforces exact 4:5 resize aspect ratio");
 e->setVisible(true);pump(70);p.set("automation",2);pump(50);
 const char* ids[]={"target","spill","vocal","music","rider","output","idle"};const float values[]={-28,-36,3,-4,2,1,-2};
 for(int i=0;i<7;++i){auto* s=dynamic_cast<juce::Slider*>(find(*e,ids[i]));check(s,"attached slider exists");p.set(ids[i],values[i]);pump(40);check(std::abs(float(s->getValue())-values[i])<.01,"programmatic parameter change reflected in GUI");s->setValue(values[i]+1,juce::sendNotificationSync);check(std::abs(p.value(ids[i])-(values[i]+1))<.01,"GUI slider changes correct parameter");double before=s->getValue();check(s->keyPressed(juce::KeyPress(juce::KeyPress::upKey)),"slider handles keyboard arrow");check(s->getValue()>before,"keyboard arrow updates value");check(std::abs(float(s->getValue())-p.value(ids[i]))<.01,"keyboard edit reaches parameter");
 double beforeMouse=s->getValue();auto centre=s->getLocalBounds().toFloat().getCentre();s->mouseDown(event(*s,centre,centre));s->mouseDrag(event(*s,centre.translated(20,-20),centre));s->mouseUp(event(*s,centre.translated(20,-20),centre));check(s->getValue()!=beforeMouse,"mouse drag changes slider value");check(std::abs(float(s->getValue())-p.value(ids[i]))<.01,"mouse drag remains bound to parameter");}
 auto* r=find(*e,"range");check(r,"dual range component exists");
 auto oldRange=r->createComponentSnapshot(r->getLocalBounds());p.set("rangeMin",-9);p.set("rangeMax",3);pump(40);auto newRange=r->createComponentSnapshot(r->getLocalBounds());bool rangeChanged=false;for(int y=0;y<oldRange.getHeight();++y)for(int x=0;x<oldRange.getWidth();++x)rangeChanged|=oldRange.getPixelAt(x,y)!=newRange.getPixelAt(x,y);check(rangeChanged,"programmatic range changes redraw real handles");p.set("rangeMin",-6);p.set("rangeMax",6);r->keyPressed(juce::KeyPress(juce::KeyPress::leftKey));r->keyPressed(juce::KeyPress(juce::KeyPress::upKey));check(std::abs(p.value("rangeMin")+5)<.01,"keyboard selects and adjusts minimum range");r->keyPressed(juce::KeyPress(juce::KeyPress::rightKey));r->keyPressed(juce::KeyPress(juce::KeyPress::downKey));check(std::abs(p.value("rangeMax")-5)<.01,"keyboard selects and adjusts maximum range");
 juce::Point<float> origin(50,16+7.f/24*float(r->getHeight()-76));auto destination=origin.translated(0,-float(r->getHeight()-76)*20.f/274.f);r->mouseDown(event(*r,origin,origin));r->mouseDrag(event(*r,destination,origin));r->mouseUp(event(*r,destination,origin));check(p.value("rangeMax")>6,"mouse drags real upper range handle");r->mouseDoubleClick(event(*r,origin,origin));check(p.value("rangeMax")==6&&p.value("rangeMin")==-6,"range double click restores existing defaults");
 auto click=[&](const char* name,const char* id,float value){auto* b=dynamic_cast<juce::Button*>(find(*e,name));check(b,"functional button exists");b->triggerClick();pump(100);check(p.value(id)==value,"button changes existing parameter");};
 click("attackFast","attack",1);click("attackSlow","attack",0);click("automationOff","automation",0);click("automationWrite","automation",1);click("automationRead","automation",2);click("bypass","bypass",1);click("bypass","bypass",0);
 for(int mode=0;mode<3;++mode){p.set("automation",float(mode));pump(45);const char* names[]={"automationOff","automationWrite","automationRead"};for(int i=0;i<3;++i)check(dynamic_cast<juce::Button*>(find(*e,names[i]))->getToggleState()==(i==mode),"external automation mode reflected by all mode buttons");}
 p.set("attack",1);pump(40);check(dynamic_cast<juce::Button*>(find(*e,"attackFast"))->getToggleState(),"external attack update reflected by switch");p.set("bypass",1);pump(40);check(dynamic_cast<juce::Button*>(find(*e,"bypass"))->getToggleState(),"external bypass update reflected by button");p.set("bypass",0);
 p.set("automation",0);pump(50);auto* rider=dynamic_cast<juce::Slider*>(find(*e,"rider"));check(!rider->isEnabled(),"Rider read-only while auto riding");p.set("automation",2);pump(50);check(rider->isEnabled(),"Rider editable in Read");
 auto* menu=dynamic_cast<juce::ComboBox*>(find(*e,"preset"));check(menu,"factory menu exists");menu->setSelectedId(2,juce::sendNotificationSync);pump(50);check(p.getCurrentProgram()==1&&p.value("spill")==-42&&p.value("rangeMax")==8,"menu applies real preset values");p.setCurrentProgram(5);pump(50);check(menu->getSelectedId()==6,"external program change reflected in preset menu");
 p.setCurrentProgram(0);pump(40);
 auto* idleComponent=find(*e,"idle");auto idlePoint=e->getLocalPoint(idleComponent,juce::Point<int>(idleComponent->getWidth()/2-5,int(16+(12-p.value("idle"))/24*float(idleComponent->getHeight()-76))));
 check(e->getComponentAt(idlePoint)==idleComponent,"Idle marker is reachable through actual editor hit testing");
 auto rangePoint=e->getLocalPoint(r,juce::Point<int>(r->getWidth()/2,int(16+(12-p.value("rangeMax"))/24*float(r->getHeight()-76))));
 check(e->getComponentAt(rangePoint)==r,"Range handle is reachable through actual editor hit testing");
 juce::AudioBuffer<float> b(4,128);juce::MidiBuffer midi;
 for(int k=0;k<750;++k){for(int c=0;c<4;++c)for(int n=0;n<128;++n)b.setSample(c,n,(c<2?.08f:.12f)*std::sin(float(k*128+n)*.04f));p.processBlock(b,midi);if(k%30==0)pump(5);}pump(40);
 check(p.activity.load()&&p.meterInput.load()>-40&&p.meterMusic.load()>-40,"running audio drives actual input, sidechain and activity meters");check(std::abs(float(rider->getValue())-p.meterGain.load())<.051,"automatic fader reflects real engine gain");
 snapshot(*e,"artifacts/compact-gui/running-editor.png");
 auto* tools=dynamic_cast<juce::Button*>(find(*e,"utilities"));auto* toolPanel=find(*e,"utilityPanel");
 check(tools&&toolPanel&&!toolPanel->isVisible(),"utilities start collapsed for reference layout");
 tools->triggerClick();pump(50);check(toolPanel->isVisible(),"utility panel opens");
 for(const char* name:{"preset","bypass","exportCapture","clearCapture","about"})check(find(*e,name)->isVisible()&&toolPanel->isParentOf(find(*e,name)),"existing utility remains visible in expanded panel");
 snapshot(*e,"artifacts/compact-gui/utilities.png");tools->triggerClick();pump(50);check(!toolPanel->isVisible(),"utility panel closes");
 // Capture the original size before synthetic scaled clicks can flash buttons.
 for(int height:{1000,600,700,800,1400}){
  int width=height*4/5;e->setSize(width,height);pump(30);
  check(e->getWidth()==width&&e->getHeight()==height,"editor reports requested proportional dimensions");
  auto* canvas=find(*e,"target")->getParentComponent();
  check(e->getLocalArea(canvas,canvas->getLocalBounds())==e->getLocalBounds(),"scaled logical canvas exactly fills editor without cropping or empty margins");
  auto path=juce::String("artifacts/compact-gui/editor-")+juce::String(width)+"x"+juce::String(height)+".png";
  snapshot(*e,path.toRawUTF8());
  if(height==600||height==700||height==1000)verifyScaledInteraction(*e,p);
 }
 juce::MemoryBlock state;p.getStateInformation(state);e.reset();Vocalis q;q.setStateInformation(state.getData(),int(state.getSize()));auto restored=std::unique_ptr<juce::AudioProcessorEditor>(q.createEditor());pump(50);check(restored->getWidth()==480&&restored->getHeight()==600,"state-restored editor also opens at compact actual dimensions");check(std::abs(dynamic_cast<juce::Slider*>(find(*restored,"target"))->getValue()-q.value("target"))<.01,"recreated editor reflects restored state");restored.reset();for(int i=0;i<5;++i){auto repeat=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());pump(5);}check(true,"repeated editor creation and destruction completed");
 auto aboutEditor=std::unique_ptr<juce::AudioProcessorEditor>(p.createEditor());
 auto* about=dynamic_cast<juce::Button*>(find(*aboutEditor,"about"));
 check(about,"About button exists"); about->triggerClick();pump(100);
 auto* dialog=find(*aboutEditor,"aboutPanel");
 check(dialog&&dialog->isVisible(),"About opens inside the editor");
 bool credits=false;
 for(auto* child:dialog->getChildren())if(auto* label=dynamic_cast<juce::Label*>(child))credits=label->getText().contains("Enos Takanaka")&&label->getText().contains("AGPL-3.0-or-later")&&label->getText().contains(JucePlugin_VersionString);
 check(credits,"About contains creator, verified licence and actual build version");
 snapshot(*dialog,"artifacts/author-attribution/about.png");
 for(auto* child:dialog->getChildren()){if(auto* button=dynamic_cast<juce::Button*>(child))button->triggerClick();}
 pump(30);
 check(!dialog->isVisible(),"About closes correctly");
 std::cout<<checks<<" GUI checks passed\n";
}
