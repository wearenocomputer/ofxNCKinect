//
//  ofxSliderGroupPlus.h
//  emptyExample
//
//  Created by Roy Macdonald on 30-07-15.
//
//

#pragma once

#include "ofxSliderGroup.h"
#include "ofxSliderPlus.h"


template<class VecType>
class ofxVecSliderPlus_ : public ofxVecSlider_<VecType> {
public:
    ofxVecSliderPlus_(){}
    ~ofxVecSliderPlus_(){}
    ofxVecSliderPlus_(ofParameter<VecType> value, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    
    ofxVecSliderPlus_ * setup(ofParameter<VecType> value, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    ofxVecSliderPlus_ * setup(std::string controlName, const VecType & value, const VecType & min, const VecType & max, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    
protected:
    void changeSlider(const void * parameter, float & value);
    void changeValue(VecType & value);

};

typedef ofxVecSliderPlus_<ofVec3f> ofxVec3SliderPlus;
typedef ofxVecSliderPlus_<ofVec2f> ofxVec2SliderPlus;
typedef ofxVecSliderPlus_<ofVec4f> ofxVec4SliderPlus;
typedef ofxVecSliderPlus_<ofVec3f> ofxPointSliderPlus;

template<typename ColorType>
class ofxColorSliderPlus_: public ofxColorSlider_<ColorType>{
public:
    ofxColorSliderPlus_(){}
    ~ofxColorSliderPlus_(){}
    ofxColorSliderPlus_(ofParameter<ofColor_<ColorType> > value, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    
    ofxColorSliderPlus_ * setup(ofParameter<ofColor_<ColorType> > value, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    ofxColorSliderPlus_ * setup(std::string controlName, const ofColor_<ColorType> & value, const ofColor_<ColorType> & min, const ofColor_<ColorType> & max, float width = ofxGuiGroup::defaultWidth, float height = ofxGuiGroup::defaultHeight);
    
protected:
    void changeSlider(const void * parameter, ColorType & value);
    void changeValue(ofColor_<ColorType> & value);
};

typedef ofxColorSliderPlus_<unsigned char> ofxColorSliderPlus;
typedef ofxColorSliderPlus_<unsigned short> ofxShortColorSliderPlus;
typedef ofxColorSliderPlus_<float> ofxFloatColorSliderPlus;
