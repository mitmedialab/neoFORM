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
