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

double elapsedTimeInSeconds();

typedef unsigned char (*pixelFunction) (unsigned char, unsigned char);
ofImage applyFunctionToPixels(const ofImage& first, const ofImage& second, pixelFunction function);
// weight * second + (1 - weight) * first ----- with [0, 1] bounds on weight
ofImage weightedAverage(const ofImage& first, const ofImage& second, double weight);
// the absolute value of the difference between each pixel
ofImage absoluteDifference(ofImage first, ofImage second);

#endif /* utils_hpp */

