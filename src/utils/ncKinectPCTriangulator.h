#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ncKinectUser.h"
#include "ncKinectv2Core.h"
#include "ncKinectCamera.h"
#include "ncKinect2dScene.h"
#include "ncKinectEventDispatcher.h"
#include "ofxMeshUtils.h"


class ncKinectPCTriangulator :ofThread {

public:
	void setup(vector<ncKinectUser*> &_users, ncKinectv2Core &_core,nCKinectCamera &_camera, ofPixels &_usermap, ofMesh & _pointcloud, int _id);
	int id;
	void update();
	void draw();
	void drawGUI();
	ofMesh & getUserMesh();
	void threadedFunction();
	
private:

	ofMesh usermesh;
	vector<ncKinectUser*> * users;
	nCKinectCamera *kinectcamera;
	ncKinect2dScene kinect2sscene;
	ncKinectv2Core *core;
	ofPixels *usermap;
	ofMesh *pointcloud;
	ofEasyCam cam;

	ofFbo silhouettebuffer;
	ofPixels silhouettespixels;

	ofxPanel gui;
	ofParameter <bool> bDrawGrid;
	ofParameter <bool> bClampPosition;
	ofParameter <bool> bRotate;
	ofParameter <bool> bDoPointCloud;
	ofParameter <float> pointSize;
	ofParameter <bool> bDrawContours;
	ofParameter <float> colorblend;

	void drawGridOneColor(float stepSize, size_t numberOfSteps, bool labels, bool x = true, bool y = true, bool z = true);
	
	//MESH STUFF
	int sampleSize;
	int depthWidth;
	int depthHeight;

	int sampledWidth;
	int sampledHeight;

	int vCount;
	int tCount;

	vector<int> vertexType;
	vector<int> vertexIndex;

	vector < glm::vec3 > vertices;
	vector < glm::vec2 > uvs;
	vector<GLuint> triangles;

	void estimateUserVertices(int &vCount, int &tCount);
	bool isUserSampleValid(int x, int y, glm::vec3 &vSpacePos);
	bool isSpacePointsClose(vector<glm::vec3> vSpacePos, float fMinDistSquared);

	ofImage silhouetteimage;

};
