//
//  DrawingApp.cpp
//  neoForm
//
//  Created by Charles Reischer on 1/21/2025
//

#include "DrawingApp.hpp"
#include "Application.hpp"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "utils.hpp"

DrawingApp::DrawingApp(SerialShapeIOManager *theCustomShapeDisplayManager) 
	: Application(theCustomShapeDisplayManager) {}

void DrawingApp::update(float dt) {
	if (ofGetMousePressed(OF_MOUSE_BUTTON_LEFT)) leftMousePressed();

	// update ages
	for (int i = 0; i < drawnPoints.size(); i++) {
		drawnPoints[i].age += dt;
	}

	// remove old points
	while (drawnPoints.size() > 0 && drawnPoints[0].age >= totalPointLifespan) {
		drawnPoints.pop_front();
		//cout << "popped!" << endl;
	}

	framesSinceLastMousePress += 1;

	updateHeights();
}

void DrawingApp::updateHeights() {
	heightsForShapeDisplay.set(0);

	for (int i = 0; i < drawnPoints.size(); i++) {
		DrawingPoint point = drawnPoints[i];

		int index = heightsForShapeDisplay.getPixelIndex(point.x, point.y);
		int height;

		if (point.age < fullIntensityLifespan) {
			height = 255;
		} else {
			float fadeRatio = (point.age - fullIntensityLifespan) / (totalPointLifespan - fullIntensityLifespan);
			height = 255 * (1.0 - fadeRatio);
		}

		heightsForShapeDisplay[index] = height;
		// draw connected lines
		if (!point.isStart && i != 0) {
			drawLineOnHeights(point.x, point.y, drawnPoints[i-1].x, drawnPoints[i-1].y, height);
		}
		
	}
}

void DrawingApp::drawLineOnHeights(int x1, int y1, int x2, int y2, int height) {
	int xStart, yStart, xEnd, yEnd;
	// always draw from left to right
	if (x1 <= x2) {
		xStart = x1;
		yStart = y1;
		xEnd = x2;
		yEnd = y2;
	} else {
		xStart = x2;
		yStart = y2;
		xEnd = x1;
		yEnd = y1;
	}

	if (x1 == x2 && y1 == y2) return;

	double yEndOfPixel = yStart + 0.5;
	double slope = (yEnd - yStart) / double(xEnd - xStart);
	int iSlope = slope >= 0 ? 1 : -1;
	yEndOfPixel += 0.5 * slope;
	int y = yStart;
	// draw column by column
	for (int x = xStart; x < xEnd; x++) {
		int index = heightsForShapeDisplay.getPixelIndex(x, y);
		heightsForShapeDisplay[index] = height;

		while (iSlope == 1 ? y < int(yEndOfPixel) : y > int(yEndOfPixel)) {
			y += iSlope;
			int index = heightsForShapeDisplay.getPixelIndex(x, y);
			heightsForShapeDisplay[index] = height;
		}

		yEndOfPixel += slope;
	}
	// last column, only go to middle of pixel height
	yEndOfPixel = yEnd + 0.5;
	int index = heightsForShapeDisplay.getPixelIndex(xEnd, y);
	heightsForShapeDisplay[index] = height;

	while (iSlope == 1 ? y < int(yEndOfPixel) : y > int(yEndOfPixel)) {
		y += iSlope;
		int index = heightsForShapeDisplay.getPixelIndex(xEnd, y);
		heightsForShapeDisplay[index] = height;
	}
}

void DrawingApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	auto heights = ofImage(heightsForShapeDisplay);
	setImageNotBlurry(heights);
	heights.draw(x, y, width, height);
	
	PossibleCoordinate coord = getMouseCoordinateInGrid(mainDisplayX, mainDisplayY, mainDisplayWidth, mainDisplayHeight,
													 	m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);
	if (coord.exists) {
		ofSetColor(ofColor::red);
		
		int pinWidth = m_CustomShapeDisplayManager->shapeDisplaySizeX;
		int pinHeight = m_CustomShapeDisplayManager->shapeDisplaySizeY;
		ofDrawRectangle(x + coord.coordinate.first * width / double(pinWidth), y + coord.coordinate.second * height / double(pinHeight), width / double(pinWidth), height / double(pinHeight));
		ofSetColor(255);
	}
}

void DrawingApp::leftMousePressed() {
	PossibleCoordinate coord = getMouseCoordinateInGrid(mainDisplayX, mainDisplayY, mainDisplayWidth, mainDisplayHeight,
													 	m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);

	if (coord.exists) {
		drawnPoints.push_back({coord.coordinate.first, coord.coordinate.second, 0.0, framesSinceLastMousePress >= 2});
	}

	framesSinceLastMousePress = 0;
}
