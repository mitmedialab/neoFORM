//
//  Telepresence.cpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#include "Telepresence.hpp"
#include "ofGraphics.h"
#include "ofVideoGrabber.h"

// Just a big member innitialization list, not doing anything interesting.
// closeCutoff should be larger than farCutoff. Both are between 0 and 65535.
Telepresence::Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager, 
						   int maxOutDist, int bottomOutDist, ofVideoGrabber *cam) : 
		Application(theCustomShapeDisplayManager), kinectManager(theKinectManager), 
		closeCutoff(maxOutDist), farCutoff(bottomOutDist), cam(cam) {}

void Telepresence::update(float dt) {
	kinectManager->update();

	ofShortPixels depth = kinectManager->getDepthPixels();
	kinectManager->cropUsingMask(depth);

	// Singles out the range between farCutoff and closeCutoff for the shape display
	// Shouldn't be needed if nearThreshold and farThreshold are correct
	// kinectManager->thresholdInterp(depth, farCutoff, closeCutoff, 0, 65535);

	// Cast to standard 8-bit representation
	refinedImage = ofPixels(depth);

	// Uses ofImage for antialiased resizing
	ofImage out = refinedImage;
	out.resize(m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	out.mirror(false, true);
	heightsForShapeDisplay = out.getPixels();
}

void Telepresence::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	refinedImage.draw(x, y, width, height);
}

void Telepresence::drawGraphicsForProjector(int x, int y, int width, int height) {
	// gray
	ofSetColor(150);
	ofDrawRectangle(x, y, width, height);
	ofSetColor(255);
}

void Telepresence::drawGraphicsForPublicDisplay(int x, int y, int width, int height) {
	float aspectRatio = cam->getWidth() / cam->getHeight();
	float displayWidth = height * aspectRatio;

	auto pix = cam->getPixels();
	pix.mirror(false, true);
	ofImage(pix).draw(x + (width - displayWidth)/2, y, displayWidth, height);
}
