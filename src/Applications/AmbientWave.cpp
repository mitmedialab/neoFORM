//
//  AmbientWave.cpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#include "AmbientWave.hpp"

double AmbientWave::wave(double x, double y, double k_x, double k_y) {
	// follows deep water dispersion relation: omega^2 proportional to k
	double omega = timescale * std::sqrt(std::sqrt(k_x * k_x + k_y * k_y));
	return 0.5 + 0.5 * std::sin(k_x * x + k_y * y + omega * currentTime);
}

void AmbientWave::update(float dt) {
	currentTime += dt;	

	for (int x = 0; x < heightsForShapeDisplay.getWidth(); x++) {
		for (int y = 0; y < heightsForShapeDisplay.getHeight(); y++) {
			int index = heightsForShapeDisplay.getPixelIndex(x, y);

			heightsForShapeDisplay[index] = 180.0 * wave(x, y, 0.1, 0.04);

			//secondary waves
			heightsForShapeDisplay[index] += 20.0 * wave(x, y, -0.2, 0.3);
			heightsForShapeDisplay[index] += 18.0 * wave(x, y, -0.4, 0.1);
			heightsForShapeDisplay[index] += 10.0 * wave(x, y, 0.7, -1.0);
			heightsForShapeDisplay[index] += 8.0 * wave(x, y, 0.3, 1.5);
			heightsForShapeDisplay[index] += 6.0 * wave(x, y, -1.9, 0.9);
			heightsForShapeDisplay[index] += 6.0 * wave(x, y, -1.7, -1.3);
			heightsForShapeDisplay[index] += 6.0 * wave(x, y, 1.4, -0.4);
		}
	}
}
