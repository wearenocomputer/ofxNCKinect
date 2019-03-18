//
//  ofxSliderPlus.h
//  ofxGuiPlus
//
//  Created by Roy Macdonald on 30-07-15.
//
//


#pragma once
#include "ofxSlider.h"
#include "ofxTextInputField.h"

template<typename Type>
class ofxSliderPlus : public ofxSlider<Type>{
public:
    ofxSliderPlus();
    ~ofxSliderPlus(){}
    
    ofxSliderPlus(ofParameter<Type> _val, float width = ofxSlider<Type>::defaultWidth, float height = ofxSlider<Type>::defaultHeight);
    ofxSliderPlus* setup(ofParameter<Type> _val, float width = ofxSlider<Type>::defaultWidth, float height = ofxSlider<Type>::defaultHeight);
    ofxSliderPlus* setup(std::string sliderName, Type _val, Type _min, Type _max, float width = ofxSlider<Type>::defaultWidth, float height = ofxSlider<Type>::defaultHeight);
	
	
   	virtual bool mouseMoved(ofMouseEventArgs & args);
    virtual bool mousePressed(ofMouseEventArgs & args);
    virtual bool mouseDragged(ofMouseEventArgs & args);
    virtual bool mouseReleased(ofMouseEventArgs & args);
    virtual bool mouseScrolled(ofMouseEventArgs & args);

    bool keyPressed(ofKeyEventArgs& args);
    bool keyReleased(ofKeyEventArgs& args);

    float fineIncrement, coarseIncrement;
    
protected:    
    void enableKeys(bool e = true);
    
    virtual void render();
    virtual void generateText();
    virtual void generateDraw();
    
    bool setValue(float mx, float my, bool bCheck);
    
    void textEditedCB(string & s);
    void textChangedCB(string & s);
    ofxTextInputField textInput;
    ofColor thisHighlightColor;
    static ofColor highlightColor;
    
    bool bPrevMouseInside;
    bool bKeysEnabled;
    
    void editValueArrows(int key);

    void enableTextEditing(bool e = true, bool bRestore = true);
    bool bIsEditingValue;
    string tempValue;
    
    void setTextInputBounds(const ofRectangle& boundingBox, const ofRectangle& sliderBox, int padding);
    void setTextInputBounds(string str);
    bool bShiftPressed, bArrowKeys;

};
typedef ofxSliderPlus<double> ofxDoubleSliderPlus;
typedef ofxSliderPlus<float> ofxFloatSliderPlus;
typedef ofxSliderPlus<int> ofxIntSliderPlus;
