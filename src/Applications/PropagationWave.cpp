//
//  PropagationWave.cpp
//  emptyExample
//
//  Ported by Charles Reischer on 1/13/25.
//  Ported from "TouchShapeObject.cpp," created by Ken Nakagaki on 6/16/15
//

#include "PropagationWave.hpp"
#include "ofGraphicsConstants.h"

PropagationWave::PropagationWave(SerialShapeIOManager *theCustomShapeDisplayManager) 
	: Application(theCustomShapeDisplayManager) {
	setup();
}

void PropagationWave::setup() {
	simWidth = m_CustomShapeDisplayManager->getGridFullWidth();
	simHeight = m_CustomShapeDisplayManager->getGridFullHeight();
	activeWidth = m_CustomShapeDisplayManager->shapeDisplaySizeX;
	activeHeight = m_CustomShapeDisplayManager->shapeDisplaySizeY;

	for (int k = 0; k < numFilterFrames; k++) {
		storedOutputPixels[k].allocate(simWidth, simHeight, OF_IMAGE_GRAYSCALE);
		storedOutputPixels[k].set(210);
	}

	// initiallize to all 0
	differenceHeight = std::vector<std::vector<int>>(activeWidth, std::vector<int>(activeHeight, 0));

	for (int k = 0; k < numWaveFrames; k++) {
		storedInputPixels[k].allocate(activeWidth, activeHeight, OF_IMAGE_GRAYSCALE);
		storedInputPixels[k].set(defaultHeight);
	}

	inputPixels.allocate(activeWidth, activeHeight, OF_IMAGE_GRAYSCALE);
}

void PropagationWave::update(float dt) {
	storedOutputPixels.shiftBack(1);
	storedOutputPixels[0] = outputPixels;
	
	// check if each pin is being told to be still 
	// only checks active pins
	auto outputStill = std::vector<std::vector<bool>>(simWidth, std::vector<bool>(simHeight));
	for (int x = 0; x < activeWidth; x++) {
		for (int y = 0; y < activeHeight; y++) {
			int fullX, fullY;
			std::tie(fullX, fullY) = m_CustomShapeDisplayManager->gridFullCoordinateFromActive({x, y});

			int maxVal = 0;
			int minVal = 255;

			for (int k = 0; k < numFilterFrames; k++) {
				maxVal = std::max(maxVal, (int)storedOutputPixels[k][storedOutputPixels[k].getPixelIndex(fullX, fullY)]);
				minVal = std::min(minVal, (int)storedOutputPixels[k][storedOutputPixels[k].getPixelIndex(fullX, fullY)]);
			}

			outputStill[x][y] = std::abs(maxVal - minVal) <= 2;
		}
	}

	// calculate difference between sent and recieved value
	for (int x = 0; x < activeWidth; x++) {
		for (int y = 0; y < activeHeight; y++) {
			int fullX, fullY;
			std::tie(fullX, fullY) = m_CustomShapeDisplayManager->gridFullCoordinateFromActive({x, y});
			
			int output = outputPixels[outputPixels.getPixelIndex(fullX, fullY)];
			int input = inputPixels[inputPixels.getPixelIndex(x, y)];

			differenceHeight[x][y] = output - input;
		}
	}

	for (int x = 0; x < activeWidth; x++) {
		for (int y = 0; y < activeHeight; y++) {
			isTouched[x][y] = outputStill[x][y] && std::abs(differenceHeight[x][y]) > 21;
		}
	}

	switch (touchMode) {
		case TouchMode::waveSurface:
			break;
		case TouchMode::singleElasticSurface:
			break;
		case TouchMode::triSurface:
			break;
	}
}



void PropagationWave::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {

}


