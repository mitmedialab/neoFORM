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

    TransformIOManager(KinectManager* kinectRef);

    // should pins that appear stuck be turned off at regular intervals?
    bool enableStuckPinSafetyToggle = false;

    // the name of this shape display
    string shapeDisplayName = "transFORM";
    
    void sendHeightsToShapeDisplay();
    
    ofPixels getPinPixelsOnly(ofPixels fullPixels);
    
    int* getPixelsToShapeDisplayIndicies();
    
    ofPixels getKinectStream();
    
    // Extracts only the actuated pixel from the full TRANSFORM surface, leaving behind the dead zones.
    ofPixels getActuatedPixelsFromFullTransformSurface( ofPixels fullSurface );
    
    //char* getShapeDisplayPinsFromVideoPixelInput(VideoPixels,videoHeight, videoWidth);
    
    float m_Transform_L_outer = 13.375; //inches
    float m_Transform_L_inner = 13.9375; //inches
    float m_Transform_R_inner = 14.25; //inches
    float m_Transform_R_outer = 13.1875; //inches

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
    std::vector<float> m_activeZoneXstarts = {13.375, 44, 75};

    std::vector<ofRectangle> createSections( float pixelsPerInch );
    ofPixels cropToActiveSurface(ofPixels fullSurface);

    float m_Transform_block = 15.75; //inches
    int m_Transform_block_h_pins = 24; // # of pins
    int m_Transform_block_w_pins = 16; // # of pins

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
    
    ofPixels combineActiveZones(ofPixels fullSurface, std::vector<ofRectangle> sections);
    
};

#endif /* TransformIOManager_hpp */
