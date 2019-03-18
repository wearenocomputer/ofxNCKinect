#pragma once

#include "ofMain.h"
#include "ncKinectv2Core.h"
#include "ncKinectCamera.h"
#include "ncKinectUserManager.h"
#include "ncKinect3dScene.h"
#include "ncKinectSeDeserializer.h"
#include "ncKinectSender.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		

		void keyPressed(int key);
			
		ncKinectSender sender;

		ncKinectv2Core kinectcore;
		nCKinectCamera kinectcamera;
		ncKinectUserManager kinectusermanager;
		ncKinect3dScene kinectscene;
		
		ncKinectSeDeserializer serializer;
};
