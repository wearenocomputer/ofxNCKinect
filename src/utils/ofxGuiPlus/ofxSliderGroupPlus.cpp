//
//  ofxSliderGroupPlus.cpp
//  emptyExample
//
//  Created by Roy Macdonald on 30-07-15.
//
//

#include "ofxSliderGroupPlus.h"
#include "ofxSliderGroup.h"
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                VEC SLIDER
//----------------------------------------------------------------------------------------------------------------------------



using namespace std;
//*
template<class VecType>
ofxVecSliderPlus_<VecType>::ofxVecSliderPlus_(ofParameter<VecType> value, float width, float height){
    ofxVecSlider_<VecType>::sliderChanging = false;
    setup(value, width, height);
}
//*/
template<class VecType>
ofxVecSliderPlus_<VecType> * ofxVecSliderPlus_<VecType>::setup(ofParameter<VecType> value, float width, float height){
    ofxGuiGroup::setup(value.getName(), "", 0, 0);
    
    ofxGuiGroup::parameters.clear();
    
    const string names[4] = {"x", "y", "z", "w"};
    
    this->value.makeReferenceTo(value);
    this->value.addListener(this, & ofxVecSliderPlus_::changeValue);
    
    VecType val = value;
    VecType min = value.getMin();
    VecType max = value.getMax();
#ifdef USE_GLM
	for (int i=0; i<ofxVecSlider_<VecType>::dim(); i++) {
#else
	for (int i=0; i<VecType::DIM; i++) {
#endif
		ofParameter<float> p(names[i], val[i], min[i], max[i]);
        ofxGuiGroup::add(new ofxSliderPlus<float>(p, width, height));
        p.addListener(this, & ofxVecSliderPlus_<VecType>::changeSlider);
    }
    
    ofxVecSlider_<VecType>::sliderChanging = false;
    return this;
    
}

template<class VecType>
ofxVecSliderPlus_<VecType> * ofxVecSliderPlus_<VecType>::setup(string controlName, const VecType & v, const VecType & min, const VecType & max, float width, float height){
    ofxVecSlider_<VecType>::value.set(controlName,v,min,max);
    return setup(ofxVecSliderPlus_<VecType>::value,width,height);
}

template<class VecType>
void ofxVecSliderPlus_<VecType>::changeSlider(const void * parameter, float & _value){
    ofxVecSlider_<VecType>::changeSlider(parameter, _value);
}

template<class VecType>
void ofxVecSliderPlus_<VecType>::changeValue(VecType & value){
    ofxVecSlider_<VecType>::changeValue(value);
}

template class ofxVecSliderPlus_<ofVec2f>;
template class ofxVecSliderPlus_<ofVec3f>;
template class ofxVecSliderPlus_<ofVec4f>;
#ifdef USE_GLM
template class ofxVecSliderPlus_<glm::vec2>;
template class ofxVecSliderPlus_<glm::vec3>;
template class ofxVecSliderPlus_<glm::vec4>;
#endif
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                COLOR SLIDER
//----------------------------------------------------------------------------------------------------------------------------

template<class ColorType>
ofxColorSliderPlus_<ColorType>::ofxColorSliderPlus_(ofParameter<ofColor_<ColorType> > value, float width, float height){
    setup(value, width, height);
}

template<class ColorType>
ofxColorSliderPlus_<ColorType> * ofxColorSliderPlus_<ColorType>::setup(ofParameter<ofColor_<ColorType> > value, float width, float height){

    ofxGuiGroup::setup(value.getName(), "", 0, 0);

    ofxGuiGroup::parameters.clear();
    
    const string names[4] = {"r", "g", "b", "a"};
    
    this->value.makeReferenceTo(value);
    this->value.addListener(this, &ofxColorSliderPlus_<ColorType>::changeValue);
    
    ofColor_<ColorType> val = value;
    ofColor_<ColorType> min = value.getMin();
    ofColor_<ColorType> max = value.getMax();
 
    for (int i=0; i<4; i++) {
        ofParameter<ColorType> p(names[i], val[i], min[i], max[i]);
        
        ofxGuiGroup::add(new ofxSliderPlus<ColorType>(p, width, height));
        p.addListener(this, & ofxColorSliderPlus_<ColorType>::changeSlider);
        ofxGuiGroup::collection[i]->setFillColor(value.get());
    }
    
    ofxColorSlider_<ColorType>::sliderChanging = false;
    
    return this;
}


template<class ColorType>
ofxColorSliderPlus_<ColorType> * ofxColorSliderPlus_<ColorType>::setup(string controlName, const ofColor_<ColorType> & v, const ofColor_<ColorType> & min, const ofColor_<ColorType> & max, float width, float height){
    this->value.set(controlName, v, min, max);
    return setup(this->value,width,height);
}

template<class ColorType>
void ofxColorSliderPlus_<ColorType>::changeSlider(const void * parameter, ColorType & _value){
    ofxColorSlider_<ColorType>::changeSlider(parameter, _value);
}

template<class ColorType>
void ofxColorSliderPlus_<ColorType>::changeValue(ofColor_<ColorType> & value){
    ofxColorSlider_<ColorType>::changeValue(value);
}


template class ofxColorSliderPlus_<unsigned char>;
template class ofxColorSliderPlus_<unsigned short>;
template class ofxColorSliderPlus_<float>;
