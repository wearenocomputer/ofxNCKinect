#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	ofBackground(0);
	kinectcore.setup(true);
	kinectcamera.setup();
	kinectusermanager.setup(kinectcore.getUsers(), kinectcamera);
	kinecttriangulator.setup(kinectusermanager.getUsers(), kinectcore, kinectcamera, kinectcore.getBodyIndexPixels(), kinectcore.getPointCloud3D(), 10);
}

//--------------------------------------------------------------
void ofApp::update(){
	kinectcore.update(true,false,false);
	kinectcamera.update(kinectcore.getFloorPlane());
	kinectusermanager.update();
	kinecttriangulator.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	kinecttriangulator.draw();
	kinecttriangulator.drawGUI();
	kinectcamera.drawGUI();	
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 10);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'f') {
		ofToggleFullscreen();
	}
}
