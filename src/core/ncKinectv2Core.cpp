#include "ncKinectv2Core.h"

ncKinectv2Core::~ncKinectv2Core() {
	SafeRelease(sensor);
	SafeRelease(reader);
	SafeRelease(mapper);
	stopThread();
}

ncKinectv2Core::ncKinectv2Core() {

}


/******************************/
/*SETUP FUNCTIONS             */
/******************************/

#ifdef GESTURES
void ncKinectv2Core::setup(string database, bool _usecolor) {
	bUseColor = _usecolor;
	prepareData();
	if (bUseColor) {
		initKinectGestures(FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Body | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Color, database);
	}
	else {
		initKinectGestures(FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Body | FrameSourceTypes::FrameSourceTypes_BodyIndex, database);
	}
	
}
void ncKinectv2Core::turnOffGestureDectors() {
	for (int i = 0; i < BODY_COUNT; i++) {
		gesturedetectors[i]->setTrackingId(0);
		gesturedetectors[i]->setPaused(true);
	}
}

void ncKinectv2Core::turnOnGestureDetectorForUser(int userid, UINT64 kinectid) {
	gesturedetectors[userid]->setTrackingId(kinectid);
	gesturedetectors[userid]->setPaused(false);
}
#endif

void ncKinectv2Core::setup(bool _usecolor) {

	bUseColor = _usecolor;
	prepareData();
	if (bUseColor) {
		initKinect(FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Body | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Color);
	}
	else {
		initKinect(FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Body | FrameSourceTypes::FrameSourceTypes_BodyIndex);
	}
}


void ncKinectv2Core::prepareData() {

	//DEPTHMAP
	depthmap = new UINT16[512 * 424];
	depthpixels.allocate(512, 424, OF_IMAGE_GRAYSCALE);

	//POINTCLOUD
	pointcloud.setMode(OF_PRIMITIVE_POINTS);
	pointcloud.getVertices().resize(512 * 424);
	pointcloud.getTexCoords().resize(512 * 424);
	pointcloud.getColors().resize(512 * 424);

	userpointcloudmesh.setMode(OF_PRIMITIVE_POINTS);

	//BODYINDEX MAP
	bodyindexmap = new BYTE[512 * 424];
	for (size_t i = 0; i < 512 * 424; i++) {
		bodyindexmap[i] = 255;
	}

	bodyindexmappixels.allocate(512, 424, OF_IMAGE_GRAYSCALE);
	for (int i = 0; i < 512 * 424; i++) {
		bodyindexmappixels[i] = 0;
	}

	//COLOR STUFF
	if (bUseColor) {
		bodyindexmappixelscolormapped.allocate(512, 424, OF_IMAGE_COLOR_ALPHA);
		colorpixels.allocate(1920, 1080, OF_IMAGE_COLOR_ALPHA);
	}
	
	//USER MANAGEMENT
	usercolors.push_back(ofColor::darkGrey);
	usercolors.push_back(ofColor::darkRed);
	usercolors.push_back(ofColor::darkBlue);
	usercolors.push_back(ofColor::darkGoldenRod);
	usercolors.push_back(ofColor::darkKhaki);
	usercolors.push_back(ofColor::darkOrchid);

	for (size_t i = 0; i < BODY_COUNT; i++) {
		ncKinectUser user;
		user.id = i;
		user.tracked = false;
		user.color = usercolors[i];
		users.push_back(user);
	}

#ifdef USETHREAD
	startThread();
#endif

}


/******************************/
/*KINECT INIT FUNCTIONS       */
/******************************/

#ifdef GESTURES
//-----------------------------------------
bool ncKinectv2Core::initKinectGestures(DWORD enabledFrameSourceTypes, string database) {

	if (FAILED(GetDefaultKinectSensor(&sensor))) {
		cout << "FAILED TO GET KINECT" << endl;
		return false;
	}

	if (sensor) {
		sensor->Open();
		HRESULT hResult = S_OK;
		hResult = sensor->get_CoordinateMapper(&mapper);
		if (FAILED(hResult)) {
			std::cout << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
		}
		hResult = sensor->OpenMultiSourceFrameReader(enabledFrameSourceTypes, &reader);
		if (FAILED(hResult)) {
			std::cout << "Error : IKinectSensor::OpenMultiSourceFrameReader()" << std::endl;
		}

		for (int i = 0; i < BODY_COUNT; ++i) {
			ncKinectv2GestureDetector * detector = new ncKinectv2GestureDetector(sensor, i, database);
			gesturedetectors.push_back(detector);

			
		}
		return true;
	}
	else {
		return false;
	}
}
#endif  


//-----------------------------------------
bool ncKinectv2Core::initKinect(DWORD enabledFrameSourceTypes) {

	if (FAILED(GetDefaultKinectSensor(&sensor))) {
		cout << "FAILED TO GET KINECT" << endl;
		return false;
	}

	if (sensor) {
		sensor->Open();
		HRESULT hResult = S_OK;
		hResult = sensor->get_CoordinateMapper(&mapper);
		if (FAILED(hResult)) {
			std::cout << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
		}
		hResult = sensor->OpenMultiSourceFrameReader(enabledFrameSourceTypes, &reader);
		if (FAILED(hResult)) {
			std::cout << "Error : IKinectSensor::OpenMultiSourceFrameReader()" << std::endl;
		}
		return true;
	}
	else {
		return false;
	}
}

/******************************/
/*KINECT DATA FUNCTIONS       */
/******************************/

//--------------------------------------------------------------
void ncKinectv2Core::getDepthData(IMultiSourceFrame* frame) {

	IDepthFrame* depthframe;
	IDepthFrameReference* frameref = NULL;
	frame->get_DepthFrameReference(&frameref);
	frameref->AcquireFrame(&depthframe);
	if (frameref) frameref->Release();
	if (!depthframe) {
		//cout << "skipped depthframe " << " " << ofToString(ofGetFrameNum()) << endl;
		return;
	}
	IFrameDescription * frameDescription = NULL;
	depthframe->get_FrameDescription(&frameDescription);
	int width;
	int height;
	frameDescription->get_Width(&width);
	frameDescription->get_Height(&height);

	depthframe->CopyFrameDataToArray(width*height, depthmap);

	SafeRelease(frameDescription);

	if (depthframe) depthframe->Release();
}

void ncKinectv2Core::getColorData(IMultiSourceFrame * frame) {
	IColorFrame* colorframe;
	IColorFrameReference* frameref = NULL;
	frame->get_ColorFrameReference(&frameref);
	frameref->AcquireFrame(&colorframe);

	if (frameref)SafeRelease(frameref);

	if (!colorframe) {
		//cout << "skipped colorframe " << " " << ofToString(ofGetFrameNum()) << endl;
		return;
	}
	colorframe->CopyConvertedFrameDataToArray(colorpixels.size(), colorpixels.getData(), ColorImageFormat_Rgba);

	if (colorframe) colorframe->Release();
}

//--------------------------------------------------------------
void ncKinectv2Core::getBodyData(IMultiSourceFrame* frame) {
	IBodyFrame* bodyframe;
	IBodyFrameReference* frameref = NULL;
	frame->get_BodyFrameReference(&frameref);
	frameref->AcquireFrame(&bodyframe);
	if (frameref) SafeRelease(frameref);

	if (!bodyframe) {
		//cout << "skipped bodyframe " << " " << ofToString(ofGetFrameNum()) << endl;
		return;
	}
	//SET FLOORPLANE
	Vector4 _floorplane;
	bodyframe->get_FloorClipPlane(&_floorplane);
	floorplane.x = _floorplane.x;
	floorplane.y = _floorplane.y;
	floorplane.z = _floorplane.z;
	floorplane.w = _floorplane.w;

	//GET JOINT DATA
	IBody* bodies[BODY_COUNT] = { 0 };

	if (SUCCEEDED(bodyframe->GetAndRefreshBodyData(BODY_COUNT, bodies))) {

		//FIRST LOOP THROUGH TRACKED USER AND DO USER MANAGEMENT
		vector<bool> totrack;
		for (int i = 0; i < BODY_COUNT; i++) {

#ifdef GESTURES
			gesturedetectors[i]->update();
#endif

			BOOLEAN tracked;
			UINT64 trackingId;
			bodies[i]->get_TrackingId(&trackingId);
			bodies[i]->get_IsTracked(&tracked);
			users[i].tracked = (bool)tracked;
			users[i].joints2dposition.clear();
			users[i].joints3dposition.clear();
			users[i].joints3drotation.clear();
			users[i].kinectid = trackingId;

			if ((bool)tracked) {
				Joint joints[JointType::JointType_Count];
				JointOrientation jointsOrient[JointType_Count];
				if (SUCCEEDED(bodies[i]->GetJoints(JointType::JointType_Count, joints))) {
					if (SUCCEEDED(bodies[i]->GetJointOrientations(JointType_Count, jointsOrient))) {
						for (int k = 0; k < JointType::JointType_Count; k++) {
							users[i].joints3dposition.push_back(ofVec3f(joints[k].Position.X, joints[k].Position.Y, joints[k].Position.Z));
							users[i].joints3drotation.push_back(ofVec4f(jointsOrient[k].Orientation.x, jointsOrient[k].Orientation.y, jointsOrient[k].Orientation.z, jointsOrient[k].Orientation.w));
							DepthSpacePoint depthPoint = { 0 };
							mapper->MapCameraPointToDepthSpace(joints[k].Position, &depthPoint);
							users[i].joints2dposition.push_back(ofVec2f(depthPoint.X, depthPoint.Y));
						}
					}
				}
			}
		}

		//After body processing is done, we're done with our bodies so release them.
		for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
			SafeRelease(bodies[bodyIndex]);
		}

	}

	SafeRelease(bodyframe);
}

//--------------------------------------------------------------
void ncKinectv2Core::getBodyIndexData(IMultiSourceFrame* frame) {
	IBodyIndexFrame* bodyframe;
	IBodyIndexFrameReference* frameref = NULL;
	frame->get_BodyIndexFrameReference(&frameref);
	frameref->AcquireFrame(&bodyframe);
	if (frameref) SafeRelease(frameref);

	if (!bodyframe) {
		//cout << "skipped bodyindex data " << " " << ofToString(ofGetFrameNum()) << endl;
		return;
	}
	IFrameDescription * frameDescription = NULL;
	bodyframe->get_FrameDescription(&frameDescription);
	int width;
	int height;
	frameDescription->get_Width(&width);
	frameDescription->get_Height(&height);

	bodyframe->CopyFrameDataToArray(width*height, bodyindexmap);

	SafeRelease(bodyframe);
}


/******************************/
/*UPDATE FUNCTIONS            */
/******************************/

#ifdef USETHREAD
void ncKinectv2Core::threadedFunction() {

	while (isThreadRunning()) {
		if (reader) {
			locker.lock();
			IMultiSourceFrame* frame = NULL;
			if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
				getDepthData(frame);
				if (bUseColor) {
					getColorData(frame);
				}
				getBodyData(frame);
				getBodyIndexData(frame);
			}
			SafeRelease(frame);
			locker.unlock();
		}
	}
}
#endif


void ncKinectv2Core::update(bool _colorpointcloud , bool _coloruserpointcloud , bool _colorbodyindexmap) {

#ifndef USETHREAD

	if (reader) {
		IMultiSourceFrame* frame = NULL;
		if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
			getDepthData(frame);
			if (bUseColor) {
				getColorData(frame);
			}
			getBodyData(frame);
			getBodyIndexData(frame);
		}
		SafeRelease(frame);
	}
#endif
	if (bUseColor) {
		if (_colorpointcloud || _coloruserpointcloud || _colorbodyindexmap) {
			mapper->MapDepthFrameToColorSpace(512 * 424, depthmap, 512 * 424, (ColorSpacePoint*)pointcloud.getTexCoordsPointer());
		}
	}

	mapper->MapDepthFrameToCameraSpace(512 * 424, depthmap, 512 * 424, (CameraSpacePoint*)pointcloud.getVerticesPointer());

	if (bUseColor) {
		if (_colorpointcloud) {
			colortexture.loadData(colorpixels);
		}
	}

	//POINTCLOUD AND BODYINDEXMAP IN ONE
	userpointcloudmesh.clear();
	for (int y = 0; y < 424; y++) {
		for (int x = 0; x < 512; x++) {
			int index = (y * 512) + x;

			//depthmap
			depthpixels[index] = 65536 - (depthmap[index] * 65536 / 7999);

			//indexbodymap
			int uBodyIdx = bodyindexmap[index];

			if (uBodyIdx > -1 && uBodyIdx < 6) {
	
				if (!_coloruserpointcloud) {
					userpointcloudmesh.addVertex(pointcloud.getVertex(index));
				}
				if (bUseColor) {
					if (_colorbodyindexmap || _coloruserpointcloud) {
						ofVec2f mappedCoord = pointcloud.getTexCoord(index);
						mappedCoord.x = floor(mappedCoord.x);
						mappedCoord.y = floor(mappedCoord.y);
						if ((0 <= mappedCoord.x) && (mappedCoord.x < 1920) && (0 <= mappedCoord.y) && (mappedCoord.y < 1080)) {

							if (_colorbodyindexmap) {
								bodyindexmappixelscolormapped.setColor(x, y, colorpixels.getColor(mappedCoord.x, mappedCoord.y));
							}

							if (_coloruserpointcloud) {
								userpointcloudmesh.addVertex(pointcloud.getVertex(index));
								userpointcloudmesh.addColor(colorpixels.getColor(mappedCoord.x, mappedCoord.y));
							}
						}
					} else if(!_coloruserpointcloud) {
						bodyindexmappixels[index] = 255;
					}
				} else {
					bodyindexmappixels[index] = 255;
				}
			}
			else {
				bodyindexmappixels[index] = 0;
				if (bUseColor) {
					if (_colorbodyindexmap) {
						bodyindexmappixelscolormapped.setColor(x, y, ofColor(0, 0, 0, 255));
					}
				}	
			}
		}
	}
}

void ncKinectv2Core::updatesepbodyindexmap(bool _colorpointcloud, bool _coloruserpointcloud) {
#ifndef USETHREAD

	if (reader) {
		IMultiSourceFrame* frame = NULL;
		if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
			getDepthData(frame);
			if (bUseColor) {
				getColorData(frame);
			}
			getBodyData(frame);
			getBodyIndexData(frame);
		}
		SafeRelease(frame);
	}
#endif

	if (bUseColor) {
		if (_colorpointcloud || _coloruserpointcloud) {
			mapper->MapDepthFrameToColorSpace(512 * 424, depthmap, 512 * 424, (ColorSpacePoint*)pointcloud.getTexCoordsPointer());
		}
	}

	mapper->MapDepthFrameToCameraSpace(512 * 424, depthmap, 512 * 424, (CameraSpacePoint*)pointcloud.getVerticesPointer());

	if (bUseColor) {
		if (_colorpointcloud) {
			colortexture.loadData(colorpixels);
		}
	}

	//POINTCLOUD AND BODYINDEXMAP IN ONE
	userpointcloudmesh.clear();
	for (int y = 0; y < 424; y++) {
		for (int x = 0; x < 512; x++) {
			int index = (y * 512) + x;

			//depthmap has value between 0 and 7999, so 8000 values. We map it to 16-bit integer here.
			depthpixels[index] = 65536 - (depthmap[index] * 65536 / 7999);

			//indexbodymap
			int uBodyIdx = bodyindexmap[index];

			if (uBodyIdx > -1 && uBodyIdx < 6) {
				users[uBodyIdx].userpixels[index] = 255;

				if (!_coloruserpointcloud) {
					userpointcloudmesh.addVertex(pointcloud.getVertex(index));
				}

				if (bUseColor) {

					if ( _coloruserpointcloud) {
						ofVec2f mappedCoord = pointcloud.getTexCoord(index);
						mappedCoord.x = floor(mappedCoord.x);
						mappedCoord.y = floor(mappedCoord.y);
						if ((0 <= mappedCoord.x) && (mappedCoord.x < 1920) && (0 <= mappedCoord.y) && (mappedCoord.y < 1080)) {
							userpointcloudmesh.addVertex(pointcloud.getVertex(index));
							userpointcloudmesh.addColor(colorpixels.getColor(mappedCoord.x, mappedCoord.y));
						}

					}
				}


			} else {
				for (size_t i = 0; i < users.size(); i++) {
					users[i].userpixels[index] = 0;
				}
			}
		}
	}
}


/******************************/
/*GETTERS                     */
/******************************/

ofVec4f ncKinectv2Core::getFloorPlane()
{
	return floorplane;
}

vector<ncKinectUser> & ncKinectv2Core::getUsers() {
	return users;
}

ofPixels_<unsigned short> & ncKinectv2Core::getDepthMap2D() {
	return depthpixels;
}

ofPixels & ncKinectv2Core::getBodyIndexPixels() {
	return bodyindexmappixels;
}

ofPixels & ncKinectv2Core::getBodyIndexPixelsColored()
{
	return bodyindexmappixelscolormapped;
}

ofPixels & ncKinectv2Core::getColorPixels() {
	return colorpixels;
}

ofTexture & ncKinectv2Core::getColorTexture()
{
	return colortexture;
}



ofMesh & ncKinectv2Core::getPointCloud3D() {
	return pointcloud;
}

ofMesh & ncKinectv2Core::getUserPointCloud3D() {
	return userpointcloudmesh;
}

ofVec3f ncKinectv2Core::convert2dPointToSpacePoint(ofVec2f _point) {

	ofVec2f intpoint = ofVec2f((int)_point.x, (int)_point.y);

	int pos = intpoint.x + 512 * intpoint.y;
	UINT16 depth = depthmap[pos];

	DepthSpacePoint d;
	d.X = intpoint.x;
	d.Y = intpoint.y;

	CameraSpacePoint *p = new CameraSpacePoint();
	mapper->MapDepthPointToCameraSpace(d, depth, p);

	ofVec3f pp = ofVec3f(p->X, p->Y, p->Z);

	delete p;

	return pp;
}

ofVec2f ncKinectv2Core::convert2dPointToColorSpacePoint(ofVec2f _point) {

	ofVec2f intpoint = ofVec2f((int)_point.x, (int)_point.y);
	int pos = intpoint.x + 512 * intpoint.y;
	UINT16 depth = depthmap[pos];

	DepthSpacePoint d;
	d.X = intpoint.x;
	d.Y = intpoint.y;

	ColorSpacePoint *p = new ColorSpacePoint();
	mapper->MapDepthPointToColorSpace(d, depth, p);

	ofVec2f pp = ofVec3f(p->X, p->Y);

	delete p;

	return pp;

}


/*
	@description: Filter to create a custom depth map image (grayscale, 8-bit). This method maps all values between frontPlaneDepth and backPlaneDepth to values between 0 and 255
	so these can be written to a PNG file to use as a heightmap in other applications like Unity.
	@author: Jan Vantomme
	@param cropRectangle: detail to crop out of the original depth image (max dimensions: 512 x 424 pixels)
	@param frontPlaneDepth: value between 0 and 7999 (maximum of kinect depth data
	@param backPlaneDepth: value between 0 and 7999 (maximum of kinect depth data

 */

ofPixels & ncKinectv2Core::getCustomDepthMap(ofRectangle cropRectangle, int frontPlaneDepth, int backPlaneDepth)
{
	customDepthPixels.allocate(512, 424, OF_IMAGE_GRAYSCALE);

	// depthmap = new UINT16[512 * 424];
	// filter depthmap data based on backplane + frontplane
	for (int j = 0; j < 424; j++) {
		for (int i = 0; i < 512; i++) {

			int index = (j * 512) + i;

			// Default pixel color is black
			int value = 0;

			// Change color of the pixel when the depth value is between front & backplane.
			if (depthmap[index] > frontPlaneDepth && depthmap[index] < backPlaneDepth) {
				value = ofMap(depthmap[index], frontPlaneDepth, backPlaneDepth, 0, 255, true);				
			}

			customDepthPixels[index] = value;

		}
	}

	// crop pixels based on rectangle
	customDepthPixels.crop( cropRectangle.getX(), cropRectangle.getY(), cropRectangle.getWidth(), cropRectangle.getHeight());

	return customDepthPixels;
}