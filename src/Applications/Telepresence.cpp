//
//  Telepresence.cpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#include "Telepresence.hpp"

Telepresence::Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager) : 
	Application(theCustomShapeDisplayManager), kinectManager(theKinectManager) {}

void Telepresence::update(float dt) {
	if (!kinectManager->isConnected()) return;

	kinectManager->update();

	ofImage depth;
	kinectManager->getRawDepthPixels(depth.getPixels());

	// uses ofImage for smoother resize
	depth.resize(m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	heightsForShapeDisplay = depth.getPixels();
}

void Telepresence::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	if (!kinectManager->isConnected()) return;

	ofPixels depth;
	kinectManager->getRawDepthPixels(depth);

	ofImage(depth).draw(x, y, width, height);
}
