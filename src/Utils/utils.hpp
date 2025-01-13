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
struct PossibleCoordinate {
    std::pair<int, int> coordinate;
    bool exists;
};

PossibleCoordinate getMouseCoordinateInGrid(int gridX, int gridY, int gridWidth, int gridHeight, int numXCells, int numYCells);

double elapsedTimeInSeconds();
void setImageNotBlurry(ofImage& image);

// simple circular buffer to prevent unnecessary moving of data.
// uses base contstructor/destructor as std::array
template<typename Type, size_t size>
class circularBuffer {
public:
	// Gets element at index, as if shiftBack actually moved data
	Type& operator[](size_t index);

	// Acts like copying each element to the spot shiftAmount after it, with wrapping.
	// e.g., in a 5-element buffer, shifting by 2 brings buf[1] -> buf[3] and buf[4] -> buf[1] 
	void shiftBack(size_t shiftAmount);
protected:
	std::array<Type, size> baseArray;
	size_t offset = 0;
};

// ------------ TEMPLATED IMPLIMENTATIONS (can't be in cpp file) ------------

template<typename Type, size_t size>
Type& circularBuffer<Type, size>::operator[](size_t index) {
	size_t trueIndex = (index + offset) % size;
	return baseArray[trueIndex];
}

template<typename Type, size_t size>
void circularBuffer<Type, size>::shiftBack(size_t amount) {
	// includes some safety, nominially equavilent to "(offset - amount) % size" if wrapping wasn't a problem
	offset = (offset + size - (amount % size)) % size;
}

#endif /* utils_hpp */

