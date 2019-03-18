#include "ncKinectAreaManager.h"



ncKinectAreaManager::~ncKinectAreaManager() {

}

ncKinectAreaManager::ncKinectAreaManager() {

}

void ncKinectAreaManager::setup() {
	loadcounter = 0;
	setupGUI();
	setKinectFrustrum();
	setInteractionSpace();
	floorplane.set(10, 10);
	
	for (size_t i = 0; i < floorplane.getMesh().getNumVertices(); i++) {
		floorplane.getMesh().addColor(ofFloatColor(0,1,0,1));
	}
}

void ncKinectAreaManager::update() {

	if (loadcounter < 3) {
		if (ofGetFrameNum() % 15 == 0) {
			loadcounter++;
			gui.loadFromFile("_settings/area.xml");
		}
	}

}

void ncKinectAreaManager::updatePositionRotation(glm::vec3 _pos, ofQuaternion _rot) {

	kinectdepthfrustrum.setPosition(_pos);
	kinectdepthfrustrum.setOrientation(_rot);
	interactionspace.setPosition(glm::vec3(_pos.x, interactionspace.getPosition().y, _pos.z));

}

void ncKinectAreaManager::setupGUI() {
	gui.setup("Kinect Area", "_settings/area.xml");
	gui.add(kinectspacedepth.set("kinect depth", 4.5, 0.5, 8));
	gui.add(interactionspacewidth.set("interac width", 1.5, 0, 10));
	gui.add(interactionspacedepth.set("interac depth", 2, 0, 10));
	gui.add(bDrawDepthFrustrum.set("draw depth fr", true));
	gui.add(bDrawInteractionSpace.set("draw inter sp", true));
	kinectspacedepth.addListener(this, &ncKinectAreaManager::kinectSpaceDepthChanged);
	interactionspacewidth.addListener(this, &ncKinectAreaManager::interactionSpaceWidthChanged);
	interactionspacedepth.addListener(this, &ncKinectAreaManager::interactionSpaceDepthChanged);


	gui.loadFromFile("_settings/area.xml");

	gui.setPosition(10, 350);
}

void ncKinectAreaManager::setKinectFrustrum() {
	kinectdepthfrustrum.create(60, 512.0f / 424.0f, kinectspacedepth);
}

void ncKinectAreaManager::setInteractionSpace() {
	interactionspace.create(ofVec2f(interactionspacewidth*2, interactionspacedepth));
}

void ncKinectAreaManager::kinectSpaceDepthChanged(float & arg) {
	setKinectFrustrum();
}

void ncKinectAreaManager::interactionSpaceDepthChanged(float & arg) {
	setInteractionSpace();
}

void ncKinectAreaManager::interactionSpaceWidthChanged(float & arg) {
	setInteractionSpace();
}

void ncKinectAreaManager::draw() {

	if (bDrawDepthFrustrum) {
		kinectdepthfrustrum.draw();
	}

	if (bDrawInteractionSpace) {
		interactionspace.draw();
	}

	ofPushMatrix();
	ofRotate(90, 1, 0, 0);
	///floorplane.draw();
	ofRotate(90, 0, 1, 0);
	ofSetColor(ofColor::lawnGreen);
	ofDrawGridPlane(1, 5);
	ofSetColor(255);
	ofPopMatrix();
}

void ncKinectAreaManager::drawGUI() {
	gui.draw();
}

bool ncKinectAreaManager::isUserInInteractionSpace(glm::vec3 _pos) {
	return interactionspace.isPointInsideInteractionSpace(_pos);
}

bool ncKinectAreaManager::isUserInInteractionSpace(ncKinectUser _user) {
	return isUserInInteractionSpace(_user.joints3dposition[ncJointType_SpineBase]);
}

bool ncKinectAreaManager::isThereAUserInInteractiveSpace(vector<ncKinectUser> _users) {

	for (size_t i = 0; i < _users.size(); i++) {
		if (isUserInInteractionSpace(_users[i])) {
			return true;
		}
	}

	return false;
}

vector<ncKinectUser> ncKinectAreaManager::returnUsersInInteractionspace(vector<ncKinectUser> _users) {
	vector<ncKinectUser> userstoreturn;
	for (size_t i = 0; i < _users.size(); i++) {
		if (isUserInInteractionSpace(_users[i])) {
			userstoreturn.push_back(_users[i]);
		}
	}
	return userstoreturn;
}

int ncKinectAreaManager::returnClosestUserInInteractionSpace(vector<ncKinectUser> _users) {
	vector<ncKinectUser> usersfor = returnUsersInInteractionspace(_users);

	vector < vector <float> > userst;


	for (size_t i = 0; i < usersfor.size(); i++) {
		vector <float> user;
		user.push_back(usersfor[i].id);
		user.push_back(usersfor[i].joints3dposition[ncJointType_SpineBase].z);
		userst.push_back(user);
	}


	std::sort(userst.begin(), userst.end(),
		[](const std::vector<float>& a, const std::vector<float>& b) {
		return a[1] < b[1];
	});

	if (userst.size() > 0) {
		return userst[0][0];
	}

	return -1;
}
