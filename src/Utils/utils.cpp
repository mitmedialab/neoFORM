//
//  utils.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "utils.hpp"
#include <optional>

optional<pair<int, int>> getMouseCoordinateInGrid(int gridX, int gridY, int gridWidth, int gridHeight, int numXCells, int numYCells) {
    // find pixel position of mouse
    int mouseX = (ofGetMouseX() - gridX);
    int mouseY = (ofGetMouseY() - gridY);
    
    // check if mouse is inside grid
    if (mouseX >= 0 && mouseX < gridWidth && mouseY >= 0 && mouseY < gridHeight) {
        int pixelX = mouseX * numXCells / float(gridWidth);
        int pixelY = mouseY * numYCells / float(gridHeight);
        return pair<int, int>{pixelX, pixelY};
    } else {
        return {}; 
    }
}

double elapsedTimeInSeconds() {
    return clock() / (double) CLOCKS_PER_SEC;
}

