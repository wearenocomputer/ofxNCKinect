#include "ncKinectSender.h"


//--------------------------------------------------------------
void ncKinectSender::setup(int _id, string _host, int _port){
	
	id = _id;
	host = _host;
	port = _port;

	bisthreadrunning = false;
	bisconnected = false;

	gui.setup("TCP", "_settings/tcp_sender.xml");
	gui.add(sleeptime.set("sleeptime", sleeptime, 1, 1000));
	gui.add(bsendpointcloud.set("send pointcloud", true));
	gui.loadFromFile("_settings/tcp_sender.xml");
	
	start();
}

void ncKinectSender::drawGUI()
{
	gui.draw();
}


//--------------------------------------------------------------
void ncKinectSender::start() {
	startThread();
}

//--------------------------------------------------------------
void ncKinectSender::stop() {
	if (bisthreadrunning) {

		if (bisconnected) {
			client->Close();
		}

		bisthreadrunning = false;		
		waitForThread();
	}
}


//--------------------------------------------------------------
void ncKinectSender::threadedFunction() {
	bisthreadrunning = true;

	//CREATE THE SERVER SOCKET

	while (bisthreadrunning) {

		int timeouts = 0;
		client = new SocketClient(host, port);
		bisconnected = client->connect();

		if (bisconnected) {
			bool bWorkdone = false;
				while (!bWorkdone) {
					sleep(sleeptime);
					string t = client->ReceiveLine();
					if (t.size() == 0) {
						timeouts++;
						if (timeouts == 20) {
							bWorkdone = true;
							client->Close();
							bisconnected = false;
						}
					}
					//SEND DATA HERE
					mutex.lock();
					int buffersize = buffer.size();
					//cout << "wim says sending header with num bytes: " << buffersize << endl;
					client->SendBytes((char*)&buffersize, sizeof(int));
					sleep(sleeptime);
					//cout << "wim says sending data : " << buffersize << endl;
					client->SendBytes((const char *)buffer.getData(), buffersize);
					mutex.unlock();
					sleep(sleeptime);
				}
			
		}
	}
	bisconnected = false;
	bisthreadrunning = false;
}

void ncKinectSender::setBuffer(ofBuffer _buffer)
{
	mutex.lock();
	buffer = _buffer;
	mutex.unlock();
}
