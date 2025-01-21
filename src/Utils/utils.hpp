//
//  utils.hpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#ifndef utils_hpp
#define utils_hpp

#include <cstring>
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

// circular buffer to prevent unnecessary moving of data.
template<typename Type>
class circularBuffer {
public:
	// constructor/destructor
	circularBuffer();
	~circularBuffer();

	// acts just like std::vector
	void push_back(Type val);
	void resize(size_t newSize);
	size_t size();

	// Acts like removing the first element (and returning it) and shifting the rest forward to take it's place
	Type pop_front();

	// Gets element at index, as if shiftBack actually moved data
	Type& operator[](size_t index);

	// Acts like copying each element to the spot shiftAmount after it.
	// The first shiftAmount elements are undefined afterwards (could have random data)
	void shiftBack(size_t shiftAmount);

	// move constructor/assignment, needed for return statements and is simple to implement
	circularBuffer& operator=(circularBuffer&& other);
	circularBuffer(circularBuffer&& other);

	// prevents copying as it is not implemented yet
	circularBuffer& operator=(const circularBuffer& other) = delete;
	circularBuffer(const circularBuffer& other) = delete;
protected:
	Type* baseArray;
	size_t front_pos = 0;
	// m_ is prepended to distinguish fields from methods
	size_t m_size = 0;
	size_t capacity;
};

// ------------ TEMPLATED IMPLIMENTATIONS (can't be in cpp file) ------------

template<typename Type>
circularBuffer<Type>::circularBuffer() {
	baseArray = new Type[1];
	capacity = 1;
}

template<typename Type>
circularBuffer<Type>::~circularBuffer() {
	if (baseArray != nullptr) delete [] baseArray;
}

// simple move constructor
template<typename Type>
circularBuffer<Type>::circularBuffer(circularBuffer&& other) {
	baseArray = other.baseArray;
	capacity = other.capacity;
	m_size = other.m_size;
	front_pos = other.front_pos;
	other.baseArray = nullptr;
}

// simple move assignment
template<typename Type>
circularBuffer<Type>& circularBuffer<Type>::operator=(circularBuffer&& other) {
	baseArray = other.baseArray;
	capacity = other.capacity;
	m_size = other.m_size;
	front_pos = other.front_pos;
	other.baseArray = nullptr;
	return this;
}

// ----- just like std::vector -----
template<typename Type>
void circularBuffer<Type>::push_back(Type element) {
	// resize if needed
	if (m_size == capacity) {
		Type* newArray = new Type[2 * capacity];
		// front section of represented buffer
		memcpy(newArray + (front_pos + capacity) * sizeof(Type), baseArray + front_pos * sizeof(Type), sizeof(Type) * (capacity - front_pos));
		// back section of represented buffer
		memcpy(newArray, baseArray, sizeof(Type) * front_pos);

		front_pos += capacity;
		capacity *= 2;
		delete [] baseArray;
		baseArray = newArray;
	}

	baseArray[(front_pos + m_size) % capacity] = element;
	m_size += 1;
}

template<typename Type>
size_t circularBuffer<Type>::size() {return m_size;}

template<typename Type>
void circularBuffer<Type>::resize(size_t newSize) {
	if (newSize > capacity) {
		size_t newCapacity = capacity * 2;
		while (newSize > newCapacity) {
			newCapacity *= 2;
		}

		Type* newArray = new Type[newCapacity];
		// front section of represented buffer
		memcpy(newArray + (front_pos + newCapacity - capacity) * sizeof(Type), baseArray + front_pos * sizeof(Type), sizeof(Type) * (capacity - front_pos));
		// back section of represented buffer
		memcpy(newArray, baseArray, sizeof(Type) * front_pos);

		front_pos += newCapacity - capacity;
		capacity = newCapacity;
		delete [] baseArray;
		baseArray = newArray;
	}

	m_size = newSize;
}
// -----------------------------

// will break if m_size = 0
template<typename Type>
Type circularBuffer<Type>::pop_front() {
	size_t pos = front_pos;
	front_pos = (front_pos + 1) % capacity;
	m_size -= 1;
	return baseArray[pos];
}

template<typename Type>
Type& circularBuffer<Type>::operator[](size_t index) {
	return baseArray[(index + front_pos) % capacity];
}

template<typename Type>
void circularBuffer<Type>::shiftBack(size_t shiftAmount) {
	front_pos = (front_pos + capacity - shiftAmount) % capacity;
}


#endif /* utils_hpp */

