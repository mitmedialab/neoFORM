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
    // Load videos appropriate for the shape display
    if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
        escherVideo.load("escher-5-slow.mov");
    } else {
        escherVideo.load("inFORM-escher-mode.mp4");
    }
	machineVideo.load("machine-mode-TRANSFORM.mov");

    // Initialize video metadata arrays
    videos = {&escherVideo, &machineVideo};
    videoNames = {"Escher Mode", "Machine Mode"};
    videosNeedCropping = {true, false};
    
    // Start playing the current video
    videos[currentVideoIndex]->play();
}

void VideoPlayerApp::update(float dt) {
	// Handle video source changes
	if (currentVideoIndex != lastVideoIndex) {
		videos[lastVideoIndex]->stop();
		videos[currentVideoIndex]->play();
		lastVideoIndex = currentVideoIndex;
	}

    videos[currentVideoIndex]->update();
    updateHeights();
}

pair<vector<bool*>, vector<string>> VideoPlayerApp::getOptions() {
    // No options needed - navigation is via keyboard
	return pair<vector<bool*>, vector<string>>({}, {});
};

std::string VideoPlayerApp::getName() {
	return "Video Player";
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    m_videoPixels = videos[currentVideoIndex]->getPixels();

    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);

    // Apply cropping if needed for this video source
	ofPixels livePixels;
	if (videosNeedCropping[currentVideoIndex]) {
		livePixels = m_CustomShapeDisplayManager->gridCropToActiveSurface(m_videoPixels);
	} else {
		livePixels = m_videoPixels;
	}

    // Directly copy all pixels from livePixels to heightsForShapeDisplay.
    heightsForShapeDisplay = livePixels;
}

void VideoPlayerApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the current video frame as a base
    videos[currentVideoIndex]->draw(30, 300, 544, 128);

    // Draw the preview of the actuated pixels sections.
    if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
        drawSectionPreviewFrameBuffer(30, 300, 544, 128);
    }
}

void VideoPlayerApp::drawSectionPreviewFrameBuffer(int x, int y, int width, int height) {
    // Get the width in inches of the the full transform surface.
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;

    // Calculate the pixels per inch conversion rate.
    float pixelsPerInch = videos[currentVideoIndex]->getWidth() / transformWidth;

    // Create the actuated pixel sections.
    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);

   // Create a frame buffer for the preview with the same dimensions as the video.
    ofFbo previewFrameBuffer;
    previewFrameBuffer.allocate(videos[currentVideoIndex]->getWidth(), videos[currentVideoIndex]->getHeight(), GL_RGBA); // GL_RGBA for transparency

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
    string instructions = "Plays video files\n";
    instructions += "Playing: " + videoNames[currentVideoIndex] + "\n";
    instructions += "[ or ← : Previous video\n";
    instructions += "] or → : Next video";
    return instructions;
}

void VideoPlayerApp::keyPressed(int key) {
	int numVideos = videos.size();
    
	// '[' or left/down arrow - previous video
    if (key == 91 || key == 57356 || key == 57359) {
		currentVideoIndex = static_cast<VideoSource>((currentVideoIndex - 1 + numVideos) % numVideos);
	}

	// ']' or right/up arrow - next video
    if (key == 93 || key == 57358 || key == 57357) {
		currentVideoIndex = static_cast<VideoSource>((currentVideoIndex + 1) % numVideos);
	}
}
