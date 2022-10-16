//
//  VideoPlayerApp.cpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#include "VideoPlayerApp.hpp"

void VideoPlayerApp::setup() {
    setupTransformedPixelMap();
    video.load("escher-5-slow.mov");
    video.play();
}

void VideoPlayerApp::update(float dt) {
    video.update();
    updateHeights();
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    //m_videoPixels = video.getPixels().getData();
    
    //m_videoPixels is the stored pixels from the current video frame, stored in this app header.
    m_videoPixels = video.getPixels();
    
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);
    //cout << plz.size() << "\n";
    //cout << m_videoToTransformIndicies[56] << " in array\n";
   
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the unwrapped array.
            int unwrappedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[unwrappedIndex] = m_videoPixels[m_videoToTransformIndicies[unwrappedIndex]];
        }
    }
}

void VideoPlayerApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the video file.
    video.draw(30, 300, 544, 128);
}

string VideoPlayerApp::appInstructionsText() {
    string instructions = (string) "Plays video files";
    
    return instructions;
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
int VideoPlayerApp::calculateBlockNumber(int x_pixel_coord){
    //note: int divison returns a truncated result that looks like a floor:
    // 1/2 would return 0, 4/2.5 would return 1
    int blockNum = ((x_pixel_coord+5)/16)+1;
    
    return blockNum;
}

// Once block has been established, the
// x coordinate of the video pixel, and the block number are input to this function.
// If its a useless block, (1, 3, or 5), made 0, will never be shown
// If it is in a useful block (2,4,6), x_pixel coordinate will be translated into
// x coordinate on the TRANSFORM pin.
int VideoPlayerApp::calculateWithinBlockX(int blockNumber, int x_pixel_coord){
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

// Runs only once on initialization.
// Fills the m_videoToTransformIndicies array with a map between flattened TRANSFORM pin numbers
// and flattened video pixels.
// m_videoToTransformIndicies(flattened TRANSFORM pixel index) = (flattened video pixel index)
// To assign TRANSFORM pixel video pixels values in the update, do something like
// (pseudocode) ==> TRANSFORM_Pin_Height(pin1) = m_video_toTransformIndicies(pin1);
void VideoPlayerApp::setupTransformedPixelMap(){
    int counter = 0;
    
    // Iterate over all pixels in the video frame.
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 102; x++) {
            int blockAliveXCoord = calculateWithinBlockX(calculateBlockNumber(x),x);
            
            if (blockAliveXCoord){
                m_videoToTransformIndicies[counter] = 102*y+x;
                counter++;
                cout << "one made it \n";
            }
        }
    }
}
int VideoPlayerApp::getBlockWidth(){
    return m_blockWidth;
}

void VideoPlayerApp::keyPressed(int key) {
    // no-op
}
