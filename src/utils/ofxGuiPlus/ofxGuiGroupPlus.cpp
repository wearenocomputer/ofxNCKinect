#include "ofxGuiGroupPlus.h"
#include "ofxSliderGroupPlus.h"
using namespace std;

ofxGuiGroupPlus::ofxGuiGroupPlus():ofxGuiGroup(){}
ofxGuiGroupPlus::ofxGuiGroupPlus(const ofParameterGroup & parameters, const std::string& filename, float x, float y):ofxGuiGroup(){
	setup(parameters, filename, x, y);
}
ofxGuiGroupPlus * ofxGuiGroupPlus::setup(const std::string& collectionName, const std::string& filename, float x, float y){
	parameters.setName(collectionName);
	return setup(parameters, filename, x, y);
}
ofxGuiGroupPlus * ofxGuiGroupPlus::setup(const ofParameterGroup & _parameters, const std::string& _filename, float x, float y){
	b.x = x;
	b.y = y;
	header = defaultHeight;
	spacing = 1;
	spacingNextElement = 3;
	if(parent != nullptr){
		b.width = parent->getWidth();
	}else{
		b.width = defaultWidth;
	}
	clear();
	filename = _filename;
	bGuiActive = false;

	for(std::size_t i = 0; i < _parameters.size(); i++){
		string type = _parameters.getType(i);
		if(type == typeid(ofParameter <int32_t> ).name()){
			auto p = _parameters.getInt(i);
			add(p);
		}else if(type == typeid(ofParameter <uint32_t> ).name()){
			auto p = _parameters.get<uint32_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <int64_t> ).name()){
			auto p = _parameters.get<int64_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <uint64_t> ).name()){
			auto p = _parameters.get<uint64_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <int8_t> ).name()){
			auto p = _parameters.get<int8_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <uint8_t> ).name()){
			auto p = _parameters.get<uint8_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <int16_t> ).name()){
			auto p = _parameters.get<int16_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <uint16_t> ).name()){
			auto p = _parameters.get<uint16_t>(i);
			add(p);
		}else if(type == typeid(ofParameter <float> ).name()){
			auto p = _parameters.getFloat(i);
			add(p);
		}else if(type == typeid(ofParameter <double> ).name()){
			auto p = _parameters.get<double>(i);
			add(p);
		}else if(type == typeid(ofParameter <bool> ).name()){
			auto p = _parameters.getBool(i);
			add(p);
		}else if(type == typeid(ofParameter <ofVec2f> ).name()){
			auto p = _parameters.getVec2f(i);
			add(p);
		}else if(type == typeid(ofParameter <ofVec3f> ).name()){
			auto p = _parameters.getVec3f(i);
			add(p);
		}else if(type == typeid(ofParameter <ofVec4f> ).name()){
			auto p = _parameters.getVec4f(i);
			add(p);
#ifdef USE_GLM
		}else if(type == typeid(ofParameter <glm::vec2> ).name()){
			auto p = _parameters.getVec2f(i);
			add(p);
		}else if(type == typeid(ofParameter <glm::vec3> ).name()){
			auto p = _parameters.getVec3f(i);
			add(p);
		}else if(type == typeid(ofParameter <glm::vec4> ).name()){
			auto p = _parameters.getVec4f(i);
			add(p);
#endif
		}else if(type == typeid(ofParameter <ofColor> ).name()){
			auto p = _parameters.getColor(i);
			add(p);
		}else if(type == typeid(ofParameter <ofShortColor> ).name()){
			auto p = _parameters.getShortColor(i);
			add(p);
		}else if(type == typeid(ofParameter <ofFloatColor> ).name()){
			auto p = _parameters.getFloatColor(i);
			add(p);
		}else if(type == typeid(ofParameter <string> ).name()){
			auto p = _parameters.getString(i);
			ofxGuiGroup::add(p);
		}else if(type == typeid(ofParameterGroup).name()){
			auto p = _parameters.getGroup(i);
			ofxGuiGroupPlus * panel = new ofxGuiGroupPlus(p);
			ofxGuiGroup::add(panel);
		}else{
			ofLogWarning() << "ofxBaseGroup; no control for parameter of type " << type;
		}
	}

	parameters = _parameters;
	registerMouseEvents();

	setNeedsRedraw();

	return this;
}


void ofxGuiGroupPlus::add(const ofParameterGroup & parameters){
	ofxGuiGroupPlus * panel = new ofxGuiGroupPlus(parameters);
	panel->parent = this;
	ofxGuiGroupPlus::add(panel);
}
void ofxGuiGroupPlus::add(ofParameter <bool> & parameter){
	ofxGuiGroup::add(parameter);
}
void ofxGuiGroupPlus::add(ofParameter <std::string> & parameter){
	ofxGuiGroup::add(parameter);
}
void ofxGuiGroupPlus::add(ofParameter <ofVec2f> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <ofVec2f>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <ofVec3f> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <ofVec3f>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <ofVec4f> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <ofVec4f>(parameter, b.width));
}
#ifdef USE_GLM
void ofxGuiGroupPlus::add(ofParameter <glm::vec2> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <glm::vec2>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <glm::vec3> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <glm::vec3>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <glm::vec4> & parameter){
	ofxGuiGroup::add(new ofxVecSliderPlus_ <glm::vec4>(parameter, b.width));
}
#endif
void ofxGuiGroupPlus::add(ofParameter <ofColor> & parameter){
	ofxGuiGroup::add(new ofxColorSliderPlus_ <unsigned char>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <ofShortColor> & parameter){
	ofxGuiGroup::add(new ofxColorSliderPlus_ <unsigned short>(parameter, b.width));
}

void ofxGuiGroupPlus::add(ofParameter <ofFloatColor> & parameter){
	ofxGuiGroup::add(new ofxColorSliderPlus_ <float>(parameter, b.width));

}
//	void ofxGuiGroupPlus::add(ofParameter <float>){
//		ofxGuiGroup::add(new ofxSliderPlus<float>(parameter));
//	}
//void ofxGuiGroupPlus::add(ofParameter <double>;
//void ofxGuiGroupPlus::add(ofParameter <int8_t>;
//void ofxGuiGroupPlus::add(ofParameter <uint8_t>;
//void ofxGuiGroupPlus::add(ofParameter <int16_t>;
//void ofxGuiGroupPlus::add(ofParameter <uint16_t>;
//void ofxGuiGroupPlus::add(ofParameter <int32_t>;
//void ofxGuiGroupPlus::add(ofParameter <uint32_t>;
//void ofxGuiGroupPlus::add(ofParameter <int64_t>;
//void ofxGuiGroupPlus::add(ofParameter <uint64_t>)

//void ofxGuiGroupPlus::add(ofxBaseGui * element){
//	ofxGuiGroup::add(element);
//}


void ofxGuiGroupPlus::add(ofxBaseGui * element){
	collection.push_back(element);
	
	element->setPosition(b.x, b.y + b.height  + spacing);
	
	b.height += element->getHeight() + spacing;
	
	//if(b.width<element->getWidth()) b.width = element->getWidth();
	
	element->unregisterMouseEvents();
	
	element->setParent(this);
	
	ofxGuiGroupPlus * subgroup = dynamic_cast <ofxGuiGroupPlus *>(element);
	if(subgroup != nullptr){
		subgroup->filename = filename;
		subgroup->setWidthElements(b.width * .98);
	}else{
		if(parent != nullptr){
			element->setSize(b.width * .98, element->getHeight());
			element->setPosition(b.x + b.width - element->getWidth(), element->getPosition().y);
		}
	}
	
	parameters.add(element->getParameter());
	setNeedsRedraw();
}



ofxIntSliderPlus & ofxGuiGroupPlus::getIntSlider(const std::string& name){
	return getControlType <ofxIntSliderPlus>(name);
}

ofxFloatSliderPlus & ofxGuiGroupPlus::getFloatSlider(const std::string& name){
	return getControlType <ofxFloatSliderPlus>(name);
}

ofxGuiGroupPlus & ofxGuiGroupPlus::getGroup(const std::string& name){
	return getControlType <ofxGuiGroupPlus>(name);
}
