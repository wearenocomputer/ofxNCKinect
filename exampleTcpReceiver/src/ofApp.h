#pragma once

#include "ofMain.h"
#include "ncKinectReceiver.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);


		ncKinectReceiver receiver_one;
		ncKinectReceiver receiver_two;

		ofEasyCam cam;

};
