#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGuiPlus\ofxSliderPlus.h"

class nCKinectCamera : public ofNode {

private:

	void draw();
	

	//GUI
	ofxPanel gui;
	ofParameter <float> kinectcamxposin3dworld;
	ofParameter <float> kinectcamyposin3dworld;
	ofParameter <float> kinectcamzposin3dworld;
	ofParameter <float> kinectyawin3dworld;
	ofParameter <bool> drawCamera;
	ofParameter <bool> mapKinectto3DWorld;
	void setupGUI();
	int loadcounter;
	int freezefloorplanecounter;
	bool floorplaneshouldbein;

public:

	~nCKinectCamera();
	nCKinectCamera();

	ofBoxPrimitive model;
	ofMatrix4x4 realworldkinecttransformmatrix;
	ofVec4f floorplane;
	ofParameter <bool> freezeFloorplane;

	void setup();

	void begin();

	void end();

	void update(ofVec4f _floorplane);

	void drawGUI();

	ofVec4f getFloorPlane();

	ofVec3f getKinectCameraPosition();
	ofQuaternion getKinectCameraRotation();

	
};