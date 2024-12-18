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

KinectHandWavy::KinectHandWavy(KinectManager* kinectManager) {
    m_kinectManager = kinectManager;
}

KinectHandWavy::KinectHandWavy(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager)
: Application(theSerialShapeIOManager) {
    
    m_kinectManager = theKinectManager;
}



void KinectHandWavy::setup() {
    cout << "hello there hand wavy";
}

void KinectHandWavy::update(float dt) {
    // prevents running without kinectManager
    if (!m_kinectManager->isConnected()) return;

    cout << "I'm updating!!!!!!";
    
    m_kinectManager->update();
    updateHeights();
    
}
// This is responsible for drawing the on screen preview of the app's behavior.
void KinectHandWavy::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // prevents running without kinectManager
    if (!m_kinectManager->isConnected()) return;

    //*** Draw the color pixels for reference.
    m_kinectManager->colorImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    //*** Overlay the depth image on top of the color image.
    // Set the color to white with 50% opacity
    ofSetColor(255, 255, 255, 127);

    // Draw the depth image
    m_kinectManager->depthImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    // Reset the color to fully opaque white
    ofSetColor(255, 255, 255, 255);

    //*** Draw the mask rectangle
    drawPreviewMaskRectangle();

    //*** Preview shape display pixels
    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
    blurredDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    //*** Contours are disabled, but maybe they will be useful in the future.
    //m_kinectManager->drawContours();

    if ( m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM" ) {
        //*** Draw preview of the actuated pixel regions (sections).
        drawPreviewActuatedSections();
    }
}

// Draw a rectangle around the shape display pixels based on the mask info from settings.xml
void KinectHandWavy::drawPreviewMaskRectangle() {
    // prevents running without kinectManager
    if (!m_kinectManager->isConnected()) return;

    // Set the drawing parameters
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofSetLineWidth(5);
    
    // Draw the rectangle with the dimensions of the mask.
    ofDrawRectangle(
                    m_kinectManager->m_mask.getX(),
                    m_kinectManager->m_mask.getY(),
                    m_kinectManager->m_mask.getWidth(),
                    m_kinectManager->m_mask.getHeight()
    );
    
    // Unset color and fill for future drawing operations
    ofSetColor(255, 255, 255);
    ofFill();
}

// Draw a semi-transparent rectangle over each of the three the actuated sections. This should only be called when the shape display is a transFORM.
void KinectHandWavy::drawPreviewActuatedSections() {
    // prevents running without kinectManager
    if (!m_kinectManager->isConnected()) return;
    
    // Get the width in inches of the the full transform surface (need to cast shape display manager object first).
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;
    
    // Get the actuated section dimensions from the CustomShapeDisplayManager
    float pixelsPerInch = m_kinectManager->m_mask.getWidth() / transformWidth;
    
    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);
    
    // Create a frame buffer with the same dimensions as the cropped signal.
    ofFbo fbo;
    fbo.allocate(m_kinectManager->m_mask.getWidth(), m_kinectManager->m_mask.getHeight(), GL_RGBA); // GL_RGBA for transparency

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
    fbo.draw(m_kinectManager->m_mask);
}

void KinectHandWavy::updateHeights() {
    // prevents running without kinectManager
    if (!m_kinectManager->isConnected()) return;
    // Add blur to the depth image.
    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
    
    // Pass the current depth image to the shape display manager to get the actuated pixels.
    ofPixels livePixels = m_CustomShapeDisplayManager->cropToActiveSurface( blurredDepthImg.getPixels() );
    
    // Directly copy all pixels from livePixels to heightsForShapeDisplay.
    heightsForShapeDisplay = livePixels;
}

ofxCvGrayscaleImage KinectHandWavy::getBlurredDepthImg() {
    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->croppedDepthImg;
    blurredDepthImg.blurGaussian(41);
    
    return blurredDepthImg;
}

void KinectHandWavy::keyPressed(int Key) {
    
}
