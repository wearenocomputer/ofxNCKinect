#pragma once

#include "ofMain.h"
#include "Kinect.h"
#include "ncKinectUser.h"

//#define GESTURES
#ifdef GESTURES
#include "ncKinectv2GestureDetector.h"
#endif  

//#define USETHREAD

class ncKinectv2Core: public ofThread {

public:
	~ncKinectv2Core();
	ncKinectv2Core();

#ifdef GESTURES
	void setup(string database, bool _usecolor = false);
	void turnOffGestureDectors();
	void turnOnGestureDetectorForUser(int userid, UINT64 kinectid);
#endif

	void setup(bool _usecolor = false);
	void update(bool _colorpointcloud = false, bool _coloruserpointcloud=false, bool _colorbodyindexmap=false);
	void updatesepbodyindexmap(bool _colorpointcloud = false, bool _coloruserpointcloud = false);

	
	//GETTERS
	ofVec4f getFloorPlane();
	vector <ncKinectUser> & getUsers();
	ofPixels_<unsigned short> & getDepthMap2D();
	ofPixels & getBodyIndexPixels();
	ofPixels & getBodyIndexPixelsColored();
	ofPixels & getColorPixels();
	ofTexture & getColorTexture();
	ofMesh & getPointCloud3D();
	ofMesh & getUserPointCloud3D();
	ofVec3f convert2dPointToSpacePoint(ofVec2f _point);
	ofVec2f convert2dPointToColorSpacePoint(ofVec2f _point);

	// Added by JAN
	ofPixels & getCustomDepthMap( ofRectangle cropRectangle, int frontPlaneDepth, int backPlaneDepth );


private:
	IKinectSensor* sensor;             // Kinect sensor
	IMultiSourceFrameReader* reader;   // Kinect data source
	ICoordinateMapper* mapper;         // Converts between depth, color, and 3d coordinates

#ifdef GESTURES
	vector<ncKinectv2GestureDetector*> gesturedetectors;
	bool initKinectGestures(DWORD enabledFrameSourceTypes,string database);
#endif  

	void prepareData();

	bool initKinect(DWORD enabledFrameSourceTypes);
	
	void getDepthData(IMultiSourceFrame* frame);
	void getColorData(IMultiSourceFrame* frame);
	void getBodyData(IMultiSourceFrame* frame);
	void getBodyIndexData(IMultiSourceFrame* frame);

	//DEPTH MAP
	UINT16*	depthmap;
	ofPixels_<unsigned short> depthpixels; // pixels with a depth of 16-bit 

	// ADDED BY JAN
	ofPixels customDepthPixels;

	//BODY INDEX MAP
	BYTE*	bodyindexmap;
	ofPixels bodyindexmappixels;
	ofPixels bodyindexmappixelscolormapped;

	//COLOR MAP
	ofPixels colorpixels;
	ofTexture colortexture;
	bool bUseColor;
	
	//FLOORPLANE
	ofVec4f floorplane;

	//POINTCLOUD
	ofMesh pointcloud;
	ofMesh userpointcloudmesh;
	
	vector <ncKinectUser> users;
	vector <ofColor> usercolors;

#ifdef USETHREAD
	void threadedFunction();
	ofMutex locker;
#endif 
};

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease) {
	if (pInterfaceToRelease != NULL) {
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}
