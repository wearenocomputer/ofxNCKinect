//
//  ofxDropDown.cpp
//  emptyExample
//
//  Created by Roy Macdonald on 14-08-15.
//
//

#include "ofxDropDown.h"

using namespace std;
//----------------------------------------------------------------------
ofxDropDown::ofxDropDown(){
    bUpdateOnReleaseOnly = false;
    bGuiActive = false;
    mouseInside = false;
}
//----------------------------------------------------------------------
ofxDropDown::~ofxDropDown(){
    value.removeListener(this,&ofxDropDown::valueChanged);
}
//----------------------------------------------------------------------
ofxDropDown::ofxDropDown(vector<string>& vals, float width, float height){
    setup(vals, width, height);
}
//----------------------------------------------------------------------
ofxDropDown::ofxDropDown(const string& dropdownName, vector<string>& vals, float width, float height){
    setup(dropdownName, vals, width, height);
}
//----------------------------------------------------------------------
ofxDropDown* ofxDropDown::setup(vector<string>& vals, float width, float height){
    ofxDropDown();
    list = vals;
 
    value = 0;
    //value.makeReferenceTo();
    b.x = 0;
    b.y = 0;
    b.width = width;
    b.height = height;
    lineHeight = height;
    setNeedsRedraw();
//    buildListTextMeshes();
    value.addListener(this,&ofxDropDown::valueChanged);
    registerMouseEvents();
    offset = ofVec2f::zero();
    return this;
}
//----------------------------------------------------------------------
ofxDropDown* ofxDropDown::setup(const string& dropDownName, vector<string>& vals, float width, float height ){
    value.set(dropDownName, 0, 0, vals.size()-1);
    setup(vals, width, height);

	return new ofxDropDown();
}
//----------------------------------------------------------------------
bool ofxDropDown::mouseMoved(ofMouseEventArgs & args){
    if( !isGuiDrawing() ){
        return false;
    }
    mouseInside = isGuiDrawing() && b.inside(ofPoint(args.x,args.y));
    return mouseInside;
}
//----------------------------------------------------------------------
bool ofxDropDown::mousePressed(ofMouseEventArgs & args){
    if( !isGuiDrawing() ){
        bGuiActive = false;
        return false;
    }
    if(bUpdateOnReleaseOnly){
        value.disableEvents();
    }
    if( b.inside(args.x, args.y) ){
        bGuiActive = true;
        setNeedsRedraw();
        generateBar(0,true);
    }else{
        bGuiActive = false;
    }
    return bGuiActive;
}
//----------------------------------------------------------------------
bool ofxDropDown::mouseDragged(ofMouseEventArgs & args){
        if( !isGuiDrawing() ){
            return  false;
        }
    if(fullRect.inside(args.x, args.y) || b.inside(args.x, args.y)){
        setNeedsRedraw();
        generateBar(args.y - offset.y);
        return true;
    }
    return  false;
}
//----------------------------------------------------------------------
bool ofxDropDown::mouseReleased(ofMouseEventArgs & args){
    if( !isGuiDrawing() ){
        return  false;
    }
    if(bUpdateOnReleaseOnly){
        value.enableEvents();
    }
    setNeedsRedraw();
    int p = generateBar(args.y - offset.y);
    if((fullRect.inside(args.x, args.y) || b.inside(args.x, args.y)) && bGuiActive ){
        if (p >= 0) {
            value = p;
        }
        
        bGuiActive = false;
        return true;
    }
    bGuiActive = false;
    return false;
}
//----------------------------------------------------------------------
bool ofxDropDown::mouseScrolled(ofMouseEventArgs & args){
    if( !isGuiDrawing() ){
        return  false;
    }
    if(mouseInside){
        if(args.y>0 || args.y<0){
           // double range = getRange(value.getMin(),value.getMax(),b.width);
            //Type newValue = value + ofMap(args.y,-1,1,-range, range);
            //newValue = ofClamp(newValue,value.getMin(),value.getMax());
            //value = newValue;
        }
        return true;
    }else{
        return false;
    }
}
//----------------------------------------------------------------------
unsigned int ofxDropDown::operator=(unsigned int v){
    value = v;
    return v;
}
//----------------------------------------------------------------------
ofxDropDown::operator const unsigned int & (){
    return value;
}
//----------------------------------------------------------------------
void ofxDropDown::generateDraw(){
    
    fullRect = b;

    listBg.clear();
     if (bGuiActive) {
         
        fullRect.x = b.getMaxX();
        fullRect.height = lineHeight*list.size();
         if (fullRect.getMaxY() > ofGetHeight() ){
             offset.y = fullRect.getMaxY() - ofGetHeight();
             fullRect.y -= offset.y;
         }
         if(fullRect.getMaxX() > ofGetWidth()) {
             offset.x = fullRect.getMaxX() - ofGetWidth();
             fullRect.x -= offset.x;
         }
         
         
         
         listBg.setFillColor(thisBackgroundColor);
        listBg.setFilled(true);
        listBg.rectangle(fullRect);
    }
    bg.clear();
    bg.setFillColor(thisBackgroundColor);
    bg.setFilled(true);
    bg.rectangle(b);
    generateText();
}
//----------------------------------------------------------------------
int ofxDropDown::generateBar(float my, bool bUseIndex){
    bar.clear();
    bar.setFillColor(thisFillColor);
    bar.setFilled(true);
    int v;
    if (bUseIndex) {
        v = value;
    }else{
        v =floor((my - fullRect.y)/lineHeight);
    }
    bar.rectangle(b.getMaxX()+1, b.y + lineHeight*v, b.width-2, lineHeight);
    return v;
}
//----------------------------------------------------------------------
void ofxDropDown::generateText(){
    textMesh = getTextMesh(getName(), b.x + textPadding, b.y + b.height / 2 + 4);
    if (bGuiActive) {
        buildListTextMeshes();
        textMesh.append(fullMesh);
    }else{
        textMesh.append(getTextMesh(list[value], b.x + b.width - textPadding - getTextBoundingBox(list[value],0,0).width, b.y +  b.height / 2 + 4));
    }
}
//----------------------------------------------------------------------
void ofxDropDown::buildListTextMeshes(){
    fullMesh.clear();
    for (int i = 0; i < list.size(); i++) {
        fullMesh.append(getTextMesh(list[i], b.x + (b.width * 2) - textPadding - getTextBoundingBox(list[i],0,0).width - offset.x,  lineHeight*i + b.y - offset.y + b.height / 2 + 4));
    }
}
//----------------------------------------------------------------------
void ofxDropDown::render(){
    ofColor c = ofGetStyle().color;
    
    bg.draw();
    if (bGuiActive) {
        listBg.draw();
        bar.draw();
    }
    
    ofBlendMode blendMode = ofGetStyle().blendingMode;
    if(blendMode!=OF_BLENDMODE_ALPHA){
        ofEnableAlphaBlending();
    }
    ofSetColor(thisTextColor);
    
    bindFontTexture();
    textMesh.draw();
    unbindFontTexture();
    
    ofSetColor(c);
    if(blendMode!=OF_BLENDMODE_ALPHA){
        ofEnableBlendMode(blendMode);
    }
}
//----------------------------------------------------------------------
bool ofxDropDown::setValue(float mx, float my, bool bCheck){
    if( !isGuiDrawing() ){
        bGuiActive = false;
        return false;
    }
    return true;
}
//----------------------------------------------------------------------
ofAbstractParameter & ofxDropDown::getParameter(){
    return value;
}
//----------------------------------------------------------------------
void ofxDropDown::setUpdateOnReleaseOnly(bool _bUpdateOnReleaseOnly){
    bUpdateOnReleaseOnly = _bUpdateOnReleaseOnly;
}
//----------------------------------------------------------------------
void ofxDropDown::valueChanged(unsigned int& value){
    setNeedsRedraw();
}
//----------------------------------------------------------------------
vector<string>& ofxDropDown::getList(){
    return list;
}


