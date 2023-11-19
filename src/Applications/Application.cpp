//
//  Application.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "Application.hpp"

Application::Application() {
    heightsForShapeDisplay.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y, OF_IMAGE_GRAYSCALE);
    heightsForShapeDisplay.set(0);
    heightsDrawingBuffer.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y);
};

void Application::getHeightsForShapeDisplay(ofPixels &heights) {
    heights = heightsForShapeDisplay;
};

void Application::getPinConfigsForShapeDisplay(PinConfigs configs[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]) {
    PinConfigs *src = (PinConfigs *) pinConfigsForShapeDisplay;
    copy(src, src + SHAPE_DISPLAY_SIZE_2D, (PinConfigs *) configs);
};

void Application::setHeightsFromShapeDisplayRef(const ofPixels *heights) {
    heightsFromShapeDisplay = heights;
    hasHeightsFromShapeDisplay = true;
};

void Application::setPixelsFromKinectRefs(const ofPixels *colorPixels, const ofPixels *depthPixels) {
    colorPixelsFromKinect = colorPixels;
    depthPixelsFromKinect = depthPixels;
    hasPixelsFromKinect = true;
};

// Dead/Alive block calculations: may move to a hardware specific class.
int Application::calculateTransformWithinBlockX(int blockNumber, int x_pixel_coord) {
    int blockDead = blockNumber%2;
    
    // Find actual location on TRANSFORM grid from video
    // NOTE TRANSFORM display is 48 pixels wide.
    // NOTE video pixels are 102 wide with 5 pixel buffer on left side. (hence 107 below)
    // right side also has a 5 pixel buffer, but doesn't matter for our caculation
    int TRANSFORM_x = (int)((48.0/107.0)*(float)(x_pixel_coord+5));
    
    if (blockDead){
        //we discard the dead block
        TRANSFORM_x = 0;
    }
    
    return TRANSFORM_x;
}

// Given an x coordinate from the video (102 pixels wide),
// We chop in into 6 segments
// Segments that are used for actuation are 2, 4, 6
// Other segments are not used by TRANSFORM
// so 102 divided into 6 segments, means each segment is 17 pixels wide
// + 5 pixels on each border
// The returned block number is the number of the block where the
// video pixel input to the function ends up.
// blocks 1,3,5, useless
// blocks 2,4,6 used in transform
int Application::calculateTransformBlockNumber(int x_pixel_coord){
    //note: int divison returns a truncated result that looks like a floor:
    // 1/2 would return 0, 4/2.5 would return 1
    int blockNum = ((x_pixel_coord+5)/16)+1;
    
    return blockNum;
}

// Runs only once on initialization.
// Fills the m_videoToTransformIndicies array with a map between flattened TRANSFORM pin numbers
// and flattened video pixels.
// m_videoToTransformIndicies(flattened TRANSFORM pixel index) = (flattened video pixel index)
// To assign TRANSFORM pixel video pixels values in the update, do something like
// (pseudocode) ==> TRANSFORM_Pin_Height(pin1) = m_video_toTransformIndicies(pin1);
void Application::setupTransformedPixelMap(){
    int counter = 0;
    
    // Iterate over all pixels in the video frame.
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 102; x++) {
            int blockAliveXCoord = calculateTransformWithinBlockX(calculateTransformBlockNumber(x),x);
            
            if (blockAliveXCoord){
                m_videoToTransformIndicies[counter] = 102*y+x;
                counter++;
                cout << "one made it \n";
            }
        }
    }
}
