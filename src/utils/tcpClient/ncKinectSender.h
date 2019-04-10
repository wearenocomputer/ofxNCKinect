#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "socketCS.h"
#include "ncKinectSeDeserializer.h"

class ncKinectSender : public ofThread {

	public:
		void setup(int _id, string _host, int _port);
		void drawGUI();

		void start();
		void stop();

		void threadedFunction();

		bool bisthreadrunning;
		bool bisconnected;

		bool bsendpointcloud;

		ofMutex mutex;
		void setKinectObject(ncKinectSeDeSerObject _object);

private:
	
	int id;
	string host;
	int port;
	SocketClient *client;

	ncKinectSeDeSerObject object;
	
	ofxPanel gui;
	ofParameter <int> sleeptime;

};
