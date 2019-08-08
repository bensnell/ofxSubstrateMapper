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


	canvas.allocate(100, 100, GL_RGBA);
	canvas.begin();
	ofClear(255, 255, 255, 255);
	canvas.end();

}

// --------------------------------------------------------------
void ofxSubstrateMapper::loadSubstratePlan(string _spFilename) {
	
	if (!_spFilename.empty()) {
		spFilename = _spFilename;
		RUI_PUSH_TO_CLIENT();
	}

	// Validate the file
	ofFile file(ofToDataPath(spFilename));
	if (!file.exists()) {
		ofLogError("ofxSubstrateMapper") << "Substrate Plan file " << spFilename << " does not exist. Exiting";
		return;
	}
	file >> js;
	if (js.find("substrate_outline") == js.end()) {  //!js.contains("substrate_outline")) {
		ofLogError("ofxSubstrateMapper") << "Substrate Plan " << spFilename << " does not contain key substrate_outline. Exiting.";
		return;
	}
	if (js.find("substrate_heights") == js.end()) {  //!js.contains("substrate_heights")) {
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
        if (js["substrate_heights"][i].find("param") == js["substrate_heights"][i].end()) {  //!js["substrate_heights"][i].contains("param")) {
			ofLogError("ofxSubstrateMapper") << "Each entry in substrate_heights in the Substrate Plan file must contain the key \"param\"";
            return;
        }
        if (i != 0 && js["substrate_heights"][i]["param"] < js["substrate_heights"][i-1]["param"]) {
			ofLogError("ofxSubstrateMapper") << "Parameters in substrate_heights in the Substrate Plan file must be listed in increasing order.";
            return;
        }
        if (float(js["substrate_heights"][i]["param"]) < 0.0 || float(js["substrate_heights"][i]["param"]) > 1.0) {
			ofLogError("ofxSubstrateMapper") << "Parameters in substrate_heights in the Substrate Plan file must be between 0 and 1.";
            return;
        }
        
        if (js["substrate_heights"][i].find("bounds") == js["substrate_heights"][i].end()) {  //!js["substrate_heights"][i].contains("bounds")) {
			ofLogError("ofxSubstrateMapper") << "Each entry in substrate_heights in the Substrate Plan file must contain the key \"bounds\"";
            return;
        }
        if (js["substrate_heights"][i]["bounds"].size() != 2) {
            ofLogError("ofxSubstrateMapper") << "Substrate Plan substrate_heights bounds must have two values: a low and a high value.";
            return;
        }
    }

	// Load the file's contents
	ofPolyline _outline;
	vector<HeightParam> _heights;
	// Load the line
	for (int i = 0; i < js["substrate_outline"].size(); i++) {
		_outline.addVertex(js["substrate_outline"][i][0], js["substrate_outline"][i][1]);
	}
	// Load the heights
	for (int i = 0; i < js["substrate_heights"].size(); i++) {
		HeightParam p;
		p.param = js["substrate_heights"][i]["param"];
		p.height = glm::vec2(js["substrate_heights"][i]["bounds"][0], js["substrate_heights"][i]["bounds"][1]);
		_heights.push_back(p);
	}

	ofLogNotice("ofxSubstrateMapper") << "Successfully loaded Substrate Plan file \"" << spFilename << "\"";

	// Save the values
	outline = _outline;
	heights = _heights;

	// Generate the mesh
	surface.clear();
	surface.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLE_STRIP);
	// Create a temporary line to create the mesh
	ofPolyline tmp = outline;
	// Insert vertices into the outline for all params in the heights
	for (int i = 0; i < heights.size(); i++) {
		float indexInterp = tmp.getIndexAtPercent(heights[i].param);
		if (abs(float(round(indexInterp)) - indexInterp) > 0.001) {
			// Add a new vertex
			tmp.insertVertex(tmp.getPointAtIndexInterpolated(indexInterp), int(ceil(indexInterp)));
		}
	}
	// For every point in the outline, add triangles to the mesh
	//	1__3
	//	|\ | . . .
	//	|_\|
	//  0  2
	//surface.enableColors();
	surface.enableNormals();
	surface.enableTextures();
	for (int i = 0; i < tmp.size(); i++) {
		
		float percent = tmp.getLengthAtIndex(i) / tmp.getPerimeter();
		glm::vec2 ht = getInterpolatedHeight(&heights, percent);

		// Add the bottom height
		glm::vec3 lo = tmp.getVertices()[i];
		lo.z = ht[0];
		surface.addVertex(lo);
		//surface.addColor(ofColor(255));
		surface.addTexCoord(glm::vec2(percent, 0));

		// Add the top height
		glm::vec3 hi = tmp.getVertices()[i];
		hi.z = ht[1];
		surface.addVertex(hi);
		//surface.addColor(ofColor(255));
		surface.addTexCoord(glm::vec2(percent, 1));
	}
	// Now add normals
	for (int i = 0; i < surface.getVertices().size(); i+=2) {

		// Lower normal
		int lastLo = CLAMP(i - 2, 0, surface.getVertices().size() - 2);
		int thisLo = i;
		int nextLo = CLAMP(i + 2, 0, surface.getVertices().size() - 2);

		// Upper normal
		int lastHi = lastLo + 1;
		int thisHi = thisLo + 1;
		int nextHi = nextLo + 1;

		surface.addNormal(glm::normalize(glm::cross(
			surface.getVertex(nextLo) - surface.getVertex(lastLo), 
			surface.getVertex(thisHi) - surface.getVertex(thisLo))));
		surface.addNormal(glm::normalize(glm::cross(
			surface.getVertex(lastHi) - surface.getVertex(nextHi),
			surface.getVertex(thisLo) - surface.getVertex(thisHi))));
	}


	//surface = ofSpherePrimitive(1, 12).getMesh(); // debug
	//surface = ofBoxPrimitive(0.5, 0.5, 0.5).getMesh();

	// Allocate an fbo of the correct size
	if (heights.size() != 0) {
		float rangeWidth = outline.getPerimeter();
		float minH = FLT_MAX;
		float maxH = FLT_MIN;
		for (int i = 0; i < heights.size(); i++) {
			if (heights[i].height[0] < minH) minH = heights[i].height[0];
			if (heights[i].height[0] > maxH) maxH = heights[i].height[0];
			if (heights[i].height[1] < minH) minH = heights[i].height[1];
			if (heights[i].height[1] > maxH) maxH = heights[i].height[1];
		}
		float rangeHeight = maxH - minH;
		int pxWidth, pxHeight;
		if (rangeWidth > rangeHeight) {
			pxWidth = maxRenderingDimension;
			pxHeight = max(1, int(rangeHeight * float(pxWidth) / rangeWidth));
		}
		else {
			pxHeight = maxRenderingDimension;
			pxWidth = max(1, int(rangeWidth * float(pxHeight) / rangeHeight));
		}
		canvas.allocate(pxWidth, pxHeight, GL_RGBA);
		canvas.begin();
		ofClear(255, 255, 255, 255);
		canvas.end();
	}

	bSubstrateLoaded = true;
}

// --------------------------------------------------------------
void ofxSubstrateMapper::renderDebug() {

	// Render the last target
	canvas.begin();
	
	ofPushMatrix();
	ofEnableAlphaBlending();

	ofSetColor(255, 255, 255, 10);
	ofDrawRectangle(0, 0, canvas.getWidth(), canvas.getHeight());

	ofSetColor(255, 0, 0, 255);
	ofDrawCircle(lastMapping.srfUVClamped[0] * float(canvas.getWidth()), lastMapping.srfUVClamped[1] * float(canvas.getHeight()), maxRenderingDimension/100);
	ofSetColor(255, 255, 255, 255);
	
	ofDisableAlphaBlending();
	ofPopMatrix();

	ofClearAlpha();
	canvas.end();

}

// --------------------------------------------------------------
void ofxSubstrateMapper::drawDebug(int x, int y, ofTexture& _canvas) {
	if (!isSubstratePlanLoaded()) return;

	ofPushMatrix();
	ofPushStyle();

	ofEnableNormalizedTexCoords();
	//ofDisableArbTex();

	canvas.getTexture().bind();
	surface.drawFaces();
	//ofDrawBox(0.5);
	canvas.getTexture().unbind();

	//ofSetColor(255, 0, 0);
	//ofDrawSphere(lastSurfacePoint, 10);

	ofDisableNormalizedTexCoords();

	ofPopStyle();
	ofPopMatrix();
}

// --------------------------------------------------------------
void ofxSubstrateMapper::drawDebug(int x, int y) {
	drawDebug(x, y, canvas.getTexture());
}

// --------------------------------------------------------------
void ofxSubstrateMapper::drawDebugImagery(int x, int y, int w, int h) {
	
	//canvas.draw(0, 0, w, h);

	canvas.getTexture().bind();
	canvas.getTexture().draw(x, y, w, h);
	canvas.getTexture().unbind();
}

// --------------------------------------------------------------
bool ofxSubstrateMapper::isSubstratePlanLoaded() {
	return bSubstrateLoaded;
}

// --------------------------------------------------------------
glm::vec2 ofxSubstrateMapper::getInterpolatedHeight(vector<HeightParam>* heights, float param) {
	if (heights->size() == 0) return glm::vec2(0,0);
	if (heights->size() == 1) return (*heights)[0].height;
	param = CLAMP(param, 0.0, 1.0);

	// Find the lo and hi index bounds
	int lo = 0;
	int hi = 1;
	while (hi < (heights->size()-1) && param > (*heights)[hi].param) {
		lo++;
		hi++;
	}

	// Map the param to the heights
	glm::vec2 outHeight;
	outHeight[0] = ofMap(param, (*heights)[lo].param, (*heights)[hi].param, (*heights)[lo].height[0], (*heights)[hi].height[0], true);
	outHeight[1] = ofMap(param, (*heights)[lo].param, (*heights)[hi].param, (*heights)[lo].height[1], (*heights)[hi].height[1], true);
	return outHeight;
}

// --------------------------------------------------------------
MappingResult ofxSubstrateMapper::getNearest(glm::vec3 inPoint) {
	MappingResult m;
	if (!isSubstratePlanLoaded()) return m;
	m.bValid = true;
	m.refPoint = inPoint;
	m.timestamp = ofGetElapsedTimeMicros();

	// Find the closest point on the outline
	float percent;
	float indexInterp;
	getClosestPoint(inPoint, outline, m.srfPoint, percent, indexInterp);
	// ^ only the x,y coordinates matter here, since the outline is in 2D
	// Set the U param
	m.srfUVClamped[0] = percent;

	// Now, find the height
	glm::vec2 heightBounds = getInterpolatedHeight(&heights, percent);
	float z = CLAMP(inPoint.z, heightBounds[0], heightBounds[1]);
	// Set the height
	m.srfPoint.z = z;
	// Set the V param
	m.srfUVClamped[1] = ofMap(inPoint.z, heightBounds[0], heightBounds[1], 0.0, 1.0, true);
	
	// Set the distance away, where negative indicates behind the surface
	// First find the tangent
	int loIndex = floor(indexInterp);
	int hiIndex = CLAMP(loIndex + 1, 0, outline.size()-1);
	if (loIndex = hiIndex) loIndex = hiIndex - 1;
	glm::vec3 positiveDirection = glm::normalize(glm::cross(outline.getVertices()[hiIndex] - outline.getVertices()[loIndex], glm::vec3(0, 0, 1))); // assumes +z up vector
	m.distance = glm::distance(inPoint, m.srfPoint);
	if (glm::dot(positiveDirection, inPoint - m.srfPoint) < 0) m.distance = -m.distance;

	lastMapping = m;
	return m;
}

// --------------------------------------------------------------
void ofxSubstrateMapper::getClosestPoint(glm::vec3& inPoint, ofPolyline& inOutline, glm::vec3& outPoint, float& outPercent, float& outIndexInterp) {
	if (inOutline.size() <= 1) return;

	// Find the closest vertex
	unsigned int* nearestIndex = new unsigned int;
	outPoint = inOutline.getClosestPoint(inPoint, nearestIndex);

	// Determine the intepolated index
	int loIndex = max((int)(*nearestIndex) - 1, 0);
	int hiIndex = min((int)(*nearestIndex) + 1, int(inOutline.size() - 1));
	if (hiIndex - loIndex > 1) {
		
		// Determine which side of the nearest index is closest
		
		// find test length
		float lengthAtNearestIndex = inOutline.getLengthAtIndex(*nearestIndex);
		float loTestLength = lengthAtNearestIndex - inOutline.getPerimeter() / 100.0; // 1000?
		float hiTestLength = lengthAtNearestIndex + inOutline.getPerimeter() / 100.0;

		// find the distance to the points at these test lengths
		float loDist = glm::distance(outPoint, inOutline.getPointAtLength(loTestLength));
		float hiDist = glm::distance(outPoint, inOutline.getPointAtLength(hiTestLength));

		if (loDist <= hiDist) {
			// loIndex is correct
			hiIndex = *nearestIndex;
		}
		else {
			loIndex = *nearestIndex;
			// hiIndex is correct
		}
	}
	// Interpolate the closestPoint between the vertices at each of these indices
	float distLoToClosest = glm::distance(inOutline.getVertices()[loIndex], outPoint);
	float distLoToHi = glm::distance(inOutline.getVertices()[loIndex], inOutline.getVertices()[hiIndex]);
	float param = distLoToHi < 0.000001 ? 0.0 : CLAMP(distLoToClosest / distLoToHi, 0.0, 1.0);
	// Calculate the interpolated index
	outIndexInterp = float(loIndex) + param;

	// Calculate the overall percent (param along the whole line)
	outPercent = inOutline.getLengthAtIndexInterpolated(outIndexInterp) / inOutline.getPerimeter();
}

// --------------------------------------------------------------

ofMesh ofxSubstrateMapper::getSurface() {
	return surface;
}

// --------------------------------------------------------------
ofMesh* ofxSubstrateMapper::getSurfacePtr() {
	return &surface;
}

// --------------------------------------------------------------
string ofxSubstrateMapper::getSubstratePlanName() {
	return spFilename;
}

// --------------------------------------------------------------


// --------------------------------------------------------------


// --------------------------------------------------------------


// --------------------------------------------------------------


// --------------------------------------------------------------
