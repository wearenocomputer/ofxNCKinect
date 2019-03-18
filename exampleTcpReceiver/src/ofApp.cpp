#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	receiver_one.setup(11999,1);
	receiver_two.setup(11998,2);
	ofSetWindowTitle("RECEIVER");

	cam.setDistance(1);
	cam.setNearClip(0.01);
}

//--------------------------------------------------------------
void ofApp::update(){
	receiver_one.update();
	receiver_two.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	receiver_one.draw();
	receiver_two.draw();
	cam.end();


	receiver_one.drawGUI();
	receiver_two.drawGUI();

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'f') {
		ofToggleFullscreen();
	}
}
