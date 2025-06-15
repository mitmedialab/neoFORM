//
//  ReliefIOManager.hpp
//  neoForm
//
//  Created by Jonathan Williams on 6/29/24.
//

#ifndef ReliefIOManager_hpp
#define ReliefIOManager_hpp

#include <stdio.h>
#include "ofMain.h"
#include "constants.h"
#include "SerialShapeIOManager.hpp"

#include "PinConfigs.h"

class ReliefIOManager : public SerialShapeIOManager {
public:
    ReliefIOManager();

    ReliefIOManager(KinectManagerSimple* kinectRef);

    // Name to identify the shape display.
    string getShapeDisplayName() {
        // This is a method instead of a property only to simplify the inheritance by making the superclass declaration virtual.
        return "Relief";
    }

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    ofPixels cropToActiveSurface(ofPixels fullSurface);

	// Completely grid-based (physical distance independent) switching between
	// fullSurface (physical) and activeSurface (the pin heights).
	ofPixels gridCropToActiveSurface(const ofPixels& fullSurface) {return fullSurface;}
	void gridApplyToFullSurface(ofPixels& fullSurface, const ofPixels& activeSurface) {fullSurface = activeSurface;}
	int getGridFullWidth() {return 30;}
	int getGridFullHeight() {return 30;}
	std::pair<int, int> gridFullCoordinateFromActive(std::pair<int, int> activeCoordinate) {return activeCoordinate;}
	bool gridFullCoordinateIsActive(std::pair<int, int> fullCoordinate) {return true;}

protected:
    // setup hardware-specific board configuration
    void configureBoards();
};

#endif /* ReliefIOManager_hpp */
