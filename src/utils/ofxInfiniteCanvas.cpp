//
//  ofxInfiniteCanvas.cpp
//  ofxInfiniteCanvas
//
//  Created by Roy Macdonald on 27-06-15.
//
//

#include "ofxInfiniteCanvas.h"


ofMatrix4x4 ofxInfiniteCanvas::FM = ofMatrix4x4( 1, 0, 0, 0,
                                       0, 1, 0, 0,
                                       0, 0, 1, 0,
                                       0, 0, 0, 1 );
ofMatrix4x4 ofxInfiniteCanvas::BM = ofMatrix4x4(-1, 0, 0, 0,
                                       0, 1, 0, 0,
                                       0, 0, 1, 0,
                                       0, 0, 0, 1 );
ofMatrix4x4 ofxInfiniteCanvas::LM = ofMatrix4x4( 0, 0, 1, 0,
                                       0, 1, 0, 0,
                                       1, 0, 0, 0,
                                       0, 0, 0, 1 );
ofMatrix4x4 ofxInfiniteCanvas::RM = ofMatrix4x4( 0, 0,-1, 0,
                                       0, 1, 0, 0,
                                       1, 0, 0, 0,
                                       0, 0, 0, 1 );
ofMatrix4x4 ofxInfiniteCanvas::TM = ofMatrix4x4( 1, 0, 0, 0,
                                       0, 0, 1, 0,
                                       0, 1, 0, 0,
                                       0, 0, 0, 1 );
ofMatrix4x4 ofxInfiniteCanvas::BoM = ofMatrix4x4( 1, 0, 0, 0,
                                        0, 0,-1, 0,
                                        0, 1, 0, 0,
                                        0, 0, 0, 1 );

static const float minDifference = 0.1e-5f;

static const unsigned long doubleclickTime = 300;

//----------------------------------------
ofxInfiniteCanvas::ofxInfiniteCanvas(){
    setLookAt(OFX2DCAM_FRONT);
    lastTap	= 0;
    bMouseOverride = false;
    bApplyInertia =false;
    bDoTranslate = false;
//    bNotifyMouseDragged = false;
//    bNotifyMousePressed = false;
//    bNotifyMouseReleased = false;
//    bNotifyMouseScrolled = false;
    bMouseListenersEnabled = false;
    bDistanceSet = false;
    bDoScale = false;
    
    reset();
    parameters.setName("ofxInfiniteCanvas");
    parameters.add(bEnableMouse.set("Enable Mouse Input", false));
    parameters.add(dragSensitivity.set("Drag Sensitivity", 1, 0, 3));
    parameters.add(scrollSensitivity.set("Scroll Sensitivity", 10, 0, 30));
    parameters.add(drag.set("Drag", 0.9, 0, 1));
    parameters.add(farClip.set("Far Clip", 2000, 5000, 10000));
    parameters.add(nearClip.set("Near Clip", -1000, -5000, 10000));
    parameters.add(bFlipY.set("Flip Y axis", false));
    
    bEnableMouse.addListener(this, &ofxInfiniteCanvas::enableMouseInputCB);
    enableMouseInput();
	
	protectedParameters.setName("ofxInfiniteCanvasParams");
	scale.setName("Scale");
	protectedParameters.add(scale);
	translation.setName("translation");
	protectedParameters.add(translation);
	lookAt.setName("Look At");
	protectedParameters.add(lookAt);
	protectedParameters.add(parameters);
}
//----------------------------------------
void ofxInfiniteCanvas::save(string path){
	ofXml xml;
	xml.serialize(protectedParameters);
	//cout << "params at save: " << ofToString(protectedParameters) << endl;
	xml.save(path);
}
//----------------------------------------
bool ofxInfiniteCanvas::load(string path){
	ofFile f(path);
	if (f.exists()) {
		reset();
		ofXml xml;
		xml.load(path);
		xml.deserialize(protectedParameters);
		setLookAt(getLookAt());
	//cout << "params loaded: " << ofToString(protectedParameters) << endl;
		return true;
	}
	return false;
}
//----------------------------------------
ofxInfiniteCanvas::~ofxInfiniteCanvas(){
    disableMouseInput();
}
//----------------------------------------
void ofxInfiniteCanvas::setOverrideMouse(bool b){
    if(bMouseOverride != b){
        enableMouseListeners(b);
        bMouseOverride = b;
    }
}
//----------------------------------------
void ofxInfiniteCanvas::reset(){
    if (!viewport.isEmpty()) {
        translation = ofVec3f(viewport.width/2, viewport.height/2);
    }else{
        translation = ofVec3f(ofGetWidth()/2, ofGetHeight()/2);
    }
    scale =1;
    move = ofVec3f::zero();
    bDoScale = false;
    bApplyInertia = false;
    bDoTranslate = false;
}
//----------------------------------------
void ofxInfiniteCanvas::begin(ofRectangle _viewport){
    viewport = _viewport;
    ofPushView();
    ofViewport(viewport);
    ofSetupScreenOrtho(viewport.width, viewport.height, nearClip, farClip);
    ofPushMatrix();
    ofRotateX(orientation.x);
    ofRotateY(orientation.y);
    
    ofTranslate(translation*orientationMatrix);
    ofScale(scale,scale * (bFlipY?-1:1),scale);
    
}
//----------------------------------------
ofxInfiniteCanvas::LookAt ofxInfiniteCanvas::getLookAt(){
    return (LookAt)lookAt.get();
}
//----------------------------------------
void ofxInfiniteCanvas::end(){
    ofPopMatrix();
    ofPopView();
}
//----------------------------------------
void ofxInfiniteCanvas::setFarClip(float fc){
    farClip = fc;
}
//----------------------------------------
void ofxInfiniteCanvas::setNearClip(float nc){
    nearClip = nc;
}
//----------------------------------------
void ofxInfiniteCanvas::setDragSensitivity(float s){
    dragSensitivity = s;}
//----------------------------------------
void ofxInfiniteCanvas::setScrollSensitivity(float s){
    scrollSensitivity = s;
}
//----------------------------------------
void ofxInfiniteCanvas::setLookAt(LookAt l){
    bool bUpdateMatrix = false;
    lookAt = l;
    switch (l) {
        case OFX2DCAM_FRONT:
            orientationMatrix = FM;
            orientation.set(0);
            break;
        case OFX2DCAM_BACK:
            orientationMatrix = BM;
            orientation.set(0,180,0);
            break;
        case OFX2DCAM_LEFT:
            orientationMatrix = LM;
            orientation.set(0, 90,0);
            break;
        case OFX2DCAM_RIGHT:
            orientationMatrix = RM;
            orientation.set(0, -90, 0);
            break;
        case OFX2DCAM_TOP:
            orientationMatrix = TM;
            orientation.set(-90, 0,0);
            break;
        case OFX2DCAM_BOTTOM:
            orientationMatrix = BoM;
            orientation.set(90, 0, 0);
            break;
        default:
            break;
    }
}
//----------------------------------------
void ofxInfiniteCanvas::setFlipY(bool bFlipped){
    bFlipY.set(bFlipped);
}
//----------------------------------------
void ofxInfiniteCanvas::setDrag(float drag){this->drag = drag;}
//----------------------------------------
float ofxInfiniteCanvas::getDrag() const{return drag;}
//----------------------------------------
void ofxInfiniteCanvas::setTranslation(ofVec3f t){
	translation = t;
}
//----------------------------------------
void ofxInfiniteCanvas::setScale(float s){
	scale = s;
}
//----------------------------------------
void ofxInfiniteCanvas::enableMouseInputCB(bool &e){
    enableMouseInput(e);
}
//----------------------------------------
void ofxInfiniteCanvas::enableMouseInput(bool e){
    if(bMouseInputEnabled != e ){
        if(e){
            ofAddListener(ofEvents().update, this, &ofxInfiniteCanvas::update, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        }else{
            ofRemoveListener(ofEvents().update, this, &ofxInfiniteCanvas::update, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        }
        if (!bMouseOverride) {
            enableMouseListeners(e);
        }
        bMouseInputEnabled = e;
        if (bEnableMouse != e) {
            bEnableMouse = e;
        }
    }
}
//----------------------------------------
void ofxInfiniteCanvas::enableMouseListeners(bool e){
    if (bMouseListenersEnabled != e) {
        if (e) {
        ofAddListener(ofEvents().mouseDragged , this, &ofxInfiniteCanvas::mouseDragged, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofAddListener(ofEvents().mousePressed, this, &ofxInfiniteCanvas::mousePressed, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofAddListener(ofEvents().mouseReleased, this, &ofxInfiniteCanvas::mouseReleased, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofAddListener(ofEvents().mouseScrolled, this, &ofxInfiniteCanvas::mouseScrolled, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
    }else{
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxInfiniteCanvas::mouseDragged, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofRemoveListener(ofEvents().mousePressed, this, &ofxInfiniteCanvas::mousePressed, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxInfiniteCanvas::mouseReleased, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
        ofRemoveListener(ofEvents().mouseScrolled, this, &ofxInfiniteCanvas::mouseScrolled, bMouseOverride?OF_EVENT_ORDER_BEFORE_APP:OF_EVENT_ORDER_AFTER_APP);
    }
        bMouseListenersEnabled = e;
    }
}
//----------------------------------------
void ofxInfiniteCanvas::disableMouseInput(){
    enableMouseInput(false);
}
//----------------------------------------
bool ofxInfiniteCanvas::getMouseInputEnabled(){
    return bMouseInputEnabled;
}
//----------------------------------------
//bool
void ofxInfiniteCanvas::mousePressed(ofMouseEventArgs & mouse){
    
    if(viewport.inside(mouse.x, mouse.y)){
        if (bMouseInputEnabled) {
            prevMouse = mouse;
            bDoTranslate =(mouse.button == OF_MOUSE_BUTTON_LEFT);
            bDoScale =(mouse.button == OF_MOUSE_BUTTON_RIGHT);
            bApplyInertia = false;
            clicPoint = mouse - translation.get() - ofVec3f(viewport.x, viewport.y);
            clicPoint /= scale;
            
            //clicPoint = screenToWorld(mouse);
            clicTranslation = translation;
            clicScale = scale;
        }
        if (bMouseOverride) {
            mouse.set(screenToWorld((ofVec3f)mouse));
            //            lastMousePressed = mouse;
            //            bNotifyMousePressed = true;
        }
        //        return bMouseOverride;
    }
    // return false;
}
//----------------------------------------
//bool
void ofxInfiniteCanvas::mouseReleased(ofMouseEventArgs & mouse){
        if(viewport.inside(mouse.x, mouse.y)){
    if (bMouseInputEnabled) {
        unsigned long curTap = ofGetElapsedTimeMillis();
        if(lastTap != 0 && curTap - lastTap < doubleclickTime){
            reset();
            return;
        }
        lastTap = curTap;
        bApplyInertia = true;
        mouseVel = mouse  - prevMouse;
        updateMouse();
        prevMouse = mouse;
    }
    if (bMouseOverride) {
        mouse.set(screenToWorld((ofVec3f)mouse));
        //        lastMouseReleased = mouse;
        //        bNotifyMouseReleased = true;
    }
        }
    //
    //    return bMouseOverride;
}
//----------------------------------------
//bool
void ofxInfiniteCanvas::mouseDragged(ofMouseEventArgs & mouse){
        if(viewport.inside(mouse.x, mouse.y)){
    if (bMouseInputEnabled) {
        mouseVel = mouse  - prevMouse;
        bApplyInertia = false;
        updateMouse();
        prevMouse = mouse;
    }
    if (bMouseOverride) {
        mouse.set(screenToWorld((ofVec3f)mouse));
        //        lastMouseDragged = mouse;
        //        bNotifyMouseDragged = true;
    }
        }
    //
    //    return bMouseOverride;
}
//----------------------------------------
//bool
void ofxInfiniteCanvas::mouseScrolled(ofMouseEventArgs & mouse){
        if(viewport.inside(mouse.x, mouse.y)){
    if (bMouseInputEnabled) {
        move.z = scrollSensitivity * mouse.scrollY / ofGetHeight();
        bDoTranslate = false;
        bDoScale = true;
        clicPoint = ofVec2f(ofGetMouseX(), ofGetMouseY()) - translation.get() - ofVec3f(viewport.x, viewport.y);
        clicPoint /= scale;
        
        clicScale = scale;
        clicTranslation = translation;
    }
    if (bMouseOverride) {
        mouse.set(screenToWorld((ofVec3f)mouse));
        //        lastMouseScrolled = mouse;
        //        bNotifyMouseScrolled = true;
    }
        }
    //
    //    return bMouseOverride;
}
//----------------------------------------
void ofxInfiniteCanvas::updateMouse(){
    move = ofVec3f::zero();
    if(bDoScale){
        move.z = dragSensitivity * mouseVel.y /ofGetHeight();
    }else if(bDoTranslate){
        move.x = mouseVel.x ;
        move.y = mouseVel.y;
    }
}
//----------------------------------------
void ofxInfiniteCanvas::update(ofEventArgs & args){
    update();
}
//----------------------------------------
void ofxInfiniteCanvas::update(){
    if(bMouseInputEnabled){
        if(bApplyInertia){
            move *= drag;
            if(ABS(move.x) <= minDifference && ABS(move.y) <= minDifference && ABS(move.z) <= minDifference){
                bApplyInertia = false;
                bDoTranslate = false;
                bDoScale = false;
            }
        }
        if(bDoTranslate){
            translation += ofVec3f(move.x , move.y, 0);
        }else if(bDoScale){
            scale += move.z + move.z*scale;
            translation = clicTranslation - clicPoint*(scale - clicScale);
        }
        if(!bApplyInertia){
            move = ofVec3f::zero();
        }
        //        if (bMouseOverride) {
        //            if (bNotifyMousePressed) {
        //                lastMousePressed.set(screenToWorld((ofVec3f)lastMousePressed));
        //                ofNotifyEvent(ofEvents().mousePressed, lastMousePressed);
        //                bNotifyMousePressed = false;
        //            }
        //            if (bNotifyMouseReleased) {
        //                lastMouseReleased.set(screenToWorld((ofVec3f)lastMouseReleased));
        //                ofNotifyEvent(ofEvents().mouseReleased, lastMouseReleased);
        //                bNotifyMouseReleased = false;
        //            }
        //            if (bNotifyMouseDragged) {
        //                lastMouseDragged.set(screenToWorld((ofVec3f)lastMouseDragged));
        //                ofNotifyEvent(ofEvents().mouseDragged, lastMouseDragged);
        //                bNotifyMouseDragged = false;
        //            }
        //            if (bNotifyMouseScrolled) {
        //                lastMouseScrolled.set(screenToWorld((ofVec3f)lastMouseScrolled));
        //                ofNotifyEvent(ofEvents().mouseScrolled, lastMouseScrolled);
        //                bNotifyMouseScrolled = false;
        //            }
        //        }
    }
}
//----------------------------------------
void ofxInfiniteCanvas::drawDebug(){
    string m = "translation: " + ofToString(translation) + "\n";
    m += "scale: " + ofToString(scale) + "\n";
    m += "clic point: " + ofToString(clicPoint) + "\n";
    ofDrawBitmapString(m, 0, 20);
}
//----------------------------------------
ofVec3f ofxInfiniteCanvas::screenToWorld(ofVec3f screen){
    ofVec3f s = screen - translation - ofVec3f(viewport.x, viewport.y);
    s = s*orientationMatrix;
    s /= scale;
	if(bFlipY)s.y*=-1;
    return s;
}

ofVec3f ofxInfiniteCanvas::worldToScreen(ofVec3f world){
	ofVec3f s = world * scale;
	s = s * orientationMatrix.getInverse();
	s = s + translation + ofVec3f(viewport.x, viewport.y);
	return s;
}
