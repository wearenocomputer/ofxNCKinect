//
//  ofxSliderPlus.cpp
//  emptyExample
//
//  Created by Roy Macdonald on 30-07-15.
//
//

#include "ofxSliderPlus.h"
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                STATIC INIT
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
ofColor ofxSliderPlus<Type>::highlightColor(200) ;

//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                CONSTRUTORS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
ofxSliderPlus<Type>::ofxSliderPlus():bPrevMouseInside(false), bKeysEnabled(false), bIsEditingValue(false), bShiftPressed(false), bArrowKeys(false){
    thisHighlightColor= this->highlightColor;
}
//--------------------------------------------------------------
template<typename Type>
ofxSliderPlus<Type>::ofxSliderPlus(ofParameter<Type> _val, float width, float height){
    setup(_val,width,height);
}

//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                SETUP
//----------------------------------------------------------------------------------------------------------------------------
template<typename Type>
ofxSliderPlus<Type>* ofxSliderPlus<Type>::setup(ofParameter<Type> _val, float width, float height){
    
    ofxSlider<Type>::setup(_val, width, height);
    textInput.setup(false);
    textInput.setVerticalPadding(0);
    textInput.setHorizontalPadding(0);
    textInput.disable();
    float range = ofxSlider<Type>::value.getMax() - ofxSlider<Type>::value.getMin();
    fineIncrement = range/1000;
    coarseIncrement = range/100;
    ofAddListener(textInput.onHitReturn, this, &ofxSliderPlus<Type>::textEditedCB);
    ofAddListener(textInput.onTextChange, this, &ofxSliderPlus<Type>::textChangedCB);
    return  this;
}
//--------------------------------------------------------------
template<typename Type>
ofxSliderPlus<Type>* ofxSliderPlus<Type>::setup(string sliderName, Type _val, Type _min, Type _max, float width, float height){
    ofxSlider<Type>::value.set(sliderName,_val,_min,_max);
    return setup(ofxSlider<Type>::value,width,height);
}

//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                SETTERS
//----------------------------------------------------------------------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::setValue(float mx, float my, bool bCheck){
    if( !ofxSlider<Type>::isGuiDrawing() ){
        ofxSlider<Type>::bGuiActive = false;
        return false;
    }
    if( bCheck ){
        if( ofxSlider<Type>::b.inside(mx, my) ){
            ofxSlider<Type>::bGuiActive = true;
        }else{
            ofxSlider<Type>::bGuiActive = false;
        }
    }
    if( ofxSlider<Type>::bGuiActive ){
        ofxSlider<Type>::value = ofMap(mx, ofxSlider<Type>::b.x, ofxSlider<Type>::b.x + ofxSlider<Type>::b.width, ofxSlider<Type>::value.getMin(), ofxSlider<Type>::value.getMax(), true);
        return true;
    }
    return false;
}
//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::setTextInputBounds(string str){
    setTextInputBounds(ofxSlider<Type>::getTextBoundingBox(str,0,0), ofxSlider<Type>::b, ofxSlider<Type>::textPadding);
}
//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::setTextInputBounds(const ofRectangle& boundingBox, const ofRectangle& sliderBox, int padding){
    textInput.bounds.height = boundingBox.height;
    textInput.bounds.width = boundingBox.width;
    textInput.bounds.x = sliderBox.getMaxX() - padding - boundingBox.width;
    textInput.bounds.y = sliderBox.y + (sliderBox.height - boundingBox.height)/ 2;
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                ENABLES
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
void ofxSliderPlus<Type>::enableTextEditing(bool e, bool bRestore){
    if (e != bIsEditingValue) {
        bIsEditingValue = e;
        if (bIsEditingValue) {
            textInput.enable();
            textInput.beginEditing();
            tempValue = textInput.text;
        }else{
            if (bRestore) {
                this->value = ofToFloat(tempValue);
            }
            textInput.endEditing();
            textInput.disable();
        }
    }
}

//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::enableKeys(bool e){
    if (e != bKeysEnabled) {
        if (e) {
            ofRegisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
        }else{
            ofUnregisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
            if (bIsEditingValue) {
                enableTextEditing(false);
            }
        }
        bKeysEnabled = e;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                MOUSE CALLBACKS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
bool ofxSliderPlus<Type>::mouseMoved(ofMouseEventArgs & args){
    bool mInside = ofxSlider<Type>::mouseMoved(args);
    if (bPrevMouseInside != mInside) {
        ofxBaseGui::setNeedsRedraw();
        bPrevMouseInside = mInside;
        enableKeys(mInside);
    }
    
    return mInside;
}
//--------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::mousePressed(ofMouseEventArgs & args){
    if (bIsEditingValue) {
        if (textInput.bounds.inside(args.x, args.y)) {
            textInput.mousePressed(args);
            return true;
        }
    }
    return  ofxSlider<Type>::mousePressed(args);
}
//--------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::mouseDragged(ofMouseEventArgs & args){
    if (bIsEditingValue) {
        if (textInput.bounds.inside(args.x, args.y)) {
            textInput.mouseDragged(args);
            return true;
        }
    }
    return  ofxSlider<Type>::mouseDragged(args);
}
//--------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::mouseReleased(ofMouseEventArgs & args){
    if (bIsEditingValue) {
        if (textInput.bounds.inside(args.x, args.y)) {
            textInput.mouseReleased(args);
            return true;
        }
    }
    return ofxSlider<Type>::mouseReleased(args);
}
//--------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::mouseScrolled(ofMouseEventArgs & args){
    return ofxSlider<Type>::mouseScrolled(args);
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                KEYS CALLBACKS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
bool ofxSliderPlus<Type>::keyPressed(ofKeyEventArgs& args){
    int k = args.key;
    if (k == ' ') {
        enableTextEditing(!bIsEditingValue);
        return true;
    }else if(k == OF_KEY_SHIFT){
        bShiftPressed = true;
    }
    
    bArrowKeys = (k == OF_KEY_LEFT || k == OF_KEY_RIGHT || k == OF_KEY_UP || k == OF_KEY_DOWN);
    if (bIsEditingValue) {
        if (bArrowKeys || k == OF_KEY_RETURN || k == OF_KEY_BACKSPACE || k == OF_KEY_DEL || k == OF_KEY_SHIFT || k == 4352 || (k >= '0' && k <= '9')  || k == '.'   ) {
            textInput.keyPressed(args);
            return true;
        }
    }else if(bArrowKeys){
        editValueArrows(k);
        return true;
    }
    return false;
}
//--------------------------------------------------------------
template<typename Type>
bool ofxSliderPlus<Type>::keyReleased(ofKeyEventArgs& args){
    if (bIsEditingValue) {
        textInput.keyReleased(args);
        return true;
    }
    if (bShiftPressed || bArrowKeys) {
        bShiftPressed = false;
        bArrowKeys = false;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                HELPERS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
void ofxSliderPlus<Type>::editValueArrows(int key){
    switch (key) {
        case OF_KEY_LEFT:
            this->value -= fineIncrement;
            break;
        case OF_KEY_RIGHT:
            this->value += fineIncrement;
            break;
        case OF_KEY_UP:
            this->value += coarseIncrement;
            break;
        case OF_KEY_DOWN:
            this->value -= coarseIncrement;
            break;
        default:
            break;
    }
    if (bShiftPressed) {
        float v = this->value.get();
        this->value = round(v);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                EVENTS CALLBACKS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
void ofxSliderPlus<Type>::textEditedCB(string & s){
    this->value = ofToFloat(s);
    enableTextEditing(false, false);
}
//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::textChangedCB(string & s){
    setTextInputBounds(textInput.text);
}
//----------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------                DRAWS
//----------------------------------------------------------------------------------------------------------------------------

template<typename Type>
void ofxSliderPlus<Type>::generateDraw(){
    ofxSlider<Type>::bg.clear();
    ofxSlider<Type>::bar.clear();
    
    ofxSlider<Type>::bg.setFillColor(ofxSlider<Type>::thisBackgroundColor);
    ofxSlider<Type>::bg.setFilled(true);
    ofxSlider<Type>::bg.rectangle(ofxSlider<Type>::b.x, ofxSlider<Type>::b.y, ofxSlider<Type>::b.width, ofxSlider<Type>::b.height);
    
    float valAsPct = ofMap( ofxSlider<Type>::value, ofxSlider<Type>::value.getMin(), ofxSlider<Type>::value.getMax(), 0, ofxSlider<Type>::b.width, true );

    ofxSlider<Type>::bar.setFillColor(ofxSlider<Type>::thisFillColor);
    
    ofxSlider<Type>::bar.setFilled(true);
    ofxSlider<Type>::bar.rectangle(ofxSlider<Type>::b.x+1, ofxSlider<Type>::b.y+1, valAsPct, ofxSlider<Type>::b.height-2);
    
    generateText();
}
//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::generateText(){
    string valStr = ofToString(ofxSlider<Type>::value);
    ofxSlider<Type>::textMesh = ofxSlider<Type>::getTextMesh(ofxSlider<Type>::getName(), ofxSlider<Type>::b.x + ofxSlider<Type>::textPadding, ofxSlider<Type>::b.y + ofxSlider<Type>::b.height / 2 + 4);
    textInput.text = valStr;
    
    setTextInputBounds(valStr);
}
//--------------------------------------------------------------
template<>
void ofxSliderPlus<unsigned char>::generateText(){
    string valStr = ofToString((int)value);
    textMesh = getTextMesh(getName(), b.x + textPadding, b.y + b.height / 2 + 4);
    textInput.text = valStr;

    setTextInputBounds(getTextBoundingBox(valStr,0,0), b, textPadding);

}
//--------------------------------------------------------------
template<typename Type>
void ofxSliderPlus<Type>::render(){
    ofColor c = ofGetStyle().color;
    
    ofxSlider<Type>::bg.draw();
    ofxSlider<Type>::bar.draw();
    
    ofBlendMode blendMode = ofGetStyle().blendingMode;
    if(blendMode!=OF_BLENDMODE_ALPHA){
        ofEnableAlphaBlending();
    }
    ofSetColor(ofxSlider<Type>::thisTextColor);
    
    ofxSlider<Type>::bindFontTexture();
    ofxSlider<Type>::textMesh.draw();
    ofxSlider<Type>::unbindFontTexture();
    
    textInput.draw();
    if (bIsEditingValue) {
        ofPushStyle();
        ofSetColor(ofColor::yellow);
        ofSetLineWidth(1);
        ofNoFill();
        ofDrawRectangle(textInput.bounds);
        ofPopStyle();
    }
    if(ofxSlider<Type>::mouseInside){
        ofPushStyle();
        ofSetColor(thisHighlightColor);
        ofSetLineWidth(2);
        ofNoFill();
        ofDrawRectangle(ofxSlider<Type>::b);
        ofPopStyle();
    }
    ofSetColor(c);
    if(blendMode!=OF_BLENDMODE_ALPHA){
        ofEnableBlendMode(blendMode);
    }
}
//--------------------------------------------------------------
template class ofxSliderPlus<float>;
template class ofxSliderPlus<double>;
template class ofxSliderPlus<int8_t>;
template class ofxSliderPlus<uint8_t>;
template class ofxSliderPlus<int16_t>;
template class ofxSliderPlus<uint16_t>;
template class ofxSliderPlus<int32_t>;
template class ofxSliderPlus<uint32_t>;
template class ofxSliderPlus<int64_t>;
template class ofxSliderPlus<uint64_t>;
