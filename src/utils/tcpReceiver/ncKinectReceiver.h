#pragma once

#include "ofMain.h"
#include "ncKinectSeDeserializer.h"
#include "NCKinectV2Objects.h"
#include "ofxGui.h"
#include "socketCS.h"

class ncKinectReceiver : public ofThread {

	public:
		void setup(int _port, string _host, int _id, int _x, int _y);
		void update();
		void draw();

		void drawGUI();

		void start();
		void stop();

		void threadedFunction();

		bool bisthreadrunning;

		ofParameter <bool> bisconnected;
		int id;

		ncKinectSeDeSerObject getData();
		ofMutex mutex;
		NCKinectScene kinectscene;

private:

	ofMesh loadedmesh;

	SocketClient *receiver;

	ncKinectSeDeSerObject data;

	ofxPanel gui;
	ofParameter <int> sleeptime;
	ofParameter <float> kinectcamxpos;
	ofParameter <float> kinectcamypos;
	ofParameter <float> kinectcamzpos;
	ofParameter <float> kinectyrotation;
	ofParameter <float> kinectxrotation;
	ofParameter <float> kinectzrotation;
	ofParameter <bool> bdrawpointcloud;
	ofParameter <bool> bdrawjoints;
	ofParameter <bool> bIgnoreHeight;


	ofxButton buttonSavemesh;
	ofxButton buttonLoadmesh;
	ofxButton buttonAskpointcloud;

	int port;
	string host;
	bool bAskPointCloud;
	void askPointCloudPressed();

	void loadMeshPressed();
	void saveMeshPressed();
};
