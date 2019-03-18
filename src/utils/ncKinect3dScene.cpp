#include "ncKinect3dScene.h"

ncKinect3dScene ::~ncKinect3dScene() {

}

ncKinect3dScene::ncKinect3dScene() {

}


void ncKinect3dScene::setupGUI() {
	gui.setup("3d scene", "_settings/kinect3dscene.xml");
	gui.add(bDrawGrid.set("draw grid", true));
	gui.add(bDrawFullPointCloud.set("draw pointcloud", true));
	gui.add(bDrawUsersPointCloud.set("draw users pointcloud", true));
	gui.add(bDrawSkeletons.set("draw users skeletons", true));

	gui.add(bFreeCam.set("set cam free", true));
	gui.add(bLeft.set("set cam left", false));
	gui.add(bRight.set("set cam right", false));
	gui.add(bTop.set("set cam top", false));
	gui.add(bBottom.set("set cam bottom", false));
	gui.add(bFront.set("set cam front", false));
	gui.add(bBack.set("set cam back", false));

	gui.loadFromFile("_settings/kinect3dscene.xml");

	bFreeCam.addListener(this, &ncKinect3dScene::freecamChanged);
	bLeft.addListener(this, &ncKinect3dScene::leftcamChanged);
	bRight.addListener(this, &ncKinect3dScene::rightcamChanged);
	bTop.addListener(this, &ncKinect3dScene::topcamChanged);
	bBottom.addListener(this, &ncKinect3dScene::bottomcamChanged);
	bFront.addListener(this, &ncKinect3dScene::frontcamChanged);
	bBack.addListener(this, &ncKinect3dScene::backcamChanged);


}

void ncKinect3dScene::setup(ncKinectv2Core & _core, ncKinectUserManager & _usermanager, nCKinectCamera & _camera) {

	kinectcore = &_core;
	kinectusermanager = &_usermanager;
	kinectcamera = &_camera;

	setupGUI();

	cam.setDistance(1);
	cam.setNearClip(0.01);
}


void ncKinect3dScene::draw() {


	if (bFreeCam) {
		cam.begin();
	}
	else {
		cam2d.begin();
	}

	ofEnableDepthTest();

	if (bDrawGrid) {
		ofSetColor(0, 0, 0, 100);
		drawGridOneColor(1, 5, true, true, true, false);
		ofSetColor(255);
	}

	kinectcamera->begin();

	if (bDrawFullPointCloud) {

		if (kinectcore->getColorTexture().getWidth()>0) {
			kinectcore->getColorTexture().bind();
		}
		kinectcore->getPointCloud3D().draw();
		if (kinectcore->getColorTexture().getWidth()>0) {
			kinectcore->getColorTexture().unbind();
		}
		
	}

	if (bDrawUsersPointCloud) {
		drawUserPointcloud();
	}

	if (bDrawSkeletons) {
		drawUserSkeletons();
	}

	kinectcamera->end();

	ofDisableDepthTest();

	if (bFreeCam) {
		cam.end();
	}
	else {
		cam2d.end();
	}

}

void ncKinect3dScene::drawGUI() {
	gui.draw();
	kinectcamera->drawGUI();
}

void ncKinect3dScene::drawUserPointcloud() {
	
	kinectcore->getUserPointCloud3D().draw();
}

void ncKinect3dScene::drawUserSkeletons()
{
	for (size_t i = 0; i <kinectusermanager->getUsers().size(); i++) {

		kinectusermanager->getUsers()[i]->setBoneColor(kinectusermanager->getUsers()[i]->color);

		for (size_t k = 0; k <kinectusermanager->getUsers()[i]->bones3D.size(); k++) {
			
			kinectusermanager->getUsers()[i]->bones3D[k].draw();
		
		}

		for (size_t k = 0; k <kinectusermanager->getUsers()[i]->bonesVanity3D.size(); k++) {
			
			kinectusermanager->getUsers()[i]->bonesVanity3D[k].draw();
			
		}
	}
}


//--------------------------------------------------------------
void ncKinect3dScene::drawGridOneColor(float stepSize, size_t numberOfSteps, bool labels, bool x, bool y, bool z) {

	if (x) {
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
	}
	if (y) {
		ofMatrix4x4 m;
		m.makeRotationMatrix(90, 0, 0, -1);
		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
		ofPopMatrix();
	}
	if (z) {
		ofMatrix4x4 m;
		m.makeRotationMatrix(90, 0, 1, 0);
		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
		ofPopMatrix();
	}

	if (labels) {
		float labelPos = stepSize * (numberOfSteps + 0.5);
		//ofSetBitmapTextMode(OF_BITMAPMODE_MODEL_BILLBOARD);
		ofDrawBitmapString("x", labelPos, 0, 0);
		ofDrawBitmapString("y", 0, labelPos, 0);
		ofDrawBitmapString("z", 0, 0, labelPos);
	}
}

void ncKinect3dScene::freecamChanged(bool & val) {
	if (val) {
		bFront = false;
		bBack = false;
		bLeft = false;
		bRight = false;
		bBottom = false;
		bTop = false;
	}
}

void ncKinect3dScene::topcamChanged(bool & val)
{
	if (val) {
		bFreeCam = false;
		bFront = false;
		bBack = false;
		bLeft = false;
		bRight = false;
		bBottom = false;

		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_TOP);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}

void ncKinect3dScene::bottomcamChanged(bool & val)
{
	if (val) {
		bFreeCam = false;
		bFront = false;
		bBack = false;
		bLeft = false;
		bRight = false;
		bTop = false;
		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_BOTTOM);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}

void ncKinect3dScene::leftcamChanged(bool & val)
{
	if (val) {
		bFreeCam = false;
		bFront = false;
		bBack = false;
		bRight = false;
		bBottom = false;
		bTop = false;
		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_LEFT);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}

void ncKinect3dScene::rightcamChanged(bool & val)
{
	if (val) {
		bFreeCam = false;
		bFront = false;
		bBack = false;
		bLeft = false;
		bBottom = false;
		bTop = false;
		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_RIGHT);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}

void ncKinect3dScene::frontcamChanged(bool & val)
{
	if (val) {
		bFreeCam = false;
		bBack = false;
		bLeft = false;
		bRight = false;
		bBottom = false;
		bTop = false;
		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_FRONT);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}

void ncKinect3dScene::backcamChanged(bool & val) {
	if (val) {
		bFreeCam = false;
		bFront = false;
		bLeft = false;
		bRight = false;
		bBottom = false;
		bTop = false;
		cam2d.setFlipY(true);
		cam2d.setLookAt(ofxInfiniteCanvas::OFX2DCAM_BACK);
		cam2d.setScale(200);
		cam2d.setTranslation(ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0));
	}
}
