//
//  Application.hpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include "ofMain.h"
#include "constants.h"
#include "utils.hpp"
#include "PinConfigs.h"
#include "../ShapeDisplayManagers/ShapeIOManager.hpp"
#include "../ShapeDisplayManagers/TransformIOManager.hpp"

class Application {
public:
    Application();
    
    Application(SerialShapeIOManager *theCustomShapeDisplayManager);
    
    void getHeightsForShapeDisplay(ofPixels &heights);
    void getPinConfigsForShapeDisplay(std::vector<std::vector<PinConfigs>>& configs);
    void setHeightsFromShapeDisplayRef(const ofPixels *heights);
    void setPixelsFromKinectRefs(const ofPixels *colorPixels, const ofPixels *depthPixels);

    virtual void update(float dt) {};
    virtual void drawGraphicsForShapeDisplay(int x, int y, int width, int height) {};
    virtual string appInstructionsText() {return "";};
    virtual void drawDebugGui(int x, int y) {};
    virtual void keyPressed(int key) {};

    virtual string getName() {return "Application";};

    // near and far boundary values for depth data captured, specified in millimeters.
    // return negative values to use the default boundaries.
    virtual pair<int, int> getDepthInputBoundaries() {return pair<int, int>(-1, -1);};

    double timeOfLastPinConfigsUpdate = -1;
    
    
    //Dan trying to add stuff
    void setRefForShapeIOManager(SerialShapeIOManager* customIOManager);
    
protected:
    
    ofPixels heightsForShapeDisplay;
    std::vector<std::vector<PinConfigs>> pinConfigsForShapeDisplay;
    const ofPixels *heightsFromShapeDisplay;
    bool hasHeightsFromShapeDisplay = false;

    const ofPixels *colorPixelsFromKinect;
    const ofPixels *depthPixelsFromKinect;
    bool hasPixelsFromKinect = false;

    ofFbo heightsDrawingBuffer;
    
    SerialShapeIOManager* m_CustomShapeDisplayManager;
};



#endif /* Application_hpp */
