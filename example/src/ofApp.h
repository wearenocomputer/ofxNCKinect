#pragma once

#include "ofMain.h"
#include "ncKinectv2Core.h"
#include "ncKinectCamera.h"
#include "ncKinectUserManager.h"
#include "ncKinect3dScene.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		ncKinectv2Core kinectcore;
		nCKinectCamera kinectcamera;
		ncKinectUserManager kinectusermanager;
		ncKinect3dScene kinectscene;
		
		ofImage bodyIndexImg;
		ofImage bodyIndexImgColor;

		ofImage depthmapimage;

		ofImage colorimage;

		void newUser(NCGenericEventArg &arg);
		void lostUser(NCGenericEventArg &arg);

};
