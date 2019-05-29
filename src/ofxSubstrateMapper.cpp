#include "ofxSubstrateMapper.h"

// --------------------------------------------------------------
ofxSubstrateMapper::ofxSubstrateMapper() {

}

// --------------------------------------------------------------
ofxSubstrateMapper::~ofxSubstrateMapper() {

}

// --------------------------------------------------------------
void ofxSubstrateMapper::setupParams() {

	RUI_NEW_GROUP("Substrate Mapper");
	RUI_SHARE_PARAM_WCN("SMap: Substrate Plan Filename", spFilename);

}

// --------------------------------------------------------------
void ofxSubstrateMapper::setup() {



}

// --------------------------------------------------------------
void ofxSubstrateMapper::update() {
	
}

// --------------------------------------------------------------
void ofxSubstrateMapper::loadSubstratePlan(string _spFilename) {
	
	if (!_spFilename.empty()) {
		spFilename = _spFilename;
		RUI_PUSH_TO_CLIENT();
	}

	// Validate the file
	ofFile file(ofToDataPath(tpFilename));
	if (!file.exists()) {
		ofLogError("ofxSubstrateMapper") << "Substrate Plan file " << spFilename << " does not exist. Exiting";
		return;
	}
	file >> js;
	if (!js.contains("substrate_outline")) {
		ofLogError("ofxSubstrateMapper") << "Substrate Plan " << spFilename << " does not contain key substrate_outline. Exiting.";
		return;
	}
	if (!js.contains("substrate_heights")) {
		ofLogError("ofxSubstrateMapper") << "Substrate Plan " << spFilename << " does not contain key substrate_heights. Exiting.";
		return;
	}
	if (js["substrate_outline"].size() < 2) {
		ofLogError("ofxSubstrateMapper") << "There must be at least 2 points in the substrate_outline in the Substrate Plan file. Exiting.";
		return;
	}
    if (js["substrate_heights"].size() == 0) {
        ofLogError("ofxSubstrateMapper") << "There must be at least 1 parameter in the substrate_heights in the Substrate Plan file. Exiting.";
        return;
    }
	for (int i = 0; i < js["substrate_outline"].size(); i++) {
		if (js["substrate_outline"][i].size() != 2) {
			ofLogError("ofxSubstrateMapper") << "Substrate Plan points must have (x, y) coordinates in the XY plane. Exiting";
			return;
		}
	}
    for (int i = 0; i < js["substrate_heights"].size(); i++) {
        if (!js["substrate_heights"][i].contains("param")) {
            ofLogError("ofxSubstrateMapper") << "Each entry in substrate_heights in the Substrate Plan file must contain the key \"param\"".
            return;
        }
        if (i != 0 && js["substrate_heights"][i]["param"] < js["substrate_heights"][i-1]["param"]) {
            ofLogError("ofxSubstrateMapper") << "Parameters in substrate_heights in the Substrate Plan file must be listed in increasing order."
            return;
        }
        if (js["substrate_heights"][i]["param"] < 0.0 || js["substrate_heights"][i]["param"] > 1.0) {
            ofLogError("ofxSubstrateMapper") << "Parameters in substrate_heights in the Substrate Plan file must be between 0 and 1."
            return;
        }
        
        if (!js["substrate_heights"][i].contains("bounds")) {
            ofLogError("ofxSubstrateMapper") << "Each entry in substrate_heights in the Substrate Plan file must contain the key \"bounds\"".
            return;
        }
        if (js["substrate_heights"][i]["bounds"].size() != 2) {
            ofLogError("ofxSubstrateMapper") << "Substrate Plan substrate_heights bounds must have two values: a low and a high value.";
            return;
        }
    }

	// Load the file's contents
    
    
//    vector<glm::vec3> _targets;
//    vector<int> _square;
//    // Load the targets
//    for (int i = 0; i < js["calibration_targets"].size(); i++) {
//        _targets.push_back(glm::vec3(
//            js["calibration_targets"][i][0],
//            js["calibration_targets"][i][1],
//            js["calibration_targets"][i][2]));
//    }
//    // Load the square
//    for (int i = 0; i < 3; i++) {
//        int index = js["calibration_square"][i];
//        if (index < 0 || index >= _targets.size()) {
//            ofLogError("ofxSubstrateMapper") << "Target Plan file is invalid. Square index is incorrect. Exiting";
//            return;
//        }
//        _square.push_back(index);
//    }
//
//    ofLogNotice("ofxSubstrateMapper") << "Successfully loaded Target Plan file \"" << tpFilename << "\"";
//
//    // Save these targets if all is successful
//    realTargets = _targets;
//    square = _square;
}

// --------------------------------------------------------------
void ofxSubstrateMapper::drawDebug(int x, int y) {
    if (!isSubstratePlanLoaded()) return;
    
    

	
}

// --------------------------------------------------------------
bool ofxSubstrateMapper::isSubstratePlanLoaded() {


}

// --------------------------------------------------------------


// --------------------------------------------------------------


// --------------------------------------------------------------
