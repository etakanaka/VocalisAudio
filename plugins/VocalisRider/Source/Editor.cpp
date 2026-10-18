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

namespace {
using namespace juce;
namespace Suite {
const Colour background{0xff152431}, border{0xff9daebb}, text{0xffd5deed}, muted{0xffb8c7d6};
const Colour violet{0xffc7d3df}, purple{0xff50748c}, yellow{0xffd3d63a};
void label(Graphics& g, const String& s, Rectangle<float> r, float size,
           Colour colour = text, bool bold = false, Justification align = Justification::centred)
{
    g.setColour(colour);
    g.setFont(Font(FontOptions("DejaVu Sans", size*1.14f, bold ? Font::bold : Font::plain)).withHorizontalScale(.87f));
    g.drawText(s, r, align);
}
// Deterministic, cached brushed grain, drawn in chassis coordinates so the finish
// remains continuous across individually framed panels. No reference pixels used.
const Image& grain()
{
    static const Image texture = [] {
        Image im(Image::ARGB,1000,1250,true); Graphics brush(im); Random random(0x564f4341);
        for(int y=0;y<1250;++y) for(int j=0;j<9;++j) {
            float x=float(random.nextInt(1000)), length=25.f+float(random.nextInt(260));
            brush.setColour((random.nextBool()?Colours::white:Colours::black).withAlpha(.012f+random.nextFloat()*.035f));
            brush.drawLine(x,float(y),jmin(1000.f,x+length),float(y),.5f);
        }
        return im;
    }();
    return texture;
}
void navy(Graphics& g, Rectangle<float> r)
{
    g.setGradientFill(ColourGradient(Colour(0xff233c51),r.getCentreX(),r.getCentreY(),Colour(0xff111d27),r.getRight(),r.getBottom(),true));g.fillRect(r);
    Graphics::ScopedSaveState save(g);g.reduceClipRegion(r.toNearestInt());g.setOpacity(.48f);g.drawImageAt(grain(),0,0);
}
void panel(Graphics& g, Rectangle<float> r, float radius = 10.f)
{
    g.setColour(Colours::black.withAlpha(.7f));g.fillRoundedRectangle(r.expanded(2).translated(0,2),radius+1);
    { Graphics::ScopedSaveState save(g);Path clip;clip.addRoundedRectangle(r,radius);g.reduceClipRegion(clip);navy(g,r); }
    g.setColour(Colour(0xffc2d0dc));g.drawRoundedRectangle(r,radius,1.1f);
    g.setColour(Colour(0xff070d11));g.drawRoundedRectangle(r.reduced(1.5f),radius-1,1);
    g.setColour(Colours::white.withAlpha(.10f));g.drawRoundedRectangle(r.reduced(2.5f),radius-2,.6f);
}
void digits(Graphics& g, const String& value, Rectangle<float> r, float height)
{
    const float digitWidth=height*.52f, gap=height*.15f, thick=height*.045f;
    float total=0;
    for(auto c:value)total+=c=='.'?height*.18f:digitWidth+gap;
    float x=r.getCentreX()-total/2, y=r.getCentreY()-height/2;
    const unsigned masks[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
    auto segment=[&](float sx,float sy,float length,bool vertical,bool on) {
        Path shape;
        if(vertical) {
            shape.startNewSubPath(sx,sy+thick);shape.lineTo(sx+thick,sy);shape.lineTo(sx+thick*2,sy+thick);
            shape.lineTo(sx+thick*2,sy+length-thick);shape.lineTo(sx+thick,sy+length);shape.lineTo(sx,sy+length-thick);
        } else {
            shape.startNewSubPath(sx+thick,sy);shape.lineTo(sx+length-thick,sy);shape.lineTo(sx+length,sy+thick);
            shape.lineTo(sx+length-thick,sy+thick*2);shape.lineTo(sx+thick,sy+thick*2);shape.lineTo(sx,sy+thick);
        }
        shape.closeSubPath();g.setColour(Colour(0xfff1f5ef).withAlpha(on?.95f:.025f));g.fillPath(shape,AffineTransform(1.f,-.065f,.065f*(y+height),0.f,1.f,0.f));
    };
    for(auto c:value) {
        if(c=='.'){g.setColour(Colour(0xfff1f5ef));g.fillEllipse(x,y+height-thick*2,thick*2,thick*2);x+=height*.18f;continue;}
        unsigned mask=c>='0'&&c<='9'?masks[c-'0']:c=='-'?0x40:0;
        segment(x+thick,y,digitWidth-thick*2,false,(mask&1)!=0);
        segment(x+digitWidth-thick*2,y+thick,height/2-thick,true,(mask&2)!=0);
        segment(x+digitWidth-thick*2,y+height/2,height/2-thick,true,(mask&4)!=0);
        segment(x+thick,y+height-thick*2,digitWidth-thick*2,false,(mask&8)!=0);
        segment(x,y+height/2,height/2-thick,true,(mask&16)!=0);
        segment(x,y+thick,height/2-thick,true,(mask&32)!=0);
        segment(x+thick,y+height/2-thick,digitWidth-thick*2,false,(mask&64)!=0);
        x+=digitWidth+gap;
    }
}
void readout(Graphics& g, Rectangle<float> r, const String& value, float size = 32)
{
    g.setColour(Colour(0xff030609));g.fillRoundedRectangle(r.expanded(2),4);
    g.setColour(Colour(0xff486475));g.drawRoundedRectangle(r,3,1.3f);
    g.setColour(Colour(0xff070909));g.fillRoundedRectangle(r.reduced(3),2);
    g.setGradientFill(ColourGradient(Colour(0xff3a413c).withAlpha(.22f),r.getCentre(),Colours::transparentBlack,r.getTopRight(),true));g.fillRoundedRectangle(r.reduced(3),2);
    if(value.isNotEmpty())digits(g,value,r.reduced(6),size);
}
void wordmark(Graphics& g, const String& word, Rectangle<float> r, Colour colour)
{
    GlyphArrangement letters;
    auto font=Font(FontOptions("DejaVu Sans",40.f,Font::plain)).withExtraKerningFactor(.16f);
    letters.addLineOfText(font,word,0,0);
    Path outline;letters.createPath(outline);auto bounds=outline.getBounds();
    auto transform=AffineTransform::translation(-bounds.getX(),-bounds.getY()).scaled(r.getWidth()/bounds.getWidth(),r.getHeight()/bounds.getHeight()).translated(r.getX(),r.getY());
    g.setColour(Colours::black.withAlpha(.6f));g.fillPath(outline,transform.translated(0,1.3f));
    g.setGradientFill(ColourGradient(colour.brighter(.2f),r.getTopLeft(),colour.darker(.18f),r.getBottomLeft(),false));g.fillPath(outline,transform);
}
void glowLine(Graphics& g, Line<float> line, float width = 3.f)
{
    g.setColour(Colour(0xff640e0e)); g.drawLine(line, width+2);
    g.setColour(Colour(0xffff4141)); g.drawLine(line, width);
    g.setColour(Colour(0xffffe3da)); g.drawLine(line, .7f);
}
void cap(Graphics& g, float x, float y, float width, float height)
{
    const bool main=height>90;
    auto r=Rectangle<float>(x-width/2,y-height/2,width,height);
    for(int i=6;i>0;--i) {
        g.setColour(Colours::black.withAlpha(.065f));
        g.fillRoundedRectangle(r.expanded(float(i)).translated(2,4),3);
    }
    ColourGradient casing(Colour(0xffe4e7e1),r.getTopLeft(),Colour(0xff424846),r.getBottomRight(),false);
    casing.addColour(.15,Colour(0xffe9ece7));casing.addColour(.7,Colour(0xff9b9f99));
    g.setGradientFill(casing);g.fillRoundedRectangle(r,3);
    g.setColour(Colour(0xff292f2e));g.drawRoundedRectangle(r,3,1);
    // The lower bevel is a separate plane, rather than a flat rounded rectangle.
    auto bevel=Rectangle<float>(r.getX()+2,r.getBottom()-(main?22.f:15.f),width-4,main?20.f:13.f);
    g.setGradientFill(ColourGradient(Colour(0xffb8bbb5),bevel.getTopLeft(),Colour(0xff5b605d),bevel.getBottomLeft(),false));g.fillRoundedRectangle(bevel,2);
    if(main) {
        auto face=r.reduced(5,5).withBottom(r.getBottom()-18);
        ColourGradient metal(Colour(0xfffafbf6),face.getTopLeft(),Colour(0xffd5d8d1),face.getBottomLeft(),false);
        metal.addColour(.09,Colour(0xffeef0e9));metal.addColour(.16,Colour(0xffaeb3ad));metal.addColour(.58,Colour(0xffd5d8d0));metal.addColour(.87,Colour(0xffe2e5dc));
        g.setGradientFill(metal);g.fillRoundedRectangle(face,2);
        for(float yy=face.getY()+24;yy<face.getBottom()-5;yy+=12.5f) {
            g.setColour(Colour(0xff757c74).withAlpha(.75f));g.drawLine(face.getX()+1,yy,face.getRight()-1,yy,1);
            g.setColour(Colours::white.withAlpha(.9f));g.drawLine(face.getX()+1,yy+2,face.getRight()-1,yy+2,1.1f);
        }
        g.setColour(Colour(0xfff4f6ef));g.drawRoundedRectangle(face,2,1);
        g.setColour(Colours::black.withAlpha(.23f));g.drawLine(face.getX()-1,face.getY()+2,face.getX()-1,face.getBottom(),1);
        glowLine(g,{{r.getX()+2,y},{r.getRight()-2,y}},2.3f);
    } else {
        auto top=Rectangle<float>(r.getX()+3,r.getY()+3,width-6,15);
        g.setGradientFill(ColourGradient(Colour(0xfff9fbf5),top.getTopLeft(),Colour(0xffd9ddd5),top.getBottomLeft(),false));g.fillRect(top);
        auto slot=Rectangle<float>(r.getX()+3,r.getY()+19,width-6,3.5f);
        g.setColour(Colour(0xff222926));g.fillRect(slot);
        g.setColour(Colour(0xffeef0e9));g.drawLine(slot.getX(),slot.getBottom()+1,slot.getRight(),slot.getBottom()+1,1.2f);
        Path face;
        face.startNewSubPath(r.getX()+4,r.getY()+25);face.lineTo(r.getRight()-4,r.getY()+25);
        face.lineTo(r.getRight()-3,r.getBottom()-14);face.lineTo(r.getX()+3,r.getBottom()-14);face.closeSubPath();
        g.setGradientFill(ColourGradient(Colour(0xffebeee6),x,r.getY()+25,Colour(0xffaeb4ab),x,r.getBottom()-14,false));g.fillPath(face);
        for(float yy=r.getY()+26;yy<r.getBottom()-14;yy+=1.5f) {
            g.setColour(Colours::white.withAlpha(.22f));g.drawLine(r.getX()+4,yy,r.getRight()-4,yy,.5f);
        }
        g.setColour(Colours::white.withAlpha(.8f));g.drawLine(r.getX()+3,r.getBottom()-13,r.getRight()-3,r.getBottom()-13,.9f);
    }
    g.setColour(Colour(0xffe3e7df));g.drawRoundedRectangle(r.reduced(1),2,.85f);
    g.setColour(Colours::white.withAlpha(.65f));g.drawLine(r.getX()+2,r.getY()+2,r.getX()+2,r.getBottom()-4,.7f);
}
void track(Graphics& g, float x, float top, float bottom, bool main = false)
{
    const float width=main?61.f:52.f;
    auto well=Rectangle<float>(x-width/2,top-39,width,bottom-top+84);
    g.setGradientFill(ColourGradient(Colour(0xff152735),well.getTopLeft(),Colour(0xff233645),well.getTopRight(),false));g.fillRoundedRectangle(well,4);
    g.setColour(Colour(0xff536671));g.drawRoundedRectangle(well,4,.8f);
    auto slot=Rectangle<float>(x-8,top-26,16,bottom-top+57);
    g.setColour(Colour(0xff020405));g.fillRoundedRectangle(slot,8);
    g.setColour(Colour(0xff7f8174));g.drawRoundedRectangle(slot,8,1.2f);
    g.setColour(Colours::black.withAlpha(.8f));g.drawLine(x-6,slot.getY()+10,x-6,slot.getBottom()-9,2);
}
void silverRail(Graphics& g, Rectangle<float> r)
{
    // Median RGB samples from the supplied Oct 18 silver reference, x=9..69.
    // Both rails use the same left-lit profile and restrained horizontal grain.
    ColourGradient metal(Colour(0xffe3e6e5),r.getTopLeft(),
                         Colour(0xff4e5964),r.getTopRight(),false);
    metal.addColour(2.0/62,Colour(0xfff0f0ee));
    metal.addColour(4.0/62,Colour(0xffe6e8e5));
    metal.addColour(12.0/62,Colour(0xffd5d8d7));
    metal.addColour(24.0/62,Colour(0xffbbc0c2));
    metal.addColour(32.0/62,Colour(0xffaab1b4));
    metal.addColour(44.0/62,Colour(0xff90999f));
    metal.addColour(60.0/62,Colour(0xff6e7983));
    g.setGradientFill(metal);g.fillRect(r);
    Random brush(0x53494c56);
    for(float y=r.getY()+5;y<r.getBottom()-2;y+=8) {
        const float inset=3+brush.nextFloat()*9;
        g.setColour(Colours::white.withAlpha(.008f+brush.nextFloat()*.008f));
        g.drawLine(r.getX()+inset,y,r.getRight()-3,y,.65f);
    }
    g.setColour(Colour(0xffabb5bb));
    g.drawLine(r.getX()+.5f,r.getY(),r.getX()+.5f,r.getBottom(),1);
    g.setColour(Colour(0xff4e5964));
    g.drawLine(r.getRight()-.5f,r.getY(),r.getRight()-.5f,r.getBottom(),1);
}
void screw(Graphics& g, float x, float y, float radius=8)
{
    auto head=Rectangle<float>(x-radius,y-radius,2*radius,2*radius);
    g.setColour(Colours::black.withAlpha(.5f));
    g.fillEllipse(head.expanded(1.7f).translated(.7f,1.3f));
    g.setColour(Colour(0xff14232c));g.fillEllipse(head.expanded(1));
    // Outer bevel and a domed face share the sampled rail's cool silver tones.
    g.setGradientFill(ColourGradient(Colour(0xffe6e8e5),head.getTopLeft(),
                                    Colour(0xff6e7983),head.getBottomRight(),false));
    g.fillEllipse(head);
    auto face=head.reduced(radius*.13f);
    g.setGradientFill(ColourGradient(Colour(0xffe6e8e5),x-radius*.35f,y-radius*.45f,
                                    Colour(0xff90999f),x+radius,y+radius,true));
    g.fillEllipse(face);
    g.setColour(Colour(0xffaab1b4));g.drawEllipse(head,.65f);
    // Long arms and an open silver face make a Phillips slot, not a dark rivet
    // centre. The smallest head has a ~4.4 px cross at the default 0.48 scale.
    const float arm=radius*.65f, half=radius*.17f;
    Path slot;
    slot.startNewSubPath(x-half,y-arm);slot.lineTo(x+half,y-arm);
    slot.lineTo(x+half,y-half);slot.lineTo(x+arm,y-half);
    slot.lineTo(x+arm,y+half);slot.lineTo(x+half,y+half);
    slot.lineTo(x+half,y+arm);slot.lineTo(x-half,y+arm);
    slot.lineTo(x-half,y+half);slot.lineTo(x-arm,y+half);
    slot.lineTo(x-arm,y-half);slot.lineTo(x-half,y-half);slot.closeSubPath();
    g.setColour(Colour(0xfff0f0ee).withAlpha(.7f));
    g.fillPath(slot,AffineTransform::translation(.4f,.5f));
    g.setColour(Colour(0xff14212b));g.fillPath(slot);
    Path shine;shine.addCentredArc(x,y,radius-.65f,radius-.65f,0,-1.5f,.1f,true);
    g.setColour(Colour(0xfff0f0ee).withAlpha(.65f));
    g.strokePath(shine,PathStrokeType(.7f));
}
void ticks(Graphics& g, float x, float top, float bottom, bool numbers = true, bool left = false)
{
    for(int step=-24;step<=24;++step) {
        const float db=float(step)*.5f,y=top+(12-db)/24*(bottom-top);
        bool major=step%12==0, medium=step%6==0;
        float length=major?28.f:medium?16.f:10.f;
        g.setColour((major?text:muted).withAlpha(major?1.f:.85f));
        g.drawLine(left?x+28-length:x,y,left?x+28:x+length,y,major?1.5f:.9f);
        if(major&&numbers&&!(left&&(step==24||step==0)))label(g,(db>0?"+":"")+String(int(db)),{left?x-43:x+40,y-13,42,26},20,text,false,left?Justification::centredRight:Justification::centredLeft);
    }
}
void led(Graphics& g, Point<float> c, bool active, Colour colour)
{
    if (active) for (int i = 5; i > 0; --i) {
        g.setColour(colour.withAlpha(.035f * float(6-i)));
        g.fillEllipse(c.x-10-float(i*2), c.y-10-float(i*2), 20+float(i*4), 20+float(i*4));
    }
    g.setColour(Colour(0xff040914)); g.fillEllipse(c.x-13, c.y-13, 26, 26);
    g.setColour(border); g.drawEllipse(c.x-13, c.y-13, 26, 26, 1);
    auto r = Rectangle<float>(c.x-9, c.y-9, 18, 18);
    g.setGradientFill(ColourGradient(active ? colour.brighter(.8f) : Colour(0xff9ca398), c.x-3,c.y-4,
                                    active ? colour.darker(.25f) : Colour(0xff171d19), c.x+8,c.y+8,true));
    g.fillEllipse(r);
    g.setColour(Colours::white.withAlpha(active?.23f:.18f));g.fillEllipse(c.x-5,c.y-6,6,4);
}
}

class SuiteLook final : public LookAndFeel_V4 {
public:
    float inputDb = -120;
    SuiteLook()
    {
        setColour(Slider::textBoxTextColourId, Suite::text);
        setColour(Slider::textBoxBackgroundColourId, Colour(0xff0a101d));
        setColour(Slider::textBoxOutlineColourId, Suite::border);
        setColour(TextEditor::highlightColourId, Suite::purple);
        setColour(ComboBox::backgroundColourId, Colour(0xff0b1220));
        setColour(ComboBox::outlineColourId, Suite::border);
        setColour(ComboBox::textColourId, Suite::text);
        setColour(PopupMenu::backgroundColourId, Colour(0xff121d30));
        setColour(PopupMenu::textColourId, Suite::text);
        setColour(PopupMenu::highlightedBackgroundColourId, Suite::purple);
    }
    Font getLabelFont(Label&) override { return Font(FontOptions("DejaVu Sans",18.f,Font::plain)); }
    void drawLabel(Graphics& g, Label& l) override
    {
        if(auto* slider=dynamic_cast<Slider*>(l.getParentComponent())) {
            auto r=l.getLocalBounds().toFloat().reduced(2);
            if(slider->getName()=="rider"||slider->getName()=="output") {
                Suite::readout(g,r,l.isBeingEdited()?String():String(slider->getValue(),1),32);
            } else if(!l.isBeingEdited()) {
                if(slider->isHorizontal())Suite::label(g,l.getText(),r,20);
                else if(slider->isMouseOverOrDragging(true)||slider->hasKeyboardFocus(true))Suite::label(g,l.getText(),r,13,Suite::muted);
            }
            return;
        }
        LookAndFeel_V4::drawLabel(g,l);
    }
    Font getComboBoxFont(ComboBox&) override { return Font(FontOptions("DejaVu Sans",16.f,Font::plain)); }
    Slider::SliderLayout getSliderLayout(Slider& s) override
    {
        Slider::SliderLayout l;
        if(s.isVertical()) {
            const bool idle=s.getName()=="idle";
            l.sliderBounds={0,16,s.getWidth(),s.getHeight()-(idle?76:168)};
            const int boxWidth=idle?70:s.getName()=="output"?120:130;
            l.textBoxBounds={s.getWidth()/2-boxWidth/2,s.getHeight()-(idle?30:86),boxWidth,idle?26:52};
            return l;
        }
        if(s.isHorizontal()) {
            l.sliderBounds={10,8,s.getWidth()-126,28};l.textBoxBounds={s.getWidth()-102,3,100,40};return l;
        }
        l.sliderBounds={0,0,s.getWidth(),s.getWidth()};
        l.textBoxBounds={s.getWidth()/2-44,s.getHeight()-18,88,18};return l;
    }
    void drawRotarySlider(Graphics& g, int x, int y, int w, int h, float pos,
                          float start, float end, Slider& s) override
    {
        ignoreUnused(s);
        const auto c=Point<float>(float(x)+float(w)/2,float(y)+float(h)/2);
        const float outer=jmin(float(w),float(h))*.5f-11, radius=outer-12;
        for(int i=0;i<=10;++i) {
            float a=start+(end-start)*float(i)/10;auto v=Point<float>(std::sin(a),-std::cos(a));
            g.setColour(Suite::text);g.drawLine({c+v*(outer-1),c+v*(outer+(i%5==0?9.f:6.f))},2.2f);
        }
        Path scale;scale.addCentredArc(c.x,c.y,outer-2,outer-2,0,start,end,true);
        g.setColour(Suite::text);g.strokePath(scale,PathStrokeType(2.4f));
        auto outerFace=Rectangle<float>(c.x-radius-5,c.y-radius-5,2*(radius+5),2*(radius+5));
        for(int i=7;i>0;--i){g.setColour(Colours::black.withAlpha(.08f));g.fillEllipse(outerFace.expanded(float(i)).translated(2,5));}
        g.setGradientFill(ColourGradient(Colour(0xffeeeeeb),outerFace.getTopLeft(),Colour(0xff414948),outerFace.getBottomRight(),false));g.fillEllipse(outerFace);
        g.setColour(Colour(0xff0b151c));g.drawEllipse(outerFace,2);
        auto face=outerFace.reduced(8);
        g.setGradientFill(ColourGradient(Colour(0xffe9ece8),face.getTopLeft(),Colour(0xff909592),face.getBottomRight(),false));g.fillEllipse(face);
        // Fine lathe rings and reflected radial streaks on the actual knob face.
        {
            Graphics::ScopedSaveState save(g);Path clip;clip.addEllipse(face);g.reduceClipRegion(clip);
            for(int i=0;i<180;++i) {
                float a=float(i)*MathConstants<float>::twoPi/180;
                float shimmer=.012f+.018f*std::abs(std::cos(a*2+.4f));
                g.setColour((i%2?Colours::white:Colours::black).withAlpha(shimmer));
                g.drawLine(c.x,c.y,c.x+std::cos(a)*radius,c.y+std::sin(a)*radius,.65f);
            }
            for(float r=4;r<radius-2;r+=1.15f){g.setColour(Colours::white.withAlpha(.04f));g.drawEllipse(c.x-r,c.y-r,2*r,2*r,.4f);}
            g.setGradientFill(ColourGradient(Colours::white.withAlpha(.3f),c.x-radius*.4f,c.y-radius*.5f,Colours::transparentWhite,c.x+radius,c.y+radius,true));g.fillEllipse(face);
        }
        g.setColour(Colour(0xffe6e8e5));g.drawEllipse(face,1);
        g.setColour(Colour(0xff727b7b));g.drawEllipse(face.expanded(1.5f),1);
        auto a=start+pos*(end-start);auto v=Point<float>(std::sin(a),-std::cos(a)),normal=Point<float>(v.y,-v.x);
        Path pointer;pointer.startNewSubPath(c+v*(radius*.38f)-normal*1.5f);pointer.lineTo(c+v*(radius*.95f)-normal*3.2f);
        pointer.lineTo(c+v*(radius*.95f)+normal*3.2f);pointer.lineTo(c+v*(radius*.38f)+normal*1.5f);pointer.closeSubPath();
        g.setColour(Colour(0xff101511));g.fillPath(pointer);
        if(s.hasKeyboardFocus(true)){g.setColour(Suite::text.withAlpha(.6f));g.drawEllipse(outerFace.expanded(2),1);}
    }

    void drawLinearSlider(Graphics& g, int x, int y, int w, int h, float pos,
                          float, float, Slider::SliderStyle style, Slider& s) override
    {
        if (style == Slider::LinearHorizontal) {
            auto r=Rectangle<float>(float(x),float(y)+float(h)*.5f-12,float(w),24);
            g.setColour(Colour(0xff050a0e));g.fillRoundedRectangle(r.expanded(11,9),5);
            g.setColour(Colour(0xff5f7c8f));g.drawRoundedRectangle(r.expanded(11,9),5,1.1f);
            g.setColour(Colours::black);g.drawRoundedRectangle(r.expanded(7,5),3,3);
            float fraction=jlimit(0.f,1.f,(inputDb+42)/42);
            for(int i=0;i<66;++i) {
                auto bar=Rectangle<float>(r.getX()+float(i)*r.getWidth()/66,r.getY(),r.getWidth()/66-2,r.getHeight());
                auto lit=i<44?Colour(0xff95ed0a):Colour(0xfff1e329);
                auto off=Colour(0xff3e4231).interpolatedWith(Colour(0xff1c2326),float(i)/66);
                g.setGradientFill(ColourGradient(float(i)/66<fraction?lit:off,bar.getTopLeft(),float(i)/66<fraction?lit.darker(.3f):off.darker(.4f),bar.getBottomRight(),false));g.fillRoundedRectangle(bar,1);
                g.setColour(Colours::white.withAlpha(.14f));g.drawLine(bar.getX()+1,bar.getY()+1,bar.getX()+1,bar.getBottom()-1,.7f);
            }
            auto cap=Rectangle<float>(pos-24,r.getY()-7,48,r.getHeight()+14);
            ColourGradient metal(Colour(0xffaab0af).withAlpha(.82f),cap.getTopLeft(),Colour(0xff6a7273).withAlpha(.82f),cap.getTopRight(),false);
            metal.addColour(.43,Colour(0xffd1d4d0).withAlpha(.85f));g.setGradientFill(metal);g.fillRect(cap);
            g.setColour(Colour(0xffd4dada).withAlpha(.55f));g.drawRect(cap,1);
            for(float xx=cap.getX()+4;xx<cap.getRight();xx+=5){g.setColour(Colours::white.withAlpha(.12f));g.drawVerticalLine(int(xx),cap.getY(),cap.getBottom());}
            Suite::glowLine(g,{{pos,cap.getY()},{pos,cap.getBottom()}},2);
            return;
        }
        auto cx = float(x+w/2);
        if (s.getName() == "idle") {
            
            Path marker; marker.addTriangle(cx-10, pos-9, cx, pos, cx-10, pos+9);
            g.setColour(Colour(0xffaeb9bb)); g.fillPath(marker); g.setColour(Suite::text);g.strokePath(marker,PathStrokeType(1.2f));
            return;
        }
        // Track wells are painted on the chassis to avoid clipping their extended ends.
        Suite::cap(g, cx, pos, s.getName() == "rider" ? 98.f : 77.f, s.getName() == "rider" ? 138.f : 57.f);
        if (s.hasKeyboardFocus(true)) {
            g.setColour(Suite::violet); g.drawRoundedRectangle(cx-36, pos-37, 72, 74, 5, 1);
        }
    }
    void drawButtonBackground(Graphics& g, Button& b, const Colour&, bool over, bool down) override
    {
        auto r=b.getLocalBounds().toFloat().reduced(2);
        if(b.getName().startsWith("attack")) {
            if(b.hasKeyboardFocus(true)){g.setColour(Suite::muted);g.drawRoundedRectangle(r.withTrimmedLeft(42),2,.7f);}
            return;
        }
        if(b.getName()=="utilities") {
            g.setColour(Suite::muted);g.drawEllipse(r.reduced(2),1.2f);
            if(over){g.setColour(Suite::text.withAlpha(.15f));g.fillEllipse(r.reduced(2));}return;
        }
        const bool mode=b.getName().startsWith("automation"),selected=b.getToggleState();
        Colour tint=b.getName()=="automationWrite"?Colour(0xff85232b):b.getName()=="automationRead"?Colour(0xff437e12):Colour(0xffc1c6c7);
        g.setColour(Colours::black.withAlpha(.8f));g.fillRoundedRectangle(r.expanded(2).translated(0,2),4);
        g.setColour(Colour(0xff071014));g.fillRoundedRectangle(r,3);
        auto face=r.reduced(3);
        auto top=selected?tint.brighter(.25f):tint, bottom=tint.darker(mode?.5f:.3f);
        if(over)top=top.brighter(.12f);
        if(down)bottom=bottom.darker(.2f);
        g.setGradientFill(ColourGradient(top,face.getTopLeft(),bottom,face.getBottomLeft(),false));g.fillRoundedRectangle(face,2);
        g.setColour(Colour(0xffafbdc2).withAlpha(.7f));g.drawRoundedRectangle(face,2,1);
        g.setColour(Colours::white.withAlpha(.4f));g.drawLine(face.getX()+2,face.getY()+1,face.getRight()-2,face.getY()+1,1.4f);
        g.setColour(Colours::black.withAlpha(.4f));g.drawLine(face.getX()+1,face.getBottom()-2,face.getRight()-1,face.getBottom()-2,2);
        if(b.hasKeyboardFocus(true)){g.setColour(Suite::text);g.drawRoundedRectangle(r.expanded(1),4,1);}
    }
    void drawButtonText(Graphics& g, TextButton& b, bool, bool) override
    {
        auto r=b.getLocalBounds().toFloat();
        if(b.getName().startsWith("attack")){r.removeFromLeft(47);Suite::label(g,b.getButtonText(),r,22,Suite::text,false,Justification::centredLeft);return;}
        if(b.getName()=="utilities"){Suite::label(g,"i",r,18);return;}
        const bool mode=b.getName().startsWith("automation"),silver=b.getName()=="automationOff";
        if(mode){Suite::led(g,{24.f,r.getCentreY()},b.getToggleState(),b.getName()=="automationRead"?Colour(0xff7ff000):b.getName()=="automationWrite"?Colour(0xfffc3334):Colour(0xffdce4dd));r.removeFromLeft(36);}
        Suite::label(g,b.getButtonText(),r,mode?19.f:14.f,silver?Colour(0xff152432):mode?Colour(0xfff4f5f0):Colour(0xff12202a));
    }
    void drawComboBox(Graphics& g, int w, int h, bool, int, int, int, int, ComboBox&) override
    {
        auto r = Rectangle<float>(.5f, .5f, float(w)-1, float(h)-1);
        g.setColour(Colour(0xff0a111e)); g.fillRoundedRectangle(r, 5);
        g.setColour(Suite::border); g.drawRoundedRectangle(r, 5, 1);
        Path arrow; arrow.startNewSubPath(float(w-26), float(h/2-3));
        arrow.lineTo(float(w-20), float(h/2+3)); arrow.lineTo(float(w-14), float(h/2-3));
        g.setColour(Suite::muted); g.strokePath(arrow, PathStrokeType(1.5f));
    }
};

class LevelMeter final : public Component {
    const std::atomic<float>& rms;
    const std::atomic<float>* peak;
public:
    LevelMeter(const std::atomic<float>& value, const std::atomic<float>* peakValue = nullptr)
        : rms(value), peak(peakValue) { setInterceptsMouseClicks(false, false); }
    void paint(Graphics& g) override
    {
        auto top = 16.f, bottom = float(getHeight()-60), cx = float(getWidth())*.53f;
        auto r = Rectangle<float>(cx-9, top, 18, bottom-top);
        g.setColour(Colour(0xff070d18)); g.fillRect(r);
        auto f = jlimit(0.f, 1.f, (rms.load()+60)/60);
        ColourGradient gradient(Colour(0xff00dba0), cx, bottom, Suite::yellow, cx, top, false);
        gradient.addColour(.68, Colour(0xff74ed79));
        g.setGradientFill(gradient); g.fillRect(r.withTop(bottom-f*r.getHeight()));
        g.setColour(Suite::border); g.drawRect(r, .75f);
        for (int db = -60; db <= 0; db += 3) {
            auto y = bottom-float(db+60)/60*r.getHeight();
            g.setColour(Colour(0xff354766)); g.drawLine(cx-9, y, cx+9, y, .6f);
        }
        for (int db : {0, -6, -12, -24, -48}) {
            auto y = bottom-float(db+60)/60*r.getHeight();
            if(getWidth()>65) Suite::label(g, String(db), {0, y-9, cx-18, 18}, 12, Suite::muted, false, Justification::centredRight);
            g.setColour(Suite::muted); g.drawLine(cx-15, y, cx-11, y, 1);
        }
        if (peak) {
            auto y = bottom-jlimit(0.f, 1.f, (peak->load()+60)/60)*r.getHeight();
            g.setColour(Suite::text); g.drawLine(cx-9, y, cx+9, y, 1.2f);
        }
        Suite::readout(g, {2, float(getHeight()-30), float(getWidth()-4), 28}, String(rms.load(), 1), 13);
    }
};

class RangeControl final : public Component, public SettableTooltipClient {
    Vocalis& processor;
    int handle = 1;
    float origin = 0, start = 0;
    bool dragging = false;
    float position(float db) const { return 16+(12-db)/24*float(getHeight()-76); }
    const char* id() const { return handle ? "rangeMax" : "rangeMin"; }
public:
    explicit RangeControl(Vocalis& p) : processor(p)
    {
        setName("range"); setWantsKeyboardFocus(true);
        setTooltip("Range Min / Max: drag either handle. Shift for fine adjustment. Arrow Left/Right selects a handle; Up/Down adjusts. Double-click resets -6 / +6 dB.");
    }
    // Leave the adjoining Idle marker's hit area clear. The visual wells are
    // chassis-painted, but the existing Range and Idle gestures stay independent.
    bool hitTest(int x, int) override { return x >= getWidth()/2-32; }
    ~RangeControl() override
    {
        if (dragging) processor.state.getParameter(id())->endChangeGesture();
    }
    void paint(Graphics& g) override
    {
        auto cx = float(getWidth())/2;

        auto top=position(processor.value("rangeMax")),bottom=position(processor.value("rangeMin"));
        g.setGradientFill(ColourGradient(Suite::yellow.withAlpha(.20f),cx-26,top,Suite::yellow.withAlpha(.04f),cx+26,bottom,false));
        g.fillRect(cx-26,top,52.f,bottom-top);
        for (int i : {0, 1}) {
            auto y = position(processor.value(i ? "rangeMax" : "rangeMin"));
            Suite::cap(g, cx, y, 77, 57);
            if (hasKeyboardFocus(true) && handle == i) { g.setColour(Suite::violet); g.drawRoundedRectangle(cx-34, y-20, 68, 40, 4, 1); }
        }
    }
    void change(float v)
    {
        if (handle) v = jlimit(jmax(-11.f, processor.value("rangeMin")), 12.f, v);
        else v = jlimit(-12.f, jmin(11.f, processor.value("rangeMax")), v);
        processor.set(id(), v); repaint();
    }
    void mouseDown(const MouseEvent& e) override
    {
        handle = std::abs(e.position.y-position(processor.value("rangeMax"))) < std::abs(e.position.y-position(processor.value("rangeMin"))) ? 1 : 0;
        start = e.position.y; origin = processor.value(id()); dragging = true;
        processor.state.getParameter(id())->beginChangeGesture();
    }
    void mouseDrag(const MouseEvent& e) override
    { change(origin+(start-e.position.y)*24/float(getHeight()-76)*(e.mods.isShiftDown() ? .1f : 1.f)); }
    void mouseUp(const MouseEvent&) override
    { if (dragging) processor.state.getParameter(id())->endChangeGesture(); dragging = false; }
    void mouseDoubleClick(const MouseEvent&) override
    {
        for (const auto& item : {std::pair{"rangeMin", -6.f}, std::pair{"rangeMax", 6.f}}) {
            auto* parameter = processor.state.getParameter(item.first);
            const bool alreadyDragging = dragging && String(id()) == item.first;
            if (!alreadyDragging) parameter->beginChangeGesture();
            processor.set(item.first, item.second);
            if (!alreadyDragging) parameter->endChangeGesture();
        }
        repaint();
    }
    bool keyPressed(const KeyPress& k) override
    {
        if (k == KeyPress::leftKey || k == KeyPress::rightKey) { handle = k == KeyPress::rightKey ? 1 : 0; repaint(); return true; }
        if (k == KeyPress::upKey || k == KeyPress::downKey) {
            processor.state.getParameter(id())->beginChangeGesture();
            change(processor.value(id())+(k == KeyPress::upKey ? 1.f : -1.f)*(k.getModifiers().isShiftDown() ? .1f : 1.f));
            processor.state.getParameter(id())->endChangeGesture(); return true;
        }
        return false;
    }
};

class AboutPanel final : public Component {
    Label content;
    TextButton close{"CLOSE"};
public:
    AboutPanel()
    {
        setName("aboutPanel");
        content.setText(String::fromUTF8("VOCALIS AUDIO\nVocalis Rider Live\nVersion ") + JucePlugin_VersionString
            + String::fromUTF8("\n\nCreated by Enos Takanaka\nDeveloped with AI assistance using OpenAI Codex, C++ and JUCE.\nNative Linux VST3 | REAPER\n\nCopyright © 2026 Enos Takanaka and applicable contributors.\nOpen-source licence: AGPL-3.0-or-later\n\nGitHub:\nhttps://github.com/etakanaka/VocalisAudio"), dontSendNotification);
        content.setFont(FontOptions(17.f));
        content.setColour(Label::textColourId, Suite::text);
        content.setJustificationType(Justification::centred);
        addAndMakeVisible(content); addAndMakeVisible(close);
        close.onClick = [this]{ setVisible(false); };
    }
    void paint(Graphics& g) override { Suite::panel(g, getLocalBounds().toFloat()); }
    void resized() override
    {
        content.setBounds(16, 12, getWidth()-32, getHeight()-70);
        close.setBounds(getWidth()/2-50, getHeight()-48, 100, 32);
    }
};

class UtilityPanel final : public Component {
public:
    void paint(Graphics& g) override {
        Suite::panel(g,getLocalBounds().toFloat().reduced(3),9);
        Suite::label(g,"PRESETS & CAPTURE",{20,12,400,30},19);
        Suite::label(g,"INPUT",{425,18,65,22},12);
        Suite::label(g,"MUSIC",{510,18,65,22},12);
    }
};

class Editor final : public AudioProcessorEditor, private Timer {
    Vocalis& p;
    SuiteLook look;
    Component canvas;
    UtilityPanel utilityPanel;
    TextButton utilities{"i"};
    Slider target, spill, vocal, music, rider, output, idle;
    RangeControl range;
    LevelMeter inputMeter, musicMeter;
    TextButton slow{"Slow"}, fast{"Fast"}, off{"OFF"}, write{"WRITE"}, read{"READ"};
    TextButton bypass{"BYPASS"}, exportButton{"EXPORT CSV"}, clear{"CLEAR"}, about{"ABOUT"};
    ComboBox presets;
    TooltipWindow tooltip{this, 650};
    std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::unique_ptr<FileChooser> chooser;
    AboutPanel aboutPanel;
    void addSlider(Slider& s, const char* id, float lo, float hi, float def, Slider::SliderStyle style)
    {
        canvas.addAndMakeVisible(s); s.setName(id); s.setTitle(id);
        s.setSliderStyle(style); s.setRange(lo, hi, .1); s.setDoubleClickReturnValue(true, def);
        s.setTextBoxStyle(Slider::TextBoxBelow, false, 96, 28);
        s.setNumDecimalPlacesToDisplay(1); s.setTextValueSuffix(" dB");
        s.setWantsKeyboardFocus(true);
        s.setTooltip(String(id)+": drag to adjust; Shift for fine adjustment, arrows for steps; double-click restores the default. Values in dB.");
        attachments.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.state, id, s));
    }
    void setControlValue(const char* id, float value)
    {
        auto* parameter = p.state.getParameter(id);
        parameter->beginChangeGesture();
        p.set(id, value);
        parameter->endChangeGesture();
    }
    void addButton(TextButton& b, const char* name, std::function<void()> f)
    { canvas.addAndMakeVisible(b); b.setName(name); b.setWantsKeyboardFocus(true); b.onClick = std::move(f); }
    void timerCallback() override
    {
        bool reading = p.value("automation") == 2;
        rider.setEnabled(reading);
        if (!reading) rider.setValue(p.meterGain.load(), dontSendNotification);
        off.setToggleState(p.value("automation") == 0, dontSendNotification);
        write.setToggleState(p.value("automation") == 1, dontSendNotification);
        read.setToggleState(reading, dontSendNotification);
        slow.setToggleState(p.value("attack") == 0, dontSendNotification);
        fast.setToggleState(p.value("attack") == 1, dontSendNotification);
        bypass.setToggleState(p.value("bypass") > .5f, dontSendNotification);
        presets.setSelectedId(p.getCurrentProgram()+1, dontSendNotification);
        look.inputDb = p.meterInput.load();
        canvas.repaint(); repaint();
    }
public:
    explicit Editor(Vocalis& processor)
        : AudioProcessorEditor(processor), p(processor), range(p), inputMeter(p.meterInput, &p.meterPeak), musicMeter(p.meterMusic)
    {
        setLookAndFeel(&look); setOpaque(true); addAndMakeVisible(canvas);
        canvas.setInterceptsMouseClicks(false, true);
        addSlider(target, "target", -42, 0, -21, Slider::LinearHorizontal);
        addSlider(spill, "spill", -48, -12, -48, Slider::RotaryHorizontalVerticalDrag);
        addSlider(vocal, "vocal", -12, 12, 0, Slider::RotaryHorizontalVerticalDrag);
        addSlider(music, "music", -12, 12, 0, Slider::RotaryHorizontalVerticalDrag);
        addSlider(rider, "rider", -12, 12, 0, Slider::LinearVertical);
        addSlider(output, "output", -12, 12, 0, Slider::LinearVertical);
        addSlider(idle, "idle", -12, 12, 0, Slider::LinearVertical);
        rider.setTooltip("Automatic Rider gain before Output/Bypass in Off and Write. Host-automated/manual Rider in Read.");
        idle.setTooltip("Idle return position: drag the yellow marker; arrows adjust, double-click resets to 0 dB. Engine bounds Idle to Range.");
        canvas.addAndMakeVisible(range); canvas.addAndMakeVisible(inputMeter); canvas.addAndMakeVisible(musicMeter);
        addButton(slow, "attackSlow", [this]{ setControlValue("attack", 0); });
        addButton(fast, "attackFast", [this]{ setControlValue("attack", 1); });
        addButton(off, "automationOff", [this]{ setControlValue("automation", 0); });
        addButton(write, "automationWrite", [this]{ setControlValue("automation", 1); });
        addButton(read, "automationRead", [this]{ setControlValue("automation", 2); });
        addButton(bypass, "bypass", [this]{ setControlValue("bypass", p.value("bypass") > .5f ? 0 : 1); });
        addButton(clear, "clearCapture", [this]{ p.clearCapture(); });
        addButton(exportButton, "exportCapture", [this]{
            chooser = std::make_unique<FileChooser>("Export captured Rider automation", File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Vocalis-rider.csv"), "*.csv");
            chooser->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting,
                [safe = Component::SafePointer<Editor>(this)](const FileChooser& c){
                    if (safe && c.getResult() != File() && !safe->p.exportCsv(c.getResult()))
                        AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Export failed", "Could not write the selected CSV file.");
                });
        });
        exportButton.setTooltip("Export actual Write-mode Rider points as seconds,dB CSV. Host recording support depends on REAPER settings.");
        addButton(about, "about", [this]{
            aboutPanel.setVisible(!aboutPanel.isVisible());
            aboutPanel.toFront(false);
        });
        clear.setTooltip("Clear volatile automation capture. Stop Write before clearing a pass.");
        canvas.addAndMakeVisible(presets); presets.setName("preset");
        for (int i = 0; i < 8; ++i) presets.addItem(p.getProgramName(i), i+1);
        presets.onChange = [this]{ p.setCurrentProgram(presets.getSelectedId()-1); };
        canvas.addChildComponent(aboutPanel);
        canvas.addChildComponent(utilityPanel);utilityPanel.setName("utilityPanel");
        for(Component* c : std::initializer_list<Component*>{&presets,&bypass,&exportButton,&clear,&about,&inputMeter,&musicMeter})utilityPanel.addAndMakeVisible(c);
        addButton(utilities,"utilities",[this]{utilityPanel.setVisible(!utilityPanel.isVisible());utilityPanel.toFront(false);});
        utilities.setTooltip("Presets, bypass, CSV export, clear capture, About, input and music meters");
        setResizable(true, false); setResizeLimits(480, 600, 1120, 1400);
        getConstrainer()->setFixedAspectRatio(.8); setSize(480, 600);
        timerCallback(); startTimerHz(30);
    }
    ~Editor() override { stopTimer(); setLookAndFeel(nullptr); }
    void resized() override
    {
        canvas.setTransform(AffineTransform()); canvas.setBounds(0, 0, 1000, 1250);
        canvas.setTransform(AffineTransform::scale(jmin(getWidth()/1000.f,getHeight()/1250.f)));
        aboutPanel.setBounds(175, 300, 650, 440);
        utilityPanel.setBounds(200,744,600,286);
        presets.setBounds(30,65,350,38);bypass.setBounds(30,120,150,38);
        exportButton.setBounds(195,120,185,38);clear.setBounds(30,177,150,38);about.setBounds(195,177,185,38);
        inputMeter.setBounds(425,45,65,220);musicMeter.setBounds(510,45,65,220);
        utilities.setBounds(715,1053,26,26);
        target.setBounds(254,34,619,48);
        spill.setBounds(288,155,126,156);vocal.setBounds(482,135,146,176);music.setBounds(705,155,126,156);
        fast.setBounds(143,198,125,38);slow.setBounds(143,239,125,38);
        range.setBounds(161,401,110,534);rider.setBounds(428,401,144,626);output.setBounds(693,401,144,626);
        idle.setBounds(154,401,56,534);
        off.setBounds(309,1076,123,50);write.setBounds(438,1076,123,50);read.setBounds(567,1076,123,50);

    }

    void paint(Graphics& g) override
    {
        Graphics::ScopedSaveState saved(g);g.fillAll(Colour(0xff111b23));
        g.addTransform(AffineTransform::scale(jmin(getWidth()/1000.f,getHeight()/1250.f)));
        Suite::navy(g,{84,0,832,1250});
        Suite::navy(g,{87,4,827,306});Suite::navy(g,{87,1143,827,104});
        for(float x:{0.f,917.f}) {
            Suite::silverRail(g,{x,1,83,1248});
            for(float y:{30.f,1198.f})Suite::screw(g,x+32,y,12);
        }
        g.setColour(Colour(0xff080e13));g.drawRect(85.f,2.f,831.f,1245.f,3.f);
        g.setColour(Colour(0xff607b91));g.drawRect(87.f,3.f,827.f,1243.f,1.f);
        for(float x:{109.f,892.f})for(float y:{23.f,1209.f})Suite::screw(g,x,y,10);
        Suite::label(g,"Target",{133,37,105,33},24,Suite::text,true);
        Suite::led(g,{155,108},p.activity.load(),Colour(0xff80f000));Suite::label(g,"Ride",{180,92,95,31},22,Suite::text,false,Justification::centredLeft);
        Suite::led(g,{155,148},!p.activity.load(),Colour(0xffbac4b6));Suite::label(g,"Idle",{180,132,95,31},22,Suite::text,false,Justification::centredLeft);
        Suite::label(g,"Sensitivity",{516,96,110,27},21);
        g.setColour(Suite::text);g.drawLine(276,112,507,112,1.2f);g.drawLine(632,112,807,112,1.2f);g.drawLine(276,112,276,121,1.2f);g.drawLine(807,112,807,121,1.2f);
        // One mechanical switch face, with the existing two button hit areas.
        auto sw=Rectangle<float>(143,200,33,77);
        g.setColour(Colours::black);g.fillRoundedRectangle(sw,3);g.setColour(Colour(0xff648193));g.drawRoundedRectangle(sw,3,1.2f);
        g.setColour(Colour(0xff38464e));g.drawRoundedRectangle(sw.reduced(3),1,1);
        float sy=p.value("attack")==1?208.f:244.f;
        auto lever=Rectangle<float>(150,sy,18,28);
        g.setColour(Colours::black);g.fillRect(lever.translated(2,3));
        g.setGradientFill(ColourGradient(Colour(0xfff2f5ed),lever.getTopLeft(),Colour(0xff949c96),lever.getBottomLeft(),false));g.fillRoundedRectangle(lever,2);
        g.setColour(Colour(0xffdfe6df));g.drawRoundedRectangle(lever,2,1);
        for(int i=4;i<19;i+=2){g.setColour(Colour(0xff737b76));g.drawLine(lever.getX()+2,sy+float(i),lever.getRight()-2,sy+float(i),.6f);}
        for(auto item:{std::pair{351.f,"Spill"},std::pair{555.f,"Vocal"},std::pair{768.f,"Music"}}) {
            Suite::label(g,item.second,{item.first-55,272,110,29},23);
            Suite::label(g,"-",{item.first-69,251,20,28},25);Suite::label(g,"+",{item.first+49,251,20,28},25);
        }
        g.setColour(Colour(0xff03090d));g.drawLine(87,311,914,311,4);
        g.setColour(Colour(0xff8ba3b6));g.drawLine(88,311,914,311,1);
        for(auto item:{std::pair{102.f,"Range"},std::pair{367.f,"Rider"},std::pair{647.f,"Output"}}) {
            float width=item.first==367.f?269.f:253.f;
            Suite::panel(g,{item.first,323,width,708},10);
            Suite::label(g,item.second,{item.first+55,335,width-110,32},23);
            Suite::screw(g,item.first+15,338,7);Suite::screw(g,item.first+15,1017,7);
            Suite::screw(g,item.first+width-15,338,7);Suite::screw(g,item.first+width-15,1017,7);
            float info=item.first+width-48;g.setColour(Suite::muted);g.drawEllipse(info-10,341,20,20,1.2f);Suite::label(g,"i",{info-8,340,16,21},16);
        }
        Suite::track(g,216,417,875);Suite::track(g,500,417,875,true);Suite::track(g,765,417,875);
        Suite::ticks(g,253,417,875);Suite::ticks(g,538,417,875);Suite::ticks(g,800,417,875);Suite::ticks(g,434,417,875,true,true);
        const float idleY=417+(12-p.value("idle"))/24*458;
        g.setColour(Suite::yellow.withAlpha(.8f));g.drawLine(192,idleY,242,idleY,1);
        Suite::glowLine(g,{{369,646},{462,646}},2);
        Suite::readout(g,{159,944,125,48},String(p.value("rangeMax")-p.value("rangeMin"),1),32);
        Suite::label(g,"dB",{290,948,40,40},21);
        Suite::label(g,String(p.value("rangeMin"),1)+" / "+String(p.value("rangeMax"),1)+" dB",{157,998,130,18},12,Suite::muted);
        Suite::label(g,"dB",{573,948,40,40},21);Suite::label(g,"dB",{838,948,40,40},21);
        Suite::label(g,"Automation",{442,1042,120,29},21);
        g.setColour(Suite::text);g.drawLine(309,1058,432,1058,1);g.drawLine(567,1058,690,1058,1);g.drawLine(309,1058,309,1067,1);g.drawLine(690,1058,690,1067,1);
        if(p.dropped.load()>0)Suite::label(g,"CAPTURE FULL / DROPPED POINTS",{345,1126,315,14},10,Suite::yellow);
        g.setColour(Colour(0xff0a131b));g.drawLine(87,1140,914,1140,5);g.setColour(Colour(0xff6e8aa0));g.drawLine(88,1141,913,1141,2);g.setColour(Colour(0xffa6b9c7).withAlpha(.5f));g.drawLine(88,1140,913,1140,1);
        for(int i=0;i<6;++i){float heights[]={18,33,50,34,19,11};auto h=heights[i];g.setColour(Colour(0xffdce2e4));g.fillRoundedRectangle(158.f+float(i)*9,1190-h/2,4,h,2);}
        Suite::wordmark(g,"VOCALIS",{227,1178,226,28},Colour(0xffd7dde1));
        Suite::wordmark(g,"RIDER",{612,1179,121,28},Colour(0xffd4dce3));
        Suite::wordmark(g,"LIVE",{757,1179,91,28},Colour(0xff65b5e8));

    }
};
}

juce::AudioProcessorEditor* Vocalis::createEditor() { return new Editor(*this); }
