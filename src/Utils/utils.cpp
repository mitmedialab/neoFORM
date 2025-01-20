//
//  utils.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "utils.hpp"
#include <chrono>
#include <optional>

PossibleCoordinate getMouseCoordinateInGrid(int gridX, int gridY, int gridWidth, int gridHeight, int numXCells, int numYCells) {
    // find pixel position of mouse
    int mouseX = (ofGetMouseX() - gridX);
    int mouseY = (ofGetMouseY() - gridY);
    
    // check if mouse is inside grid
    if (mouseX >= 0 && mouseX < gridWidth && mouseY >= 0 && mouseY < gridHeight) {
        int pixelX = mouseX * numXCells / float(gridWidth);
        int pixelY = mouseY * numYCells / float(gridHeight);
        return PossibleCoordinate{std::pair<int, int>{pixelX, pixelY}, true} ;
    } else {
        return PossibleCoordinate{{0,0}, false};
    }
}

double elapsedTimeInSeconds() {
	//std::chrono::steady_clock clock;
	//std::chrono::duration<double, std::ratio<1, 1>> time = clock.now().time_since_epoch();
	//return time.count();

    return clock() / (double) CLOCKS_PER_SEC;
}

//used to easily disable upscale blurring of ofImages
void setImageNotBlurry(ofImage &image) {
    image.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_NEAREST);
}

