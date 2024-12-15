//
//  AmbientWave.cpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#include "AmbientWave.hpp"

const double mainWaveK_x = 0.1;
const double mainWaveK_y = 0.04;

AmbientWave::AmbientWave(SerialShapeIOManager* theCustomShapeDisplayManager): Application(theCustomShapeDisplayManager) {
	std::srand(1);
	for (Boat &boat : boats) {
		boat.x = heightsForShapeDisplay.getWidth() * (std::rand() / double(RAND_MAX));
		boat.y = heightsForShapeDisplay.getHeight() * (std::rand() / double(RAND_MAX));
	}
}

double AmbientWave::wave(double x, double y, double k_x, double k_y) {
	// follows deep water dispersion relation: omega^2 proportional to k
	double omega = timescale * std::sqrt(std::sqrt(k_x * k_x + k_y * k_y));
	return 0.5 + 0.5 * std::sin(k_x * x + k_y * y + omega * currentTime);
}

std::pair<double, double> AmbientWave::gradientOfWave(double x, double y, double k_x, double k_y, double phase) {
	// 2d derivitave
	double omega = timescale * std::sqrt(std::sqrt(k_x * k_x + k_y * k_y));
	double w = std::cos(k_x * x + k_y * y + omega * currentTime + phase);
	return {k_x * w, k_y * w};	
}

void AmbientWave::update(float dt) {
	currentTime += dt;	

	// draw the actual waves
	for (int x = 0; x < heightsForShapeDisplay.getWidth(); x++) {
		for (int y = 0; y < heightsForShapeDisplay.getHeight(); y++) {
			int index = heightsForShapeDisplay.getPixelIndex(x, y);

			heightsForShapeDisplay[index] = 180.0 * wave(x, y, mainWaveK_x, mainWaveK_y);

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

	// draw the boats
	for (Boat boat : boats) {
		double x = boat.x + 40 * gradientOfWave(boat.x, boat.y, mainWaveK_x, mainWaveK_y, 0.7).first;
		double y = boat.y + 40 * gradientOfWave(boat.x, boat.y, mainWaveK_x, mainWaveK_y, 0.7).second;

		if (x < 0 || x >= heightsForShapeDisplay.getWidth() || y < 0 || y >= heightsForShapeDisplay.getHeight()) continue;

		int index = heightsForShapeDisplay.getPixelIndex(x, y);
		heightsForShapeDisplay[index] = std::min(heightsForShapeDisplay[index] + 60, 255);
	}
}
