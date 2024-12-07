//
//  Telepresence.cpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#include "Telepresence.hpp"
#include "ofVideoGrabber.h"

Telepresence::Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager, 
						   int nearClip, int farClip, int maxOutDist, int bottomOutDist, ofVideoGrabber *cam) : 
		Application(theCustomShapeDisplayManager), kinectManager(theKinectManager), 
		nearClip(nearClip), farClip(farClip), maxOutDist(maxOutDist), bottomOutDist(bottomOutDist), cam(cam) {
	// lazy way to ensure image is allocated to correct dimensions
	//ofShortImage im;
	//kinectManager->getRawDepthPixels(im);
	//kinectManager->crop(im);
	//refinedImage = im;
}

void Telepresence::update(float dt) {
	kinectManager->update();

	ofShortPixels depth = kinectManager->getDepthPixels();
	kinectManager->crop(depth);
	kinectManager->thresholdInterp(depth, maxOutDist, bottomOutDist, 254, 0);

	refinedImage = ofPixels(depth);

	ofImage out = refinedImage;
	out.resize(m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	heightsForShapeDisplay = out.getPixels();
}

void Telepresence::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	refinedImage.draw(x, y, width, height);
}

void Telepresence::drawGraphicsForPublicDisplay(int x, int y, int width, int height) {
	float aspectRatio = cam->getWidth() / cam->getHeight();
	float displayWidth = height * aspectRatio;

	cam->draw(x + (width - displayWidth)/2, y, displayWidth, height);
}
