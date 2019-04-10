#include "ncKinectReceiver.h"

//--------------------------------------------------------------
void ncKinectReceiver::setup(int _port, int _id, int _x, int _y) {
	
	port = _port;
	bisthreadrunning = false;
	bisconnected = false;
	sleeptime = 5;

	gui.setup("TCP", "_settings/tcp_receiver_"+ofToString(_id)+".xml");
	gui.add(sleeptime.set("sleeptime", sleeptime, 1, 1000));
	gui.add(kinectcamxpos.set("kinect x pos", 0, -5, 5));
	gui.add(kinectcamypos.set("kinect y pos", 0, -5, 5));
	gui.add(kinectcamzpos.set("kinect z pos", 0, -5, 5));
	gui.add(kinectyrotation.set("kinect y rotation", 0, -90, 90));
	gui.add(buttonAskpointcloud.setup("ask pointcloud"));
	gui.add(bisconnected.set("connected", false));

	gui.loadFromFile("_settings/tcp_receiver_" + ofToString(_id) + ".xml");

	gui.setPosition(_x, _y);

	bAskPointCloud = true;
	buttonAskpointcloud.addListener(this, &ncKinectReceiver::askPointCloudPressed);

	kinectscene.setup();
	
	start();
}

void ncKinectReceiver::update() {

	kinectscene.cameraposition = ofVec3f(kinectcamxpos, kinectcamypos, kinectcamzpos);
	kinectscene.camerarotation = ofQuaternion(kinectyrotation, ofVec3f(0, 1, 0));
	
}

void ncKinectReceiver::draw() {
	kinectscene.customDraw();
}

void ncKinectReceiver::drawGUI() {

	gui.draw();
}

void ncKinectReceiver::start() {
	bisconnected = false;
	startThread();
}

void  ncKinectReceiver::stop() {
	if (bisthreadrunning) {
		receiver_one->Close();
		delete receiver_one;
		receiver_one = NULL;
		bisthreadrunning = false;
		waitForThread();
	}
}

//--------------------------------------------------------------
void ncKinectReceiver::threadedFunction() {
	cout << "START THREAD" << endl;
	receiver_one = new SocketServer(port, NonBlockingSocket);
	bisthreadrunning = true;
	
	while (bisthreadrunning) {
		    cout << "WAIT FOR SENDER" << endl;

			NCSocket *s = receiver_one->Accept();

			bool bWorkdone = false;
			bisconnected = true;

			    cout << "SENDER CONNECTED" << endl;
				while (!bWorkdone) {

					mutex.lock();
					if (bAskPointCloud) {
						s->SendLine("B");
					}
					else {
						s->SendLine("A");
					}
					bAskPointCloud = false;
					mutex.unlock();

					sleep(sleeptime);

					int numofbytestoreceive = s->ReceiveBytesInt();
					if (numofbytestoreceive == 0) {
						bWorkdone = true;
						bisconnected = false;
						s->Close();
					}
					sleep(sleeptime);

					ofBuffer buffer = s->ReceiveBytesofBuffer(numofbytestoreceive);
					if (buffer.size() > 0) {
						mutex.lock();
						ncKinectSeDeserializer tcpobject;
						data = tcpobject.deserialize(buffer);

						//SET MESH
						if (data.vertices.size() > 0) {
							kinectscene.pointcloud.mesh.getVertices() = data.vertices;
						}
						//SET FLOORPLANE
						kinectscene.floorplane = data.floorplane;

						//SET USERS/SKELETONS
						vector<NCJoints> joints;
						for (int i = 0; i < data.users.size(); i++) {
							NCJoints joint;
							joint.skeletonid = data.users[i].id;
							joint.setup();
							joint.positions = data.users[i].joints3dposition;
							joints.push_back(joint);
						}
						
						kinectscene.joints = joints;
						mutex.unlock();
					}
					else {
						bWorkdone = true;
						bisconnected = false;
						s->Close();
					}
					sleep(sleeptime);
				}
				s->Close();
				bisconnected = false;
	
	}
	bisconnected = false;
	bisthreadrunning = false;

}

ncKinectSeDeSerObject ncKinectReceiver::getData()
{
	ncKinectSeDeSerObject tosend;
	if (bisconnected) {
		mutex.lock();
		tosend = data;
		mutex.unlock();
	}
	return tosend;
}

void ncKinectReceiver::askPointCloudPressed()
{
	mutex.lock();
	bAskPointCloud = true;
	mutex.unlock();
}
