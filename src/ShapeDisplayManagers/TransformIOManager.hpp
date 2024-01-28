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
    
    void sendHeightsToShapeDisplay();
    
    ofPixels getPinPixelsOnly(ofPixels fullPixels);
    
    int* getPixelsToShapeDisplayIndicies();
    
    
    
    //char* getShapeDisplayPinsFromVideoPixelInput(VideoPixels,videoHeight, videoWidth);
    

protected:
    // setup hardware-specific board configuration
    void configureBoards();
    
    
    // TRANSFORM related dead block calculations
    int m_videoToTransformIndicies[1152];
    int m_videoPixelSize = 2448;
    int calculateTransformWithinBlockX(int blockNumber, int x_pixel_coord);
    int calculateTransformBlockNumber(int x_pixel_coord);
    void setupTransformedPixelMap();
    
    // Send in an uncorrected pixel matrix that is full width (including dead blocks) and get back a corrected pixel matrix with only active pixels.
    ofPixels getPixelsWithoutDeadBlocks(ofPixels fullPixelMatrix);
    
    
};

#endif /* TransformIOManager_hpp */
