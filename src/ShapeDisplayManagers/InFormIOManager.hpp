//
//  InFormIOManager.hpp
//  neoForm
//
//  Created by Jonathan Williams on 6/29/24.
//

#ifndef InFormIOManager_hpp
#define InFormIOManager_hpp

#include <stdio.h>
#include "ofMain.h"
#include "constants.h"
#include "SerialShapeIOManager.hpp"

#include "PinConfigs.h"

class InFormIOManager : public SerialShapeIOManager {
public:
    InFormIOManager();

    InFormIOManager(KinectManagerSimple* kinectRef);

    // Destructor to handle power supply deactivation
    ~InFormIOManager();

    // Name to identify the shape display.
    string getShapeDisplayName() {
        // This is a method instead of a property only to simplify the inheritance by making the superclass declaration virtual.
        return "inFORM";
    }

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    ofPixels cropToActiveSurface(ofPixels fullSurface);

	// Completely grid-based (physical distance independent) switching between
	// fullSurface (physical) and activeSurface (the pin heights).
	ofPixels gridCropToActiveSurface(const ofPixels& fullSurface) {return fullSurface;}
	void gridApplyToFullSurface(ofPixels& fullSurface, const ofPixels& activeSurface) {fullSurface = activeSurface;}
	int getGridFullWidth() {return 24;}
	int getGridFullHeight() {return 24;}
	double getPinSizeInInches() {return 0.5;} // TODO: GUESS, SHOULD ACTUALLY MEASURE
	std::pair<int, int> gridFullCoordinateFromActive(std::pair<int, int> activeCoordinate) {return activeCoordinate;}
	bool gridFullCoordinateIsActive(std::pair<int, int> fullCoordinate) {return true;}

protected:
    // setup hardware-specific board configuration
    void configureBoards();

private:
    // Serial connection for power supply control
    ofSerial powerSupplySerial;

    // Power supply control functions
    void activatePowerSupply();
    void deactivatePowerSupply();
};

#endif /* InFormIOManager_hpp */
