//
//  InFormIOManager.cpp
//  neoForm
//
//  Created by Jonathan Williams on 6/29/24.
//

#include "InFormIOManager.hpp"


InFormIOManager::InFormIOManager() {
    // Set the InForm specific hardware parameters here.
    //shapeDisplayName = "InForm";
    
    shapeDisplaySizeX = 24;
    shapeDisplaySizeY = 24;
    
    numberOfArduinos = 96;
 
    // Size the pinBoards vector appropriately.
    pinBoards.resize(numberOfArduinos);
    
    // Size the 2d heights array appropriately for the specific shape display hardware, and initialize it with zero values.
    // This needs to happen in the subclass constructor because the superclass constructor fires first, and won't yet have the subclass specific constants.
    heightsForShapeDisplay.resize(shapeDisplaySizeX, std::vector<unsigned char>(shapeDisplaySizeY, 0));
    // Also size the array that receives height values from the shape display.
    heightsFromShapeDisplay.resize(shapeDisplaySizeX, std::vector<unsigned char>(shapeDisplaySizeY, 0));
    
    pinHeightMin = 50;
    pinHeightMax = 210;
    pinHeightRange = pinHeightMax - pinHeightMin;
    
    // Pin config values, might be abstracted into a single array.
    gainP    = 1.5;
    gainI    = 0.045;
    maxI     = 25;
    deadZone = 2;
    maxSpeed = 200;
    
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
    serialPorts.push_back("/dev/tty.usbserial-A30010PW");
    serialPorts.push_back("/dev/tty.usbserial-A702YLM3");
    serialPorts.push_back("/dev/tty.usbserial-A702YMNY");
    
    // Connect to shape display.
    connectToDisplay();
    
    configureBoards();
}

// Secondary Constructor delegates to the primary constructor and adds the kinect reference.
InFormIOManager::InFormIOManager(KinectManager* kinectRef) : InFormIOManager() {
    m_kinectManagerRef = kinectRef;
}

void InFormIOManager::configureBoards() {
    // set up coordinates for
    for (int i = 0; i < numberOfArduinos; i++) {
        // determine which serial connection each board is on:
        // every 3rd and 4th board is on the second
        if (i < 36) { //64
          pinBoards[i].serialConnection = 0; //((i / 2) % 2 == 0) ? 0 : 1
        } else if (i < 60) { //128
          pinBoards[i].serialConnection = 1; //((i / 2) % 2 == 0) ? 2 : 3
        } else {
          pinBoards[i].serialConnection = 2; //((i / 2) % 2 == 0) ? 4 : 5
        }
        // every 5th to 8th board is mounted upside down, so invert the height
        if (i % 12 == 4 || i % 12 == 5 || i % 12 == 6 || i % 12 == 7) {
          //printf(“%d\n”, i % 12 == 8 || i % 12 == 9 || i % 12 == 10 || i % 12 == 11);
          pinBoards[i].invertHeight = true;
        } else{
          printf("%d\n", i % 12 == 8 || i % 12 == 9 || i % 12 == 10 || i % 12 == 11);
          pinBoards[i].invertHeight = false;
        }
        //pinBoards[i].invertHeight = ((i / 4) % 2 == 0) ? false : true;
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
          int currentRow = (int)(i / 4);
          int currentColumn = 5 - j + (i % 4 * 6);
          pinBoards[i].heights[j] = 0;
          pinBoards[i].pinCoordinates[j][0] = currentRow;
          pinBoards[i].pinCoordinates[j][1] = currentColumn;
        }
        //    if ((i / 2) % 2 == 0) { //
        //      int pinCoordinateRows[NUM_PINS_ARDUINO];
        //
        //       //invert pin order if the boards are mounted rotated
        //      for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
        //        pinCoordinateRows[NUM_PINS_ARDUINO - count - 1] = pinBoards[i].pinCoordinates[count][1];
        //      }
        //      for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
        //        pinBoards[i].pinCoordinates[count][1] = pinCoordinateRows[count];
        //      }
        //
        //      // also invert the pin height again if they are:
        //      pinBoards[i].invertHeight = !pinBoards[i].invertHeight;
        //    }
        // last, orient the x-y coordinate axes to the desired external axes
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
          unsigned char j0 = pinBoards[i].pinCoordinates[j][0];
          unsigned char j1 = pinBoards[i].pinCoordinates[j][1];
          pinBoards[i].pinCoordinates[j][0] = shapeDisplaySizeX - 1 - j0;
          pinBoards[i].pinCoordinates[j][1] = shapeDisplaySizeY - 1 - j1;
        }
      }
    
      printBoardConfiguration();
      // flag configuration as complete
      boardsAreConfigured = true;
}

ofPixels InFormIOManager::cropToActiveSurface(ofPixels fullSurface) {
    // Inform needs no cropping, but it does need to be resized to pin dimensions of the active surface.
    // NOTE video mode doesn't need resizing, so check to see if the dimensions differ before resizing.
    if (fullSurface.getWidth() != shapeDisplaySizeX || fullSurface.getHeight() != shapeDisplaySizeY) {
        fullSurface.resize(shapeDisplaySizeX, shapeDisplaySizeY);
    }

    return fullSurface;
}
