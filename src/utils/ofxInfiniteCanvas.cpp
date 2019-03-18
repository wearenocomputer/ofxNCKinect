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
    cam.enableOrtho();
    bUseOfCam = true;
    
}
//----------------------------------------
void ofxInfiniteCanvas::toggleOfCam(){
    bUseOfCam ^= true;
}
//----------------------------------------
void ofxInfiniteCanvas::save(string path){
    ofXml xml;
    xml.save(path);
}
//----------------------------------------
bool ofxInfiniteCanvas::load(string path){
    ofFile f(path);
    if (f.exists()) {
        reset();
        ofXml xml;
        xml.load(path);
        setLookAt(getLookAt());
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
        translation = viewport.getCenter();
        //        translation = {viewport.width/2, viewport.height/2, 0.};
    }else{
        translation = {(ofGetWidth()/2.), (ofGetHeight()/2.), 0};
    }
    offset = {0,0,0};
    scale =1;
    move = {0,0,0};
    bDoScale = false;
    bApplyInertia = false;
    bDoTranslate = false;
}
//----------------------------------------
void ofxInfiniteCanvas::begin(ofRectangle _viewport){
    glm::vec3 t = glm::vec3(glm::vec4(translation.get() + offset,1.0) * orientationMatrix);
    viewport = _viewport;
    if(!bUseOfCam){
        ofPushView();
        ofViewport(viewport);
        ofSetupScreenOrtho(viewport.width, viewport.height, nearClip, farClip);
        ofPushMatrix();
        ofRotateXDeg(orientation.x);
        ofRotateYDeg(orientation.y);
        
        ofTranslate(t);
        ofScale(scale,scale * (bFlipY?-1:1),scale);
    }else{
//        cam.setOrientation(orientation);
        cam.setPosition(t.x * -1.0f, t.y * (bFlipY?-1:1), t.z * -1);
        cam.setScale(scale,scale * (bFlipY?-1:1),scale);
        cam.begin();
    }
}
//----------------------------------------
ofxInfiniteCanvas::LookAt ofxInfiniteCanvas::getLookAt(){
    return (LookAt)lookAt.get();
}
//----------------------------------------
void ofxInfiniteCanvas::end(){
    if(bUseOfCam){
        cam.end();
    }else{
        ofPopMatrix();
        ofPopView();
    }
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
            orientation={0,0,0};//.set(0);
            break;
        case OFX2DCAM_BACK:
            orientationMatrix = BM;
            orientation = {0,180,0};//.set(0,180,0);
            break;
        case OFX2DCAM_LEFT:
            orientationMatrix = LM;
            orientation = {0, 90,0};//.set(0, 90,0);
            break;
        case OFX2DCAM_RIGHT:
            orientationMatrix = RM;
            orientation = {0, -90, 0};//.set(0, -90, 0);
            break;
        case OFX2DCAM_TOP:
            orientationMatrix = TM;
            orientation = {-90, 0,0};//.set(-90, 0,0);
            break;
        case OFX2DCAM_BOTTOM:
            orientationMatrix = BoM;
            orientation = {90, 0, 0};//.set(90, 0, 0);
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
void ofxInfiniteCanvas::setTranslation(glm::vec3 t){
    translation = t;
}
//----------------------------------------
void ofxInfiniteCanvas::setScale(float s){
    scale = s;
}
//----------------------------------------
void ofxInfiniteCanvas::setOffset(const glm::vec3& o){
    offset = o;
}
//----------------------------------------
glm::vec3 ofxInfiniteCanvas::getOffset(){
    return offset;
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
void ofxInfiniteCanvas::mousePressed(ofMouseEventArgs & mouse){
    
    if(viewport.inside(mouse.x, mouse.y)){
        if (bMouseInputEnabled) {
            prevMouse = mouse;
            bDoTranslate =(mouse.button == OF_MOUSE_BUTTON_LEFT);
            bDoScale =(mouse.button == OF_MOUSE_BUTTON_RIGHT);
            bApplyInertia = false;
            clicPoint = mouse - translation.get() - viewport.getPosition();
            clicPoint /= scale;
            
            //clicPoint = screenToWorld(mouse);
            clicTranslation = translation.get();
            clicScale = scale;
        }
        if (bMouseOverride) {
            // mouse.set(screenToWorld((glm::vec3)mouse));//convertir a glm
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
            //mouse.set(screenToWorld((glm::vec3)mouse));//convertir a glm
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
            //        mouse.set(screenToWorld((glm::vec3)mouse));//convertir a glm
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
            clicPoint = glm::vec2(ofGetMouseX(), ofGetMouseY()) - translation.get()- viewport.getPosition();
            clicPoint /= scale;
            
            clicScale = scale;
            clicTranslation = translation.get();
        }
        if (bMouseOverride) {
            //        mouse.set(screenToWorld((glm::vec3)mouse));//convertir a glm
            //        lastMouseScrolled = mouse;
            //        bNotifyMouseScrolled = true;
        }
    }
    //
    //    return bMouseOverride;
}
//----------------------------------------
void ofxInfiniteCanvas::updateMouse(){
    move = {0,0,0};
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
            translation += glm::vec3(move.x , move.y, 0);
        }else if(bDoScale){
            scale += move.z + move.z*scale;
            translation = clicTranslation - clicPoint*(scale - clicScale);
        }
        if(!bApplyInertia){
            move = {0,0,0};
        }
        //        if (bMouseOverride) {
        //            if (bNotifyMousePressed) {
        //                lastMousePressed.set(screenToWorld((glm::vec3)lastMousePressed));
        //                ofNotifyEvent(ofEvents().mousePressed, lastMousePressed);
        //                bNotifyMousePressed = false;
        //            }
        //            if (bNotifyMouseReleased) {
        //                lastMouseReleased.set(screenToWorld((glm::vec3)lastMouseReleased));
        //                ofNotifyEvent(ofEvents().mouseReleased, lastMouseReleased);
        //                bNotifyMouseReleased = false;
        //            }
        //            if (bNotifyMouseDragged) {
        //                lastMouseDragged.set(screenToWorld((glm::vec3)lastMouseDragged));
        //                ofNotifyEvent(ofEvents().mouseDragged, lastMouseDragged);
        //                bNotifyMouseDragged = false;
        //            }
        //            if (bNotifyMouseScrolled) {
        //                lastMouseScrolled.set(screenToWorld((glm::vec3)lastMouseScrolled));
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
glm::vec3 ofxInfiniteCanvas::screenToWorld(glm::vec3 screen){
    glm::vec3 s = screen - translation.get() - offset - viewport.getPosition();
    s = glm::vec3(glm::vec4(s,1.) * orientationMatrix);
    s /= scale;
    if(bFlipY)s.y*=-1;
    return s;
}

glm::vec3 ofxInfiniteCanvas::worldToScreen(glm::vec3 world){
    glm::vec3 s = world * scale.get();
    s = glm::vec3(glm::vec4(s,1.) *  glm::inverse(orientationMatrix));
    //	s = s * orientationMatrix.getInverse();
    s = s + translation.get() + offset + viewport.getPosition();
    return s;
}
