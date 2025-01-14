//
//  PropagationWave.cpp
//  emptyExample
//
//  Ported by Charles Reischer on 1/13/25.
//  Ported from "TouchShapeObject.cpp," created by Ken Nakagaki on 6/16/15
//

#include "PropagationWave.hpp"
#include "ofEvents.h"
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
	// don't set values, just vector sizes
	isTouched = std::vector<std::vector<bool>>(activeWidth, std::vector<bool>(activeHeight));

	touchMode = TouchMode::waveSurface;

	for (int k = 0; k < numWaveFrames; k++) {
		storedInputPixels[k].allocate(activeWidth, activeHeight, OF_IMAGE_GRAYSCALE);
		storedInputPixels[k].set(defaultHeight);
	}

	inputPixels.allocate(activeWidth, activeHeight, OF_IMAGE_GRAYSCALE);
	outputPixels.allocate(simWidth, simHeight, OF_IMAGE_GRAYSCALE);

	inputPixels.set(defaultHeight);
	outputPixels.set(defaultHeight);
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
			waveSurface();
			break;
		case TouchMode::singleElasticSurface:
			singleElasticSurface();
			break;
		case TouchMode::triSurface:
			triSurface();
			break;
	}

	heightsForShapeDisplay = m_CustomShapeDisplayManager->gridCropToActiveSurface(outputPixels);
}

void PropagationWave::waveSurface() {
	storedInputPixels.shiftBack(1);
	auto input = m_CustomShapeDisplayManager->getHeightsFromShapeDisplay();
	
	// transfers the vector-style input image into ofPixels
	for (int x = 0; x < activeWidth; x++) {
		for (int y = 0; y < activeHeight; y++) {
			storedInputPixels[0][storedInputPixels[0].getPixelIndex(x, y)] = input[x][y];	
		}
	}

	outputPixels.set(defaultHeight);

	// ------------ added code for debuging purposes -------------
	
	if (m_CustomShapeDisplayManager->getIsConnected()) {
		storedInputPixels[0].set(defaultHeight);
	}
	
	if (ofGetMousePressed(OF_MOUSE_BUTTON_LEFT)) {
		int x = ofGetMouseX();
		int y = ofGetMouseY();

		if (x >= 401 && x < 1006 && y >= 356 && y < 961) {
			int gridX = (x - 401) / 605.0 * activeWidth;
			int gridY = (y - 356) / 605.0 * activeHeight;

			storedInputPixels[0][storedInputPixels[0].getPixelIndex(gridX, gridY)] = 0;
		}
	}

	// -------------------- end added code -----------------------

	// check for any pins below default, and make a spreading circle (larger for older frames)
	for (int k = 0; k < numWaveFrames; k++) {
		for (int x = 0; x < activeWidth; x++) {
			for (int y = 0; y < activeHeight; y++) {
				auto storedVal = storedInputPixels[k][storedInputPixels[k].getPixelIndex(x, y)];
				if (storedVal < defaultHeight) {
					addCircleToOutput(x, y, k, (defaultHeight - storedVal)/2);
				}
			}
		}
	}


	for (int x = 0; x < activeWidth; x++) {
		for (int y = 0; y < activeHeight; y++) {
			if (isTouched[x][y]) {

				int fullX, fullY;
				std::tie(fullX, fullY) = m_CustomShapeDisplayManager->gridFullCoordinateFromActive({x, y});

				outputPixels[outputPixels.getPixelIndex(fullX, fullY)] = defaultHeight;
				for (int k = 0; k < numFilterFrames; k++) {
					storedOutputPixels[k][storedOutputPixels[k].getPixelIndex(fullX, fullY)] = defaultHeight;
				}
			}
		}
	}
}

void PropagationWave::addCircleToOutput(int centerX, int centerY, int radius, int amount) {
	for (int x = std::max(0, centerX - radius); x < std::min(activeWidth, centerX + radius + 1); x++) {
		for (int y = std::max(0, centerY - radius); y < std::min(activeHeight, centerY + radius + 1); y++) {
			int dist = ofDist(centerX, centerY, x, y);

			if (dist == radius) {
				int fullX, fullY;
				std::tie(fullX, fullY) = m_CustomShapeDisplayManager->gridFullCoordinateFromActive({x, y});
				auto &pixel = outputPixels[outputPixels.getPixelIndex(fullX, fullY)];
				pixel = std::min(210, pixel + amount);
			}
		}
	}
}

// not ported yet
void PropagationWave::singleElasticSurface() {}
void PropagationWave::triSurface() {}

void PropagationWave::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {

}


