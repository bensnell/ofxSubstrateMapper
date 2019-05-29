#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(120);

	// Setup params 
	RUI_SETUP();
    // load params
    mapper.setupParams();
	RUI_LOAD_FROM_XML();

    // setup the mapper
    mapper.setup();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Get the 3D point
    glm::vec3 pt = glm::vec3(0.5, 0.5, 0.5);
    
    // Map this 3D point to the chosen surface
    glm::vec3 ptMapped = mapper.getSurfacePoint(pt);
    glm::vec2 ptParams = mapper.getSurfaceParams(pt);
    
    

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(200);

    mapper.drawDebug();
    
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
