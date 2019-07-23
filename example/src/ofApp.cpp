#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(120);

	// Setup params 
	RUI_SETUP();
    // Load params
    mapper.setupParams();
	RUI_LOAD_FROM_XML();

    // Setup the mapper
    mapper.setup();

	// Load the substrate
	mapper.loadSubstratePlan("substrate.plan");

	cam.setNearClip(0.001);
	cam.setFarClip(10000);
	cam.setPosition(1, 0.3, 0.4);
	cam.lookAt(glm::vec3(0, 0.327, 0.2835), glm::vec3(0, 0, 1));

	light.setup();
	light.setGlobalPosition(glm::vec3(100, -100, 100));
	light.setPointLight();
	light.enable();
	light.setAttenuation(1.25);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Get the 3D point
    glm::vec3 pt = glm::vec3(0, float(ofGetMouseX())/float(ofGetWidth()), 1.0 - float(ofGetMouseY())/float(ofGetHeight()));
    
    // Map this 3D point to the chosen surface
	MappingResult result = mapper.getNearest(pt);
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(255);

	mapper.renderDebug();

	cam.begin();
	ofEnableDepthTest();
	ofEnableAlphaBlending();
	ofEnableLighting();

	light.draw();

	mapper.drawDebug();

	//ofLogNotice() << "Mesh info";
	//ofLogNotice() << "\tverts:  " << mapper.getSurface().getNumVertices();
	//ofLogNotice() << "\tcolors: " << mapper.getSurface().getNumColors();
	//ofLogNotice() << "\tnorms:  " << mapper.getSurface().getNumNormals();
	//ofLogNotice() << "\tindcs:  " << mapper.getSurface().getNumIndices();
	//ofLogNotice() << "\tindcs:  " << mapper.getSurface().getNumTexCoords();

	ofDisableDepthTest();
	ofDisableAlphaBlending();
	ofDisableLighting();
	cam.end();

}

//--------------------------------------------------------------
void ofApp::exit() {


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
