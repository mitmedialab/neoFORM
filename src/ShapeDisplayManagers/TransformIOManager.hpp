//
//  TransformIOManager.hpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#ifndef TransformIOManager_hpp
#define TransformIOManager_hpp

#include <stdio.h>
#include "ofMain.h"
#include "constants.h"
#include "SerialShapeIOManager.hpp"

#include "PinConfigs.h"

class TransformIOManager : public SerialShapeIOManager {
public:
    TransformIOManager();

    TransformIOManager(KinectManager* kinectRef);

    // the name of this shape display
    string getShapeDisplayName() {
        // This is a method instead of a property only to simplify the inheritance by making the superclass declaration virtual.
        return "TRANSFORM";
    }
    
    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;
    
    ofPixels getKinectStream();

    // ***********************
    // Transform Slicing Dimensions
    // ***********************
    
    //          |-- block --|        |-- block --|        |-- block --|
    //  _______________________________________________________________________   ___
    //  |   z0  |    z1     |   z2   |     z3    |   z4   |    z5     |  z6   |    |
    //  |       |           |        |           |        |           |       |    |
    //  |  dead |  active   |  dead  |   active  |  dead  |  active   | dead  |    H
    //  |       |           |        |           |        |           |       |    |
    //  |_______|___________|________|___________|________|___________|_______|    |
    //          |                    |                    |                       ___
    //          |<-- X start 1       |<-- X start 2       |<-- X start 3
    //
    //  |--------------------------------- W ---------------------------------|
    
    float m_Transform_W = 104.75; //inches
    float m_Transform_H = 26; //inches
    
    // These numbers represent the start points (in inches) of the 3 active zones of the TRANSFORM surface.
    std::vector<float> m_activeZoneXstarts = {10, 43.75, 77};

    std::vector<ofRectangle> createSections( float pixelsPerInch );
    ofPixels cropToActiveSurface(ofPixels fullSurface);

    float m_Transform_block = 15.75; //inches
    int m_Transform_block_h_pins = 24; // # of pins
    int m_Transform_block_w_pins = 16; // # of pins

protected:
    // setup hardware-specific board configuration
    void configureBoards();
    
    ofPixels combineActiveZones(ofPixels fullSurface, std::vector<ofRectangle> sections);
    
};

#endif /* TransformIOManager_hpp */
