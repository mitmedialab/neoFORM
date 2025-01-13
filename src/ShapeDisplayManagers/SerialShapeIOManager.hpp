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

// includes for the sensor classes, e.g., Kinect
#include "KinectManagerSimple.hpp"

class SerialPinBoard {
public:
    unsigned char pinCoordinates[NUM_PINS_ARDUINO][2]; // what physical x and y does each pin on the board map to?
    unsigned char heights[NUM_PINS_ARDUINO]; // what height should each pin have?
    PinConfigs configs[NUM_PINS_ARDUINO]; // what configs should each pin have?
    bool invertHeight; // is it mounted upside down? if so, the height is inverted
    double timeOfLastConfigsUpdate = -1; // when were the configs last updated?
    int serialConnection; // what serial connection is it on?
};

class SerialShapeIOManager {
public:
    SerialShapeIOManager();
    SerialShapeIOManager(KinectManagerSimple* kinectRef);
    // Destructor
    virtual ~SerialShapeIOManager(){
        disconnectFromDisplay();
    };
    
    // send and receive height values
    void sendHeightsToShapeDisplay(const std::vector<std::vector<unsigned char>>& heights);

    const std::vector<std::vector<unsigned char>>& getHeightsFromShapeDisplay() const {
        return heightsFromShapeDisplay;
    }
    
    const std::vector<std::vector<unsigned char>>& getHeightsForShapeDisplay() const {
        return heightsForShapeDisplay;
    }
    
    void clearShapeDisplayHeights(int value=0);

    // setters for pin config values
    void setPinConfigs(std::vector<std::vector<PinConfigs>>& configs);
    void setGlobalPinConfigs(PinConfigs configs);

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    virtual string getShapeDisplayName() { return "Shape Display Name"; }
    
    // Dan and Jonathan Custom API-like commands
    virtual ofPixels getKinectStream(){return feebsTEMP;}
    
    // Virtual class for hardware specific pin layouts.
    virtual ofPixels cropToActiveSurface(ofPixels fullSurface) = 0;
    
    // Virtual class for hardware specific pin layouts.
    virtual std::vector<ofRectangle> createSections(float pixelsPerInch) {return std::vector<ofRectangle>();}


	// Completely grid-based (physical distance independent) switching between 
	// fullSurface (physical) and activeSurface (the pin heights).
	virtual ofPixels gridCropToActiveSurface(const ofPixels& fullSurface) = 0;
	// modifies fullSurface, leaving portions not in activeSurface untouched
	virtual void gridApplyToFullSurface(ofPixels& fullSurface, const ofPixels& activeSurface) = 0;
	virtual int getGridFullWidth() = 0;
	virtual int getGridFullHeight() = 0;
	// these functions assume input is a valid coordinate in it's respective system
	virtual std::pair<int, int> gridFullCoordinateFromActive(std::pair<int, int> activeCoordinate) = 0;
	virtual bool gridFullCoordinateIsActive(std::pair<int, int> fullCoordinate) = 0;


    // Public getters for protected hardware constants, these are specific to the pin configs so might be abstracted into a single array of values.
    float getGainP()    const { return gainP; }
    float getGainI()    const { return gainI; }
    int   getMaxI()     const { return maxI; }
    int   getDeadZone() const { return deadZone; }
    int   getMaxSpeed() const { return maxSpeed; }
    
    // can heights be read from the display?
    const bool heightsFromShapeDisplayAvailable = SHAPE_DISPLAY_CAN_TALK_BACK;
    
    // Shape display hardware constants, to be initialized by the relevant sub-class.
    int shapeDisplaySizeX;
    int shapeDisplaySizeY;
    
    int numberOfArduinos;
    
    
    // shape display height values (both intended and actual values)
    std::vector<std::vector<unsigned char>> heightsForShapeDisplay;
    std::vector<std::vector<unsigned char>> heightsFromShapeDisplay;

protected:
    // manage the connection to the shape display
    void connectToDisplay();
    void disconnectFromDisplay(bool clearHeights=false);
    void openSerialConnections();

    // setup hardware-specific board configuration
    virtual void configureBoards() = 0;
    void printBoardConfiguration();

    struct PinLocation {
        int x;
        int y;
    };
    // specific to device pin-disabling, return pin location to disable
    virtual vector<PinLocation> getDisabledPins() {return {};}

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
    std::vector<std::unique_ptr<SerialShapeIO>> serialConnections;

    std::vector<SerialPinBoard> pinBoards;

    // pin behavior configurations
    std::vector<std::vector<PinConfigs>> pinConfigsForShapeDisplay;

    // initialization flags
    bool boardsAreConfigured = false;
    bool isConnected = false;

    // pin behavior configuration trackers
    double timeOfLastConfigsUpdate;
    double timeOfLastConfigsRefresh;

    // properties for detecting stuck pins to toggle
    std::vector<std::vector<int>> pinDiscrepancy;
    std::vector<std::vector<bool>> pinEnabled;
    std::vector<std::vector<double>> pinStuckSinceTime;
    
    const int pinDiscrepancyToggleThreshold = 100;
    const float secondsUntilPinToggledOff = 1.0;
    const float secondsUntilPinToggledOn = 3.0;
    
    int geebsTEMP[8];
    ofPixels feebsTEMP;
    
    KinectManagerSimple* m_kinectManagerRef;
    
    // Shape display hardware constants (previously defined using #define preprocessor statements.
    // These values are designed to be overridden by their respective individual shape display sub-classes (Transform, Inform, Cooperform, ets.)
    
    int pinHeightMin;
    int pinHeightMax;
    int pinHeightRange;
    
    // Pin configs, maybe split out into a single array instead of separate values.
    float gainP;
    float gainI;
    int maxI;
    int deadZone;
    int maxSpeed;
    
    // Serial connection id strings
    std::vector<std::string> serialPorts;

};

#endif /* SerialShapeIOManager_hpp */

