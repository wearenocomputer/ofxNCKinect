
#pragma once

#include "ofMain.h"
#include "ncKinectUser.h"
#include "ncKinectCamera.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxGuiPlus\ofxSliderPlus.h"

class ncKinect2dScene {

private:
	vector<ncKinectUser*> * users;
	ofImage depthmap;
	ofImage usermap;
	ofImage colorimage;

	ofxCvContourFinder  contourfinder;
	ofxCvGrayscaleImage contourimage;

	ofxPanel gui;
	ofParameter <bool> bDoBlur;
	ofParameter <int> bluramount;
	ofParameter <float> scalecontour;
	ofParameter <bool> bDoResample;
	ofParameter <int> resamplecount;
	ofParameter <int> erodecount;
	ofParameter <int> dilatecount;
	ofParameter <bool> dilatefirst;
	void setupGUI();
	int id;
	int loadcounter;

public:
	~ncKinect2dScene();
	ncKinect2dScene();


	static ofPath polysToPath(vector<ofPolyline> _polylines);

	void setup(vector<ncKinectUser*> &_users, int _id = 0);
	void setup();
	void update();
	void drawDepthMap2d(ofPixels_<unsigned short> &_depthmap, int x = 0, int y = 0, float scale = 1);
	void drawUserMap2d(ofPixels &_usermap, int x = 0, int y = 0);
	void drawColorImage(ofPixels &_colorpixels, int x = 0, int y = 0, float scale = 1);
	void drawContourImage(int x = 0, int y = 0);
	void drawSkeletons(int x = 0, int y = 0, float scale = 1);
	void drawContourLines(ofPixels &_usermap, int x = 0, int y = 0);
	void drawFilledContours(ofPixels &_usermap, int x = 0, int y = 0, float scale = 1);
	void drawGUI();

	//GETTERS
	vector<ofPolyline> getContours(ofPixels & _usermap);
	vector<ofRectangle> getBoundingBoxes(ofPixels & _usermap);
	vector<ofRectangle> getBoundingBoxesSeparateBodyIndex();
	ofPath getFilledContours(ofPixels & _usermap);


};