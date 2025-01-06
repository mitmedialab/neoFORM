//
//  KinectHandWavy.cpp
//  neoForm
//
//  Created by admin on 2/11/24.
//

#include "KinectHandWavy.hpp"
#include <iostream>

#include "ofxXmlSettings.h"
#include <opencv2/imgproc.hpp>

KinectHandWavy::KinectHandWavy(KinectManagerSimple* kinectManager) {
    m_kinectManager = kinectManager;
}

KinectHandWavy::KinectHandWavy(SerialShapeIOManager *theSerialShapeIOManager, KinectManagerSimple *theKinectManager)
: Application(theSerialShapeIOManager) {
    
    m_kinectManager = theKinectManager;
}

void KinectHandWavy::setup() {
    cout << "hello there hand wavy";
}

void KinectHandWavy::update(float dt) {
    //cout << "I'm updating!!!!!!";
    
    m_kinectManager->update();
    updateHeights();
    
}
// This is responsible for drawing the on screen preview of the app's behavior.
void KinectHandWavy::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    //*** Draw the color pixels for reference.
    ofImage colorImg = m_kinectManager->getColorPixels();
    colorImg.draw(2, 2, colorImg.getWidth(), colorImg.getHeight());
    
    //*** Overlay the depth image on top of the color image.
    // Set the color to white with 50% opacity
    ofSetColor(255, 255, 255, 127);

    // Draw the depth image
    ofShortImage depthImg = m_kinectManager->getDepthPixels();
    depthImg.draw(2, 2, depthImg.getWidth(), depthImg.getHeight());
    
    // Reset the color to fully opaque white
    ofSetColor(255, 255, 255, 255);

    //*** Draw the mask rectangle
    drawPreviewMaskRectangle();

    //*** Preview shape display pixels
    m_kinectManager->crop(depthImg.getPixels());
    depthImg.draw(2, 400, depthImg.getWidth(), depthImg.getHeight());

    //*** Contours are disabled, but maybe they will be useful in the future.
    //m_kinectManager->drawContours();

    if ( m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM" ) {
        //*** Draw preview of the actuated pixel regions (sections).
        drawPreviewActuatedSections();
    }
}

// Draw a rectangle around the shape display pixels based on the mask info from settings.xml
void KinectHandWavy::drawPreviewMaskRectangle() {
    // Set the drawing parameters
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofSetLineWidth(5);
    
    // Draw the rectangle with the dimensions of the mask.
    ofDrawRectangle(
                    m_kinectManager->mask.getX(),
                    m_kinectManager->mask.getY(),
                    m_kinectManager->mask.getWidth(),
                    m_kinectManager->mask.getHeight()
    );
    
    // Unset color and fill for future drawing operations
    ofSetColor(255, 255, 255);
    ofFill();
}

// Draw a semi-transparent rectangle over each of the three the actuated sections. This should only be called when the shape display is a transFORM.
void KinectHandWavy::drawPreviewActuatedSections() {
    // Get the width in inches of the the full transform surface (need to cast shape display manager object first).
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;
    
    // Get the actuated section dimensions from the CustomShapeDisplayManager
    float pixelsPerInch = m_kinectManager->mask.getWidth() / transformWidth;
    
    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);
    
    // Create a frame buffer with the same dimensions as the cropped signal.
    ofFbo fbo;
    fbo.allocate(m_kinectManager->mask.getWidth(), m_kinectManager->mask.getHeight(), GL_RGBA); // GL_RGBA for transparency

    // Begin drawing into the frame buffer
    fbo.begin();
    ofClear(0, 0, 0, 0); // Clear the buffer with transparent black

    // Draw each of the section rectangles into the frame buffer
    for (const auto& section : sections) {
        ofSetColor(100,100,255,200);
        ofDrawRectangle(section);
    }

    // End drawing into the frame buffer
    fbo.end();
    fbo.draw(m_kinectManager->mask);
}

void KinectHandWavy::updateHeights() {
    // Add blur to the depth image.
    //ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
    
    ofShortPixels pix = m_kinectManager->getDepthPixels();
    m_kinectManager->crop(pix);

    // Apply thresholding and interpolation directly to the 16-bit depth pixel values.
    // This effectively isolates a portion of the depth image that is within a certain range of values in z-space.
    // Pixels with depth values below 51200 (200*256) are set to 0.
    // Pixels with depth values above 56320 (220*256) are set to 65280 (255*256).
    // Pixels with depth values between 51200 and 56320 are linearly interpolated between 0 and 65280.
    // The function acts on a reference to the original object, so the original object is modified.
    m_kinectManager->thresholdInterp(pix, 200*256, 220*256, 0, 255*256);
    
    
    // Apply a blur to smooth out the kinect depth image.
    // Convert the higher precision depth image to a lower precision OpenCV grayscale image in order to add a blur.
    // This is the last stage of processing, so hopefully the downsampling won't impact the quality of the actuated pixels.
    ofxCvGrayscaleImage grayImage;
    grayImage.allocate(pix.getWidth(), pix.getHeight()); // Allocate with the correct dimensions
    grayImage.setFromPixels(pix);
 
    // Apply a Gaussian blur
    grayImage.blurGaussian(41);
    
    // Pass the blurred depth image pixels to the shape display manager to get the actuated pixels.
    // We can use the ofxGrayscaleImages getPixels, it appears to be compatible with passing the pix object.
    ofPixels livePixels = m_CustomShapeDisplayManager->cropToActiveSurface( grayImage.getPixels() );
    
    // Directly copy all pixels from livePixels to heightsForShapeDisplay.
    heightsForShapeDisplay = livePixels;
}

//ofxCvGrayscaleImage KinectHandWavy::getBlurredDepthImg() {
//    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->croppedDepthImg;
//    blurredDepthImg.blurGaussian(41);
//
//    return blurredDepthImg;
//}

void KinectHandWavy::keyPressed(int Key) {
    
}
