#include "ncKinectUserManager.h"

ncKinectUserManager::~ncKinectUserManager() {

}

ncKinectUserManager::ncKinectUserManager() {

}

void ncKinectUserManager::setup(vector<ncKinectUser>& _users, nCKinectCamera &_camera) {
	originalusers = &_users;
	kinectcamera = &_camera;
	//USER MANAGEMENT
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);

	closestuserid = -1;
}

void ncKinectUserManager::setup(vector<ncKinectUser>& _users) {
	originalusers = &_users;
	//USER MANAGEMENT
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);
	trackedusers.push_back(false);

	closestuserid = -1;
}

vector<ncKinectUser*>& ncKinectUserManager::getUsers() {
	return usersforoutput;
}

vector<ncKinectUser> ncKinectUserManager::getUsersRemapped3dposition() {
	vector<ncKinectUser> myusers;
	for (size_t i = 0; i < usersforoutput.size(); i++) {

		ncKinectUser user = *usersforoutput[i];
		for (size_t k = 0; k < user.joints3dposition.size(); k++) {
			ofVec3f oldpoint = user.joints3dposition[k];
			ofVec3f newpoint = oldpoint*kinectcamera->realworldkinecttransformmatrix;
			user.joints3dposition[k] = newpoint;
		}
		myusers.push_back(user);
	}

	return myusers;
}

ncKinectUser  ncKinectUserManager::getRemapped3dPositionForUser(int _id) {

	ncKinectUser *user = getUserById(_id);

	if (user != nullptr) {

		ncKinectUser usernew = *user;

		for (size_t k = 0; k < user->joints3dposition.size(); k++) {
			ofVec3f oldpoint = user->joints3dposition[k];
			ofVec3f newpoint = oldpoint*kinectcamera->realworldkinecttransformmatrix;
			usernew.joints3dposition[k] = newpoint;
		}

		return usernew;
	}

	return ncKinectUser();
}

ncKinectUser * ncKinectUserManager::getClosestUser() {

	vector < vector <float> > users;
	for (size_t i = 0; i < usersforoutput.size(); i++) {
		vector <float> user;
		user.push_back(usersforoutput[i]->id);
		user.push_back(usersforoutput[i]->joints3dposition[ncJointType_SpineBase].z);
		users.push_back(user);
	}

	std::sort(users.begin(), users.end(),
		[](const std::vector<float>& a, const std::vector<float>& b) {
		return a[1] < b[1];
	});

	if (users.size() > 0) {
		return getUserById(users[0][0]);
	}

	return nullptr;
}

int ncKinectUserManager::getClosestUserId() {
	ncKinectUser *user  = getClosestUser();
	if (user != nullptr) {
		return user->id;
	}
	return -1;
}

ncKinectUser * ncKinectUserManager::getUserById(int _id) {
	for (size_t i = 0; i < usersforoutput.size(); i++) {
		if (usersforoutput[i]->id == _id) {
			return usersforoutput[i];
		}
	}
	return nullptr;
}

void ncKinectUserManager::update() {

	vector<bool> totrack;
	for (size_t i = 0; i < originalusers->size(); i++) {

		bool tracked = (*originalusers)[i].tracked;
		if ((bool)tracked) {

			(*originalusers)[i].recordPositions();
			(*originalusers)[i].generate3DSkeleton();

			totrack.push_back(true);
		}
		else {
			totrack.push_back(false);
		}
	}
	doUserManagement(totrack);

	if (getClosestUserId() != closestuserid) {
		closestuserid = getClosestUserId();
		NCGenericEventArg arg;
		arg.userid = closestuserid;
		ncKinectEventDispatcher::NCEVENTDISPATCHER.dispatchClosetUserChanged(arg);
	}
}


void ncKinectUserManager::doUserManagement(vector<bool> compare) {
	for (int i = 0; i < compare.size(); i++) {

		bool incomingvalue = compare[i];
		bool currentvalue = trackedusers[i];

		if (incomingvalue != currentvalue) {

			if (incomingvalue) {
				
				usersforoutput.push_back(&(*originalusers)[i]);
				NCGenericEventArg arg;
				arg.userid = i;
				ncKinectEventDispatcher::NCEVENTDISPATCHER.dispatchNewUser(arg);
			}
			else {
				//USER LOST
				for (int j = 0; j < usersforoutput.size(); j++) {
					if (usersforoutput[j]->id == i) {
						NCGenericEventArg arg;
						arg.userid = i;
						ncKinectEventDispatcher::NCEVENTDISPATCHER.dispatchLostUser(arg);
						usersforoutput.erase(usersforoutput.begin() + j);
						//do not delete this pointer because the data is tied to a memry spot that is still in use
					}
				}
			}
		}
	}
	trackedusers = compare;
}
