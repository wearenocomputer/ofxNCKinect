#pragma once

#include "ofMain.h"
#include "ofxInfiniteCanvas.h"
#include "ncKinectv2Core.h"
#include "ncKinectCamera.h"
#include "ncKinectUserManager.h"
#include "ofxGui.h"

class ncKinect3dScene :public ofNode {

private:
	
	ofxPanel gui;
	ofParameter <bool> bDrawGrid;
	ofParameter <bool> bDrawFullPointCloud;
	ofParameter <bool> bDrawUsersPointCloud;
	ofParameter <bool> bDrawSkeletons;

	ofParameter <bool> bFreeCam;
	ofParameter <bool> bFront;
	ofParameter <bool> bBack;
	ofParameter <bool> bLeft;
	ofParameter <bool> bRight;
	ofParameter <bool> bBottom;
	ofParameter <bool> bTop;

	void freecamChanged(bool &val);
	void topcamChanged(bool &val);
	void bottomcamChanged(bool &val);
	void leftcamChanged(bool &val);
	void rightcamChanged(bool &val);
	void frontcamChanged(bool &val);
	void backcamChanged(bool &val);

	void setupGUI();

	ofEasyCam cam;
	ofxInfiniteCanvas cam2d;
	void drawGridOneColor(float stepSize, size_t numberOfSteps, bool labels, bool x = true, bool y = true, bool z = true);

	ncKinectv2Core *kinectcore;
	ncKinectUserManager *kinectusermanager;
	nCKinectCamera *kinectcamera;

	
public:

	~ncKinect3dScene();
	ncKinect3dScene();

	void setup(ncKinectv2Core &_core, ncKinectUserManager &_usermanager, nCKinectCamera &_camera);
	void draw();
	void drawGUI();

	void drawUserPointcloud();
	void drawUserSkeletons();
};