#pragma once
#include "ofMain.h"


class NCAbstractEventBaseObject {

public:
	

	NCAbstractEventBaseObject() {

	};

	~NCAbstractEventBaseObject() {

	}

	virtual void iamHereForTheShowSoThatThisWillCompile() {

	}
};

class NCGenericEventArg : public ofEventArgs {

public:
	NCAbstractEventBaseObject *pointertosender;
	string message;
	float value;
	int userid;
};

class ncKinectEventDispatcher {

public:

	static ncKinectEventDispatcher NCEVENTDISPATCHER;

	~ncKinectEventDispatcher();
	ncKinectEventDispatcher();

	ofEvent <NCGenericEventArg> DISCRETE_GESTURE;
	void dispatchDiscreteGesture(NCGenericEventArg &arg);

	ofEvent <NCGenericEventArg> CONTINUOUS_GESTURE;
	void dispatchContinuousGesture(NCGenericEventArg &arg);

	ofEvent <NCGenericEventArg> CLOSEST_USER_CHANGED;
	void dispatchClosetUserChanged(NCGenericEventArg &arg);

	ofEvent <NCGenericEventArg> NEW_USER;
	void dispatchNewUser(NCGenericEventArg &arg);

	ofEvent <NCGenericEventArg> LOST_USER;
	void dispatchLostUser(NCGenericEventArg &arg);
};