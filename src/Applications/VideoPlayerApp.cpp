//
//  VideoPlayerApp.cpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#include "VideoPlayerApp.hpp"

VideoPlayerApp::VideoPlayerApp(SerialShapeIOManager *theCustomShapeDisplayManager, string mode) 
    : Application(theCustomShapeDisplayManager), mode(mode) {
    cout << "VideoPlayerApp constructor for mode: " << mode << "\n";
    
    // Configure based on mode
    if (mode == "escher") {
        displayName = "Escher Mode";
        needsCropping = true;
        if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
            videoFilename = "escher-5-slow.mov";
        } else {
            videoFilename = "inFORM-escher-mode.mp4";
        }
    } else if (mode == "machine") {
        displayName = "Machine Mode";
        needsCropping = false;
        videoFilename = "machine-mode-TRANSFORM.mov";
    }
}

void VideoPlayerApp::setup() {
    video.load(videoFilename);
    video.play();
}

void VideoPlayerApp::update(float dt) {
    video.update();
    updateHeights();
}

std::string VideoPlayerApp::getName() {
	return displayName;
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    m_videoPixels = video.getPixels();

    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);

    // Apply cropping if needed for this video source
	ofPixels livePixels;
	if (needsCropping) {
		livePixels = m_CustomShapeDisplayManager->gridCropToActiveSurface(m_videoPixels);
	} else {
		livePixels = m_videoPixels;
	}

    // Directly copy all pixels from livePixels to heightsForShapeDisplay.
    heightsForShapeDisplay = livePixels;
}

void VideoPlayerApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the current video frame as a base
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
    return "Plays " + displayName + " video";
}

void VideoPlayerApp::onBecameActive() {
    // Restart machine mode from beginning when activated
    // (escher mode can resume from anywhere since it's a seamless loop)
    if (mode == "machine") {
        video.setPosition(0.0f);  // Restart from beginning
        video.play();
    }
}

void VideoPlayerApp::onBecameInactive() {
    // Optional: could pause video here to save resources
    // video.setPaused(true);
}
