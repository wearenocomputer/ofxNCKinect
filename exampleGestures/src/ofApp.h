#pragma once

#include "ofMain.h"
#include "ncKinectv2Core.h"
#include "ncKinectUserManager.h"
#include "ncKinect2dScene.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

		void discreteTracked(NCGenericEventArg &arg);
		ncKinectv2Core kinect;
		ncKinectUserManager kinectusermanager;
		ncKinect2dScene kinect2dscene;

		void newUser(NCGenericEventArg &arg);
		void lostUser(NCGenericEventArg &arg);

		vector<float> kk;
};
