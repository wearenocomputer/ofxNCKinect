
#pragma once

#include "ofMain.h"
#include "ncKinectUser.h"
#include "ncKinectCamera.h"
#include "ncKinectEventDispatcher.h"

class ncKinectUserManager {

private:

	vector<ncKinectUser*>  usersforoutput;
	vector<ncKinectUser> * originalusers;

	nCKinectCamera *kinectcamera;

	void doUserManagement(vector<bool> compare);
	
	vector <bool> trackedusers;

	int closestuserid;

public:
	~ncKinectUserManager();
	ncKinectUserManager();

	void setup(vector<ncKinectUser> &_users, nCKinectCamera &_camera);
	void setup(vector<ncKinectUser> &_users);

	vector <ncKinectUser*> & getUsers();
	vector <ncKinectUser> getUsersRemapped3dposition();
	ncKinectUser  getRemapped3dPositionForUser(int _id);
	
	ncKinectUser * getClosestUser();
	int getClosestUserId();
	ncKinectUser * getUserById(int _id);

	void update();
};