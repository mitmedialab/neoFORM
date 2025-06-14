//
//  SmallWaveApprox.cpp
//  neoFORM
//
//  Created by Charles Reischer on 6/12/25.
//

#ifndef SmallWaveApprox_cpp
#define SmallWaveApprox_cpp

#include "SmallWaveApprox.hpp"
#include "KinectManagerSimple.hpp"
#include "ofGLUtils.h"
#include "utils.hpp"

SmallWaveApprox::SmallWaveApprox(SerialShapeIOManager *shapeDisplayManager, KinectManagerSimple *kinectManager)
: Application(shapeDisplayManager) {
	m_kinectManager = kinectManager;

	int cols = shapeDisplayManager->shapeDisplaySizeX;
	int rows = shapeDisplayManager->shapeDisplaySizeY;

	heightDiffs = std::vector<std::vector<float>>(cols, std::vector<float>(rows, 0));
	xVelocities = std::vector<std::vector<float>>(cols - 1, std::vector<float>(rows, 0));
	yVelocities = std::vector<std::vector<float>>(cols, std::vector<float>(rows - 1, 0));

	kinectIm.allocate(cols, rows, OF_IMAGE_GRAYSCALE);

	// initiallize the sim with something interesting
	const float startScale = 150.0f;
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			heightDiffs[x][y] = startScale * std::sin(0.1 * x + 0.1 * y);
		}
	}

	for (int x = 0; x < cols - 1; x++) {
		for (int y = 0; y < rows; y++) {
			xVelocities[x][y] = 0.001f * std::cos(0.1 * x + 0.1 * y);
		}
	}

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows - 1; y++) {
			yVelocities[x][y] = 0.001f * std::sin(0.1 * x - 0.1 * y);
		}
	}
}

void SmallWaveApprox::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
	kinectIm.draw(x, y, width, height);
}

void SmallWaveApprox::update(float dt) {
	int cols = m_CustomShapeDisplayManager->shapeDisplaySizeX;
	int rows = m_CustomShapeDisplayManager->shapeDisplaySizeY;

	m_kinectManager->update();
	// hand interaction
    ofShortPixels pixels = m_kinectManager->getDepthPixels();
    m_kinectManager->cropUsingMask(pixels);

	prevKinectIm = kinectIm;
	kinectIm = ofPixels(pixels);
	setImageNotBlurry(kinectIm);
	kinectIm.resize(cols, rows);

	int iterations = 3;

	for (int i = 0; i < iterations; i++) {
		// dh/dt + H(du/dx + dv/dy) = 0
		for (int x = 0; x < cols; x++) {
			for (int y = 0; y < rows; y++) {
				float leftXVel  = x ==        0 ? 0 : xVelocities[x-1][y];
				float rightXVel = x == cols - 1 ? 0 : xVelocities[x][y];
				float downYVel  = y ==        0 ? 0 : yVelocities[x][y-1];
				float upYVel    = y == rows - 1 ? 0 : yVelocities[x][y];

				heightDiffs[x][y] += dt / iterations * averageHeight * (leftXVel - rightXVel + downYVel - upYVel);

				// add fake height, to push water away from hand
				heightDiffs[x][y] += (kinectIm.getPixels().getColor(x, y).getBrightness() - prevKinectIm.getPixels().getColor(x, y).getBrightness());
			}
		}

		// du/dt = -g * dh/dx - ku
		for (int x = 0; x < cols - 1; x++) {
			for (int y = 0; y < rows; y++) {
				xVelocities[x][y] -= dt / iterations * (gravConst * (heightDiffs[x+1][y] - heightDiffs[x][y]) + dragConst * xVelocities[x][y]);
			}
		}

		// dv/dt = -g * dh/dy - kv
		for (int x = 0; x < cols; x++) {
			for (int y = 0; y < rows - 1; y++) {
				yVelocities[x][y] -= dt / iterations * (gravConst * (heightDiffs[x][y+1] - heightDiffs[x][y]) + dragConst * yVelocities[x][y]);
			}
		}

		// take away fake height to maintain volume conservation
		for (int x = 0; x < cols; x++) {
			for (int y = 0; y < rows; y++) {
				heightDiffs[x][y] -= (kinectIm.getPixels().getColor(x, y).getBrightness() - prevKinectIm.getPixels().getColor(x, y).getBrightness());
			}
		}
	}

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			heightsForShapeDisplay.setColor(x, y, std::clamp(heightDiffs[x][y] * 1.0f + 127.0f, 0.0f, 255.0f));
		}
	}
}

void SmallWaveApprox::mousePressed(int x, int y, int mouse) {
}


#endif // SmallWaveApprox_cpp

