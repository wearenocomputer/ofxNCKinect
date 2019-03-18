#pragma once
#include "ofMain.h"



class  NCJoints :public ofNode {
public:
	
	ofSpherePrimitive sphere;
	vector<glm::vec3> positions;

	virtual void setup() {
		sphere.set(0.05, 10);
	}

	virtual void customDraw() {
		
			for (int i = 0; i < positions.size(); ++i) {
				transformGL();
				sphere.setGlobalPosition(positions[i]);
				sphere.drawWireframe();
				restoreTransformGL();
			}
		
	}
};


class NCKinectPointcloud3DModel : public ofNode {

public:

	ofMesh mesh;

	virtual void setup() {
		mesh.setMode(OF_PRIMITIVE_POINTS);
		mesh.getVertices().resize(512 * 424);
	}

	virtual void customDraw() {
		transformGL();
		mesh.draw();
		restoreTransformGL();
	}

};


class NCKinectCamera3DModel : public ofNode {

public:

	ofBoxPrimitive model;

	virtual void setup() {
		model.set(0.3, 0.1, 0.1);
	}

	virtual void customDraw() {
		transformGL();
		ofDrawAxis(0.5);
		model.drawWireframe();
		restoreTransformGL();
	}

};


class NCKinectScene : public ofNode {

public:

	NCKinectCamera3DModel camera;
	NCKinectPointcloud3DModel pointcloud;
	ofVec4f floorplane;
	vector<NCJoints> heads;
	ofVec3f cameraposition;
	ofQuaternion camerarotation;

	bool bDoCameraToWorld;
	bool bDrawPointCloud;
	bool bDrawJoints;
	bool bDrawCamera;

	ofColor skeletoncolor;
		

	virtual void setup() {
		cameraposition = ofVec3f(0, 0, 0);
		camerarotation = ofQuaternion(0, ofVec3f(1, 0, 0), 0, ofVec3f(0, 1, 0), 0, ofVec3f(0, 0, 1));
		bDoCameraToWorld = true;
		bDrawCamera = true;
		bDrawJoints = true;
		bDrawPointCloud = true;
		camera.setup();
		pointcloud.setup();
		srand(time(NULL));
		skeletoncolor = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
		setGlobalPosition(cameraposition);
	}

	virtual void customDraw() {

		if (bDoCameraToWorld) {
			restoreTransformGL();
			transformGL();
			//http://blog.hackandi.com/inst/blog/2014/03/18/convert-kinect-cameraspace-to-worldspace-relative-to-floor/
			ofVec3f up = ofVec3f(floorplane.x, floorplane.y, floorplane.z);
			ofVec3f forward = up.getCrossed(ofVec3f(1, 0, 0));
			forward.normalize();
			ofVec3f right = up.getCrossed(forward);
			ofMatrix4x4 mymat = ofMatrix4x4(
				right.x, up.x, forward.x, 0,
				right.y, up.y, forward.y, 0,
				right.z, up.z, forward.z, 0,
				0, floorplane.w, 0, 1);

			ofMatrix4x4 currenttranslation;
			currenttranslation.translate(cameraposition);
			ofMatrix4x4 currentrotation;
			currentrotation.rotate(camerarotation);

			setTransformMatrix(mymat*currentrotation*currenttranslation);

			if (bDrawCamera) {
				camera.draw();
			}

			if (bDrawPointCloud) {
				pointcloud.draw();
			}

			if (bDrawJoints) {
				ofSetColor(skeletoncolor);
				for (int i = 0; i < heads.size(); i++) {
					heads[i].customDraw();
				}
				ofSetColor(255);
			}
			restoreTransformGL();
		}
		else {

			transformGL();
			resetTransform();
			ofMatrix4x4 currenttranslation;
			currenttranslation.translate(cameraposition);

			ofMatrix4x4 currentrotation;
			currentrotation.rotate(camerarotation);

			setTransformMatrix(currentrotation*currenttranslation);

			if (bDrawCamera) {
				camera.draw();
			}

			if (bDrawPointCloud) {
				pointcloud.draw();
			}

			if (bDrawJoints) {
				ofSetColor(skeletoncolor);
				for (int i = 0; i < heads.size(); i++) {
					heads[i].customDraw();
				}
				ofSetColor(255);
			}

			restoreTransformGL();
		}
	}

	virtual void setTransformMatrix(const ofMatrix4x4 &m44) {
		ofVec3f position;
		ofQuaternion orientation;
		ofVec3f scale;
		ofQuaternion so;
		m44.decompose(position, orientation, scale, so);
		setPosition(position);
		setOrientation(orientation);
		setScale(scale);
		updateAxis();
		onPositionChanged();
		onOrientationChanged();
		onScaleChanged();
	}
};