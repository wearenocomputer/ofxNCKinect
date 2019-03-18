#pragma once

#include "ofMain.h"
#include "ncKinectSeDeserializer.h"
#include "NCKinectV2Objects.h"
#include "ofxGui.h"
#include "socketCS.h"

class ncKinectReceiver : public ofThread {

	public:
		void setup(int _port, int _id);
		void update();
		void draw();

		void drawGUI();


		void start();
		void stop();

		void threadedFunction();

		bool bisthreadrunning;
		bool bisconnected;
		
		ncKinectSeDeSerObject getData();
		ofMutex mutex;

private:
	SocketServer *receiver_one;
	
	ncKinectSeDeSerObject data;

	ofxPanel gui;
	ofParameter <int> sleeptime;
	ofParameter <float> kinectcamxpos;
	ofParameter <float> kinectcamypos;
	ofParameter <float> kinectcamzpos;
	ofParameter <float> kinectyrotation;

	int port;
	NCKinectScene kinectscene;


};
