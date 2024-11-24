//
//  Telepresence.cpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#include "Telepresence.hpp"
#include "ofVideoGrabber.h"

Telepresence::Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager, 
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
	if (!kinectManager->isConnected()) return;

	kinectManager->update();

	ofShortImage depth;
	kinectManager->getRawDepthPixels(depth.getPixels());
	//kinectManager->crop(depth);

	ofImage tmpImage = depth;
	int i = 0;
	for (auto pix : depth.getPixels()) {
		if (pix > farClip || pix < nearClip || pix > bottomOutDist) {
			tmpImage.getPixels()[i] = 0.0;
		} else if (pix < maxOutDist) {
			tmpImage.getPixels()[i] = 254.0;
		} else {
			tmpImage.getPixels()[i] = 254.0f * (1.0f - (pix - maxOutDist) / float(bottomOutDist - maxOutDist));
		}
		i++;
	}
	kinectManager->crop(tmpImage);
	refinedImage = tmpImage;
	refinedImage.update();
	
	ofImage out = refinedImage;
	out.resize(m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	heightsForShapeDisplay = out.getPixels();
}

void Telepresence::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	if (!kinectManager->isConnected()) return;

	refinedImage.draw(x, y, width, height);
}

void Telepresence::drawGraphicsForPublicDisplay(int x, int y, int width, int height) {
	float aspectRatio = cam->getWidth() / cam->getHeight();
	float displayWidth = height * aspectRatio;

	cam->draw(x + (width - displayWidth)/2, y, displayWidth, height);
}
