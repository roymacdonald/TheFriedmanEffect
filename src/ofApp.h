#pragma once

#include "ofMain.h"
#include "ofxGui.h"



class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	bool loadImage();
	
	void saveScaled();
	void saveFull();
	void reset();
	
	void paramsChanged(float &f);
	
	void keyReleased (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	ofShader shader;
	ofxPanel gui;
	ofParameter<float> stripWidth, stripOffset, scale;
	ofxButton bSaveScaled, bSaveFull, bReset;
	ofxToggle bAutoScale;
	
	string imgPath;
	
	ofImage img;
	
	ofVec2f mousePressedPoint, mouseOffset, mouseOffsetAccum;
	
	bool bDrawGui, bDrawHelp, bSaving;
	
	
	
};

