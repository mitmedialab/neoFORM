//
//  AmbientWave.cpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#include "AmbientWave.hpp"

void AmbientWave::update(float dt) {
	currentTime += dt;	

	for (int x = 0; x < heightsForShapeDisplay.getWidth(); x++) {
		for (int y = 0; y < heightsForShapeDisplay.getHeight(); y++) {
			int index = heightsForShapeDisplay.getPixelIndex(x, y);

			heightsForShapeDisplay[index] = 255.0 * (1.0 + std::sin(0.1*x + 0.04*y + 15.0*currentTime))/2.0;
		}
	}
}
