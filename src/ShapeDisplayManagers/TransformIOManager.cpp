//
//  TransformIOManager.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "TransformIOManager.hpp"

// Create new transformIOManager instance, setting up transFORM-specific board
// configuration
TransformIOManager::TransformIOManager() {
    configureBoards();
    setupTransformedPixelMap();
}

// setup transFORM-specific board configuration
void TransformIOManager::configureBoards() {
    // set up coordinates for
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        // determine which serial connection each board is on:
        // every 3rd and 4th board is on the second
        if (i < 64) {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 0 : 1;
        } else if (i < 128) {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 2 : 3;
        } else {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 4 : 5;
        }
        
        // every 5th to 8th board is mounted upside down, so invert the height
        pinBoards[i].invertHeight = ((i / 4) % 2 == 0) ? false : true;
        
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int currentRow = (int)(i / 4);
            int currentColumn = j + (i % 4 * 6);
            pinBoards[i].heights[j] = 0;
            pinBoards[i].pinCoordinates[j][0] = currentRow;
            pinBoards[i].pinCoordinates[j][1] = currentColumn;
        }
        
        if ((i / 2) % 2 == 0) {
            int pinCoordinateRows[NUM_PINS_ARDUINO];
            
            // invert pin order if the boards are mounted rotated
            for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
                pinCoordinateRows[NUM_PINS_ARDUINO - count - 1] = pinBoards[i].pinCoordinates[count][1];
            }
            for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
                pinBoards[i].pinCoordinates[count][1] = pinCoordinateRows[count];
            }
            
            // also invert the pin height again if they are:
            pinBoards[i].invertHeight = !pinBoards[i].invertHeight;
        }
        
        // last, orient the x-y coordinate axes to the desired external axes
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            unsigned char j0 = pinBoards[i].pinCoordinates[j][0];
            unsigned char j1 = pinBoards[i].pinCoordinates[j][1];
            pinBoards[i].pinCoordinates[j][0] = SHAPE_DISPLAY_SIZE_X - 1 - j0;
            pinBoards[i].pinCoordinates[j][1] = SHAPE_DISPLAY_SIZE_Y - 1 - j1;
        }
    }

    // flag configuration as complete
    boardsAreConfigured = true;
}

// Dead/Alive block calculations: may move to a hardware specific class.
int TransformIOManager::calculateTransformWithinBlockX(int blockNumber, int x_pixel_coord) {
    int blockDead = blockNumber%2;
    
    // Find actual location on TRANSFORM grid from video
    // NOTE TRANSFORM display is 48 pixels wide.
    // NOTE video pixels are 102 wide with 5 pixel buffer on left side. (hence 107 below)
    // right side also has a 5 pixel buffer, but doesn't matter for our caculation
    int TRANSFORM_x = (int)((48.0/107.0)*(float)(x_pixel_coord+5));
    
    if (blockDead){
        //we discard the dead block
        TRANSFORM_x = 0;
    }
    
    return TRANSFORM_x;
}

// Given an x coordinate from the video (102 pixels wide),
// We chop in into 6 segments
// Segments that are used for actuation are 2, 4, 6
// Other segments are not used by TRANSFORM
// so 102 divided into 6 segments, means each segment is 17 pixels wide
// + 5 pixels on each border
// The returned block number is the number of the block where the
// video pixel input to the function ends up.
// blocks 1,3,5, useless
// blocks 2,4,6 used in transform
int TransformIOManager::calculateTransformBlockNumber(int x_pixel_coord){
    //note: int divison returns a truncated result that looks like a floor:
    // 1/2 would return 0, 4/2.5 would return 1
    int blockNum = ((x_pixel_coord+5)/16)+1;
    
    return blockNum;
}

// Runs only once on initialization.
// Fills the m_videoToTransformIndicies array with a map between flattened TRANSFORM pin numbers
// and flattened video pixels.
// m_videoToTransformIndicies(flattened TRANSFORM pixel index) = (flattened video pixel index)
// To assign TRANSFORM pixel video pixels values in the update, do something like
// (pseudocode) ==> TRANSFORM_Pin_Height(pin1) = m_video_toTransformIndicies(pin1);
void TransformIOManager::setupTransformedPixelMap(){
    int counter = 0;
    
    // Iterate over all pixels in the video frame.
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 102; x++) {
            int blockAliveXCoord = calculateTransformWithinBlockX(calculateTransformBlockNumber(x),x);
            
            if (blockAliveXCoord){
                m_videoToTransformIndicies[counter] = 102*y+x;
                counter++;
                cout << "one made it \n";
            }
        }
    }
}


ofPixels TransformIOManager::getPinPixelsOnly(ofPixels fullPixels){
    ofPixels lalalala;
    return lalalala;
}


int* TransformIOManager::getPixelsToShapeDisplayIndicies(){
    return m_videoToTransformIndicies;
}

//DAN AND JONTHAN
ofPixels SerialShapeIOManager::getPinPixelsOnly(ofPixels snobby){
    cout << "get me transformey pixels matey\n";
    return snobby;
}
