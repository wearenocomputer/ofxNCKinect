//
//  ofxDropDown.h
//  emptyExample
//
//  Created by Roy Macdonald on 14-08-15.
//
//

#pragma once

#include "ofxBaseGui.h"
#include "ofMain.h"



class ofxDropDown : public ofxBaseGui{
    friend class ofPanel;
    
public:
    ofxDropDown();
    ~ofxDropDown();
    ofxDropDown(vector<string>& vals, float width = defaultWidth, float height = defaultHeight);
    ofxDropDown(const string& dropdownName, vector<string>& vals, float width = defaultWidth, float height = defaultHeight);
    ofxDropDown* setup(vector<string>& vals, float width = defaultWidth, float height = defaultHeight);
    ofxDropDown* setup(const string& dropdownName, vector<string>& vals, float width = defaultWidth, float height = defaultHeight);
    
    virtual bool mouseMoved(ofMouseEventArgs & args);
    virtual bool mousePressed(ofMouseEventArgs & args);
    virtual bool mouseDragged(ofMouseEventArgs & args);
    virtual bool mouseReleased(ofMouseEventArgs & args);
    virtual bool mouseScrolled(ofMouseEventArgs & args);
    
    void setUpdateOnReleaseOnly(bool bUpdateOnReleaseOnly);
    
    
    template<class ListenerClass, typename ListenerMethod>
    void addListener(ListenerClass * listener, ListenerMethod method){
        value.addListener(listener,method);
    }
    
    template<class ListenerClass, typename ListenerMethod>
    void removeListener(ListenerClass * listener, ListenerMethod method){
        value.removeListener(listener,method);
    }
    
    unsigned int operator=(unsigned int v);
    operator const unsigned int & ();
    
    ofAbstractParameter & getParameter();
    
    vector<string>& getList();
    
    
protected:
    virtual void render();
    ofParameter<unsigned int> value;
    bool bUpdateOnReleaseOnly;
    bool bGuiActive;
    bool mouseInside;
    bool setValue(float mx, float my, bool bCheck);
    virtual void generateDraw();
    virtual void generateText();
    void valueChanged(unsigned int & value);
    ofPath bg, bar, listBg;
    vector<string>list;
	
    ofVboMesh textMesh;
    void buildListTextMeshes();
    int generateBar(float my, bool bUseIndex = false);
    ofMesh fullMesh;
    float lineHeight;
    ofRectangle fullRect;
    
    ofVec2f offset;
};