#pragma once

#include "ofMain.h"
#include "ncKinectv2Core.h"
#include "ncKinectCamera.h"
#include "ncKinectUserManager.h"
#include "ncKinectPCTriangulator.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		ncKinectv2Core kinectcore;
		nCKinectCamera kinectcamera;
		ncKinectUserManager kinectusermanager;
		ncKinectPCTriangulator kinecttriangulator;
};
