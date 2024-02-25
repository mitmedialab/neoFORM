//
//  VideoPlayerApp.cpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#include "VideoPlayerApp.hpp"

void VideoPlayerApp::setup() {
    //setupTransformedPixelMap();
    video.load("escher-5-slow.mov");
    video.play();
}

void VideoPlayerApp::update(float dt) {
    video.update();
    updateHeights();
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    // m_videoPixels is the stored pixels from the current video frame, stored in this app header.
    m_videoPixels = video.getPixels();
    
    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);
   
    //ofPixels onlyPinPixels = m_CustomShapeDisplayManager->getPinPixelsOnly(m_videoPixels);
    
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = m_videoPixels[m_CustomShapeDisplayManager->getPixelsToShapeDisplayIndicies()[flattenedIndex]];
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

void VideoPlayerApp::keyPressed(int key) {
    // no-op
}
