//
//  VideoPlayerApp.cpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#include "VideoPlayerApp.hpp"

void VideoPlayerApp::setup() {
    video.load("escher-5-slow.mov");
    video.play();
}

void VideoPlayerApp::update(float dt) {
    video.update();
    updateHeights();
}

void VideoPlayerApp::updateHeights() {
    // get pixel values from the video and map them to pin heights here.
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