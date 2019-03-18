#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0);
	kinect.setup("gestures.gbd", false);
	kinectusermanager.setup(kinect.getUsers());
	kinect2dscene.setup(kinectusermanager.getUsers());

	ofAddListener(ncKinectEventDispatcher::NCEVENTDISPATCHER.DISCRETE_GESTURE, this, &ofApp::discreteTracked);
	ofAddListener(ncKinectEventDispatcher::NCEVENTDISPATCHER.NEW_USER, this, &ofApp::newUser);
	ofAddListener(ncKinectEventDispatcher::NCEVENTDISPATCHER.LOST_USER, this, &ofApp::lostUser);
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
	kinectusermanager.update();
	
	if (kk.size() == 30) {
		kk.erase(kk.begin());
	}

}

//--------------------------------------------------------------
void ofApp::draw(){
	kinect2dscene.drawDepthMap2d(kinect.getDepthMap2D());

	float startx =  540;
	ofSetLineWidth(2);
	ofNoFill();
	
	ofDrawRectangle(startx, 0, 210, 210);
	ofFill();
	for (size_t i = 0; i < kk.size(); i++) {
	ofDrawLine(ofPoint((i * 7) + startx, 210), ofPoint((i * 7) + startx, 210 - (kk[i] * 210)));
	}
	ofSetColor(ofColor::red);
	ofDrawLine(ofPoint(startx, 210 - (0.4 * 210)), ofPoint(startx + 210, 210 - (0.4 * 210)));
	ofSetColor(255);
	ofSetLineWidth(1);
	ofDrawBitmapString("Crane Kick", startx, 230);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::discreteTracked(NCGenericEventArg & arg) {
	
	if (arg.message == "KarateKid") {
		kk.push_back(arg.value);
		
	}


}

//--------------------------------------------------------------
void ofApp::newUser(NCGenericEventArg & arg) {
	ncKinectUser * user = kinectusermanager.getUserById(arg.userid);
	kinect.turnOffGestureDectors();
	kinect.turnOnGestureDetectorForUser(user->id, user->kinectid);
}

//--------------------------------------------------------------
void ofApp::lostUser(NCGenericEventArg & arg) {
	kinect.turnOffGestureDectors();
}
