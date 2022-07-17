//
//  SerialShapeIOManager.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "SerialShapeIOManager.hpp"

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

    // initialize per-pin data arrays
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            heightsForShapeDisplay[x][y] = 0;
            heightsFromShapeDisplay[x][y] = 0;
            pinDiscrepancy[x][y] = 0;
            pinEnabled[x][y] = true;
            pinStuckSinceTime[x][y] = timeOfLastConfigsRefresh;
        }
    }

    // connect to shape display
    connectToDisplay();
}

// Destructor
SerialShapeIOManager::~SerialShapeIOManager() {
    disconnectFromDisplay();
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
    closeSerialConnections();
}

// Open serial connections to the display. Connections close automatically when
// destroyed.
void SerialShapeIOManager::openSerialConnections() {
    for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i++) {
        serialConnections[i] = new SerialShapeIO(SERIAL_PORTS[i], SERIAL_BAUD_RATE, heightsFromShapeDisplayAvailable);
    }
}

// Close all serial connections to the display
void SerialShapeIOManager::closeSerialConnections() {
    for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i++) {
        delete serialConnections[i];
    }
}

// Print board configuration settings to console for debugging
void SerialShapeIOManager::printBoardConfiguration() {
    for (int i = 0; i < NUM_ARDUINOS; i++) {
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
void SerialShapeIOManager::sendHeightsToShapeDisplay(unsigned char heights[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]) {
    unsigned char *src = (unsigned char *) heights;
    copy(src, src + SHAPE_DISPLAY_SIZE_2D, (unsigned char *) heightsForShapeDisplay);

    // update display
    update();
}

// Get the actual height values on the shape display. They will be copied into
// the destination array passed in as the argument.
void SerialShapeIOManager::getHeightsFromShapeDisplay(unsigned char heights[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]) {
    if (!heightsFromShapeDisplayAvailable) {
        throw ("height data from shape display is not available on " + shapeDisplayName);
    }

    unsigned char *src = (unsigned char *) heightsFromShapeDisplay;
    copy(src, src + SHAPE_DISPLAY_SIZE_2D, (unsigned char *) heights);
}

// Set a single height for the display.
//
// Note: shape display heights will be adjusted to fit within the clipping range
// regardless of the value set.
void SerialShapeIOManager::clearShapeDisplayHeights(int value) {
    for (int i = 0; i < SHAPE_DISPLAY_SIZE_X; i++) {
        for (int j = 0; j < SHAPE_DISPLAY_SIZE_Y; j++) {
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

void SerialShapeIOManager::setPinConfigs(PinConfigs configs[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]) {
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            pinConfigs[x][y] = configs[x][y];
        }
    }
}

void SerialShapeIOManager::setGlobalPinConfigs(PinConfigs configs) {
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            pinConfigs[x][y] = configs;
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

    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
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
    float thresholdScalar = 1.0 * HEIGHT_RANGE / 255;
    for (int i = 0; i < SHAPE_DISPLAY_SIZE_X; i++) {
        for (int j = 0; j < SHAPE_DISPLAY_SIZE_Y; j++) {
            // to rescale the values instead of clipping them, use this line:
            //heightsForShapeDisplay[i][j] = heightsForShapeDisplay[i][j] * thresholdScalar + HEIGHT_MIN;

            if (heightsForShapeDisplay[i][j] <= HEIGHT_MIN) {
                heightsForShapeDisplay[i][j] = (unsigned char) HEIGHT_MIN;
            }
            else if (heightsForShapeDisplay[i][j] >= HEIGHT_MAX) {
                heightsForShapeDisplay[i][j] = (unsigned char) HEIGHT_MAX;
            }
        }
    }
}

// Copy data from storage in the 2D array to the corresponding arduino board
// structures. Flip height values where needed to match the board's orientation.
void SerialShapeIOManager::readyDataForArduinos() {
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int x = pinBoards[i].pinCoordinates[j][0];
            int y = pinBoards[i].pinCoordinates[j][1];
            
            // copy the height value to the board
            pinBoards[i].heights[j] = heightsForShapeDisplay[x][y];

            // if they've been updated, copy the pin configs to the board
            if (pinBoards[i].configs[j].timeOfUpdate < pinConfigs[x][y].timeOfUpdate) {
                pinBoards[i].configs[j] = pinConfigs[x][y];
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
    readyDataForArduinos();

    // send height data. if the display talks back, ask it what it's doing
    if (heightsFromShapeDisplayAvailable) {
        for (int i = 0; i < NUM_ARDUINOS; i++) {
            sendHeightsToBoardAndRequestFeedback(i + 1, pinBoards[i].heights, pinBoards[i].serialConnection);
        }
        readHeightsFromBoards(); // gets actual heights from arduino boards
    } else {
        for (int i = 0; i < NUM_ARDUINOS; i++) {
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
    
    for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i++) {
        serialConnections[i]->writeMessage(messageContents);
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
    for (int i = 0; i < NUM_ARDUINOS; i++) {
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
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        sendConfigsToBoard(i + 1, pinBoards[i].configs, pinBoards[i].serialConnection);
    }
    timeOfLastConfigsUpdate = elapsedTimeInSeconds();
    timeOfLastConfigsRefresh = elapsedTimeInSeconds();
}

// Read actual heights from the boards
void SerialShapeIOManager::readHeightsFromBoards() {
    // receive the current heights on the shape display
    for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i++) {
        while (serialConnections[i]->hasNewMessage()) {
            unsigned char messageContent[MSG_SIZE_RECEIVE];
            serialConnections[i]->readMessage(messageContent);
            if (messageContent[0] == TERM_ID_HEIGHT_RECEIVE) {
                int boardAddress = messageContent[1] - 1;
                if (boardAddress >= 0 && boardAddress <= NUM_ARDUINOS) {
                    for (int j = 0; j < 6; j++) {
                        int height = messageContent[j + 2];
                        if (pinBoards[boardAddress].invertHeight) {
                            height = 255 - height;
                        }
                        
                        if (height >= 0 && height <= 255) {
                            int x = pinBoards[boardAddress].pinCoordinates[j][0];
                            int y = pinBoards[boardAddress].pinCoordinates[j][1];
                            heightsFromShapeDisplay[x][y] = height;
                        }
                    }
                }
            }
        }
    }
}

