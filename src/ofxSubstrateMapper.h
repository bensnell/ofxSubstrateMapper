#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"

// The Substrate Plan contains geometric information defining the projection surface in a json format
// with extension .plan :
// (1) key "substrate_outline" corresponds to a list of real space XY plane vertices defining the
// projection surface from a top-down view
// (2) key "substrate_heights" corresponds to a list of low and high heights at normalized
// parameters along the outline

struct HeightParam {
	float param;
	glm::vec2 height;
};

// This tool finds the nearest surface coordinates of a 3D point 
// This class only contains one surface
// The surface topology is limited to vertical surfaces and the closest point isn't always
// the closest point for points that lie beyond the boundary of the surface.
class ofxSubstrateMapper {
public:

	ofxSubstrateMapper();
	~ofxSubstrateMapper();

	void setupParams();
	void setup();
    
	// This returns the nearest point on the surface
    //glm::vec3 getNearestPoint(glm::vec3 inPoint);
	// This returns the normalized (U,V) coordinate of the surface
    //glm::vec2 getNearestParam(glm::vec3 inPoint);
	// This returns all of the following
	//void getNearest(glm::vec3 inPoint, glm::vec3& outPoint, glm::vec2& outParam, glm::vec3& outDirection);
	//void getNearest(glm::vec3 inPoint, glm::mat4x4& outFrame, glm::vec2& outParam);
	void getNearest(glm::vec3 inPoint, glm::vec3& outPoint, glm::vec2& outParam);

	void loadSubstratePlan(string _spFilename = "");
	bool isSubstratePlanLoaded();

	void drawDebug(int x = 0, int y = 0);
	void drawDebugImagery(int x, int y, int w, int h);

	ofMesh getSurface();
	ofMesh* getSurfacePtr();

	void renderDebug();

protected:

	bool bLoaded = false;

	string spFilename = "substrate.plan";
	bool bSubstrateLoaded = false;
	ofPolyline outline; // the upper and lower bounds of the substrate // should not contain duplicate vertices
	vector<HeightParam> heights; // all params but be scaled to [0,1]
	
	ofMesh surface;
	ofFbo canvas;

	// Json helper
	ofJson js;
    
	glm::vec3 lastInPoint;
    glm::vec3 lastOutPoint;
    glm::vec2 lastOutParam;

	glm::vec2 getInterpolatedHeight(vector<HeightParam>* heights, float param);

	void getClosestPoint(glm::vec3& inPoint, ofPolyline& inOutline, glm::vec3& outPoint, float& outPercent, float &outIndexInterp);

	int maxRenderingDimension = 1000; // px
};
