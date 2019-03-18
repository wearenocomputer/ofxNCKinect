#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "socketCS.h"

class ncKinectSender : public ofThread {

	public:
		void setup(int _id, string _host, int _port);
		void drawGUI();

		void start();
		void stop();

		void threadedFunction();

		bool bisthreadrunning;
		bool bisconnected;

		ofParameter <bool> bsendpointcloud;

		ofMutex mutex;

		void setBuffer(ofBuffer _buffer);

private:
	
	int id;
	string host;
	int port;
	SocketClient *client;
	
	ofBuffer buffer;
	
	ofxPanel gui;
	ofParameter <int> sleeptime;

};
