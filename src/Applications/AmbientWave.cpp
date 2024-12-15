//
//  AmbientWave.cpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#include "AmbientWave.hpp"

double AmbientWave::wave(double x, double y, double k_x, double k_y, double phase) {
	double omega = timescale * std::sqrt(std::sqrt(k_x * k_x + k_y * k_y));
	return 0.5 + 0.5 * std::sin(k_x * x + k_y * y + omega * currentTime + phase);
}

void AmbientWave::update(float dt) {
	currentTime += dt;	

	for (int x = 0; x < heightsForShapeDisplay.getWidth(); x++) {
		for (int y = 0; y < heightsForShapeDisplay.getHeight(); y++) {
			int index = heightsForShapeDisplay.getPixelIndex(x, y);

			heightsForShapeDisplay[index] = 180.0 * wave(x, y, 0.1, 0.04, 0);
		}
	}
}
