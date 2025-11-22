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

	current_name = "Escher Mode";
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

void VideoPlayerApp::switchFile(std::string filename) {
	if (filename == "escher-5-slow.mov" || filename == "inFORM-escher-mode.mp4") {
		current_name = "Escher Mode";
		cut_video = filename == "escher-5-slow.mov";
	} else if (filename == "machine-mode-TRANSFORM.mov") {
		current_name = "Machine Mode";
		cut_video = false;
	} else {
		current_name = "Video Player";
		cut_video = false;
	}
	video.close();
	video.load(filename);
	video.play();
}

std::string VideoPlayerApp::getName() {
	return current_name;
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    // m_videoPixels is the stored pixels from the current video frame, stored in this app header.
    m_videoPixels = video.getPixels();

    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);

    // Pass the current video frame to the shape display manager to get the actuated pixels.
	ofPixels livePixels;
	if (cut_video) {
		livePixels = m_CustomShapeDisplayManager->gridCropToActiveSurface(m_videoPixels);
	} else {
		livePixels = m_videoPixels;
	}

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
    string instructions = (string) "Plays video files;\n press 1 for Escher and 2 for Machine";

    return instructions;
}

void VideoPlayerApp::keyPressed(int key) {
	int next_video = current_video;
	// '[', back, or down arrow
    if (key == 91 || key == 57356 || key == 57359) {
		next_video -=1;
		next_video %=2;
	}

	// ']', foreward, or up arrow
    if (key == 93 || key == 57358 || key == 57357) {
		next_video +=1;
		next_video %=2;
	}

	if (next_video == 1) {
		if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
    	    switchFile("escher-5-slow.mov");
    	} else {
    	    switchFile("inFORM-escher-mode.mp4");
    	}
	} else {
		switchFile("machine-mode-TRANSFORM.mov");
	}

	current_video = next_video;
}
