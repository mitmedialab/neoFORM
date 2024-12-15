//
//  TransitionApp.cpp
//  neoForm
//
//  Created by Charles Reischer 12/14/24.
//{

#include "TransitionApp.hpp"
#include "ofGraphics.h"
#include <sstream>

std::string TransitionApp::getName() {
	if (startApp == nullptr || endApp == nullptr) return "Interpolate";
	std::stringstream name = std::stringstream("Interpolate (");
	name << startApp->getName() << " -> " << endApp->getName() << ")"; 
	return name.str();
}

void TransitionApp::startTransition(Application* start, Application* end, float duration, Application** pointerToActiveApplication, bool* pointerToBlock) {
	startApp = start;
	endApp = end;
	transitionDuration = duration;
	transitionProgress = 0.0f;
	appManagerPointer = pointerToActiveApplication;
	appManagerBlock = pointerToBlock;
	std::cout << "transition started" << std::endl;
}

void TransitionApp::update(float dt) {
	if (startApp == nullptr || endApp == nullptr || appManagerPointer == nullptr) {
		std::cout << "breaking (nullptr)" << std::endl;
		return;
	}
	transitionProgress += dt;
	
	if (transitionProgress >= transitionDuration) {
		// manually sets the appManager's active app
		*appManagerPointer = endApp;
		*appManagerBlock = false;
		std::cout << "switched!" << std::endl;
		endApp->update(dt);
		return;
	}

	startApp->update(dt);
	endApp->update(dt);

	// blend heights from startApp and endApp
	startApp->getHeightsForShapeDisplay(heightsForShapeDisplay);
	ofPixels endHeights;
	endApp->getHeightsForShapeDisplay(endHeights);
	
	for (int i = 0; i < heightsForShapeDisplay.size(); i++) {
		float l = transitionProgress/transitionDuration;
		heightsForShapeDisplay[i] = (1.0f - l) * heightsForShapeDisplay[i] + l * endHeights[i];
	}
}

void fade(int x, int y, int width, int height, double alpha) {
	ofSetColor(0, 0, 0, alpha);
	ofDrawRectangle(x, y, width, height);
	ofSetColor(255);
}

void TransitionApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	if (startApp == nullptr || endApp == nullptr) return;
	if (transitionProgress <= transitionDuration/2.0f) {
		startApp->drawGraphicsForShapeDisplay(x, y, width, height);
		fade(x, y, width, height, 255.0f * 2.0f * transitionProgress  / transitionDuration);
	} else {
		endApp->drawGraphicsForShapeDisplay(x, y, width, height);
		fade(x, y, width , height, 255.0 * 2.0f * (transitionDuration - transitionProgress) / transitionDuration);
	}
}

void TransitionApp::drawGraphicsForPublicDisplay(int x, int y, int width, int height) {
	if (startApp == nullptr || endApp == nullptr) return;
	if (transitionProgress <= transitionDuration/2.0f) {
		startApp->drawGraphicsForPublicDisplay(x, y, width, height);
		fade(x, y, width, height, 255.0f * 2.0f * transitionProgress  / transitionDuration);
	} else {
		endApp->drawGraphicsForPublicDisplay(x, y, width, height);
		fade(x, y, width , height, 255.0 * 2.0f * (transitionDuration - transitionProgress) / transitionDuration);
	}
}

void TransitionApp::drawGraphicsForProjector(int x, int y, int width, int height) {
	if (startApp == nullptr || endApp == nullptr) return;
	if (transitionProgress <= transitionDuration/2.0f) {
		startApp->drawGraphicsForProjector(x, y, width, height);
		fade(x, y, width, height, 255.0f * 2.0f * transitionProgress  / transitionDuration);
	} else {
		endApp->drawGraphicsForProjector(x, y, width, height);
		fade(x, y, width , height, 255.0 * 2.0f * (transitionDuration - transitionProgress) / transitionDuration);
	}
}

void TransitionApp::keyPressed(int key) {
	if (startApp == nullptr || endApp == nullptr) return;
	startApp->keyPressed(key);
	endApp->keyPressed(key);
}
