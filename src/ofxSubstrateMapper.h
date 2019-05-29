#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"

// The Substrate Plan contains geometric information defining the projection surface in a json format
// with extension .plan :
// (1) key "substrate_outline" corresponds to a list of real space XY plane vertices defining the
// projection surface from a top-down view
// (2) key "substrate_heights" corresponds to a list of low and high heights at normalized
// parameters along the outline

// This tool transforms virtual raw coordinates to virtual real coordinates
class ofxSubstrateMapper {
public:

	ofxSubstrateMapper();
	~ofxSubstrateMapper();

	void setupParams();
	void setup();

	// Update this tool with the tool center point used for calibration
//    void update();
    
    glm::vec3 getSurfacePoint(glm::vec3 point);
    glm::vec2 getSurfaceParam(glm::vec3 point);

	void loadSubstratePlan(string _spFilename = "");
	bool isSubstratePlanLoaded();

	void drawDebug(int x = 0, int y = 0);


private:

	bool bLoaded = false;

	string spFilename = "substrate.plan";

	// Json helper
	ofJson js;
    
    glm::vec3 lastSurfacePoint;
    glm::vec2 lastSurfaceParam;

};
