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
    
    //plz is the stored pixels from the current video frame, stored in this app header.
    plz = video.getPixels();
    
    plz.setImageType(OF_IMAGE_GRAYSCALE);
    cout << plz.size() << "\n";
    //cout << m_videoToTransformIndicies[56] << " in array\n";
   
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            int unwrappedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            char foo = plz[m_videoToTransformIndicies[unwrappedIndex]];
            
            heightsForShapeDisplay[unwrappedIndex] = plz[m_videoToTransformIndicies[unwrappedIndex]];
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

int VideoPlayerApp::calculateBlockNumber(int x_pixel_coord){
    //note: int divison returns a truncated result that looks like a floor:
    // 1/2 would return 0, 4/2.5 would return 1
    int blockNum = ((x_pixel_coord+5)/16)+1;
    
    return blockNum;
}

int VideoPlayerApp::calculateWithinBlockX(int blockNumber, int x_pixel_coord){
    int blockDead = blockNumber%2;
    
    //find actual location on TRANSFORM grid from video
    int TRANSFORM_x = (int)((48.0/107.0)*(float)(x_pixel_coord+5));
    
    if (blockDead){
        //we discard the dead block
        TRANSFORM_x = 0;
    }
    
    return TRANSFORM_x;
}

void VideoPlayerApp::setupTransformedPixelMap(){
    int counter = 0;
    
    //go through all
 
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
