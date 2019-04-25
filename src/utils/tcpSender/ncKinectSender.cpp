#include "ncKinectSender.h"


//--------------------------------------------------------------
void ncKinectSender::setup(int _id, int _port){
	
	id = _id;
	
	port = _port;

	bisthreadrunning = false;
	bisconnected = false;
	bsendpointcloud = false;
	sleeptime = 5;

	gui.setup("TCP", "_settings/tcp_sender.xml");
	gui.add(sleeptime.set("sleeptime", sleeptime, 1, 1000));
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
		sender->Close();
		
		bisconnected = false;
		bisthreadrunning = false;
		//waitForThread();
	}
}


//--------------------------------------------------------------
void ncKinectSender::threadedFunction() {

	bisthreadrunning = true;
	sender = new SocketServer(port, NonBlockingSocket);

	while (bisthreadrunning) {
		cout << "WAIT FOR THE RECEIVER TO CONNECT" << endl;
		NCSocket *s = sender->Accept();
		cout << "OK, CONNECTED" << endl;
		bool bWorkdone = false;
		bisconnected = true;

		while (!bWorkdone) {
		
			sleep(sleeptime);
			string t = s->ReceiveLine();
			if (t.size() == 0) {
				s->Close();
				bWorkdone = true;
				bisconnected = false;
			}
			else {
				if (t == "B\n") {
					mutex.lock();
					bsendpointcloud = true;
					mutex.unlock();
				}
				mutex.lock();
				if (!bsendpointcloud) {
					object.vertices.clear();
				}
				ncKinectSeDeserializer tcpobject;
				ofBuffer data = tcpobject.serialize(object);
				int buffersize = data.size();

				int tt = s->SendBytes((char*)&buffersize, sizeof(int));
				if (tt == -1) {
					s->Close();
					bWorkdone = true;
					bisconnected = false;
				}

				sleep(sleeptime);
				int ttt = s->SendBytes((const char *)data.getData(), buffersize);
				if (ttt == -1) {
					s->Close();
					bWorkdone = true;
					bisconnected = false;
				}
				bsendpointcloud = false;
				mutex.unlock();
				sleep(sleeptime);	
			}
		}
	}
	
}


void ncKinectSender::setKinectObject(ncKinectSeDeSerObject _object)
{

	mutex.lock();
	object = _object;
	mutex.unlock();

}
