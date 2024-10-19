//
//  utils.hpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <ctime>
#include "ofMain.h"

// returns "empty" if mouse is outside of grid
optional<pair<int, int>> getMouseCoordinateInGrid(int gridX, int gridY, int gridWidth, int gridHeight, int numXCells, int numYCells);

double elapsedTimeInSeconds();

#endif /* utils_hpp */

