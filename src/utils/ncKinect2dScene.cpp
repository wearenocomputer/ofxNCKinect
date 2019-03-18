#include "ncKinect2dScene.h"



ofPath ncKinect2dScene::polysToPath(vector<ofPolyline> _polylines) {

	ofPath path;

	for (int i = 0; i<_polylines.size(); i++) {
		vector<glm::vec3> vertices = _polylines[i].getVertices();

		for (int v = 0; v<vertices.size(); v++) {
			if (v == 0) path.moveTo(vertices[v].x, vertices[v].y);
			else path.lineTo(vertices[v].x, vertices[v].y);
		}

		path.close();
	}

	return path;
}

void ncKinect2dScene::setupGUI() {
	gui.setup("KinectTwoDScene" + ofToString(id), "_settings/kinect2dscene" + ofToString(id) + ".xml");
	gui.add(bDoBlur.set("do blur contours", false));
	gui.add(bluramount.set("blur amount", 0, 0, 20));
	gui.add(scalecontour.set("scale contour", 1, 0.1, 2.0));
	gui.add(bDoResample.set("resample contour", false));
	gui.add(resamplecount.set("resample count", 80, 10, 1000));
	gui.add(erodecount.set("erode count", 0, 0, 10));
	gui.add(dilatecount.set("dilate count", 0, 0, 10));
	gui.add(dilatefirst.set("dilate first", false));
	gui.loadFromFile("_settings/kinect2dscene" + ofToString(id) + ".xml");
}

ncKinect2dScene::~ncKinect2dScene() {

}

ncKinect2dScene::ncKinect2dScene() {

}

void ncKinect2dScene::setup(vector<ncKinectUser*>& _users, int _id) {
	id = _id;
	users = &_users;
	contourimage.allocate(512, 424);
	setupGUI();
	loadcounter = 0;
}

void ncKinect2dScene::setup() {
	contourimage.allocate(512, 424);
	setupGUI();
}

void ncKinect2dScene::update() {
	//BUG IN OFXGUIPLUS LOADING VALUES
	//RELOAD IT HERE THREE TIMES AS FAST AS POSSIBLE
	if (loadcounter < 3) {
		if (ofGetFrameNum() % 1 == 0) {
			loadcounter++;
			gui.loadFromFile("_settings/kinect2dscene" + ofToString(id) + ".xml");
		}
	}
}

void ncKinect2dScene::drawDepthMap2d(ofPixels_<unsigned short> &_depthmap, int x, int y, float scale) {
	depthmap.setFromPixels(_depthmap);
	depthmap.draw(x, y, 512 * scale, 424 * scale);
}

void ncKinect2dScene::drawUserMap2d(ofPixels & _usermap, int x, int y) {
	usermap.setFromPixels(_usermap);
	usermap.draw(x, y);
}

void ncKinect2dScene::drawColorImage(ofPixels & _colorpixels, int x, int y, float scale) {
	colorimage.setFromPixels(_colorpixels);
	colorimage.draw(x, y, 1920 * scale, 1080* scale);
}

void ncKinect2dScene::drawContourImage(int x, int y) {
	contourimage.draw(x, y);
}

void ncKinect2dScene::drawSkeletons(int x, int y, float scale) {

	ofPushMatrix();
	ofTranslate(x, y);
	ofScale(scale, scale);
	for (size_t i = 0; i < users->size(); i++) {

		ofSetColor((*users)[i]->color);
		for (size_t k = 0; k < (*users)[i]->joints2dposition.size(); k++) {
			ofDrawCircle((*users)[i]->joints2dposition[k], 5);
		}
		ofSetColor(255);
	}
	ofPopMatrix();
}

void ncKinect2dScene::drawContourLines(ofPixels & _usermap, int x, int y) {
	ofSetLineWidth(3);
	ofSetColor(ofColor::chartreuse);
	vector<ofPolyline> cont = getContours(_usermap);
	for (size_t i = 0; i < cont.size(); i++) {
		cont[i].draw();
		ofDrawCircle(cont[i].getCentroid2D(), 5);
	}
	ofSetLineWidth(1);
	ofSetColor(255);
}

void ncKinect2dScene::drawFilledContours(ofPixels & _usermap, int x, int y, float scale) {

	ofPushMatrix();
	ofTranslate(x, y);
	ofScale(scale, scale);
	ncKinect2dScene::polysToPath(getContours(_usermap)).draw(0, 0);
	ofPopMatrix();
}

void ncKinect2dScene::drawGUI() {
	gui.draw();
}

vector<ofPolyline> ncKinect2dScene::getContours(ofPixels & _usermap) {
	contourimage.setFromPixels(_usermap);
	if (bDoBlur) {
		contourimage.blurGaussian(bluramount);
	}

	if (dilatefirst) {

		for (size_t i = 0; i < dilatecount; i++) {
			contourimage.dilate();
		}

		for (size_t i = 0; i < erodecount; i++) {
			contourimage.erode();
		}
	}
	else {

		for (size_t i = 0; i < erodecount; i++) {
			contourimage.erode();
		}

		for (size_t i = 0; i < dilatecount; i++) {
			contourimage.dilate();
		}
	}

	contourfinder.findContours(contourimage, 1000, (_usermap.getWidth() * _usermap.getHeight()) / 2, 6, false);

	vector<ofPolyline>  contourlines;
	contourlines.clear();

	for (int i = 0; i < contourfinder.nBlobs; ++i) {
		
		ofPolyline cline;
		for (int j = 0; j < contourfinder.blobs[i].pts.size(); ++j) {
			cline.addVertex(contourfinder.blobs[i].pts[j]);
		}
		cline.close();

		if (bDoResample) {
			ofPolyline c = cline.getResampledByCount(resamplecount);
			contourlines.push_back(c);
		}
		else {
			contourlines.push_back(cline);
		}
	}

	for (int i = 0; i<(int)contourlines.size(); i++) {
		glm::vec2 center = contourlines[i].getCentroid2D();
		ofPoint centerdd = contourlines[i].getCentroid2D();
		ofPoint centerd = centerdd *scalecontour;

		glm::vec2 diff = center - centerd;
		for (int j = 0; j<(int)contourlines[i].size(); j++) {
			contourlines[i][j].x *= scalecontour;
			contourlines[i][j].y *= scalecontour;
			contourlines[i][j].x += diff.x;
			contourlines[i][j].y += diff.y;
		}
	}
	return contourlines;
}

vector<ofRectangle> ncKinect2dScene::getBoundingBoxes(ofPixels & _usermap)
{

	contourimage.setFromPixels(_usermap);
	if (bDoBlur) {
		contourimage.blurGaussian(bluramount);
	}

	if (dilatefirst) {

		for (size_t i = 0; i < dilatecount; i++) {
			contourimage.dilate();
		}

		for (size_t i = 0; i < erodecount; i++) {
			contourimage.erode();
		}
	}
	else {

		for (size_t i = 0; i < erodecount; i++) {
			contourimage.erode();
		}

		for (size_t i = 0; i < dilatecount; i++) {
			contourimage.dilate();
		}
	}

	contourfinder.findContours(contourimage, 1000, (_usermap.getWidth() * _usermap.getHeight()) / 2, 6, false);
	vector<ofRectangle> rectangles;
	for (int i = 0; i < contourfinder.nBlobs; ++i) {
		rectangles.push_back(contourfinder.blobs[i].boundingRect);
	}

	return rectangles;
}

vector<ofRectangle> ncKinect2dScene::getBoundingBoxesSeparateBodyIndex()
{
	vector<ofRectangle> rectangles;

	for (size_t i = 0; i < users->size(); i++) {
		contourimage.setFromPixels((*users)[i]->userpixels);
		contourfinder.findContours(contourimage, 5000, ((*users)[i]->userpixels.getWidth() * (*users)[i]->userpixels.getHeight()) / 2, 6, false);

		for (int i = 0; i < contourfinder.nBlobs; ++i) {
			rectangles.push_back(contourfinder.blobs[i].boundingRect);
		}

	}
	return rectangles;
}
ofPath ncKinect2dScene::getFilledContours(ofPixels & _usermap) {
	return ncKinect2dScene::polysToPath(getContours(_usermap));
}
