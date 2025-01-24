//
//  KinectMaskMaker.cpp
//  emptyExample
//
//  Created by Charles Reischer on 1/15/25.
//

#include "KinectMaskMaker.hpp"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofRectangle.h"

KinectMaskMaker::KinectMaskMaker(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager) 
	: Application(theCustomShapeDisplayManager) {
	
	kinectManager = theKinectManager;
	mouseIsDown = false;
	cornerSelected = UpperLeft;
}

void KinectMaskMaker::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	ofShortPixels depth = kinectManager->getDepthPixels();
	auto mask = kinectManager->mask;
	double xScale = width / (double)depth.getWidth();
	double yScale = height / (double)depth.getHeight();

	// draw full kinect color info
	ofImage(kinectManager->getColorPixels()).draw(x, y, width, height);

	// draw mask bound
	ofNoFill();
	ofSetColor(255, 0, 0);
	ofDrawRectangle(mask.x * xScale + x, mask.y * yScale + y, mask.width * xScale, mask.height * yScale);

	// draw depth information within mask
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 180);
	kinectManager->cropUsingMask(depth);
	ofImage(depth).draw(mask.x * xScale + x, mask.y * yScale + y, mask.width * xScale, mask.height * yScale);

	ofFill();
	ofDisableAlphaBlending();
	ofSetColor(255);
}

bool mouseIsInGraphicsWindow() {
	return (ofGetMouseX() > 400 && ofGetMouseX() <= 1006 && ofGetMouseY() > 356 && ofGetMouseY() <= 962);
}

void KinectMaskMaker::update(float dt) {
	kinectManager->update();

	// position within graphics box
	int mouseX = ofGetMouseX() - 400;
	int mouseY = ofGetMouseY() - 356;

	// only used for size, wasteful
	ofShortPixels depth = kinectManager->getDepthPixels();

	auto mask = kinectManager->mask;
	double xScale = 600 / (double)depth.getWidth();
	double yScale = 600 / (double)depth.getHeight();


	// mouse is pressed for the first time
	if (ofGetMousePressed(OF_MOUSE_BUTTON_LEFT) && !mouseIsDown && mouseIsInGraphicsWindow()) {
		mouseIsDown = true;


		double centerX = (mask.x + mask.width / 2) * xScale;
		double centerY = (mask.y + mask.height / 2) * yScale;

		if (mouseX < centerX && mouseY < centerY) cornerSelected = UpperLeft;
		else if (mouseX < centerX && mouseY >= centerY) cornerSelected = LowerLeft;
		else if (mouseX >= centerX && mouseY < centerY) cornerSelected = UpperRight;
		else cornerSelected = LowerRight;
	}

	// mouse is released after being pressed (or leaves graphic window)
	if ((!mouseIsInGraphicsWindow() || !ofGetMousePressed(OF_MOUSE_BUTTON_LEFT)) && mouseIsDown) mouseIsDown = false;

	// early return if mouse isn't being pressed
	if (!mouseIsDown) return;

	// update corner of mask being moved
	switch (cornerSelected) {
	case UpperLeft:
		kinectManager->mask.width -= int(mouseX / xScale) - kinectManager->mask.x;
		kinectManager->mask.x = int(mouseX / xScale);
		kinectManager->mask.height -= int(mouseY / yScale) - kinectManager->mask.y;
		kinectManager->mask.y = int(mouseY / yScale);
		break;
	case UpperRight:
		kinectManager->mask.width = int((mouseX / xScale) - mask.x);
		kinectManager->mask.height -= int(mouseY / yScale) - kinectManager->mask.y;
		kinectManager->mask.y = int(mouseY / yScale);
		break;
	case LowerLeft:
		kinectManager->mask.width -= int(mouseX / xScale) - kinectManager->mask.x;
		kinectManager->mask.x = int(mouseX / xScale);
		kinectManager->mask.height = int((mouseY / yScale) - mask.y);
		break;
	case LowerRight:
		kinectManager->mask.width = int((mouseX / xScale) - mask.x);
		kinectManager->mask.height = int((mouseY / yScale) - mask.y);
		break;
	}
}

void KinectMaskMaker::keyPressed(int key) {
	if (key == 's') {
		kinectManager->saveMaskAndClip();
	}
}
