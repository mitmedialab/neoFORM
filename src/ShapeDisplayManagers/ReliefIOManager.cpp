//
//  ReliefIOManager.cpp
//  neoForm
//
//  Created by Jonathan Williams on 6/29/24.
//

#include "ReliefIOManager.hpp"
#include "constants.h"


ReliefIOManager::ReliefIOManager() {
    // Set the Relief specific hardware parameters here.
    //shapeDisplayName = "Relief";

	heightsFromShapeDisplayAvailable = false;

    shapeDisplaySizeX = 30;
    shapeDisplaySizeY = 30;

    numberOfArduinos = 150;

    // Size the pinBoards vector appropriately.
    pinBoards.resize(numberOfArduinos);
	pinBoardHeights.resize(numberOfArduinos);
	prevPinBoardHeights.resize(numberOfArduinos);

    // Size the 2d heights array appropriately for the specific shape display hardware, and initialize it with zero values.
    // This needs to happen in the subclass constructor because the superclass constructor fires first, and won't yet have the subclass specific constants.
    heightsForShapeDisplay.resize(shapeDisplaySizeX, std::vector<unsigned char>(shapeDisplaySizeY, 0));
    // Also size the array that receives height values from the shape display.
    heightsFromShapeDisplay.resize(shapeDisplaySizeX, std::vector<unsigned char>(shapeDisplaySizeY, 0));

    pinHeightMin = 30;
    pinHeightMax = 240;
    pinHeightRange = pinHeightMax - pinHeightMin;

    // Pin config values, might be abstracted into a single array.
    gainP    = 1.3;
    gainI    = 0.2;
    maxI     = 60;
    deadZone = 30;
    maxSpeed = 220;

    // Make a new PinConfigs struct instance with the default values.
    PinConfigs defaultPinConfigs;
    defaultPinConfigs.timeOfUpdate = 0;
    defaultPinConfigs.gainP = gainP;
    defaultPinConfigs.gainI = gainI;
    defaultPinConfigs.maxI = maxI;
    defaultPinConfigs.deadZone = deadZone;
    defaultPinConfigs.maxSpeed = maxSpeed;



    // Set the dimensions of the pinConfigs, and set all the elements to the defaultPinConfigs struct.
    pinConfigsForShapeDisplay.resize(shapeDisplaySizeX, std::vector<PinConfigs>(shapeDisplaySizeY, defaultPinConfigs));

    // Initialize pin tracking vectors.
    pinDiscrepancy.resize(shapeDisplaySizeX, std::vector<int>(shapeDisplaySizeY, 0));
    pinEnabled.resize(shapeDisplaySizeX, std::vector<bool>(shapeDisplaySizeY, true));
    pinStuckSinceTime.resize( shapeDisplaySizeX, std::vector<double>(shapeDisplaySizeY, elapsedTimeInSeconds() ));

    // Add serial connection strings to the vector of serial connections.
#ifdef TARGET_LINUX
    serialPorts.push_back("ttyUSB0");
    serialPorts.push_back("ttyUSB1");
    serialPorts.push_back("ttyUSB2");
    serialPorts.push_back("ttyUSB3");
    serialPorts.push_back("ttyUSB4");
#else
    serialPorts.push_back("/dev/tty.usbserial-A4011F1E");
    serialPorts.push_back("/dev/tty.usbserial-A101NU5H");
    serialPorts.push_back("/dev/tty.usbserial-A101NU5G");
    serialPorts.push_back("/dev/tty.usbserial-AL01WNPH");
    serialPorts.push_back("/dev/tty.usbserial-A101NUAR");
#endif

    // Connect to shape display.
    connectToDisplay();

    // fixes strangle delay issue, not ideal
    forceDelayMilliseconds = 15;

    configureBoards();
}

// Secondary Constructor delegates to the primary constructor and adds the kinect reference.
ReliefIOManager::ReliefIOManager(KinectManagerSimple* kinectRef) : ReliefIOManager() {
    m_kinectManagerRef = kinectRef;
}

void ReliefIOManager::configureBoards() {
    // set up coordinates for each board
    for (int i = 0; i < 60; i++) {
        // every 3rd and 4th board is mounted upside down
        pinBoards[i].invertHeight = (((i/2)%2) == 0) ? false : true;
        // determine which serial connection each board is on
        if (i < 28) {
            pinBoards[i].serialConnection = 0;
        } else {
            pinBoards[i].serialConnection = 1;
        }
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int currentRow = (int)(i / 2);
            int currentColumn = j + (i%2 * 6);
            //pinBoards[i].lastFramePinHeight[j] = 0;
            //pinBoards[i].currentFramePinHeight[j] = 0;
            //pinBoards[i].rollingAverageHeight[j] = 0;
            //pinBoards[i].pinIsIdle[j] = true;
            pinBoards[i].pinCoordinates[j][0] = currentRow;
            pinBoards[i].pinCoordinates[j][1] = currentColumn;
        }
    }

    for (int i = 60; i < 150; i++) {
        // every 4th, 5th and 6th board is mounted upside down

        pinBoards[i].invertHeight = ((((i-60)/3)%2) == 0) ? false : true;
        if (i < 90) {
            pinBoards[i].serialConnection = 2;
        } else if (i < 120){
            pinBoards[i].serialConnection = 3;
        } else {
            pinBoards[i].serialConnection = 4;
        }
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int currentRow = (int)((i - 60) / 3);
            int currentColumn = 12 + j + ((i-60)%3 * 6);
            //pinBoards[i].lastFramePinHeight[j] = 0;
            //pinBoards[i].currentFramePinHeight[j] = 0;
            //pinBoards[i].rollingAverageHeight[j] = 0;
            //pinBoards[i].pinIsIdle[j] = true;
            pinBoards[i].pinCoordinates[j][0] = currentRow;
            pinBoards[i].pinCoordinates[j][1] = currentColumn;
        }
    }


    // switch the coordinates for boards that have been set out of order for serial connection 0 and 1:
    int wrongCols[8]= {0,3,4,23,26,27,28,29};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            for (int k = 0; k < 2; k++) {
                unsigned char temp = pinBoards[(wrongCols[i]*2)+1].pinCoordinates[j][k];
                pinBoards[(wrongCols[i]*2)+1].pinCoordinates[j][k] = pinBoards[(wrongCols[i]*2)].pinCoordinates[j][k];
                pinBoards[(wrongCols[i]*2)].pinCoordinates[j][k] = temp;
            }
        }
    }
    // change the row order according to how the Arduinos have been mounted
    int rowOrder[30] = {1,0,2,3,5,4,7,6,9,8,11,10,13,12,14,15,16,17,19,18,21,20,23,22,25,24,27,26,28,29};
    for (int i = 0; i < 15; i++) {
        if (rowOrder[i*2] > rowOrder[(i*2) + 1]) {
            for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
                for (int k = 0; k < 2; k++) {
                    unsigned char temp = pinBoards[(i*4)+ 2].pinCoordinates[j][k];
                    pinBoards[(i*4) + 2].pinCoordinates[j][k] = pinBoards[i*4].pinCoordinates[j][k];
                    pinBoards[i*4].pinCoordinates[j][k] = temp;

                    temp = pinBoards[(i*4)+ 3].pinCoordinates[j][k];
                    pinBoards[(i*4) + 3].pinCoordinates[j][k] = pinBoards[(i*4)+ 1].pinCoordinates[j][k];
                    pinBoards[(i*4)+ 1].pinCoordinates[j][k] = temp;
                }
            }
        }
    }

    // invert pin order if the pins have been mounted inverted for first 60 boards
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            pinBoards[i].pinCoordinates[j][1] = (!pinBoards[i].invertHeight) ? 11 - pinBoards[i].pinCoordinates[j][1] : pinBoards[i].pinCoordinates[j][1];
        }
    }
    // do the same for the remaining
    for (int i = 60; i < 150; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            pinBoards[i].pinCoordinates[j][1] = (!pinBoards[i].invertHeight) ? 12 + 29 - pinBoards[i].pinCoordinates[j][1] : pinBoards[i].pinCoordinates[j][1];
        }
    }

	// rotate 90 degrees counterclockwise (and mirrors)
	for (int i = 0; i < 150; i++) {
		for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
			std::swap(pinBoards[i].pinCoordinates[j][0], pinBoards[i].pinCoordinates[j][1]);
			pinBoards[i].pinCoordinates[j][1] *= -1;
			pinBoards[i].pinCoordinates[j][1] += 29;
		}
	}

    printBoardConfiguration();
    // Flag configuration as complete.
    boardsAreConfigured = true;
}


ofPixels ReliefIOManager::cropToActiveSurface(ofPixels fullSurface) {
    // Inform needs no cropping, but it does need to be resized to pin dimensions of the active surface.
    // NOTE video mode doesn't need resizing, so check to see if the dimensions differ before resizing.
    if (fullSurface.getWidth() != shapeDisplaySizeX || fullSurface.getHeight() != shapeDisplaySizeY) {

        // Originally we just resized directly, but for inForm there may be some quality benefit to converting to ofImage first.
        // We're not doing this for TRANSFORM, because it messes up the escher mode video, so the original scaler is here commented out in case we decide to revert back.
        //fullSurface.resize(shapeDisplaySizeX, shapeDisplaySizeY);

        // This is the ofImage scaler, which may help smoothness? Maybe it makes a difference when working with 16 bit pixels versus 8 bit?
        // Convert to ofImage before resizing to improve quality.
        ofImage fullSurfaceIm = fullSurface;
        // Resize the image.
        fullSurfaceIm.resize(shapeDisplaySizeX, shapeDisplaySizeY);
        // Convert back to ofPixels for consistency.
        fullSurface = fullSurfaceIm.getPixels();
    }

    return fullSurface;
}
