#include "ncKinectEventDispatcher.h"

ncKinectEventDispatcher ncKinectEventDispatcher::NCEVENTDISPATCHER;

ncKinectEventDispatcher::~ncKinectEventDispatcher() {

}

ncKinectEventDispatcher::ncKinectEventDispatcher() {
	ncKinectEventDispatcher::NCEVENTDISPATCHER = *this;
}

void ncKinectEventDispatcher::dispatchDiscreteGesture(NCGenericEventArg &arg) {
	ofNotifyEvent(DISCRETE_GESTURE, arg, this);
}

void ncKinectEventDispatcher::dispatchContinuousGesture(NCGenericEventArg &arg) {
	ofNotifyEvent(CONTINUOUS_GESTURE, arg, this);
}

void ncKinectEventDispatcher::dispatchClosetUserChanged(NCGenericEventArg & arg) {
	ofNotifyEvent(CLOSEST_USER_CHANGED, arg, this);
}

void ncKinectEventDispatcher::dispatchNewUser(NCGenericEventArg & arg) {
	ofNotifyEvent(NEW_USER, arg, this);
}

void ncKinectEventDispatcher::dispatchLostUser(NCGenericEventArg & arg) {
	ofNotifyEvent(LOST_USER, arg, this);
}
