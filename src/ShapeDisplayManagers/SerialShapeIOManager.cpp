//
//  SerialShapeIOManager.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "SerialShapeIOManager.hpp"
#include "constants.h"
#include "utils.hpp"

//--------------------------------------------------------------
//
// Setup and Teardown
//
//--------------------------------------------------------------

// Constructor: Create an I/O manager for a serial-communication shape display.
//
// Note: derived classes should call `configureBoards` and `connectToDisplay` in
// their constructors. These initializations cannot be done from this
// constructor since it runs before the derived class is constructed.
SerialShapeIOManager::SerialShapeIOManager() {
    timeOfLastConfigsUpdate = elapsedTimeInSeconds();
    timeOfLastConfigsRefresh = elapsedTimeInSeconds();

    // stuck pin safety toggling can only be implemented if we have height data
    // from the shape display telling us whether pins are stuck
    enableStuckPinSafetyToggle = enableStuckPinSafetyToggle && heightsFromShapeDisplayAvailable;
}

// This constructor may not be necessary, it doesn't get called.
SerialShapeIOManager::SerialShapeIOManager(KinectManager* kinectRef) {
    timeOfLastConfigsUpdate = elapsedTimeInSeconds();
    timeOfLastConfigsRefresh = elapsedTimeInSeconds();

    // stuck pin safety toggling can only be implemented if we have height data
    // from the shape display telling us whether pins are stuck
    enableStuckPinSafetyToggle = enableStuckPinSafetyToggle && heightsFromShapeDisplayAvailable;

    // connect to shape display
    connectToDisplay();
    
    //DAN ADDED
    m_kinectManagerRef = kinectRef;
}


// Connect to the display
void SerialShapeIOManager::connectToDisplay() {
    if (isConnected) {
        return;
    }

    openSerialConnections();
    isConnected = true;
}

// Disconnect from the display. If clearHeights is passed, shape display heights
// are first cleared to 0.
//
// Note: this should be called when exiting the app.
void SerialShapeIOManager::disconnectFromDisplay(bool clearHeights) {
    if (!isConnected) {
        return;
    }

    if (clearHeights) {
        clearShapeDisplayHeights();
        ofSleepMillis(1000);
    }

    // turn off the display by setting all pin speeds to 0
    sendValueToAllBoards(TERM_ID_MAX_SPEED, (unsigned char) 0);
    ofSleepMillis(1000);

    // close connections
    isConnected = false;
    
    // No longer necessary, the unique_ptrs will automatically deallocate the objects when the vector is cleared or goes out of scope.
    //closeSerialConnections();
}

// Open serial connections to the display. Connections close automatically when
// destroyed.
void SerialShapeIOManager::openSerialConnections() {
    for (const auto& port : serialPorts) {
        serialConnections.push_back(std::make_unique<SerialShapeIO>(port, SERIAL_BAUD_RATE, heightsFromShapeDisplayAvailable));
    }
    // No need for a closeSerialConnections function, as the unique_ptrs will automatically
    // deallocate the objects when the vector is cleared or goes out of scope.
}

// Print board configuration settings to console for debugging
void SerialShapeIOManager::printBoardConfiguration() {
    for (int i = 0; i < numberOfArduinos; i++) {
        printf("board: %d: ", i);
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            printf("%d,%d(%d); ", pinBoards[i].pinCoordinates[j][0], pinBoards[i].pinCoordinates[j][1], pinBoards[i].invertHeight);
        }
        printf("\n");
    }
};


//--------------------------------------------------------------
//
// Height I/O: Public Interface
//
//--------------------------------------------------------------

// Set the desired heights for the shape display
void SerialShapeIOManager::sendHeightsToShapeDisplay( const std::vector<std::vector<unsigned char>>& heights ) {
    heightsForShapeDisplay = heights;

    // update display
    update();
}

// Set a single height for the display.
//
// Note: shape display heights will be adjusted to fit within the clipping range
// regardless of the value set.
void SerialShapeIOManager::clearShapeDisplayHeights(int value) {
    for (int i = 0; i < shapeDisplaySizeX; i++) {
        for (int j = 0; j < shapeDisplaySizeY; j++) {
            heightsForShapeDisplay[i][j] = value;
        }
    }
    update();
}


//--------------------------------------------------------------
//
// Setters for pin behavior configs
//
//--------------------------------------------------------------

// This may be unecessary, it's no different from setGlobalPinConfigs and it never seems to be called.
void SerialShapeIOManager::setPinConfigs(std::vector<std::vector<PinConfigs>>& configs) {
    for (int x = 0; x < shapeDisplaySizeX; x++) {
        for (int y = 0; y < shapeDisplaySizeY; y++) {
            pinConfigsForShapeDisplay[x][y] = configs[x][y];
        }
    }
}

// Set all of the values of pinConfigsForShapeDisplay to the same value, passed as the configs parameter.
// I can't really say why this is necessary, but the boards don't get the right configs without it.
void SerialShapeIOManager::setGlobalPinConfigs(PinConfigs configs) {
    for (int x = 0; x < shapeDisplaySizeX; x++) {
        for (int y = 0; y < shapeDisplaySizeY; y++) {
            pinConfigsForShapeDisplay[x][y] = configs;
        }
    }
}

//--------------------------------------------------------------
//
// Pin Height Data Processors
//
//--------------------------------------------------------------

// If pins appear stuck, periodically turn them off to avoid damaging the motors
void SerialShapeIOManager::toggleStuckPins() {
    if (!heightsFromShapeDisplayAvailable) {
        throw "cannot toggle stuck pins without pin height data from the shape display";
    }

    double currentTime = elapsedTimeInSeconds();

    for (int x = 0; x < shapeDisplaySizeX; x++) {
        for (int y = 0; y < shapeDisplaySizeY; y++) {
            int expectedHeight = heightsForShapeDisplay[x][y];
            int actualHeight = heightsFromShapeDisplay[x][y];
            pinDiscrepancy[x][y] = abs(expectedHeight - actualHeight);

            // disable pin if it's position has been too far off for too long
            if (pinEnabled[x][y]) {
                if (pinDiscrepancy[x][y] < pinDiscrepancyToggleThreshold) {
                    pinStuckSinceTime[x][y] = currentTime;
                } else if (currentTime - pinStuckSinceTime[x][y] > secondsUntilPinToggledOff) {
                    pinEnabled[x][y] = false;
                }
            // reenable pin if it has been off for awhile; else, overwrite value
            } else {
                if (currentTime - pinStuckSinceTime[x][y] > secondsUntilPinToggledOn) {
                    pinEnabled[x][y] = true;
                    pinStuckSinceTime[x][y] = currentTime;
                } else {
                    heightsForShapeDisplay[x][y] = heightsFromShapeDisplay[x][y];
                }
            }
        }
    }
}

// Clip all values to fit within the allowed range
void SerialShapeIOManager::clipAllHeightValuesToBeWithinRange() {
    float thresholdScalar = 1.0 * pinHeightRange / 255;
    for (int i = 0; i < shapeDisplaySizeX; i++) {
        for (int j = 0; j < shapeDisplaySizeY; j++) {
            // to rescale the values instead of clipping them, use this line:
            //heightsForShapeDisplay[i][j] = heightsForShapeDisplay[i][j] * thresholdScalar + pinHeightMin;

            if (heightsForShapeDisplay[i][j] <= pinHeightMin) {
                heightsForShapeDisplay[i][j] = (unsigned char) pinHeightMin;
            }
            else if (heightsForShapeDisplay[i][j] >= pinHeightMax) {
                heightsForShapeDisplay[i][j] = (unsigned char) pinHeightMax;
            }
        }
    }
}

// Limit all values, to reduce the maximum transient power draw
void SerialShapeIOManager::limitPowerDraw() {
    double totalCost = 0;
    for(int i = 0; i < shapeDisplaySizeX; i++) {
        for(int j = 0; j < shapeDisplaySizeY; j++) {
            double pinDiff = heightsForShapeDisplay[i][j] - (float)previousHeightsForShapeDisplay[i][j];
            pinDiff /= pinHeightMax - pinHeightMin; // 1 for min -> max, 0 for h -> h, -1 for max -> min
            totalCost += abs(pinDiff);
        }
    }

    if (totalCost > shapeDisplaySizeX * shapeDisplaySizeY * getMaxPowerLoad()) {
        double scaleRatio = (shapeDisplaySizeX * shapeDisplaySizeY *getMaxPowerLoad())/totalCost;
        for(int i = 0; i < shapeDisplaySizeX; i++) {
            for(int j = 0; j < shapeDisplaySizeY; j++) {
                heightsForShapeDisplay[i][j] = pinHeightMin + (heightsForShapeDisplay[i][j] - pinHeightMin) * scaleRatio;
            }
        }
    }

    previousHeightsForShapeDisplay = heightsForShapeDisplay;
}

// Copy data from storage in the 2D array to the corresponding arduino board
// structures. Flip height values where needed to match the board's orientation.
void SerialShapeIOManager::readyDataForArduinos() {
    // set any disabled pins to 0
    for (PinLocation pinLoc : getDisabledPins()) {
        heightsForShapeDisplay[pinLoc.x][pinLoc.y] = pinHeightMin;
    }

    for (int i = 0; i < numberOfArduinos; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int x = pinBoards[i].pinCoordinates[j][0];
            int y = pinBoards[i].pinCoordinates[j][1];
            
            // copy the height value to the board
            pinBoards[i].heights[j] = heightsForShapeDisplay[x][y];

            // if they've been updated, copy the pin configs to the board
            if (pinBoards[i].configs[j].timeOfUpdate < pinConfigsForShapeDisplay[x][y].timeOfUpdate) {
                pinBoards[i].configs[j] = pinConfigsForShapeDisplay[x][y];
                pinBoards[i].timeOfLastConfigsUpdate = elapsedTimeInSeconds();
            }

            // invert the values if needed (this affects boards mounted upside down)
            if (pinBoards[i].invertHeight) {
                pinBoards[i].heights[j] = 255 - pinBoards[i].heights[j];
            }
        }
    }
}


//--------------------------------------------------------------
//
// Update Manager
//
//--------------------------------------------------------------

// Central management: run data preprocessors, then send data to the display
void SerialShapeIOManager::update() {
    if (!isConnected) {
        throw "must connect to shape display before updating it";
    }
    if (!boardsAreConfigured) {
        throw "must configure boards before updating shape display";
    }

    // for safest machine operation, periodically disable stuck pins
    if (enableStuckPinSafetyToggle && heightsFromShapeDisplayAvailable) {
        toggleStuckPins();
    }

    // prepare height data for sending to shape display
    clipAllHeightValuesToBeWithinRange();
    limitPowerDraw();
    readyDataForArduinos();

    // send height data. if the display talks back, ask it what it's doing
    if (heightsFromShapeDisplayAvailable) {
        for (int i = 0; i < numberOfArduinos; i++) {
            sendHeightsToBoardAndRequestFeedback(i + 1, pinBoards[i].heights, pinBoards[i].serialConnection);
        }
        readHeightsFromBoards(); // gets actual heights from arduino boards
    } else {
        for (int i = 0; i < numberOfArduinos; i++) {
            sendHeightsToBoard(i + 1, pinBoards[i].heights, pinBoards[i].serialConnection);
        }
    }

    // when config values have changed, resend them. periodically resend them
    // even when they haven't changed to correct any errors that cropped up
    if (elapsedTimeInSeconds() > timeOfLastConfigsRefresh + PIN_CONFIGS_RESET_FREQUENCY) {
        sendAllConfigValues();
    } else {
        sendUpdatedConfigValues();
    }
}


//--------------------------------------------------------------
//
// Serial Communications with Display
//
//--------------------------------------------------------------

// Send a value to all boards (given a term id)
void SerialShapeIOManager::sendValueToAllBoards(unsigned char termId, unsigned char value) {
    if (!(boardsAreConfigured && isConnected)) {
        throw "must initialize shape IO manager before calling `sendValueToAllBoards`";
    }

    unsigned char messageContents[8];
    messageContents[0] = (unsigned char) termId;
    messageContents[1] = (unsigned char) 0;
    for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
        messageContents[i + 2] = (unsigned char) value;
    }
    
    // Iterate through all serial connections and send the message to each one.
    for (auto& connection : serialConnections) {
        connection->writeMessage(messageContents);
    }
}

// Send values for some parameter (given by termId) to a board
void SerialShapeIOManager::sendValuesToBoard(unsigned char termId, unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection) {
    if (termId == TERM_ID_HEIGHT_SEND_AND_RECEIVE) {
        // height feedback communication requires 10 byte messages
        throw "'height feedback' serial communication can't be sent through `sendValuesToBoard`";
    }

    if (!(boardsAreConfigured && isConnected)) {
        throw "must initialize shape IO manager before calling `sendValuesToBoard`";
    }

    unsigned char messageContents[8];
    messageContents[0] = termId;
    messageContents[1] = boardId;
    for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
        messageContents[i + 2] = value[i];
    }

    serialConnections[serialConnection]->writeMessage(messageContents);
}

// Send height data to a board without requesting feedback. Uses 8 byte message.
void SerialShapeIOManager::sendHeightsToBoard(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection) {
    sendValuesToBoard(TERM_ID_HEIGHT_SEND, boardId, value, serialConnection);
}

// Send height data to a board and request feedback. Uses 10 byte message.
void SerialShapeIOManager::sendHeightsToBoardAndRequestFeedback(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection) {
    if (!(boardsAreConfigured && isConnected)) {
        throw "must initialize shape IO manager before calling `sendHeightsToBoardAndRequestFeedback`";
    }

    unsigned char messageContents[10];
    messageContents[0] = TERM_ID_HEIGHT_SEND_AND_RECEIVE;
    messageContents[1] = boardId;
    for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
        messageContents[i + 2] = value[i];
    }
    // extra bytes!
    messageContents[8] = 255;
    messageContents[9] = 255;
    
    serialConnections[serialConnection]->writeMessageRequestFeedback(messageContents);
}

void SerialShapeIOManager::sendConfigsToBoard(unsigned char boardId, PinConfigs configs[NUM_PINS_ARDUINO], int serialConnection) {
    unsigned char gainPValues[NUM_PINS_ARDUINO];
    unsigned char gainIValues[NUM_PINS_ARDUINO];
    unsigned char maxIValues[NUM_PINS_ARDUINO];
    unsigned char deadZoneValues[NUM_PINS_ARDUINO];
    unsigned char maxSpeedValues[NUM_PINS_ARDUINO];

    for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
        gainPValues[i] = (unsigned char) (configs[i].gainP * 25);
        gainIValues[i] = (unsigned char) (configs[i].gainI * 100);
        maxIValues[i] = (unsigned char) configs[i].maxI;
        deadZoneValues[i] = (unsigned char) configs[i].deadZone;
        maxSpeedValues[i] = (unsigned char) (configs[i].maxSpeed / 2);
    }

    sendValuesToBoard(TERM_ID_GAIN_P, boardId, gainPValues, serialConnection);
    sendValuesToBoard(TERM_ID_GAIN_I, boardId, gainIValues, serialConnection);
    sendValuesToBoard(TERM_ID_MAX_I, boardId, maxIValues, serialConnection);
    sendValuesToBoard(TERM_ID_DEAD_ZONE, boardId, deadZoneValues, serialConnection);
    sendValuesToBoard(TERM_ID_MAX_SPEED, boardId, maxSpeedValues, serialConnection);
}

// Send configuration values that have been updated to the display
void SerialShapeIOManager::sendUpdatedConfigValues() {
    for (int i = 0; i < numberOfArduinos; i++) {
        if (timeOfLastConfigsUpdate < pinBoards[i].timeOfLastConfigsUpdate) {
            sendConfigsToBoard(i + 1, pinBoards[i].configs, pinBoards[i].serialConnection);
        }
    }
    timeOfLastConfigsUpdate = elapsedTimeInSeconds();
}

// Send all current configuration values to the display.
//
// This method should be called periodically to reset the display and fix pins
// that appear broken; invalid values can crop up over time from firmware issues
// and connection noise.
void SerialShapeIOManager::sendAllConfigValues() {
    for (int i = 0; i < numberOfArduinos; i++) {
        sendConfigsToBoard(i + 1, pinBoards[i].configs, pinBoards[i].serialConnection);
    }
    timeOfLastConfigsUpdate = elapsedTimeInSeconds();
    timeOfLastConfigsRefresh = elapsedTimeInSeconds();
}

// Read actual heights from the boards
void SerialShapeIOManager::readHeightsFromBoards() {
    // receive the current heights on the shape display
    // Iterate through all serial connections and read messages from each one.
    for (size_t i = 0; i < serialConnections.size(); i++) {
        while (serialConnections[i]->hasNewMessage()) {
            // Make an array of 8 bytes to store the message, and read the message into it.
            unsigned char messageContent[MSG_SIZE_RECEIVE];
            serialConnections[i]->readMessage(messageContent);

            // If the first byte of the message is TERM_ID_HEIGHT_RECEIVE, then the message is a height message.
            if (messageContent[0] == TERM_ID_HEIGHT_RECEIVE) {
                // The second byte of the message is the board address, which is used to determine which board the message is from.
                // Board addresses are 1-indexed, so subtract 1 to adjust it for zero indexing.
                int boardAddress = messageContent[1] - 1;
                // If the board address is valid, then the message is a height message from a board.
                if (boardAddress >= 0 && boardAddress <= numberOfArduinos) {
                    // Iterate through the next 6 bytes of the message, which contain the height values for the 6 pins on the board.
                    for (int j = 0; j < 6; j++) {
                        // The height value is the third byte of the message. 
                        int height = messageContent[j + 2];
                        // If the board inverts height values, then invert the height value.
                        if (pinBoards[boardAddress].invertHeight) {
                            height = 255 - height;
                        }
                        
                        // Check if the height value is within the valid range of 0 to 255.
                        if (height >= 0 && height <= 255) {

                            // Get the x and y coordinates of the pin on the board.
                            int x = pinBoards[boardAddress].pinCoordinates[j][0];
                            int y = pinBoards[boardAddress].pinCoordinates[j][1];

                            // Store the height value in the heightsFromShapeDisplay array.
                            heightsFromShapeDisplay[x][y] = height;
                        }
                    }
                }
            }
        }
    }
}

