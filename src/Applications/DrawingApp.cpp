//
//  DrawingApp.cpp
//  neoForm
//
//  Created by Charles Reischer on 1/21/2025
//

#include "DrawingApp.hpp"
#include "Application.hpp"
#include "ofEvents.h"
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

		if (point.age < fullIntensityLifespan) {
			heightsForShapeDisplay[index] = 255;
		} else {
			float fadeRatio = (point.age - fullIntensityLifespan) / (totalPointLifespan - fullIntensityLifespan);
			int height = 255 * (1.0 - fadeRatio);
			heightsForShapeDisplay[index] = height;
		}
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
