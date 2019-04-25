#include "ncKinectReceiver.h"

//--------------------------------------------------------------
void ncKinectReceiver::setup(int _port, string _host, int _id, int _x, int _y) {
	
	id = _id;
	port = _port;
	host = _host;
	bisthreadrunning = false;
	bisconnected = false;
	bisconnected = false;
	sleeptime = 5;

	gui.setup("TCP", "_settings/tcp_receiver_"+ofToString(_id)+".xml");
	gui.add(sleeptime.set("sleeptime", sleeptime, 1, 1000));
	gui.add(kinectcamxpos.set("kinect x pos", 0, -5, 5));
	gui.add(kinectcamypos.set("kinect y pos", 0, -5, 5));
	gui.add(kinectcamzpos.set("kinect z pos", 0, -5, 5));
	gui.add(kinectyrotation.set("kinect y rotation", 0, -180, 180));
	gui.add(buttonAskpointcloud.setup("ask pointcloud"));
	gui.add(buttonSavemesh.setup("save mesh"));
	gui.add(buttonLoadmesh.setup("load latest mesh"));
	gui.add(bdrawpointcloud.set("draw pointcloud",true));
	gui.add(bdrawjoints.set("draw joints",true));
	gui.add(bIgnoreHeight.set("ignore height", false));
	gui.add(bisconnected.set("connected", false));

	gui.loadFromFile("_settings/tcp_receiver_" + ofToString(_id) + ".xml");

	gui.setPosition(_x, _y);

	bAskPointCloud = false;
	buttonAskpointcloud.addListener(this, &ncKinectReceiver::askPointCloudPressed);
	buttonSavemesh.addListener(this, &ncKinectReceiver::saveMeshPressed);
	buttonLoadmesh.addListener(this, &ncKinectReceiver::loadMeshPressed);

	kinectscene.setup();
	
	loadMeshPressed();

	start();
}

void ncKinectReceiver::update() {

	kinectscene.cameraposition = ofVec3f(kinectcamxpos, kinectcamypos, kinectcamzpos);
	kinectscene.camerarotation = ofQuaternion(kinectyrotation, ofVec3f(0, 1, 0));

	kinectscene.bDrawJoints = bdrawjoints;
	kinectscene.bDrawPointCloud = bdrawpointcloud;
	kinectscene.bIgnoreHeight = bIgnoreHeight;
}

void ncKinectReceiver::draw() {
	kinectscene.customDraw();
}

void ncKinectReceiver::drawGUI() {

	gui.draw();
}

void ncKinectReceiver::start() {
	
	startThread();
}

void  ncKinectReceiver::stop() {
	if (bisthreadrunning) {
		
		receiver->Close();
		delete receiver;
		receiver = NULL;
		bisconnected = false;
		bisthreadrunning = false;
		waitForThread();
	}
}

//--------------------------------------------------------------
void ncKinectReceiver::threadedFunction() {

	bisthreadrunning = true;
	while (bisthreadrunning) {
		receiver = new SocketClient(host, port);
		bisconnected = receiver->connect();
		
		while (bisconnected) {
			bool bWorkdone = false;
			while (!bWorkdone) {

				mutex.lock();
				if (bAskPointCloud) {
					int t = receiver->SendLine("B");
					if (t == -1) {
						receiver->Close();
						bWorkdone = true;
						bisconnected = false;
					}
				}
				else {
					int t= receiver->SendLine("A");
					if (t == -1) {
						receiver->Close();
						bWorkdone = true;
						bisconnected = false;
					}
				}
				bAskPointCloud = false;
				mutex.unlock();

				sleep(sleeptime);
				int numofbytestoreceive = receiver->ReceiveBytesInt();
				if (numofbytestoreceive == 0) {
					bWorkdone = true;
					bisconnected = false;
					receiver->Close();
				}
				sleep(sleeptime);

				ofBuffer buffer = receiver->ReceiveBytesofBuffer(numofbytestoreceive);
				if (buffer.size() > 0) {
					mutex.lock();
					ncKinectSeDeserializer tcpobject;
					data = tcpobject.deserialize(buffer);

					//SET MESH
					if (loadedmesh.getNumVertices() > 0) {
						kinectscene.pointcloud.mesh.getVertices() = loadedmesh.getVertices();
					}
					else {
						if (data.vertices.size() > 0) {
							kinectscene.pointcloud.mesh.getVertices() = data.vertices;
						}
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

					kinectscene.skeletons = joints;

					mutex.unlock();
				}
				else {
					bWorkdone = true;
					bisconnected = false;
					receiver->Close();
				}
				sleep(sleeptime);

			}
		}
	}

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
	loadedmesh.clear();
	loadedmesh.clearVertices();
	bAskPointCloud = true;
	mutex.unlock();
}

void ncKinectReceiver::loadMeshPressed()
{
	loadedmesh.load("mesh_" + ofToString(id) + ".obj");
}

void ncKinectReceiver::saveMeshPressed() {
	kinectscene.pointcloud.mesh.save("mesh_" + ofToString(id) + ".obj");
}
