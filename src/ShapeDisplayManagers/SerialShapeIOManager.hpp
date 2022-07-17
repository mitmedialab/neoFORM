//
//  SerialShapeIOManager.hpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#ifndef SerialShapeIOManager_hpp
#define SerialShapeIOManager_hpp

#include <stdio.h>
#include "ofMain.h"
#include "constants.h"
#include "utils.hpp"
#include "PinConfigs.h"
#include "ShapeIOManager.hpp"
#include "SerialShapeIO.hpp"

class SerialPinBoard {
public:
    unsigned char pinCoordinates[NUM_PINS_ARDUINO][2]; // what physical x and y does each pin on the board map to?
    unsigned char heights[NUM_PINS_ARDUINO]; // what height should each pin have?
    PinConfigs configs[NUM_PINS_ARDUINO]; // what configs should each pin have?
    bool invertHeight; // is it mounted upside down? if so, the height is inverted
    double timeOfLastConfigsUpdate = -1; // when were the configs last updated?
    int serialConnection; // what serial connection is it on?
};


class SerialShapeIOManager : public ShapeIOManager {
public:
    SerialShapeIOManager();
    ~SerialShapeIOManager();
    
    // send and receive height values
    void sendHeightsToShapeDisplay(unsigned char heights[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]);
    void getHeightsFromShapeDisplay(unsigned char heights[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]);
    void clearShapeDisplayHeights(int value=0);

    // setters for pin config values
    void setPinConfigs(PinConfigs configs[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]);
    void setGlobalPinConfigs(PinConfigs configs);

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    // the name of this shape display
    string shapeDisplayName = "Serial Shape Display";

protected:
    // manage the connection to the shape display
    void connectToDisplay();
    void disconnectFromDisplay(bool clearHeights=false);
    void openSerialConnections();
    void closeSerialConnections();

    // setup hardware-specific board configuration
    virtual void configureBoards() = 0;
    void printBoardConfiguration();

    // pin height data processors
    void toggleStuckPins();
    void clipAllHeightValuesToBeWithinRange();
    void readyDataForArduinos();

    // central update manager
    void update();

    // send data to the shape display
    void sendValueToAllBoards(unsigned char termId, unsigned char value);
    void sendValuesToBoard(unsigned char termId, unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection);
    void sendHeightsToBoard(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection);
    void sendHeightsToBoardAndRequestFeedback(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection);
    void sendConfigsToBoard(unsigned char boardId, PinConfigs configs[NUM_PINS_ARDUINO], int serialConnection);
    void sendUpdatedConfigValues();
    void sendAllConfigValues();

    // read data received from shape display
    void readHeightsFromBoards();

    // serial communications objects
    SerialShapeIO *serialConnections[NUM_SERIAL_CONNECTIONS];
    SerialPinBoard pinBoards[NUM_ARDUINOS];

    // shape display height values (both intended and actual values)
    unsigned char heightsForShapeDisplay[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];
    unsigned char heightsFromShapeDisplay[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];

    // pin behavior configurations
    PinConfigs pinConfigs[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];

    // initialization flags
    bool boardsAreConfigured = false;
    bool isConnected = false;

    // pin behavior configuration trackers
    double timeOfLastConfigsUpdate;
    double timeOfLastConfigsRefresh;

    // properties for detecting stuck pins to toggle
    int pinDiscrepancy[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];
    bool pinEnabled[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];
    double pinStuckSinceTime[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y];
    const int pinDiscrepancyToggleThreshold = 100;
    const float secondsUntilPinToggledOff = 1.0;
    const float secondsUntilPinToggledOn = 3.0;
};

#endif /* SerialShapeIOManager_hpp */

