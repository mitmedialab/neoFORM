//
//  Telepresence.cpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#include "Telepresence.hpp"

Telepresence::Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager, int nearClip, int farClip) : 
	Application(theCustomShapeDisplayManager), kinectManager(theKinectManager), nearClip(nearClip), farClip(farClip) {
	// lazy way to ensure image is allocated to correct dimensions
	ofShortImage im;
	kinectManager->getRawDepthPixels(im);
	//kinectManager->crop(im);
	refinedImage = im;
}

void Telepresence::update(float dt) {
	if (!kinectManager->isConnected()) return;

	kinectManager->update();

	ofShortImage depth;
	kinectManager->getRawDepthPixels(depth.getPixels());
	//kinectManager->crop(depth);

	int i = 0;
	for (auto pix : depth.getPixels()) {
		if (pix < nearClip || pix > farClip) {
			refinedImage.getPixels()[i] /= 2;
		} else {
			refinedImage.getPixels()[i] = 255.0f * (1.0f - (pix - nearClip) / float(farClip - nearClip));
		}
		i++;
	}
	refinedImage.update();
	
	ofImage out = refinedImage;
	out.resize(m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	heightsForShapeDisplay = out.getPixels();
}

void Telepresence::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	if (!kinectManager->isConnected()) return;

	ofShortImage depth;
	kinectManager->getRawDepthPixels(depth.getPixels());
	depth.update();

	depth.draw(x, y, width, height);
}
