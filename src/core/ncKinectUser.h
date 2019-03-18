#pragma once

#include "ofMain.h"
//https://vvvv.org/sites/default/files/images/kinectskeleton-map2.png

//http://peted.azurewebsites.net/wp-content/uploads/2015/04/hierarchy_thumb.png
enum _ncJointType
{
	ncJointType_SpineBase = 0,
	ncJointType_SpineMid = 1,
	ncJointType_Neck = 2,
	ncJointType_Head = 3,
	ncJointType_ShoulderLeft = 4,
	ncJointType_ElbowLeft = 5,
	ncJointType_WristLeft = 6,
	ncJointType_HandLeft = 7,
	ncJointType_ShoulderRight = 8,
	ncJointType_ElbowRight = 9,
	ncJointType_WristRight = 10,
	ncJointType_HandRight = 11,
	ncJointType_HipLeft = 12,
	ncJointType_KneeLeft = 13,
	ncJointType_AnkleLeft = 14,
	ncJointType_FootLeft = 15,
	ncJointType_HipRight = 16,
	ncJointType_KneeRight = 17,
	ncJointType_AnkleRight = 18,
	ncJointType_FootRight = 19,
	ncJointType_SpineShoulder = 20,
	ncJointType_HandTipLeft = 21,
	ncJointType_ThumbLeft = 22,
	ncJointType_HandTipRight = 23,
	ncJointType_ThumbRight = 24,
	ncJointType_Count = (ncJointType_ThumbRight + 1)
};

class nc3DBone :public ofNode {
public:

	ofBoxPrimitive bonecube;
	ofSpherePrimitive bonejoint;
	bool bInverse;

	ofColor bonecolor;
	ofColor bonejointcolor;

	nc3DBone() {
		bonecolor = ofColor(190, 190, 190);
		bonejointcolor = ofColor(74, 245, 254);
		bonecube.set(0.1, 0.1, 0.1);
		bonejoint.set(0.01, 10);
	}

	void setBoneDimension(ofVec2f _dimensions) {
		bonecube.setWidth(_dimensions.x);
		bonecube.setDepth(_dimensions.y);
	}

	void setJointDimension(float _radius) {
		bonejoint.setRadius(_radius);
	}


	void update(ofVec3f _startpos, ofVec3f _endpos) {
		setGlobalPosition(_startpos);
		ofQuaternion rotation;
		rotation.makeRotate(ofVec3f(0, 1, 0), (_startpos - _endpos).getNormalized());
		setGlobalOrientation(rotation);

		if (_endpos.distance(ofVec3f(0, 0, 0)) != 0) {
			float distance = _startpos.distance(_endpos);
			bonecube.setGlobalPosition(0, -distance*0.5, 0);
			bonecube.setHeight(distance);
		}

	}

	void draw() {
		if (getGlobalPosition() != glm::vec3(0)) {
			transformGL();
			ofSetColor(bonecolor);
			bonecube.drawWireframe();
			ofSetColor(bonejointcolor);
			bonejoint.draw();
			ofSetColor(255);
			restoreTransformGL();
		}
	}
};

class ncKinectUser {

public:

	bool tracked;
	int id;
	UINT64 kinectid;
	ofColor color;
	vector<glm::vec3> joints3dposition;
	vector<ofQuaternion> joints3drotation;
	vector<glm::vec2> joints2dposition;
	vector<nc3DBone> bones3D;
	vector<nc3DBone> bonesVanity3D;
	ofPixels userpixels;
	ncKinectUser() {

		userpixels.allocate(512, 424, OF_IMAGE_GRAYSCALE);

		framestorecord = 30;
		threshold = 0.01;
		
		for (size_t i = 0; i < ncJointType_Count; i++) {
			bones3D.push_back(nc3DBone());
		}
		for (size_t i = 0; i < 4; i++) {
			bonesVanity3D.push_back(nc3DBone());
		}
	}

	void recordPositions() {
		if (joints3dposition.size() > 0) {
			if (recordedpositions.size() == 30) {
				recordedpositions.erase(recordedpositions.begin());
			}
			recordedpositions.push_back(joints3dposition[ncJointType_SpineBase]);
		}
	}

	void generate3DSkeleton() {

		if (joints3dposition.size() > 0) {
			
			bones3D[ncJointType_Head].update(joints3dposition[ncJointType_Head], joints3dposition[ncJointType_Neck]);
			bones3D[ncJointType_Neck].update(joints3dposition[ncJointType_Neck], joints3dposition[ncJointType_SpineShoulder]);
			bones3D[ncJointType_SpineShoulder].update(joints3dposition[ncJointType_SpineShoulder], joints3dposition[ncJointType_SpineMid]);

			bones3D[ncJointType_SpineMid].update(joints3dposition[ncJointType_SpineMid], joints3dposition[ncJointType_SpineBase]);

			bones3D[ncJointType_ShoulderLeft].update(joints3dposition[ncJointType_ShoulderLeft], joints3dposition[ncJointType_SpineShoulder]);
			bones3D[ncJointType_ElbowLeft].update(joints3dposition[ncJointType_ElbowLeft], joints3dposition[ncJointType_ShoulderLeft]);
			bones3D[ncJointType_WristLeft].update(joints3dposition[ncJointType_WristLeft], joints3dposition[ncJointType_ElbowLeft]);
			
			bones3D[ncJointType_HandLeft].update(joints3dposition[ncJointType_HandLeft], joints3dposition[ncJointType_WristLeft]);
			bones3D[ncJointType_HandTipLeft].update(joints3dposition[ncJointType_HandTipLeft], joints3dposition[ncJointType_HandLeft]);
			bones3D[ncJointType_ThumbLeft].update(joints3dposition[ncJointType_ThumbLeft], joints3dposition[ncJointType_WristLeft]);

			bones3D[ncJointType_ShoulderRight].update(joints3dposition[ncJointType_ShoulderRight], joints3dposition[ncJointType_SpineShoulder]);
			bones3D[ncJointType_ElbowRight].update(joints3dposition[ncJointType_ElbowRight], joints3dposition[ncJointType_ShoulderRight]);
			bones3D[ncJointType_WristRight].update(joints3dposition[ncJointType_WristRight], joints3dposition[ncJointType_ElbowRight]);
			
			bones3D[ncJointType_HandRight].update(joints3dposition[ncJointType_HandRight], joints3dposition[ncJointType_WristRight]);
			bones3D[ncJointType_HandTipRight].update(joints3dposition[ncJointType_HandTipRight], joints3dposition[ncJointType_HandRight]);
			bones3D[ncJointType_ThumbRight].update(joints3dposition[ncJointType_ThumbRight], joints3dposition[ncJointType_WristRight]);

			bones3D[ncJointType_HipLeft].update(joints3dposition[ncJointType_HipLeft], joints3dposition[ncJointType_SpineBase]);
			bones3D[ncJointType_KneeLeft].update(joints3dposition[ncJointType_KneeLeft], joints3dposition[ncJointType_HipLeft]);
			bones3D[ncJointType_AnkleLeft].update(joints3dposition[ncJointType_AnkleLeft], joints3dposition[ncJointType_KneeLeft]);
			
			bones3D[ncJointType_FootLeft].update(joints3dposition[ncJointType_FootLeft], joints3dposition[ncJointType_AnkleLeft]);
			
			bones3D[ncJointType_HipRight].update(joints3dposition[ncJointType_HipRight], joints3dposition[ncJointType_SpineBase]);
			bones3D[ncJointType_KneeRight].update(joints3dposition[ncJointType_KneeRight], joints3dposition[ncJointType_HipRight]);
			bones3D[ncJointType_AnkleRight].update(joints3dposition[ncJointType_AnkleRight], joints3dposition[ncJointType_KneeRight]);
			bones3D[ncJointType_FootRight].update(joints3dposition[ncJointType_FootRight], joints3dposition[ncJointType_AnkleRight]);
	
			bonesVanity3D[0].update(joints3dposition[ncJointType_ShoulderLeft], joints3dposition[ncJointType_SpineMid]);
			bonesVanity3D[1].update(joints3dposition[ncJointType_ShoulderRight], joints3dposition[ncJointType_SpineMid]);
			bonesVanity3D[2].update(joints3dposition[ncJointType_HipLeft], joints3dposition[ncJointType_SpineMid]);
			bonesVanity3D[3].update(joints3dposition[ncJointType_HipRight], joints3dposition[ncJointType_SpineMid]);

		}
	}

	bool isUserMoving() {

		//USE STANDARD DEVIATION
		glm::vec3 sum = glm::vec3(0, 0, 0), mean = glm::vec3(0, 0, 0), standardDeviation = glm::vec3(0, 0, 0);

		for (int i = 0; i < recordedpositions.size(); ++i)
		{
			sum += recordedpositions[i];
		}

		mean = sum / recordedpositions.size();

		for (int i = 0; i < recordedpositions.size(); ++i) {

			glm::vec3 dev = glm::vec3(pow(recordedpositions[i].x - mean.x, 2), pow(recordedpositions[i].y - mean.y, 2), pow(recordedpositions[i].z - mean.z, 2));
			standardDeviation += dev;
		}

		standardDeviation = standardDeviation / recordedpositions.size();

		glm::vec3 stddev = glm::vec3(sqrt(standardDeviation.x), sqrt(standardDeviation.y), sqrt(standardDeviation.z));

		glm::vec3 normal = glm::vec3(0, 0, 0);

		float distance = glm::distance(stddev, normal);//stddev.distance(normal);

		if (distance > threshold) {
			return true;
		}
		else {
			return false;
		}
	}

	void setBoneDimensions(glm::vec2 _dimensions) {
		for (size_t i = 0; i < bones3D.size(); i++) {
			bones3D[i].setBoneDimension(_dimensions);
		}

		for (size_t i = 0; i < bonesVanity3D.size(); i++) {
			bonesVanity3D[i].setBoneDimension(_dimensions);
		}
	}

	void setJointDimensions(float _radius) {
		for (size_t i = 0; i < bones3D.size(); i++) {
			bones3D[i].setJointDimension(_radius);
		}

		for (size_t i = 0; i < bonesVanity3D.size(); i++) {
			bonesVanity3D[i].setJointDimension(_radius);
		}
	}

	void setJointColor(ofColor _color) {
		for (size_t i = 0; i < bones3D.size(); i++) {
			bones3D[i].bonejointcolor = _color;
		}

		for (size_t i = 0; i < bonesVanity3D.size(); i++) {
			bonesVanity3D[i].bonejointcolor = _color;
		}
	}

	void setBoneColor(ofColor _color) {
		for (size_t i = 0; i < bones3D.size(); i++) {
			bones3D[i].bonecolor = _color;
		}

		for (size_t i = 0; i < bonesVanity3D.size(); i++) {
			bonesVanity3D[i].bonecolor = _color;
		}
	}

private:
	int framestorecord;
	float threshold;
	vector<glm::vec3> recordedpositions;
};