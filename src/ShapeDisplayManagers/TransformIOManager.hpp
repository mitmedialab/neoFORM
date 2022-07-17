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

class TransformIOManager : public SerialShapeIOManager {
public:
    TransformIOManager();

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    // the name of this shape display
    string shapeDisplayName = "transFORM";

private:
    // setup hardware-specific board configuration
    void configureBoards();
};

#endif /* TransformIOManager_hpp */
