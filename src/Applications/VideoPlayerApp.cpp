//
//  VideoPlayerApp.cpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#include "VideoPlayerApp.hpp"

VideoPlayerApp::VideoPlayerApp(SerialShapeIOManager *theCustomShapeDisplayManager) : Application(theCustomShapeDisplayManager) {
    cout << "VideoPlayerApp constructor\n";
}

void VideoPlayerApp::setup() {
    //setupTransformedPixelMap();
    
    // Select a video appropriate for the shape display.
    if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
        video.load("escher-5-slow.mov");
    } else {
        video.load("inFORM-escher-mode.mp4");
    }

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

    // Pass the current video frame to the shape display manager to get the actuated pixels.
    ofPixels livePixels = m_CustomShapeDisplayManager->gridCropToActiveSurface(m_videoPixels);
    
    // Directly copy all pixels from livePixels to heightsForShapeDisplay.
    heightsForShapeDisplay = livePixels;
}

void VideoPlayerApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the current video frame as a base; .
    video.draw(30, 300, 544, 128);
    
    // Draw the preview of the actuated pixels sections.
    if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
        drawSectionPreviewFrameBuffer(30, 300, 544, 128);
    }
}

void VideoPlayerApp::drawSectionPreviewFrameBuffer(int x, int y, int width, int height) {
    // Get the width in inches of the the full transform surface.
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;
    
    // Calculate the pixels per inch conversion rate.
    float pixelsPerInch = video.getWidth() / transformWidth;
    
    // Create the actuated pixel sections.
    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);
    
   // Create a frame buffer for the preview with the same dimensions as the video.
    ofFbo previewFrameBuffer;
    previewFrameBuffer.allocate(video.getWidth(), video.getHeight(), GL_RGBA); // GL_RGBA for transparency

    // Begin drawing into the frame buffer
    previewFrameBuffer.begin();
    ofClear(0, 0, 0, 0); // Clear the buffer with transparent black

    // Draw the rectangles into the frame buffer
    for (int i = 0; i < sections.size(); i++) {
        ofRectangle section = sections[i];
        
        ofSetColor(100,100,255,200);
        ofDrawRectangle(section);
    }

    // End drawing into the frame buffer
    previewFrameBuffer.end();

    // Draw the frame buffer at the same position and scale as the video
    previewFrameBuffer.draw(x, y, width, height);
}

string VideoPlayerApp::appInstructionsText() {
    string instructions = (string) "Plays video files";
    
    return instructions;
}

void VideoPlayerApp::keyPressed(int key) {
    // no-op
}
