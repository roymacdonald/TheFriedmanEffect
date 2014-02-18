
#define STRINGIFY(A) #A

#include "ofApp.h"
#include "tileSaver.h"
//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(34, 34, 34);
	ofSetVerticalSync(false);
	ofEnableAlphaBlending();
	
	string fragShader = STRINGIFY(uniform sampler2DRect tex0;
								  uniform float stripWidth;
								  uniform float stripOffset;
								  uniform vec2 mouse;
								  uniform float w;
								  uniform float h;
								  uniform float wi;
								  uniform float hi;
								  uniform float scale;
								  void main(){
									  /*
									   float s = 1.0;
									   if(wi > 0.0){
									   s = w/(stripWidth * ((wi-stripWidth)/stripOffset));
									   }
									   //*/
									  float x = gl_FragCoord.x + mouse.x;
									  float y = h - gl_FragCoord.y +mouse.y;
									  x/=scale;
									  y/=scale;
									  //float x = texCoordVarying.x;
									  //float y = texCoordVarying.y;
									  float indMaxX = floor((wi - stripWidth)/(stripWidth - stripOffset)) +1.0;
									  float indMaxY = floor((hi - stripWidth)/(stripWidth - stripOffset)) +1.0;
									  
									  
									  float indX = floor(x/stripWidth);
									  float indY = floor(y/stripWidth);
									  
									  vec2 coord= vec2( x  - indX*stripOffset,  y - indY*stripOffset);
									  if(coord.x > wi  || coord.y > hi || coord.x < 0.0 || coord.y < 0.0 || indX > indMaxX || indY > indMaxY ){
										  gl_FragColor = vec4 (0.0,0.0,0.0,1.0);
									  }else{
										  gl_FragColor =  texture2DRect(tex0, coord);
									  }
									  
								  });
	string vertShader = STRINGIFY(void main(){
		gl_Position = ftransform();
	});
	shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
    shader.linkProgram();
	
	
	
	gui.setup();
	gui.add(stripWidth.set("Block Width", 100, 1, 300));
	gui.add(stripOffset.set("Block offset", 25, 0, 300));
	gui.add(scale.set("Scale", 1, 0, 2));	
	
	gui.add(bAutoScale.setup("Auto Scale"));
	gui.add(bSaveFull.setup("Save Full Res"));
	gui.add(bSaveScaled.setup("Save Scaled"));
	gui.add(bReset.setup("Reset"));
	
	
	stripWidth.addListener(this, &ofApp::paramsChanged);
	stripOffset.addListener(this, &ofApp::paramsChanged);
	bSaveScaled.addListener(this, &ofApp::saveScaled);
	bSaveFull.addListener(this, &ofApp::saveFull);
	bReset.addListener(this, &ofApp::reset);
	
	
	bDrawGui = bDrawHelp =true;
	ofSetWindowTitle("The Friedman Effect App by Roy J. Macdonald");
	loadImage();
	bSaving = false;
}
//--------------------------------------------------------------
void ofApp::paramsChanged(float &f){
	
	if (stripOffset>=stripWidth) {
		stripOffset = stripWidth -1;
	}
	
}
//--------------------------------------------------------------
bool ofApp::loadImage(){
	ofFileDialogResult r = ofSystemLoadDialog();
	if (r.bSuccess) {
		if (img.loadImage(r.getPath())){
			imgPath = r.getPath();
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
void ofApp::saveScaled(){
	ofFileDialogResult res = ofSystemSaveDialog(ofFilePath::getBaseName(imgPath)+"_processed."+ofFilePath::getFileExt(imgPath), "Save scaled image");
	if (res.bSuccess) {
		bool pDrawGui = bDrawGui;
		bool pDrawHelp = bDrawHelp;
		bDrawGui = bDrawHelp = false;
		draw();
		ofSaveScreen(res.getPath());
		bDrawGui = pDrawGui;
		bDrawHelp = pDrawHelp;
	}
}
//--------------------------------------------------------------
void ofApp::saveFull(){
	ofFileDialogResult res = ofSystemSaveDialog(ofFilePath::getBaseName(imgPath)+"_full."+ofFilePath::getFileExt(imgPath), "Save full res image");
	if (res.bSuccess) {
		
		
		bSaving = true;
		int w = (int)ceil((( (img.width -stripWidth) /(stripWidth - stripOffset)) * stripWidth)/ofGetWidth());
		int h = (int)ceil((((img.height -stripWidth) /(stripWidth - stripOffset)) * stripWidth)/ofGetHeight());
		
		int croppedWidth = (1 + ((img.width -stripWidth) /(stripWidth - stripOffset))) * stripWidth;
		int croppedHeight = (1 + ((img.height -stripWidth) /(stripWidth - stripOffset))) * stripWidth;
		
		ofxTileSaverImage saver;
		saver.init(w, h, ofGetWidth(), ofGetHeight(), croppedWidth, croppedHeight);
		ofImage temp;
		bool bAutoScalePrev = bAutoScale;
		bAutoScale = false;
		ofVec2f prevMouse = mouseOffset;
		
		bool pGui = bDrawGui;
		bool pHelp = bDrawHelp;
		bDrawGui = false;
		bDrawHelp = false;
		mouseOffset.zero();
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				mouseOffset.set(-x*ofGetWidth(), -y*ofGetHeight());
				draw();
				temp.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
				saver.addPixels(x, y, temp.getPixels());
			}
		}
		
		bAutoScale = bAutoScalePrev;
		mouseOffset = prevMouse;
		bDrawGui = pGui;
		bDrawHelp = pHelp;
		
		bSaving = false;		
		saver.done(res.getPath(), false);
		
		
		
		
	}
}
//--------------------------------------------------------------
void ofApp::reset(){
	mouseOffset.zero();
	scale.set(1);
	stripWidth.set(100);
	stripOffset.set(25);
}
//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (img.isAllocated()){
		ofSetColor(225);
		
		float w = ( 1 + (img.width -stripWidth) /(stripWidth - stripOffset)) * stripWidth;
		shader.begin();
		shader.setUniform1f("stripWidth", stripWidth);
		shader.setUniform1f("stripOffset", stripOffset);
		shader.setUniform2f("mouse", -mouseOffset.x, -mouseOffset.y);
		shader.setUniform1f("scale", (!bAutoScale?(float)scale:(ofGetWidth()/w)));
		shader.setUniform1f("w", ofGetWidth());
		shader.setUniform1f("h", ofGetHeight());	
		shader.setUniform1f("wi", img.width);		
		shader.setUniform1f("hi", img.height);				
		img.draw(0, 0, ofGetWidth(), ofGetHeight());
		shader.end();
		
	}
	if (bDrawGui) {
		gui.draw();
	}
	if (bDrawHelp) {
		string str = "Drag your mouse to move image.";
		str += "\nKeyboard shortcuts:";
		str += "\nf : Toggle Fullscreen.";
		str += "\nl : Load Image";
		str += "\ng : Show/Hide GUI";
		str += "\nh : Show/Hide this help";
		
		ofDrawBitmapStringHighlight(str, 300, 30, ofColor::black, ofColor::fromHex(0xec008c));
		
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	if(!bSaving){
		switch(key){
			case 'f':
				ofToggleFullscreen();
				break;
			case 'l': 
				loadImage();
				break;
			case 'g':
				bDrawGui ^= true;
				break;
			case 'h':
				bDrawHelp ^= true;
				break;
			default:
				break;
		}
	}
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (!bSaving) {
		mouseOffset = ofVec2f(x,y) - mousePressedPoint + mouseOffsetAccum;
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!bSaving) {
		mousePressedPoint.set(x, y);
	}
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (!bSaving) {
		mouseOffsetAccum = mouseOffset;
	}
}

