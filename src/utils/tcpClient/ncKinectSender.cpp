#include "ncKinectSender.h"


//--------------------------------------------------------------
void ncKinectSender::setup(int _id, string _host, int _port){
	
	id = _id;
	host = _host;
	port = _port;

	bisthreadrunning = false;
	bisconnected = false;
	bsendpointcloud = false;

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
					} else {
						if (t == "B\n") {
							mutex.lock();
							bsendpointcloud = true;
							mutex.unlock();
						}
						
					}
					//SEND DATA HERE
					mutex.lock();
					if (!bsendpointcloud) {
						object.vertices.clear();
					}
					ncKinectSeDeserializer tcpobject;
					ofBuffer data = tcpobject.serialize(object);

					int buffersize = data.size();
					//cout << "wim says sending header with num bytes: " << buffersize << endl;
					client->SendBytes((char*)&buffersize, sizeof(int));
					sleep(sleeptime);
					//cout << "wim says sending data : " << buffersize << endl;
					client->SendBytes((const char *)data.getData(), buffersize);
					bsendpointcloud = false;
					mutex.unlock();
					sleep(sleeptime);
				}
			
		}
	}
	bisconnected = false;
	bisthreadrunning = false;
}


void ncKinectSender::setKinectObject(ncKinectSeDeSerObject _object)
{

	mutex.lock();
	object = _object;
	mutex.unlock();

}
