#include "ncKinectCamera.h"

void nCKinectCamera::setupGUI() {
	gui.setup("Kinect Camera", "_settings/kinectcamera.xml");
	gui.add(new ofxFloatSliderPlus(kinectcamxposin3dworld.set("x pos in 3d world", 0, -5, 5)));
	gui.add(new ofxFloatSliderPlus(kinectcamyposin3dworld.set("y pos in 3d world", 0, -5, 5)));
	gui.add(new ofxFloatSliderPlus(kinectcamzposin3dworld.set("z pos in 3d world", 0, -5, 5)));
	gui.add(new ofxFloatSliderPlus(kinectyawin3dworld.set("yaw in 3d world", 0, -180, 180)));
	gui.add(buttonFreezeFloorPlane.setup("toggle freeze floorplane"));
	gui.add(bfloorplaneisfrozen.set("isfloorplanefrozen", false));
	gui.add(drawCamera.set("draw camera", true));
	gui.add(mapKinectto3DWorld.set("map Kinect to 3d world", true));

	gui.loadFromFile("_settings/kinectcamera.xml");

	gui.setPosition(10, 470);

	buttonFreezeFloorPlane.addListener(this, &nCKinectCamera::freezeFloorPlanePressed);
}

void nCKinectCamera::freezeFloorPlanePressed() {
	bfloorplaneisfrozen = !bfloorplaneisfrozen;

	if (bfloorplaneisfrozen) {
		floorplane = _incomingfloorplane;
	}
}

nCKinectCamera ::~nCKinectCamera(){

}

nCKinectCamera::nCKinectCamera() {
	
}

void nCKinectCamera::setup() {
	setupGUI();
	model.set(0.3, 0.1, 0.1);
}

void nCKinectCamera::update(ofVec4f _floorplane){

	_incomingfloorplane = _floorplane;

	if (!bfloorplaneisfrozen) {
		floorplane = _floorplane;
	}

	//http://blog.hackandi.com/inst/blog/2014/03/18/convert-kinect-cameraspace-to-worldspace-relative-to-floor/
	ofVec3f up = ofVec3f(floorplane.x, floorplane.y, floorplane.z);
	ofVec3f forward = up.getCrossed(ofVec3f(1, 0, 0));
	forward.normalize();
	ofVec3f right = up.getCrossed(forward);
	//ofVec3f origin = floorplane.w * up;

	ofMatrix4x4 mymat = ofMatrix4x4(
		right.x, up.x, forward.x, 0,
		right.y, up.y, forward.y, 0,
		right.z, up.z, forward.z, 0,
		0, floorplane.w, 0, 1);


	ofMatrix4x4 currenttranslation;
	currenttranslation.translate(ofVec3f(kinectcamxposin3dworld, kinectcamyposin3dworld, kinectcamzposin3dworld));
	ofMatrix4x4 currentrotation;
	currentrotation.rotate(ofQuaternion(kinectyawin3dworld, ofVec3f(0, 1, 0)));
	ofMatrix4x4 currentscale;
	currentscale.makeScaleMatrix(-1, 1, 1);

	if (mapKinectto3DWorld) {
		realworldkinecttransformmatrix = mymat*currentrotation*currenttranslation*currentscale;
	}
	else {
		realworldkinecttransformmatrix = currentrotation*currenttranslation*currentscale;
	}

}

void nCKinectCamera::drawGUI() {
	//gui.setPosition(ofGetWidth() - gui.getWidth() - 30, 10);
	gui.draw();

	float deg = ofWrapDegrees(getKinectCameraRotation().getEuler().x, -90, 90);

	ofDrawBitmapStringHighlight("height in real world: "+ofToString(getKinectCameraPosition().y), gui.getPosition().x+5, gui.getHeight()+ gui.getPosition().y+20);
	ofDrawBitmapStringHighlight("pitch in real world: " + ofToString(deg), gui.getPosition().x+5, gui.getHeight() + gui.getPosition().y + 40);
}

ofVec4f nCKinectCamera::getFloorPlane() {
	return floorplane;
}

ofVec3f nCKinectCamera::getKinectCameraPosition() {
	ofVec3f trans;
	ofQuaternion rot;
	ofVec3f scale;
	ofQuaternion scaleor;
	realworldkinecttransformmatrix.decompose(trans, rot, scale, scaleor);

	return trans;

}

ofQuaternion nCKinectCamera::getKinectCameraRotation() {
	ofVec3f trans;
	ofQuaternion rot;
	ofVec3f scale;
	ofQuaternion scaleor;
	realworldkinecttransformmatrix.decompose(trans, rot, scale, scaleor);

	return rot;
}

void nCKinectCamera::begin() {
	transformGL();
	setTransformMatrix(realworldkinecttransformmatrix);
	if (drawCamera) {
		draw();
	}
}

void nCKinectCamera::end() {
	restoreTransformGL();
}

void nCKinectCamera::draw() {
	ofDrawAxis(0.5);
	model.drawWireframe();
}