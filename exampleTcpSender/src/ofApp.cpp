#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);

	//LOAD TCP SETTINGS
	//OSC
	string host;
	int port;
	ofFile myfile("tcp.txt");
	ofBuffer mybuffer(myfile);

	int count = 0;
	for (auto &line : mybuffer.getLines()) {
		if (line != "") {

			if (count == 0) {
				host = line;
			}

			if (count == 1) {
				port = ofToInt(line);
			}
			count++;
		}
	}
	
	sender.setup(1,host,port);

	kinectcore.setup();
	kinectcamera.setup();
	kinectusermanager.setup(kinectcore.getUsers(), kinectcamera);
	kinectscene.setup(kinectcore, kinectusermanager, kinectcamera);


	ofSetWindowTitle("SENDER");
}

//--------------------------------------------------------------
void ofApp::update(){

	kinectcore.update(false, false, false);
	kinectcamera.update(kinectcore.getFloorPlane());
	kinectusermanager.update();

	ncKinectSeDeSerObject kinectobject;
	kinectobject.floorplane = kinectcore.getFloorPlane();

	//use this to send the pointcloud over TCP
	kinectobject.vertices.clear();
	if (sender.bsendpointcloud) {
		kinectobject.vertices = kinectcore.getPointCloud3D().getVertices();
	}
	kinectobject.users.clear();
	vector<ncKinectUser> users;
	for (size_t i = 0; i < kinectusermanager.getUsers().size(); i++) {
		users.push_back(*kinectusermanager.getUsers()[i]);
	}
	kinectobject.users = users;
	ncKinectSeDeserializer tcpobject;
	ofBuffer data = tcpobject.serialize(kinectobject);
	
	sender.setBuffer(data);
}

//--------------------------------------------------------------
void ofApp::draw(){

	kinectscene.draw();
	kinectscene.drawGUI();

	sender.drawGUI();
	

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 10);
}

void ofApp::exit() {
	sender.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'f') {
		ofToggleFullscreen();
	}
}
