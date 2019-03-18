#pragma once

#include "ofxGuiGroupPlus.h"
#include "ofxPanel.h"
class ofxGuiGroupPlus;

class ofxPanelPlus : public ofxGuiGroupPlus {
public:
	ofxPanelPlus();
	ofxPanelPlus(const ofParameterGroup & parameters, const std::string& filename="settings.xml", float x = 10, float y = 10);
	~ofxPanelPlus();

	ofxPanelPlus * setup(const std::string& collectionName="", const std::string& filename="settings.xml", float x = 10, float y = 10);
	ofxPanelPlus * setup(const ofParameterGroup & parameters, const std::string& filename="settings.xml", float x = 10, float y = 10);

	bool mouseReleased(ofMouseEventArgs & args);

	ofEvent<void> loadPressedE;
	ofEvent<void> savePressedE;
protected:
	void render();
	bool setValue(float mx, float my, bool bCheck);
	void generateDraw();
	void loadIcons();
private:
	ofRectangle loadBox, saveBox;
	static ofImage loadIcon, saveIcon;
    
    ofPoint grabPt;
	bool bGrabbed;
};
