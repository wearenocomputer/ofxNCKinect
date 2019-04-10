#pragma once

#include "ofMain.h"
#include "ncKinectSeDeserializer.h"
#include "NCKinectV2Objects.h"
#include "ofxGui.h"
#include "socketCS.h"

class ncKinectReceiver : public ofThread {

	public:
		void setup(int _port, int _id, int _x, int _y);
		void update();
		void draw();

		void drawGUI();

		void start();
		void stop();

		void threadedFunction();

		bool bisthreadrunning;

		ofParameter <bool> bisconnected;
		
		ncKinectSeDeSerObject getData();
		ofMutex mutex;

		void askPointCloudPressed();

		NCKinectScene kinectscene;

private:
	SocketServer *receiver_one;
	
	ncKinectSeDeSerObject data;

	ofxPanel gui;
	ofParameter <int> sleeptime;
	ofParameter <float> kinectcamxpos;
	ofParameter <float> kinectcamypos;
	ofParameter <float> kinectcamzpos;
	ofParameter <float> kinectyrotation;
	ofxButton buttonAskpointcloud;

	int port;
	

	bool bAskPointCloud;


};
