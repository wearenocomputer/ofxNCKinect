#pragma once
#include "ofxBaseGui.h"

#include "ofxGuiGroup.h"

#include "ofParameterGroup.h"
#include "ofParameter.h"
#include "ofxSliderPlus.h"

class ofxGuiGroupPlus : public ofxGuiGroup  {
	public:
		ofxGuiGroupPlus();
		ofxGuiGroupPlus(const ofParameterGroup & parameters, const std::string& _filename = "settings.xml", float x = 10, float y = 10);
		virtual ~ofxGuiGroupPlus(){
		}
		ofxGuiGroupPlus * setup(const std::string& collectionName = "", const std::string& filename = "settings.xml", float x = 10, float y = 10);
		ofxGuiGroupPlus * setup(const ofParameterGroup & parameters, const std::string& filename = "settings.xml", float x = 10, float y = 10);
		void add(ofxBaseGui * element);
		void add(const ofParameterGroup & parameters);

		template<typename T>
		typename std::enable_if<std::is_arithmetic<T>::value, void>::type add(ofParameter<T> & p){
			ofxGuiGroup::add(new ofxSliderPlus<T>(p));
		}
//		void add(ofParameter <long long> & parameter);
//		void add(ofParameter <unsigned long long> & parameter);
		void add(ofParameter <bool> & parameter);
		void add(ofParameter <std::string> & parameter);
		void add(ofParameter <ofVec2f> & parameter);
		void add(ofParameter <ofVec3f> & parameter);
		void add(ofParameter <ofVec4f> & parameter);
#ifdef USE_GLM
		void add(ofParameter <glm::vec2> & parameter);
		void add(ofParameter <glm::vec3> & parameter);
		void add(ofParameter <glm::vec4> & parameter);
#endif
		void add(ofParameter <ofColor> & parameter);
		void add(ofParameter <ofShortColor> & parameter);
		void add(ofParameter <ofFloatColor> & parameter);


		ofxIntSliderPlus & getIntSlider(const std::string& name);
		ofxFloatSliderPlus & getFloatSlider(const std::string& name);
		ofxGuiGroupPlus & getGroup(const std::string& name);

};