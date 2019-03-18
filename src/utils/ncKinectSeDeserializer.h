#pragma once
#include "ofMain.h"
#include "ncKinectUser.h"

class ncKinectSeDeSerObject {

public:
	ofVec4f floorplane;
	vector < glm::vec3 > vertices;
	vector<ncKinectUser> users;
};


class ncKinectSeDeserializer
{
    
public:

	const string FLOORPLANEBEGIN = "[floor]";
	const string FLOORPLANEEND = "[/floor]";
	const string VERTS_BEGIN = "[verts]";
	const string VERTS_END = "[/verts]";
	const string NUM_VERTS_BEGIN = "[numverts]";
	const string NUM_VERTS_END = "[/numverts]";

	const string NUM_SKEL_BEGIN = "[numskeletons]";
	const string NUM_SKEL_END = "[/numskeletons]";

	const string SKELS_BEGIN = "[skels]";
	const string SKELS_END = "[/skels]";
	
	ncKinectSeDeserializer(){    
    }


	int getValue(ofBuffer buffer, string openTag, string closeTag)
	{
		unsigned char * data = (unsigned char *)buffer.getData();
		int start = findDelimiter(buffer, openTag);
		if (start < 0)
		{
			ofLogError() << openTag << " NOT FOUND";
			return start;
		}

		int end = findDelimiter(buffer, closeTag);
		if (end < 0)
		{
			ofLogError() << closeTag << " NOT FOUND";
			return end;
		}

		int startPos = start + openTag.length();

		data += startPos;

		int counter = startPos;
		stringstream ss;
		while (counter != end)
		{
			ss << data[0];
			data++;
			counter++;
		}
		int result = ofToInt(ss.str());
		return result;

	}

	int findDelimiter(ofBuffer inputBuffer, string delimiter)
	{
		char * data = (char *)inputBuffer.getData();
		int size = inputBuffer.size();

		unsigned int posInDelimiter = 0;
		for (int i = 0; i < size; i++)
		{
			if (data[i] == delimiter[posInDelimiter])
			{
				posInDelimiter++;
				if (posInDelimiter == delimiter.size())
				{
					return i - delimiter.size() + 1;
				}
			}
			else
			{
				posInDelimiter = 0;
			}
		}
		return -1;
	}
    
	void internal_deserialize(ncKinectSeDeSerObject& object, ofBuffer buffer) {
		unsigned char * data = (unsigned char *)buffer.getData();

		//GET FLOORPLAN
		int startfloorplan = findDelimiter(buffer, FLOORPLANEBEGIN);
		int endfloorplan = findDelimiter(buffer, FLOORPLANEEND);

		int startPosFloorPlan = startfloorplan + FLOORPLANEBEGIN.length();
		data += startPosFloorPlan;
		memcpy(&object.floorplane, data, sizeof(glm::vec4));

		//GET NUM VERTICES
		int numVerts = getValue(buffer, NUM_VERTS_BEGIN, NUM_VERTS_END);

		//GET VERTICES
		unsigned char * datavertices = (unsigned char *)buffer.getData();
		int startvertices = findDelimiter(buffer, VERTS_BEGIN);
		//int endvertices = findDelimiter(buffer, VERTS_END);
		int startPosVertices = startvertices + VERTS_BEGIN.length();
		datavertices += startPosVertices;
		int stepSize = sizeof(glm::vec3);
		for (int i = 0; i < numVerts; i++) {
			glm::vec3 value;
			memcpy(&value, datavertices, stepSize);
			datavertices += stepSize;
			object.vertices.push_back(value);
		}

		//GET NUM SKELETONS
		int numSkeletons = getValue(buffer, NUM_SKEL_BEGIN, NUM_SKEL_END);

		if (numSkeletons > 0) {

			//GET SKELETONS
			for (int i = 0; i < numSkeletons; i++) {
				//GET THE VALUE OF THIS SKELETON ID

				//find skelidvalue
				int skelid = getValue(buffer, "[SKEL_ID" + ofToString(i) + "_START]", "[SKEL_ID" + ofToString(i) + "_END]");
			

				//create the user
				ncKinectUser user;
	
				object.users.push_back(user);
				object.users[i].id= skelid;
				object.users[i].joints2dposition.resize(_ncJointType::ncJointType_Count);
				object.users[i].joints3dposition.resize(_ncJointType::ncJointType_Count);
				object.users[i].joints3drotation.resize(_ncJointType::ncJointType_Count);

				string tofind = "[SKEL_JOINTS" + ofToString(i) + "_START]";
				unsigned char * jointsdata = (unsigned char *)buffer.getData();
				int startjoints = findDelimiter(buffer, tofind);
				int startPosJoints = startjoints + tofind.length();
				jointsdata += startPosJoints;
				int stepSizeJoints = sizeof(glm::vec3);
				//find joints
				for (int j = 0; j < _ncJointType::ncJointType_Count; j++) {
					glm::vec3 jvalue;
					memcpy(&jvalue, jointsdata, stepSize);
					jointsdata += stepSize;
					object.users[i].joints3dposition[j] = jvalue;
				}


				//find the joints rotation data
				string tofindrot = "[SKEL_JOINTS_ROTATION_" + ofToString(i) + "_START]";
				int delimeterrotbegin = findDelimiter(buffer, tofindrot);
				int delimeterrotend = delimeterrotbegin + tofindrot.length();
				unsigned char * jointsrotdata = (unsigned char *)buffer.getData();
				jointsrotdata += delimeterrotend;
				int stepSizeJointsRot = sizeof(ofQuaternion);
		
				for (int j = 0; j < ncJointType_Count; j++) {
					ofQuaternion kvalue;
					memcpy(&kvalue, jointsrotdata, stepSizeJointsRot);
					jointsrotdata += stepSizeJointsRot;
					object.users[i].joints3drotation[j] = kvalue;
				}
				
				//find the joints position data 2d
				string tofindpos2d = "[SKEL_JOINTS_POSITION_2D" + ofToString(i) + "_START]";
				int delimeterpos2dbegin = findDelimiter(buffer, tofindpos2d);
				int delimeterpos2dend = delimeterpos2dbegin + tofindpos2d.length();
				unsigned char * jointspos2ddata = (unsigned char *)buffer.getData();
				jointspos2ddata += delimeterpos2dend;
				int stepSizeJointsPos2d = sizeof(glm::vec2);
			
				for (int j = 0; j < ncJointType_Count; j++) {
					glm::vec3 zvalue;
					memcpy(&zvalue, jointspos2ddata, stepSizeJointsPos2d);
					jointspos2ddata += stepSizeJointsPos2d;
					object.users[i].joints2dposition[j] = zvalue;
				}
			}
		}
	}
    

    void internal_serialize(ncKinectSeDeSerObject& source, ofBuffer& buffer) {
		
		//FLOORPLANE
		buffer.append(FLOORPLANEBEGIN);
		char sendData[sizeof(glm::vec4)];
		memcpy(sendData, &source.floorplane, sizeof(glm::vec4));
		buffer.append(sendData,sizeof(glm::vec4));
		buffer.append(FLOORPLANEEND);

		//VERTICES
		//FIRST ADD NUMBER OF VERTICES
		buffer.append(NUM_VERTS_BEGIN);
		buffer.append(ofToString(source.vertices.size()));
		buffer.append(NUM_VERTS_END);
		if (source.vertices.size() > 0) {
			//SECOND ADD THE VERTICES
			buffer.append(VERTS_BEGIN);
			glm::vec3 *data = &source.vertices[0];
			int dataSize = sizeof(glm::vec3)*source.vertices.size();
			buffer.append((const char *)data, dataSize);
			buffer.append(VERTS_END);
		}

		//SKELETONS
		buffer.append(NUM_SKEL_BEGIN);
		buffer.append(ofToString(source.users.size()));
		buffer.append(NUM_SKEL_END);

		if (source.users.size() > 0) {

			for (int i = 0; i < source.users.size(); ++i) {

				string idstart = "[SKEL_ID" + ofToString(i) + "_START]";
				buffer.append(idstart);
				buffer.append(ofToString(source.users[i].id));
				string idend = "[SKEL_ID" + ofToString(i) + "_END]";
				buffer.append(idend);

				string jointsstart = "[SKEL_JOINTS" + ofToString(i) + "_START]";
				buffer.append(jointsstart);
				glm::vec3 *jdata = &source.users[i].joints3dposition[0];
				int jdataSize = sizeof(glm::vec3)*source.users[i].joints3dposition.size();
				buffer.append((const char *)jdata, jdataSize);

				string jointsstartor = "[SKEL_JOINTS_ROTATION_" + ofToString(i) + "_START]";
				buffer.append(jointsstartor);
				ofQuaternion *jdataor = &source.users[i].joints3drotation[0];
				int jdataSizeor = sizeof(ofQuaternion)*source.users[i].joints3drotation.size();
				buffer.append((const char *)jdataor, jdataSizeor);

				string jointsstart2d = "[SKEL_JOINTS_POSITION_2D" + ofToString(i) + "_START]";
				buffer.append(jointsstart2d);
				glm::vec2 *jdata2d = &source.users[i].joints2dposition[0];
				int jdataSize2d = sizeof(glm::vec2)*source.users[i].joints2dposition.size();
				buffer.append((const char *)jdata2d, jdataSize2d);
			}
		}
    }
    
    ofBuffer serialize(ncKinectSeDeSerObject& object) {
        ofBuffer buffer;
		internal_serialize(object, buffer);
        return buffer;
    }
    
    ncKinectSeDeSerObject deserialize(ofBuffer buffer) {
		ncKinectSeDeSerObject obj;
        internal_deserialize(obj, buffer);
        return obj;
    }

};
