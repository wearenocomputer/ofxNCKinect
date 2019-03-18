#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	ofBackground(0);
	kinectcore.setup(true);
	kinectcamera.setup();
	kinectusermanager.setup(kinectcore.getUsers(), kinectcamera);
	kinectscene.setup(kinectcore, kinectusermanager, kinectcamera);

	ofAddListener(ncKinectEventDispatcher::NCEVENTDISPATCHER.NEW_USER, this, &ofApp::newUser);
	ofAddListener(ncKinectEventDispatcher::NCEVENTDISPATCHER.LOST_USER, this, &ofApp::lostUser);

}

//--------------------------------------------------------------
void ofApp::update(){

	kinectcore.update(true,true,true);
	kinectcamera.update(kinectcore.getFloorPlane());
	kinectusermanager.update();
	
	bodyIndexImg.setFromPixels(kinectcore.getBodyIndexPixels());

	bodyIndexImgColor.setFromPixels(kinectcore.getBodyIndexPixelsColored());

	depthmapimage.setFromPixels(kinectcore.getDepthMap2D());

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	depthmapimage.draw(0, 0);
	bodyIndexImg.draw(512, 0);
	bodyIndexImgColor.draw(512*2, 0);
	
	kinectscene.draw();
	kinectscene.drawGUI();

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'f') {
		ofToggleFullscreen();
	}
}


//--------------------------------------------------------------
void ofApp::newUser(NCGenericEventArg & arg) {
	cout << arg.userid << endl;
}

//--------------------------------------------------------------
void ofApp::lostUser(NCGenericEventArg & arg) {
	cout << arg.userid << endl;
}