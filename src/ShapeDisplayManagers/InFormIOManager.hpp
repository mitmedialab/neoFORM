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
    
    InFormIOManager(KinectManager* kinectRef);
    
    // Name to identify the shape display.
    string getShapeDisplayName() {
        // This is a method instead of a property only to simplify the inheritance by making the superclass declaration virtual.
        return "inFORM";
    }

    // specify pins to disable here, in (x, y)
    vector<PinLocation> getDisabledPins() {
        return {};
    }
    
    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;
    
    ofPixels cropToActiveSurface(ofPixels fullSurface);
    
protected:
    // setup hardware-specific board configuration
    void configureBoards();
};

#endif /* InFormIOManager_hpp */
