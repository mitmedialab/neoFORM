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
        escher_video.load("escher-5-slow.mov");
    } else {
        escher_video.load("inFORM-escher-mode.mp4");
    }
	machine_video.load("machine-mode-TRANSFORM.mov");

	cur_video = &escher_video;
    cur_video->play();
}

void VideoPlayerApp::update(float dt) {
	if (is_escher != last_is_escher) {
		cur_video->stop();
		if (is_escher) {
			cur_video = &escher_video;
			cut_video = true;
		} else {
			cur_video = &machine_video;
			cut_video = false;
		}
		cur_video->play();
	}
	last_is_escher = is_escher;

    cur_video->update();
    updateHeights();
}

pair<vector<bool*>, vector<string>> VideoPlayerApp::getOptions() {
	return pair<vector<bool*>, vector<string>>({&is_escher}, {is_escher ? "switch to: Machine Mode" : "switch to: Escher Mode"});
};

std::string VideoPlayerApp::getName() {
	return "Video Player";
}

void VideoPlayerApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    // m_videoPixels is the stored pixels from the current video frame, stored in this app header.
    m_videoPixels = cur_video->getPixels();

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
    cur_video->draw(30, 300, 544, 128);

    // Draw the preview of the actuated pixels sections.
    if (m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM") {
        drawSectionPreviewFrameBuffer(30, 300, 544, 128);
    }
}

void VideoPlayerApp::drawSectionPreviewFrameBuffer(int x, int y, int width, int height) {
    // Get the width in inches of the the full transform surface.
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;

    // Calculate the pixels per inch conversion rate.
    float pixelsPerInch = cur_video->getWidth() / transformWidth;

    // Create the actuated pixel sections.
    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);

   // Create a frame buffer for the preview with the same dimensions as the video.
    ofFbo previewFrameBuffer;
    previewFrameBuffer.allocate(cur_video->getWidth(), cur_video->getHeight(), GL_RGBA); // GL_RGBA for transparency

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
    string instructions = is_escher ?
		"Plays video files;\nplaying: Escher Mode" :
		"Plays video files;\nplaying: Machine Mode";

    return instructions;
}

void VideoPlayerApp::keyPressed(int key) {
	// '[', back, or down arrow
    if (key == 91 || key == 57356 || key == 57359) {
		is_escher = !is_escher;
	}

	// ']', forward, or up arrow
    if (key == 93 || key == 57358 || key == 57357) {
		is_escher = !is_escher;
	}
}
